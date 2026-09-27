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




void alles_aus(){


     /*
      if (cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
              Serial.println("Rebooting: Pump Off");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_aus + "%20" + oops + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              //client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_aus + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), 0);
          }
    */
    if (cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
              Serial.println("Rebooting: Chlorinator Off");

              client.print(String("GET ") + "/cm?cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), 0);
          }


}




void call_pumpe_aus(){
          // v7.0: Manual-Pump-ON friert Pumpenstufe ein
          if (pump_manual_on) {
            Serial.println("Pump Off blocked: Manual ON aktiv");
            return;
          }
          if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
              return;
          }
          else{
              Serial.println("Pump Off");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_aus + "%20" + oops + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), 0);
              pumpe_on = false;
          }
}


void call_pumpe_hand(){
          if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
              return;
          }
          else{
              Serial.println("Pump ON (Manual)");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_hand + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), atoi(pumpe_hand));
              pumpe_on = true;
          }
}


void call_pumpe_dosierung(){
          // v7.0: Manual-Pump-ON friert Pumpenstufe ein - Dosierung laeuft normal
          // weiter (Chlorinator, PH-Minus), nur die Pumpenstufe wird nicht geaendert.
          if (pump_manual_on) {
            Serial.println("Pump Dosage call blocked: Manual ON aktiv");
            return;
          }
          if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
              return;
          }
          else{
              Serial.println("            Pump On (Dosage calling)           ****");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_dosierung + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), atoi(pumpe_dosierung));
              pumpe_on = true;
          }
}


// v7.0: 24h-Timeout fuer Manual Pump ON. Wird aus dem Loop aufgerufen.
// Nach Ablauf wird Manual deaktiviert; die normale Timer-Logik uebernimmt
// im naechsten Tick (kein Force-Off hier - timer2() schaltet ggf. ab).
void pump_manual_tick() {
  if (!pump_manual_on) return;
  if (millis() - pump_manual_start_ms < PUMP_MANUAL_TIMEOUT_MS) return;

  Serial.println("");
  Serial.println("######  Manual Pump ON: 24h-Timeout - zurueck auf Automatik  ######");
  pump_manual_on = false;
  pump_manual_start_ms = 0;
  check_pump_on  = "";
  check_pump_off = "checked";
  timer_interval_delay = standard_timer_interval_delay;
  previousMillis = 0;
}


// v7.0: Manual-Pump-ON Block fuer das Dashboard. Nichts ausgeben wenn nicht aktiv.
// Fette Warnbox damit klar ist dass Automatik aktuell uebersteuert ist.
void pump_manual_dashboard_block(String &message) {
  if (!pump_manual_on) return;
  unsigned long elapsed = millis() - pump_manual_start_ms;
  unsigned long remain  = (elapsed < PUMP_MANUAL_TIMEOUT_MS) ? (PUMP_MANUAL_TIMEOUT_MS - elapsed) : 0;
  unsigned long remain_min = remain / 60000UL;
  unsigned long h = remain_min / 60;
  unsigned long m = remain_min % 60;

  message += F("<div style=\"background:#c40; color:#fff; padding:0.8em 1.2em; margin:0.6em auto; "
               "border:3px solid #ff0; border-radius:0.5em; max-width:560px; "
               "font-size:1.25em; font-weight:bold; text-align:center; "
               "text-shadow:0 0 4px #000;\">");
  message += F("&#9888; <span style=\"font-size:1.15em\">MANUAL PUMP ON</span> &#9888;<br>");
  message += F("<span style=\"font-size:0.85em; font-weight:normal\">Pumpe Stufe ");
  message += pumpe_hand;
  message += F(" &mdash; Timer / Chlor / Winter sind blockiert<br>");
  message += F("Auto-Fallback in ");
  if (h < 10) message += F("0");
  message += h;
  message += F(":");
  if (m < 10) message += F("0");
  message += m;
  message += F(" h</span>");
  message += F("</div>");
}


void check_flowcontrol_switch(){
            if (millis() - flowcontrol_fail_time < HOST_RETRY_DELAY) {
              flow = false;
              return;  // Host war kuerzlich nicht erreichbar
            }
            flow = false;
            http.begin(client, "http://" + String(hostname_flowcontrol) + "/cm?user=admin&password=" + password_flowcontrol + "&cmnd=status%2010");  // Starte Abfrage FlowControl
            int httpCode = http.GET();

            if (httpCode > 0) {
              if (httpCode == HTTP_CODE_OK) {
                  String payload = http.getString();

                  if (payload.substring(54,56) == "ON"){       // Wenn FlowControl An dann Chlor An
                      Serial.println("FlowControl positive");
                      flow = true;
                      ThingSpeak.setField(String(flowID).toInt(), 1);


                  }
                  else{
                      ThingSpeak.setField(String(flowID).toInt(), 0);
                  }

               }  // http Code OK
            }  // http Code > 0

            else {
              flow = false;
              flowcontrol_fail_time = millis();  // Failure-Cache setzen
              ThingSpeak.setField(String(flowID).toInt(), 0);
              Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

          http.end();
}



