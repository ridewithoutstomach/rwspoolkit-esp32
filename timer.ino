/* *****************************************************************
   RWS Pool-Kit v6.3
   Copyright (c) 2022-2026 Ridewithoutstomach
   https://rws.casa-eller.de
   https://github.com/ridewithoutstomach/rwspoolkit-esp32
  *****************************************************************
   Hardware: Adafruit Feather HUZZAH32 ESP32 (Atlas Scientific Wi-Fi Pool Kit V1.8)
  *****************************************************************
   MIT License - see LICENSE file for details.
  *******************************************************************/

// v6.0: NTP-Sync alle 30 Minuten statt bei jedem Timer-Aufruf
// ESP8266 driftet ca. 1-2 Sekunden pro Stunde, das reicht fuer Minuten-Timer
unsigned long previousMillis_ntp = 0;
constexpr unsigned long NTP_SYNC_INTERVAL = 1800000UL;  // 30 Minuten
bool ntp_synced = false;
uint8_t ntp_fail_count = 0;

// NTP-Server Liste (Fallback)
const char* ntp_pool_list[] = {
  "pool.ntp.org",
  "time.google.com",
  "time.cloudflare.com"
};
const uint8_t NTP_SERVER_COUNT = 3;
uint8_t ntp_server_index = 0;

void ntp_sync() {
  unsigned long now = millis();
  // Beim ersten Aufruf oder nach 30 Minuten
  // Wenn noch nicht gesynct: alle 10 Sekunden versuchen
  unsigned long interval = ntp_synced ? NTP_SYNC_INTERVAL : 10000UL;

  if (now - previousMillis_ntp >= interval) {
    previousMillis_ntp = now;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("NTP: kein WiFi");
      return;
    }

    Serial.print("NTP Sync (");
    Serial.print(ntp_pool_list[ntp_server_index]);
    Serial.print(")... ");

    bool ok = timeClient.forceUpdate();
    if (ok) {
      ntp_synced = true;
      ntp_fail_count = 0;
      Serial.print("OK: ");
      Serial.print(timeClient.getHours());
      Serial.print(":");
      Serial.println(timeClient.getMinutes());
    }
    else {
      ntp_fail_count++;
      Serial.print("FEHLGESCHLAGEN (#");
      Serial.print(ntp_fail_count);
      Serial.println(")");

      // Nach 3 Fehlversuchen: naechsten Server probieren
      if (ntp_fail_count >= 3) {
        ntp_fail_count = 0;
        ntp_server_index = (ntp_server_index + 1) % NTP_SERVER_COUNT;
        timeClient.setPoolServerName(ntp_pool_list[ntp_server_index]);
        Serial.print("NTP: wechsle zu Server: ");
        Serial.println(ntp_pool_list[ntp_server_index]);
      }
    }
  }
}


// Wintermode
void timer(){
  // NTP unabhaengig vom Timer-Intervall synchronisieren
  ntp_sync();

  if ( winter_modus == true ){
     if (RTD.get_last_received_reading() < String(winter_temp).toInt() || dht_temp() < String(winter_shaft_temp).toInt() ){
        Serial.println("");
        Serial.print("WinterTemp unterschritten");
        Serial.println("");
             if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
                 Serial.println("connection failed");
             }
             else{
                 client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_temp + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
                 client.stop();
                 ThingSpeak.setField(String(pumpID).toInt(), atoi(pumpe_temp));
                 zeit_on = true;

             }
     }
     else{
          if ( check_pump_off == "checked" ){
          timer2();
          }
     }


  }
  else{
        if ( check_pump_off == "checked" ){
        timer2();
        }
  }
}
 // Wintermode


// v6.0 Tasmota-Style Timer Logik
// Suche den zuletzt vergangenen aktiven Timer und setze dessen Speed
void timer2(){

  Serial.println("Void Timer (Tasmota-Style)");

  currentMillis = millis();
  if (currentMillis - previousMillis >= timer_interval_delay) {
    previousMillis = currentMillis;

    // Abfragezeit auf Standard setzen
    timer_interval_delay = standard_timer_interval_delay;

    // Aktuelle Zeit ausgeben (NTP wird separat in ntp_sync() geholt)
    Serial.print("Time: ");
    Serial.print(timeClient.getHours());
    Serial.print(":");
    Serial.println(timeClient.getMinutes());
    Serial.print("Call timer every: ");
    Serial.print(timer_interval_delay / 1000);
    Serial.println("s");

    if (!ntp_synced) {
      Serial.println("WARNUNG: NTP noch nicht synchronisiert - Timer laeuft mit 0:00");
    }

    // Umrechnung UTC Zeit in minute_of_day
    int minute_of_day;
    if ( summer == true ){
       minute_of_day = (timeClient.getHours() * 60 + timeClient.getMinutes() + 60);
    }
    else{
       minute_of_day = (timeClient.getHours() * 60 + timeClient.getMinutes());
    }
    // Mitternachts-Korrektur (Sommerzeit kann ueber 1440 gehen)
    if (minute_of_day >= 1440) minute_of_day -= 1440;

    Serial.print("minute_of_day = ");
    Serial.println(minute_of_day);

    // Finde den zuletzt vergangenen aktiven Timer
    // Logik: Der Timer mit der groessten Zeit <= minute_of_day gewinnt
    // Falls kein Timer <= minute_of_day, nimm den letzten Timer des Vortages (groesste Zeit ueberhaupt)
    int best_speed = -1;
    int best_time = -1;
    int wrap_speed = -1;   // fuer Wrap-Around ueber Mitternacht
    int wrap_time = -1;

    for (int i = 0; i < TIMER_COUNT; i++) {
      if (!timers[i].aktiv) continue;
      int t = timer_minuten[i];

      if (t <= minute_of_day) {
        // Timer liegt heute in der Vergangenheit oder genau jetzt
        if (t > best_time) {
          best_time = t;
          best_speed = timers[i].speed;
        }
      }
      // Fuer Wrap-Around: merke den spaetesten Timer ueberhaupt
      if (t > wrap_time) {
        wrap_time = t;
        wrap_speed = timers[i].speed;
      }
    }

    // Wenn kein Timer heute in der Vergangenheit lag, nehme den vom Vortag (Wrap-Around)
    if (best_speed == -1 && wrap_speed != -1) {
      best_speed = wrap_speed;
      best_time = wrap_time;
    }

    Serial.print("Aktiver Speed: ");
    Serial.println(best_speed);

    if (best_speed > 0) {
      // Pumpe auf diesen Speed setzen
      Serial.print("****** Timer aktiv: Speed ");
      Serial.print(best_speed);
      Serial.print(" (von ");
      Serial.print(best_time / 60);
      Serial.print(":");
      if (best_time % 60 < 10) Serial.print("0");
      Serial.print(best_time % 60);
      Serial.println(") ******");

      if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
        Serial.println("connection failed");
      }
      else {
        String cmd = String("/cm?cmnd=Power") + String(best_speed) + "%201";
        Serial.print("Timer CMD: GET ");
        Serial.print(cmd);
        Serial.print(" -> ");
        Serial.println(hostname_pumpe);
        client.print(String("GET ") + cmd + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
        client.stop();
        ThingSpeak.setField(String(pumpID).toInt(), best_speed);
        zeit_on = true;
      }
    }

    // Ergebnis auswerten
    if ( zeit_on == true ){
      Serial.println("");
      Serial.print("pump_on true / timer programmiert");
      zeit_on = false;
      pumpe_on = true;
    }
    else{
      Serial.println("");
      Serial.println("pump_on false / No timer active");
      pumpe_on = false;
      call_pumpe_aus();
    }

    Serial.println("End Void Timer");

  }
}
