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
//          PHMinus Anfang        PHMinus Anfang


void check_phMinus(){
  Serial.println("");
  Serial.println("######################");
  Serial.print("Check PH-Minus every: ");
  Serial.print(check_phMinus_interval_delay); 
  Serial.println(" min");
  Serial.print("PH Value:");
  Serial.print(PH.get_last_received_reading());
  Serial.print(" (");
  Serial.print (atof(phMinus));
  Serial.print(" )");
  Serial.println("");
  Serial.print("Temperature: ");
  Serial.println(RTD.get_last_received_reading());
  Serial.println("");
  Serial.print("DoubleCheckCounterRead: ");
  Serial.println(phminus_dblchk_counter_read);
  
  
 // PHMinus Prüfung Anfang  -----------------------------------------------------------------------


    if (pumpe_on == true){
      Serial.println("Pumpe ist an");
    }
      else{
      Serial.println("Pumpe Aus");
      
    }

    // Test
    //if (PH.get_last_received_reading() < atof(phMinus)){      // Wenn wir über Sollwert sind dann phminus an 

    
    if (PH.get_last_received_reading() > atof(phMinus) && pumpe_on == true ){      // Wenn wir über Sollwert sind dann phminus an 
   
      phminus_dblchk_counter ++;
      Serial.print("phminus_dblchk_counter=");
      Serial.println(phminus_dblchk_counter);
      // Falls zwischendurch rebootet wird:
      Serial.println("Write PHMinus");
      write_phminuspmp();
    }
    else {
      phminus_dblchk_counter = 0;
      Serial.print("phminus_dblchk_counter=");
      Serial.println(phminus_dblchk_counter);
      
      if (   phminus_dblchk_counter_read != phminus_dblchk_counter){
          Serial.println("Write PHMinus");
          // Falls zwischendurch rebootet wird:
          phminus_dblchk_counter_read = phminus_dblchk_counter;
          write_phminuspmp();  
      }

    }

       Serial.print("phminus_dblchk=");
       Serial.println(atol(phminus_dblchk));
       
        if (phminus_dblchk_counter >= atol(phminus_dblchk)){      // Wenn wir über Sollwert sind dann phminus an 
    
          Serial.println("PH above setpoint"); 
      
            // Pumpe auf pumpe_dosierung anschalten
            call_pumpe_dosierung();
            Serial.print("Set the pump follow-up time (ORP): ");
            Serial.print(pumpennachlaufzeit);
            Serial.println(" min");
            timer_interval_delay = atol(pumpennachlaufzeit) * 1000 * 60; // Timer für die Pumpennachlaufzeit einschalten ( Pumpe läuft um diese Zeit nach
            previousMillis = millis(); // reset millis für pumpennachlaufzeit
            
          
                   


             if (!cached_connect(hostname_phminus, 80, phminus_fail_time)) {
                Serial.println("connection PHMinus-Pump failed");
                phminus_dblchk_counter = 0;
                write_phminuspmp();
                //return;
            }
            else{
                  
                  Serial.println("Impuls to PHMinis PMP");
                  Serial.println("######################");
                  Serial.println("");      
                  Serial.print("Set the pump follow-up time (PHMinus_PMP): ");
                  Serial.print(pumpennachlaufzeit);
                  Serial.println(" min");
                  timer_interval_delay = atol(pumpennachlaufzeit) * 1000 * 60; // Timer für die Pumpennachlaufzeit einschalten ( Pumpe läuft um diese Zeit nach
                  previousMillis = millis(); // reset millis für pumpennachlaufzeit

                   
                  
                  client.print(String("GET ") + "/cm?user=admin&password=" + password_phminus + "&cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_phminus + "\r\n" + "Connection: close\r\n\r\n");
                  client.stop();
                  //client.print(String("GET ") + "/cm?cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_phminus + "\r\n" + "Connection: close\r\n\r\n");
                  // phminus_fuellstand aktualisieren
                  // phminus_fuellstand = phminus_fuellstand - phminus_dosiermenge;
                  strcpy(phminus_fuellstand, String(atof(phminus_fuellstand) - atof(phminus_dosiermenge)).c_str());
                  ThingSpeak.setField(String(phminusID).toInt(), phminus_fuellstand);
                  Serial.print("Send PHMinus Level to Thingspeak: ");
                  Serial.println(phminus_fuellstand);
                  Serial.println("Write PHMinus");
                  phminus_dblchk_counter = 0;
                  write_phminuspmp();
                  
                
            }
  
                
      
    }                     // if (RTD.get_last_received_reading() > temp ){
    else{                 // PHminus ist nicht über Sollwert, wir schreiben trotzdem den phminus_fuellstand
               Serial.println("PH OK");
               //strcpy(check_phMinus_interval_delay, check_phMinus_interval_delay_std);
               ThingSpeak.setField(String(phminusID).toInt(), phminus_fuellstand);
               Serial.print("send PHMinus Level to Thingspeak: ");
               Serial.println(phminus_fuellstand);
               Serial.println("######################");
               Serial.println("");

    }

    // PHMinus Prüfung Ende  -------------------------------------------------------------------------


  
}

 //     PH Minus Ende       PH Minus Ende         PH Minus Ende
