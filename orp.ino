/* *****************************************************************
   RWS Pool-Kit v7.0
   Copyright (c) 2022-2026 Ridewithoutstomach
   https://rws.casa-eller.de
   https://github.com/ridewithoutstomach/rwspoolkit-esp32
  *****************************************************************
   Hardware: Adafruit Feather HUZZAH32 ESP32 (Atlas Scientific Wi-Fi Pool Kit V1.8)
  *****************************************************************
   MIT License - see LICENSE file for details.
  *******************************************************************/

// v7.0: Phasen-basierte Chlorinator-Regelung
//
// BEOBACHTEN -> CHLOREN -> VERTEILEN -> BEOBACHTEN
//
// Zweck: hydraulische Totzeit zwischen Chlor-Einleitung (Rueckluft) und
// ORP-Sonde (Skimmer) abwarten, bevor wieder bewertet wird.
//
// Pumpe wird von Chloren-Beginn bis Verteilen-Ende durchgehend auf
// Pump Dosage Output (pumpe_dosierung) forciert. Erst beim Uebergang
// Verteilen -> Beobachten uebernimmt der Timer wieder. Nach Verteilen
// wird die Beobachten-Logik mit dem normalen Double-Checks-Counter
// fortgesetzt - liegt ORP weiter zu niedrig, triggert ein neuer Zyklus.

#include <time.h>   // gmtime_r, struct tm fuer Datum-Anzeige

// ----------------------------------------------------------------------
//  Forward-Declarations fuer Symbole aus alphabetisch spaeteren .ino-Files
// ----------------------------------------------------------------------
extern bool ntp_synced;             // definiert in timer.ino
extern bool chlor_resume_done;      // definiert in NewPoolKit_v6.3_GIT.ino
extern unsigned long chlor_phase_start_epoch;  // definiert in NewPoolKit_v6.3_GIT.ino
extern unsigned long chlor_last_start_epoch;   // definiert in NewPoolKit_v6.3_GIT.ino
extern unsigned long pump_warmup_start_ms;     // definiert in NewPoolKit_v6.3_GIT.ino
extern bool          chlor_pump_was_on;        // definiert in NewPoolKit_v6.3_GIT.ino
extern char          chlor_warmup_min[];       // definiert in NewPoolKit_v6.3_GIT.ino


// v7.2: Spiegelt timer2()'s Slot-Auswahl: latest aktiver Slot mit Zeit <= now,
// mit Wrap-Around zum letzten aktiven Slot ueberhaupt (= "yesterday's last slot").
// Rueckgabe: 1..4 = Pumpenstufe (1 = aus). 0 = keine aktiven Timer-Slots ueberhaupt
// (fallback: timer2() ruft dann call_pumpe_aus -> Pumpe physisch aus).
int current_timer_speed() {
  if (!ntp_synced) return 0;
  int now_min = (int)timeClient.getHours() * 60 + (int)timeClient.getMinutes();

  int latest_speed = -1;
  int latest_time  = -1;
  int wrap_speed   = -1;
  int wrap_time    = -1;
  for (int i = 0; i < TIMER_COUNT; i++) {
    if (!timers[i].aktiv) continue;
    int t = (int)timer_minuten[i];
    if (t <= now_min && t > latest_time) {
      latest_time  = t;
      latest_speed = timers[i].speed;
    }
    if (t > wrap_time) {
      wrap_time  = t;
      wrap_speed = timers[i].speed;
    }
  }
  if (latest_speed < 0) latest_speed = wrap_speed;   // wrap-around zu gestern letzter Slot
  if (latest_speed < 0) return 0;                    // gar keine aktiven Slots
  return latest_speed;
}

// v7.2: Minuten von jetzt bis zum spaetesten aktiven speed==1-Slot, der heute noch
// in der Zukunft liegt. Genau das ist die "letzte Aus-Anweisung des Tages".
// Liefert 9999, wenn keine Deadline anwendbar:
//   - NTP nicht synchron
//   - pump_manual_on (Manual ueberschreibt Timer)
//   - heute kommt kein Aus-Slot mehr (= entweder schon vorbei, oder gar keiner)
// Damit werden Mittagsruhe-Slots, die der Chlor sowieso durchfeuert, ignoriert.
unsigned long minutes_until_last_pump_off_today() {
  if (!ntp_synced) return 9999UL;
  if (pump_manual_on) return 9999UL;

  int now_min = (int)timeClient.getHours() * 60 + (int)timeClient.getMinutes();

  int latest_off = -1;
  for (int i = 0; i < TIMER_COUNT; i++) {
    if (!timers[i].aktiv) continue;
    if (timers[i].speed != 1) continue;
    int t = (int)timer_minuten[i];
    if (t > now_min && t > latest_off) latest_off = t;
  }
  if (latest_off < 0) {
    // Heute kommt kein weiterer Aus-Slot. Wenn die Pumpe per Timer aktuell
    // schon aus ist (Speed 1) oder gar keine Slots aktiv sind, bleibt sie aus
    // -> 0 verfuegbare Minuten. Sonst laeuft die Pumpe heute durch -> keine
    // Deadline (9999). Ohne diesen Check wuerde der Chlorinator nachts gegen
    // den Timer-Plan starten.
    int cs = current_timer_speed();
    if (cs <= 1) return 0UL;
    return 9999UL;
  }
  return (unsigned long)(latest_off - now_min);
}


// ----------------------------------------------------------------------
//  Helfer
// ----------------------------------------------------------------------

const char* chlor_phase_name(uint8_t p) {
  switch (p) {
    case CHLOR_PHASE_OBSERVE:    return "BEOBACHTEN";
    case CHLOR_PHASE_DOSE:       return "CHLOREN";
    case CHLOR_PHASE_DISTRIBUTE: return "VERTEILEN";
    default:                     return "?";
  }
}

// Chlorinator-Tasmota schalten + ThingSpeak-Field setzen.
// Pumpe wird hier nicht angefasst.
void chlorinator_on() {
  if (!cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
    Serial.println("connection chlorinator failed");
    return;
  }
  Serial.println("Switch on Chlorinator");
  client.print(String("GET ") + "/cm?cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
  client.stop();
  ThingSpeak.setField(String(chlorinatorID).toInt(), 1);
  orp_on = true;
}

void chlorinator_off() {
  if (!cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
    Serial.println("connection chlorinator failed");
    orp_on = false;
    ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
    return;
  }
  Serial.println("Switch off Chlorinator");
  client.print(String("GET ") + "/cm?cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
  client.stop();
  ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
  orp_on = false;
}

// Pumpe auf Dosier-Stufe forcieren und timer_interval_delay so setzen,
// dass timer2() in der laufenden Phase nicht dazwischenfunkt.
// hold_min = Wie viele Minuten der Timer-Block halten soll (Phase-Restzeit + Puffer).
void chlor_force_pump_dose(unsigned long hold_min) {
  // v7.0: Manual-Pump-ON friert Pumpenstufe ein. Phase laeuft trotzdem
  // (Chlorinator wird normal an/aus geschaltet), nur die Pumpenstufe
  // bleibt unangetastet und timer_interval_delay wird nicht ueberschrieben.
  if (pump_manual_on) return;
  call_pumpe_dosierung();
  unsigned long m = hold_min;
  if (m < (unsigned long)atol(pumpennachlaufzeit)) m = atol(pumpennachlaufzeit);
  timer_interval_delay = m * 60UL * 1000UL;
  previousMillis = millis();
}

// Phase wechseln + Logging + Phase-Start zuruecksetzen.
// v7.1: Phase + Start-Epoch wird in chlorinator.cfg persistiert,
// damit ein Reboot mitten in Chloren/Verteilen die Phase fortsetzen kann.
void chlor_phase_set(uint8_t new_phase) {
  Serial.println("");
  Serial.print("######  Chlor-Phase: ");
  Serial.print(chlor_phase_name(chlor_phase));
  Serial.print("  ->  ");
  Serial.print(chlor_phase_name(new_phase));
  Serial.println("  ######");
  chlor_phase = new_phase;
  chlor_phase_start_ms    = millis();
  chlor_phase_start_epoch = ntp_synced ? timeClient.getEpochTime() : 0;
  chlor_last_tick_ms = 0;
  write_chlorinator();
}

// Mitternachts-Reset des Tagesbudgets. Aufruf bei jedem Tick.
void chlor_check_day_rollover() {
  if (!ntp_synced) return;
  int day = (int)timeClient.getDay();
  if (chlor_today_day < 0) {
    chlor_today_day = day;
    return;
  }
  if (day != chlor_today_day) {
    Serial.print("Chlor: Tageswechsel - Budget-Counter Reset (war ");
    Serial.print(chlor_today_count);
    Serial.println(")");
    chlor_today_count = 0;
    chlor_today_day = day;
    write_chlorinator();   // v7.1: Tagesreset persistieren (Reboot darf Budget nicht doppelt freigeben)
  }
}

// True, wenn Tagesbudget erschoepft (0 = aus = nie erschoepft).
bool chlor_budget_exceeded() {
  long budget = atol(chlor_daily_budget);
  if (budget <= 0) return false;
  return chlor_today_count >= (uint16_t)budget;
}

int chlor_now_minute_of_day() {
  if (!ntp_synced) return -1;
  return (int)timeClient.getHours() * 60 + (int)timeClient.getMinutes();
}

// Eintritt in CHLOREN: Tagesbudget pruefen, Chlorinator AN, Pumpe forcieren.
// Rueckgabe: true wenn Phase betreten, false wenn Budget erschoepft oder Deadline zu nah.
bool chlor_enter_dose() {
  if (chlor_budget_exceeded()) {
    Serial.print("Chlor: Tagesbudget erschoepft (");
    Serial.print(chlor_today_count);
    Serial.print("/");
    Serial.print(chlor_daily_budget);
    Serial.println(") - bleibe in BEOBACHTEN");
    return false;
  }
  // v7.2: Nacht-Deadline pruefen. Reicht Pulsetime + 5 min Spuelzeit bis zur
  // letzten Aus-Anweisung des Tages? Sonst keinen neuen Zyklus starten.
  unsigned long pulse_min = (unsigned long)atol(ChlorInterval);
  unsigned long needed    = pulse_min + 5UL;
  unsigned long until_off = minutes_until_last_pump_off_today();
  if (until_off < needed) {
    Serial.print("Chlor: zu wenig Zeit bis Pumpe-Aus (");
    Serial.print(until_off);
    Serial.print(" min < benoetigt ");
    Serial.print(needed);
    Serial.println(" min) - bleibe in BEOBACHTEN");
    return false;
  }
  chlor_today_count++;
  chlor_last_start_minute = chlor_now_minute_of_day();
  chlor_last_start_epoch  = ntp_synced ? timeClient.getEpochTime() : 0;
  Serial.print("Chlor: Tagesphase #");
  Serial.println(chlor_today_count);
  unsigned long hold = (unsigned long)atol(ChlorInterval)
                     + (unsigned long)atol(chlor_distribute_min)
                     + 5UL;
  chlor_force_pump_dose(hold);
  chlorinator_on();
  chlor_phase_set(CHLOR_PHASE_DOSE);   // persistiert today_count, last_start_minute, phase, start_epoch
  return true;
}


// v7.1: Reboot-Resume. Rechnet aus chlor_phase_start_epoch wieviele Minuten
// seit Phasen-Start vergangen sind und springt in die richtige Phase weiter.
//
// DOSE       elapsed < ChlorInterval                          -> DOSE fortsetzen
// DOSE       elapsed < ChlorInterval + chlor_distribute_min   -> direkt VERTEILEN, Restzeit aus VERTEIL-Phase
// DOSE       elapsed >= ChlorInterval + chlor_distribute_min  -> BEOBACHTEN
// DISTRIBUTE elapsed < chlor_distribute_min                   -> VERTEILEN fortsetzen
// DISTRIBUTE elapsed >= chlor_distribute_min                  -> BEOBACHTEN
// OBSERVE                                                     -> nichts zu tun
//
// Wird nur einmal nach Boot aufgerufen, sobald NTP synchron ist.
void chlor_resume_after_boot() {
  unsigned long now_epoch = timeClient.getEpochTime();

  Serial.println("");
  Serial.print("###### Chlor Resume nach Boot. Stored phase=");
  Serial.print(chlor_phase_name(chlor_phase));
  Serial.print(" start_epoch=");
  Serial.print(chlor_phase_start_epoch);
  Serial.print(" now_epoch=");
  Serial.println(now_epoch);

  // Kein gueltiger Anker (alte Configdatei oder Phase wurde nie persistiert)
  // -> einfach in der gespeicherten / Default-Phase bleiben, Anker neu setzen.
  if (chlor_phase_start_epoch == 0 || now_epoch <= chlor_phase_start_epoch) {
    Serial.println("  -> kein gueltiger Resume-Anker, neu starten");
    chlor_phase_start_epoch = now_epoch;
    chlor_phase_start_ms    = millis();
    chlor_last_tick_ms      = 0;
    write_chlorinator();
    return;
  }

  unsigned long elapsed_s   = now_epoch - chlor_phase_start_epoch;
  unsigned long elapsed_min = elapsed_s / 60UL;
  Serial.print("  elapsed seit Phasen-Start: ");
  Serial.print(elapsed_min);
  Serial.println(" min");

  unsigned long pulse_min     = (unsigned long)atol(ChlorInterval);
  if (pulse_min < 1) pulse_min = 1;
  unsigned long distribute_min = (unsigned long)atol(chlor_distribute_min);
  if (distribute_min < 1) distribute_min = 1;

  uint8_t       resume_phase     = CHLOR_PHASE_OBSERVE;
  unsigned long resume_phase_min = 0;        // wieviele Minuten in der Ziel-Phase schon vorbei sind
  unsigned long resume_epoch     = now_epoch;

  if (chlor_phase == CHLOR_PHASE_DOSE) {
    if (elapsed_min < pulse_min) {
      resume_phase     = CHLOR_PHASE_DOSE;
      resume_phase_min = elapsed_min;
      resume_epoch     = chlor_phase_start_epoch;
    }
    else if (elapsed_min < pulse_min + distribute_min) {
      resume_phase     = CHLOR_PHASE_DISTRIBUTE;
      resume_phase_min = elapsed_min - pulse_min;
      resume_epoch     = chlor_phase_start_epoch + pulse_min * 60UL;
    }
    else {
      resume_phase = CHLOR_PHASE_OBSERVE;
    }
  }
  else if (chlor_phase == CHLOR_PHASE_DISTRIBUTE) {
    if (elapsed_min < distribute_min) {
      resume_phase     = CHLOR_PHASE_DISTRIBUTE;
      resume_phase_min = elapsed_min;
      resume_epoch     = chlor_phase_start_epoch;
    }
    else {
      resume_phase = CHLOR_PHASE_OBSERVE;
    }
  }
  else {
    // OBSERVE: nichts mitzunehmen, frischer Anker
    resume_phase = CHLOR_PHASE_OBSERVE;
  }

  Serial.print("  -> Resume Phase: ");
  Serial.print(chlor_phase_name(resume_phase));
  Serial.print("  bereits ");
  Serial.print(resume_phase_min);
  Serial.println(" min in dieser Phase");

  // Variablen direkt setzen (kein chlor_phase_set() weil das ms+epoch ueberschreiben wuerde)
  chlor_phase             = resume_phase;
  chlor_phase_start_epoch = resume_epoch;
  // ms-Anker so setzen, dass (millis() - chlor_phase_start_ms) der bereits vergangenen Phasenzeit entspricht
  unsigned long resume_phase_ms = resume_phase_min * 60UL * 1000UL;
  chlor_phase_start_ms = (millis() > resume_phase_ms) ? (millis() - resume_phase_ms) : 0;
  chlor_last_tick_ms   = 0;

  // Hardware in den passenden Zustand bringen
  if (resume_phase == CHLOR_PHASE_DOSE) {
    unsigned long remain_min = (pulse_min > resume_phase_min) ? (pulse_min - resume_phase_min) : 1UL;
    unsigned long hold       = remain_min + distribute_min + 5UL;
    chlor_force_pump_dose(hold);
    chlorinator_on();
  }
  else if (resume_phase == CHLOR_PHASE_DISTRIBUTE) {
    unsigned long remain_min = (distribute_min > resume_phase_min) ? (distribute_min - resume_phase_min) : 1UL;
    unsigned long hold       = remain_min + 5UL;
    chlor_force_pump_dose(hold);
    chlorinator_off();   // defensiv, falls Tasmota PowerOnState die Stufe wiederbelebt hat
  }
  else {
    // OBSERVE: nichts ein-/ausschalten, Timer uebernimmt Pumpe wie ueblich
    timer_interval_delay = standard_timer_interval_delay;
  }

  write_chlorinator();
}


// ----------------------------------------------------------------------
//  Phasen-Ticks
// ----------------------------------------------------------------------

// BEOBACHTEN: alle check_orp_interval_delay Sekunden ORP messen.
// Dosier-Bedingung: ORP < orp_min UND Pumpe an UND (Flow-Gate aus ODER flow).
void phase_observe_tick() {
  unsigned long now = millis();
  unsigned long iv = (unsigned long)atol(check_orp_interval_delay) * 1000UL;
  if (iv == 0) iv = 20000UL;
  if (chlor_last_tick_ms != 0 && now - chlor_last_tick_ms < iv) return;
  chlor_last_tick_ms = now;

  // v7.2: Warmup nach Pumpe-AN. Erst nach Ablauf wieder ORP bewerten.
  if (pump_warmup_start_ms > 0) {
    unsigned long warmup_ms = (unsigned long)atol(chlor_warmup_min) * 60UL * 1000UL;
    if (warmup_ms > 0 && now - pump_warmup_start_ms < warmup_ms) {
      unsigned long rem_min = (warmup_ms - (now - pump_warmup_start_ms)) / 60000UL + 1UL;
      Serial.println("");
      Serial.print("Phase BEOBACHTEN  Warmup laeuft, noch ~");
      Serial.print(rem_min);
      Serial.println(" min - kein ORP-Trigger");
      return;
    }
    Serial.println("Chlor: Warmup beendet - BEOBACHTEN nimmt Trigger wieder auf");
    pump_warmup_start_ms = 0;
  }

  Serial.println("");
  Serial.println("######################");
  Serial.print("Phase BEOBACHTEN  ORP=");
  Serial.print(ORP.get_last_received_reading());
  Serial.print(" (");
  Serial.print(orp_min);
  Serial.print(" - ");
  Serial.print(orp_max);
  Serial.print(")  Counter=");
  Serial.print(orp_chk_counter);
  Serial.print("/");
  Serial.print(orp_dblchk);
  Serial.print("  pumpe_on=");
  Serial.println(pumpe_on);

  bool gate_ok = pumpe_on && (!check_flow || flow);
  if (!gate_ok) {
    // Pumpe aus (oder Flow weg) -> Counter resetten. Sonst wuerde ein bei
    // Pumpe-Aus eingefrorener Counter (z.B. 9/10) beim naechsten Pumpe-AN
    // sofort durchschlagen und den Chlorinator ohne Vorlauf zuenden.
    if (orp_chk_counter != 0 || orp_chk_counter_read != 0) {
      orp_chk_counter      = 0;
      orp_chk_counter_read = 0;
      write_chlorinator();
    }
    return;
  }

  if (ORP.get_last_received_reading() < atol(orp_min)) {
    orp_chk_counter++;
    if (orp_chk_counter >= (unsigned int)atol(orp_dblchk)) {
      Serial.println("ORP unter Min und Double-Check erreicht -> CHLOREN");
      orp_chk_counter = 0;
      chlor_enter_dose();
      write_chlorinator();
      return;
    }
    write_chlorinator();
  } else {
    if (orp_chk_counter != 0) {
      orp_chk_counter = 0;
      write_chlorinator();
    }
  }
}

// CHLOREN: Chlorinator laeuft. Sicherheitsabbruch bei ORP >= ORP_Max.
// Ende: nach ChlorInterval Minuten oder Sicherheitsabbruch -> VERTEILEN.
void phase_dose_tick() {
  unsigned long now = millis();

  // v7.2: Nacht-Deadline. Wenn die letzte Pumpe-Aus-Anweisung des Tages in
  // <=5 min kommt: Chlorinator AUS, DISTRIBUTE ueberspringen, direkt OBSERVE.
  // Die verbleibenden Minuten Pumpe-Restlauf wirken als Spuelung.
  unsigned long until_off = minutes_until_last_pump_off_today();
  if (until_off <= 5UL) {
    Serial.print("Chlor: Nacht-Deadline naht (");
    Serial.print(until_off);
    Serial.println(" min) - DOSE-Abbruch, Chlorinator AUS, Pumpe an Timer freigegeben");
    chlorinator_off();
    timer_interval_delay = standard_timer_interval_delay;
    chlor_phase_set(CHLOR_PHASE_OBSERVE);
    return;
  }

  unsigned long elapsed_min = (now - chlor_phase_start_ms) / 60000UL;
  long ci = atol(ChlorInterval);
  if (ci < 1) ci = 1;
  long remain = ci - (long)elapsed_min;
  if (remain < 1) remain = 1;
  unsigned long hold = (unsigned long)remain
                     + (unsigned long)atol(chlor_distribute_min)
                     + 5UL;
  if (timer_interval_delay < hold * 60UL * 1000UL) {
    chlor_force_pump_dose(hold);
  }

  unsigned long iv = (unsigned long)atol(check_orp_interval_delay) * 1000UL;
  if (iv == 0) iv = 20000UL;
  if (chlor_last_tick_ms == 0 || now - chlor_last_tick_ms >= iv) {
    chlor_last_tick_ms = now;
    Serial.println("");
    Serial.print("Phase CHLOREN  ORP=");
    Serial.print(ORP.get_last_received_reading());
    Serial.print("  elapsed=");
    Serial.print(elapsed_min);
    Serial.print("/");
    Serial.print(ci);
    Serial.println(" min");

    if (chlor_safety_stop && ORP.get_last_received_reading() >= atol(orp_max)) {
      Serial.println("Sicherheitsabbruch: ORP >= ORP_Max -> VERTEILEN");
      chlorinator_off();
      chlor_phase_set(CHLOR_PHASE_DISTRIBUTE);
      return;
    }
  }

  if (elapsed_min >= (unsigned long)ci) {
    Serial.println("Pulsetime erreicht -> VERTEILEN");
    chlorinator_off();
    chlor_phase_set(CHLOR_PHASE_DISTRIBUTE);
  }
}

// VERTEILEN: Pflichtpause. Chlorinator AUS, Pumpe forciert auf Dosier-Stufe.
// ORP wird gemessen und im Dashboard angezeigt, loest aber keinen Trigger.
void phase_distribute_tick() {
  unsigned long now = millis();

  // v7.2: Nacht-Deadline. <=5 min vor Pumpe-Aus VERTEILEN beenden, OBSERVE,
  // Pumpe an Timer freigeben - der schaltet sie sauber ab.
  unsigned long until_off = minutes_until_last_pump_off_today();
  if (until_off <= 5UL) {
    Serial.print("Chlor: Nacht-Deadline naht (");
    Serial.print(until_off);
    Serial.println(" min) - DISTRIBUTE-Abbruch, Pumpe an Timer freigegeben");
    orp_chk_counter = 0;
    timer_interval_delay = standard_timer_interval_delay;
    chlor_phase_set(CHLOR_PHASE_OBSERVE);
    return;
  }

  unsigned long elapsed_min = (now - chlor_phase_start_ms) / 60000UL;
  long dur = atol(chlor_distribute_min);
  if (dur < 1) dur = 1;
  long remain = dur - (long)elapsed_min;
  if (remain < 1) remain = 1;

  unsigned long hold = (unsigned long)remain + 5UL;
  if (timer_interval_delay < hold * 60UL * 1000UL) {
    chlor_force_pump_dose(hold);
  }

  unsigned long iv = (unsigned long)atol(check_orp_interval_delay) * 1000UL;
  if (iv == 0) iv = 20000UL;
  if (chlor_last_tick_ms == 0 || now - chlor_last_tick_ms >= iv) {
    chlor_last_tick_ms = now;
    Serial.println("");
    Serial.print("Phase VERTEILEN  ORP=");
    Serial.print(ORP.get_last_received_reading());
    Serial.print(" (Anzeige, kein Trigger)  elapsed=");
    Serial.print(elapsed_min);
    Serial.print("/");
    Serial.print(dur);
    Serial.println(" min");
  }

  if (elapsed_min >= (unsigned long)dur) {
    Serial.println("Verteilen-Dauer abgelaufen -> BEOBACHTEN (Timer uebernimmt Pumpe wieder)");
    orp_chk_counter = 0;
    chlor_phase_set(CHLOR_PHASE_OBSERVE);
    timer_interval_delay = standard_timer_interval_delay;
  }
}

// ----------------------------------------------------------------------
//  Dispatcher (vom Loop aufgerufen)
// ----------------------------------------------------------------------

void chlor_phase_run() {
  // v7.1: Reboot-Resume erst ausfuehren, wenn NTP synchronisiert ist.
  // Vorher ist chlor_phase_start_epoch nicht interpretierbar.
  if (!chlor_resume_done && ntp_synced) {
    chlor_resume_after_boot();
    chlor_resume_done = true;
  }
  chlor_check_day_rollover();

  // v7.2: Warmup nach JEDEM Pumpe-AN (Morgen-Start, Mittagsruhe-Wiederanlauf,
  // jede Aus->An-Flanke). Edge-Detection ueber chlor_pump_was_on. Stehendes
  // Wasser am ORP-Sensor liefert nach jeder Off-Phase unbrauchbare Werte -
  // Warmup verzoegert die Auswertung, bis der Pool wieder durchmischt ist.
  if (ntp_synced) {
    bool pump_now_on = (current_timer_speed() >= 2) || pump_manual_on;
    if (pump_now_on && !chlor_pump_was_on) {
      long warmup = atol(chlor_warmup_min);
      if (warmup > 0) {
        pump_warmup_start_ms = millis();
        if (pump_warmup_start_ms == 0) pump_warmup_start_ms = 1;  // 0 bedeutet "inaktiv"
      }
      else {
        pump_warmup_start_ms = 0;   // per Config deaktiviert
      }
      orp_chk_counter      = 0;
      orp_chk_counter_read = 0;
      Serial.println("");
      Serial.print("######  Chlor: Pumpe-AN-Flanke - Warmup ");
      Serial.print(warmup);
      Serial.println(" min, ORP-Counter Reset  ######");
      write_chlorinator();
    }
    chlor_pump_was_on = pump_now_on;
  }

  switch (chlor_phase) {
    case CHLOR_PHASE_OBSERVE:    phase_observe_tick();    break;
    case CHLOR_PHASE_DOSE:       phase_dose_tick();       break;
    case CHLOR_PHASE_DISTRIBUTE: phase_distribute_tick(); break;
  }
}


// ----------------------------------------------------------------------
//  Dashboard-Helper
// ----------------------------------------------------------------------

// Format minute_of_day -> "HH:MM"
String chlor_fmt_hhmm(int mod) {
  if (mod < 0) return String("--:--");
  mod = mod % (24 * 60);
  int h = mod / 60;
  int m = mod % 60;
  String s;
  if (h < 10) s += "0";
  s += h;
  s += ":";
  if (m < 10) s += "0";
  s += m;
  return s;
}

// Format epoch (lokal, NTPClient liefert Zeit inkl. TZ-Offset) -> "dd.mm. HH:MM"
String chlor_fmt_date_hhmm(unsigned long epoch) {
  if (epoch == 0) return String("--");
  time_t t = (time_t)epoch;
  struct tm tmv;
  gmtime_r(&t, &tmv);
  char buf[16];
  snprintf(buf, sizeof(buf), "%02d.%02d. %02d:%02d",
           tmv.tm_mday, tmv.tm_mon + 1, tmv.tm_hour, tmv.tm_min);
  return String(buf);
}

// Block fuer das Dashboard. Wird sowohl im Login- als auch im Eingeloggt-Block aufgerufen.
void chlor_dashboard_block(String &message) {
  unsigned long now = millis();
  unsigned long elapsed_min = (now - chlor_phase_start_ms) / 60000UL;

  // Indikator je nach Phase - Form + Farbe (rot-gruen-schwaeche-tauglich)
  const char* dot;
  switch (chlor_phase) {
    case CHLOR_PHASE_DOSE:       dot = "<span style=\"color:#f33\">&#9654;</span> "; break;   // rot Play - aktiv chloren
    case CHLOR_PHASE_DISTRIBUTE: dot = "<span style=\"color:#fc0\">&#9203;</span> "; break;   // gelb Sanduhr - verteilen
    default:                     dot = (check_chlorinator
                                          ? "<span style=\"color:#0c0\">&#x2714;</span> "    // gruener Haken - aktiv/OK
                                          : "<span style=\"color:#888\">&#9675;</span> ");   // grauer Kreis - standby
  }
  message += dot;
  message += F("Chlor-Phase: <b>");
  message += chlor_phase_name(chlor_phase);
  message += F("</b>");

  // Phase-Detailzeile
  if (chlor_phase == CHLOR_PHASE_DOSE) {
    long total = atol(ChlorInterval);
    long rem = total - (long)elapsed_min;
    if (rem < 0) rem = 0;
    message += F(" &nbsp; ");
    message += elapsed_min;
    message += F("/");
    message += total;
    message += F(" min (rem ");
    message += rem;
    message += F(")");
  }
  else if (chlor_phase == CHLOR_PHASE_DISTRIBUTE) {
    long total = atol(chlor_distribute_min);
    long rem = total - (long)elapsed_min;
    if (rem < 0) rem = 0;
    message += F(" &nbsp; ");
    message += elapsed_min;
    message += F("/");
    message += total;
    message += F(" min (rem ");
    message += rem;
    message += F(")");
  }
  // BEOBACHTEN: keine zusaetzliche Zeile - ORP-Counter steht jetzt oben separat
  message += F("<br>");

  // Heute-Counter (eigene Zeile)
  message += F("Today: ");
  message += chlor_today_count;
  message += F(" / ");
  long budget = atol(chlor_daily_budget);
  if (budget <= 0) message += F("&infin;");
  else             message += budget;
  message += F(" phases<br>");

  // Letzter / naechster Start - kleiner und unauffaelliger
  if (chlor_today_count > 0) {
    // Heute schon chloriert: HH:MM + next earliest
    message += F("<small style=\"color:#888\">last: ");
    message += chlor_fmt_hhmm(chlor_last_start_minute);
    if (chlor_last_start_minute >= 0) {
      int earliest = chlor_last_start_minute
                   + (int)atol(ChlorInterval)
                   + (int)atol(chlor_distribute_min);
      message += F(" &nbsp; next earliest: ");
      message += chlor_fmt_hhmm(earliest);
    }
    message += F("</small><br>");
  }
  else if (chlor_last_start_epoch > 0) {
    // Heute noch nicht: zeige Datum der letzten Chlorung
    message += F("<small style=\"color:#888\">last: ");
    message += chlor_fmt_date_hhmm(chlor_last_start_epoch);
    message += F("</small><br>");
  }

  // v7.2: Warmup-Status + Nacht-Deadline
  if (pump_warmup_start_ms > 0) {
    unsigned long warmup_ms = (unsigned long)atol(chlor_warmup_min) * 60UL * 1000UL;
    if (warmup_ms > 0 && now - pump_warmup_start_ms < warmup_ms) {
      unsigned long rem_min = (warmup_ms - (now - pump_warmup_start_ms)) / 60000UL + 1UL;
      message += F("<span style=\"color:#fc0\">&#9203;</span> Warmup: noch ");
      message += rem_min;
      message += F(" min (kein ORP-Trigger)<br>");
    }
  }
  unsigned long until_off = minutes_until_last_pump_off_today();
  if (until_off == 0UL) {
    message += F("<span style=\"color:#f33\">&#x2716;</span> Pumpe ist aus &mdash; kein neuer Zyklus heute<br>");
  }
  else if (until_off < 9999UL) {
    long pulse_min = atol(ChlorInterval);
    long needed    = pulse_min + 5L;
    int now_min = (int)timeClient.getHours() * 60 + (int)timeClient.getMinutes();
    int off_at  = now_min + (int)until_off;
    message += F("<small style=\"color:#888\">Pumpe-Aus in ");
    message += until_off;
    message += F(" min (um ");
    message += chlor_fmt_hhmm(off_at);
    message += F(")");
    if ((long)until_off < needed) {
      message += F(" &mdash; <span style=\"color:#f33\">zu kurz fuer neuen Zyklus (brauche ");
      message += needed;
      message += F(" min)</span>");
    }
    message += F("</small><br>");
  }
}
