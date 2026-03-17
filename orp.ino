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
//                 ORP       ORP         ORP 
void check_orp(){
  
  Serial.println("");
  Serial.println("######################");
  Serial.print("Check ORP every: ");
  Serial.print(check_orp_interval_delay); 
  Serial.println(" sec");
  Serial.print("DoubleChecks: ");
  Serial.println(orp_dblchk);
  Serial.print("ORP Value:");
  Serial.print(ORP.get_last_received_reading());
  Serial.print(" (");
  Serial.print(orp_min);
  Serial.print(" - ");
  Serial.print(orp_max);
  Serial.println(" )");
  Serial.print("ORP DoubleCheckCounterRead: ");
  Serial.println(orp_chk_counter_read);
  Serial.print("pumpe_on=");
  Serial.println(pumpe_on);
  

     
          
                
      
 
   
  // ORP prüfen ob unterer Sollwert unterschritten und unterhalb oberstem Sollwert (chlor_hyst)
  
  if ((ORP.get_last_received_reading() < atol(orp_min) || chlor_hyst == 1) && pumpe_on == true ){


         
         
         //chlor_hyst = 1;

        
         orp_chk_counter ++;   // nur zählen wenn wir unterhalb sind
         write_chlorinator();
         
         if ( orp_chk_counter >= atol(orp_dblchk) ){
          orp_chk_counter = atol(orp_dblchk);
          chlor_hyst = 1;
         }


        
        // prüfen ob wir den oberen Sollwert erreicht haben, wenn nicht weiter chloren                                    
        if (ORP.get_last_received_reading() < atol(orp_max) && orp_chk_counter >= atol(orp_dblchk)) {  


            // beim ersten Aufruf :previousMillisChlorInterval = currentMillis; 

            ChlorInterval_counter ++;
            if ( ChlorInterval_counter == 1 ){
              previousMillisChlorInterval = currentMillis; 
            }
            
         


            



                                  
            Serial.println("ORP Level to low");
            call_pumpe_dosierung();
            timer_interval_delay = atol(pumpennachlaufzeit) * 1000 * 60; // Timer für die Pumpennachlaufzeit einschalten ( Pumpe läuft um diese Zeit nach
            
                      
    
            if (!cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
                Serial.println("connection chlorinator failed");
                return;
            }
            else{
                Serial.println("Switch on Chlorinator");
                Serial.println("######################");
                Serial.println("");      
                timer_interval_delay = atol(pumpennachlaufzeit) * 1000 * 60; // Timer für die Pumpennachlaufzeit einschalten ( Pumpe läuft um diese Zeit nach
                previousMillis = millis(); // reset millis für pumpennachlaufzeit
                client.print(String("GET ") + "/cm?cmnd=Power1%201" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
                client.stop();
                ThingSpeak.setField(String(chlorinatorID).toInt(), 1);
                orp_on = true;
                
            }

             // hier bauen wir eine Prüfung des Intervalles ein und schalten orp_chk_counter = 0; chlor_hyst = 0;
            // Sprich wie lange darf der Chlorinator maximal laufen

            if ( atol(ChlorInterval) > 0) {
              currentMillis = millis();
                if (currentMillis - previousMillisChlorInterval >= atol(ChlorInterval) * 1000 * 60) { 
                  previousMillisChlorInterval = currentMillis; 
                                      
                      orp_chk_counter = 0;
                      chlor_hyst = 0;
                      ChlorInterval_counter = 0;

                      
                      // Chlorinator aus
                      if (!cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
                        Serial.println("connection chlorinator failed");
                        return;
                      }
                      else{   
            
                      orp_chk_counter = 0;
                      ChlorInterval_counter = 0;
                      Serial.println("Switch off Chlorinator");
                      Serial.println("######################");
                      Serial.println("");
                    
                      client.print(String("GET ") + "/cm?cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
                      client.stop();
                      ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
                      orp_on = false;

                    }
                    // Chlorinator aus




                      
                                  
                }
           }
            // hier bauen wir eine Prüfung des Intervalles ein und schalten orp_chk_counter = 0; chlor_hyst = 0;
            // Sprich wie lange darf der Chlorinator maximal laufen
         
            

            
              
        } // prüfen ob wir den oberen Sollwert erreicht haben, wenn nicht weiter chloren
        
        else if (ORP.get_last_received_reading() > atol(orp_max)){   // oberster Sollwert erreicht!
           orp_chk_counter = 0;
           chlor_hyst = 0;
           ChlorInterval_counter = 0;
           
           
          // Chlorinator aus
          if (!cached_connect(hostname_chlorinator, 80, chlorinator_fail_time)) {
              Serial.println("connection chlorinator failed");
              return;
              }
          else{   
            
            orp_chk_counter = 0;
            ChlorInterval_counter = 0;
            Serial.println("Switch off Chlorinator");
            Serial.println("######################");
            Serial.println("");
            
            client.print(String("GET ") + "/cm?cmnd=Power1%200" + " HTTP/1.1\r\n" + "Host: " + hostname_chlorinator + "\r\n" + "Connection: close\r\n\r\n");
            client.stop();
            ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
            orp_on = false;

          }
          // Chlorinator aus

          
        } // prüfen ob wir den oberen Sollwert erreicht haben, wenn nicht weiter chloren



        

        
  }    

  // ORP prüfen ob unterer Sollwert unterschritten und unterhalb oberstem Sollwert (chlor_hyst)

  

  else{    // wir sind nicht unterhalb ORP_Min
    
    orp_chk_counter = 0;
    chlor_hyst = 0;
    ChlorInterval_counter = 0;

    Serial.println("");
    Serial.println("ORP OK");
    Serial.println("######################");
    Serial.println("");
    ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
    
    if ( orp_chk_counter_read != orp_chk_counter ){
    orp_chk_counter_read = orp_chk_counter;
    write_chlorinator();
    }
    
    
  }
}  // Ende Void check_orp


//          ORP Ende    ORP Ende    ORP Ende
