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
          if (!cached_connect(hostname_pumpe, 80, pumpe_fail_time)) {
              return;
          }
          else{
              Serial.println("Pump Off");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_aus + "%20" + oops + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              //client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_aus + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
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
              Serial.println("Pump ON");
              client.print(String("GET ") + "/cm?cmnd=" + "Power" + pumpe_hand + "%201" + " HTTP/1.1\r\n" + "Host: " + hostname_pumpe + "\r\n" + "Connection: close\r\n\r\n");
              client.stop();
              ThingSpeak.setField(String(pumpID).toInt(), 0);
              pumpe_on = true;
          }
}


void call_pumpe_dosierung(){
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



