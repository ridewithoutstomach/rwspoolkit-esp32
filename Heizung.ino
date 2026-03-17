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

// v6.1: AM2315C wieder aktiviert + Timeout-Fix (kein Endlos-Loop mehr)
// ####     TEMP Anfang           TEMP Anfang

void check_temp(){
  // v6.1: Nicht-blockierender AM2315C Read mit Timeout
  int dht_rc = DHT.requestData();
  if (dht_rc == 0) {
    unsigned long dht_start = millis();
    while (DHT.isMeasuring()) {
      if (millis() - dht_start > 100) {
        Serial.println("AM2315C: TIMEOUT in check_temp!");
        dht_rc = -99;
        break;
      }
      yield();
    }
    if (dht_rc != -99) {
      dht_rc = DHT.readData();
      if (dht_rc > 0) DHT.convert();
    }
  }
  if (dht_rc < 0 && dht_rc != AM2315C_ERROR_LASTREAD) {
    Serial.print("AM2315C Fehler: ");
    Serial.println(dht_rc);
  }
  Serial.println("");
  Serial.println("######################");
  Serial.print("Check Shaft-TEMP/Humidity every: ");
  Serial.print(check_temp_interval_delay);
  Serial.println(" s");
  Serial.print("Temp Value:");
  Serial.print(dht_temp(), 1);
  Serial.print(" (");
  Serial.print("Humidity Value:");
  Serial.print(dht_hum(), 1);
  Serial.print(" (");
  Serial.print(temp_min);
  Serial.print(" - ");
  Serial.print(temp_max);
  Serial.println(" )");
  ThingSpeak.setField(6, String(dht_temp(), 2));


  // TEMP Pruefung Anfang !!  --------------------------------------------------------

  if (dht_temp() < String(temp_min).toInt() || temp_hyst == 1 ){
         temp_hyst = 1;

        if (dht_temp() < String(temp_max).toInt() && check_heizung){

            Serial.println("Temperature low");

            if (cached_connect(hostname_heater, 80, heater_fail_time)) {
                  Serial.println("Switch on Heater");
                  Serial.println("######################");
                  Serial.println("");
                  client.print(String("GET ") + "/cm?user=admin&password=" + password_heater + "&cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_heater + "\r\n" + "Connection: close\r\n\r\n");
                  client.stop();
            }

        }

        else{

           Serial.println( "Upper setpoint reached, switch off the Heater ");
           temp_hyst =0;
           if (cached_connect(hostname_heater, 80, heater_fail_time)) {
            Serial.println("Switch off Heater");
            Serial.println("######################");
            Serial.println("");
            client.print(String("GET ") + "/cm?user=admin&password=" + password_heater + "&cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_heater + "\r\n" + "Connection: close\r\n\r\n");
            client.stop();
           }
        }
  }

  else{
    Serial.println("");
    Serial.println("Temperature OK");
    Serial.println("######################");
    Serial.println("");
  }


 if (dht_hum() > String(humidity_max).toInt() || humidity_hyst == 1 ){
         humidity_hyst = 1;

        if (dht_hum() > String(humidity_min).toInt() && check_fan){

            Serial.println("Humidity high");

            if (cached_connect(hostname_fan, 80, fan_fail_time)) {
                  Serial.println("Switch on Fan");
                  Serial.println("######################");
                  Serial.println("");
                  client.print(String("GET ") + "/cm?user=admin&password=" + password_fan + "&cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_fan + "\r\n" + "Connection: close\r\n\r\n");
                  client.stop();
            }

        }

        else{

           Serial.println( "Lower setpoint reached, switch off the Fan ");
           humidity_hyst =0;
           if (cached_connect(hostname_fan, 80, fan_fail_time)) {
            Serial.println("Switch off Fan");
            Serial.println("######################");
            Serial.println("");
            client.print(String("GET ") + "/cm?user=admin&password=" + password_fan + "&cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_fan + "\r\n" + "Connection: close\r\n\r\n");
            client.stop();
           }
        }
  }

  else{
    Serial.println("");
    Serial.println("Humidity OK");
    Serial.println("######################");
    Serial.println("");
  }

}  // Ende Void check_temp
