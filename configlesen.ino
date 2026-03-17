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



void write_thing(){
        Serial.println("Write Thingspeak");
        File file = LittleFS.open(datei_thing, "w");
        if (!file) { Serial.println("LittleFS write error: thing"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&send_to_thingspeak), sizeof(send_to_thingspeak));
        file.write(reinterpret_cast<uint8_t*>(&thingspeak_delay), sizeof(thingspeak_delay));
        file.write(reinterpret_cast<uint8_t*>(&myChannelNumber), sizeof(myChannelNumber));
        file.write(reinterpret_cast<uint8_t*>(&myWriteAPIKey), sizeof(myWriteAPIKey));
        //file.write(reinterpret_cast<uint8_t*>(&myReadAPIKey), sizeof(myReadAPIKey));
        file.write(reinterpret_cast<uint8_t*>(&phminusID), sizeof(phminusID));
        file.write(reinterpret_cast<uint8_t*>(&flowID), sizeof(flowID));
        file.write(reinterpret_cast<uint8_t*>(&chlorID), sizeof(chlorID));
        file.write(reinterpret_cast<uint8_t*>(&pumpID), sizeof(pumpID));
        file.write(reinterpret_cast<uint8_t*>(&chlorinatorID), sizeof(chlorinatorID));
        // polling removed in v6.0 - always on

        file.close();
   
}



void read_thing(){
      
      File file = LittleFS.open(datei_thing, "r");
      if ( file ){
        Serial.println("Read Thingspeak");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&send_to_thingspeak), sizeof(send_to_thingspeak));
        file.read(reinterpret_cast<uint8_t*>(&thingspeak_delay), sizeof(thingspeak_delay));
        file.read(reinterpret_cast<uint8_t*>(&myChannelNumber), sizeof(myChannelNumber));
        file.read(reinterpret_cast<uint8_t*>(&myWriteAPIKey), sizeof(myWriteAPIKey));
        //file.read(reinterpret_cast<uint8_t*>(&myReadAPIKey), sizeof(myReadAPIKey));
        file.read(reinterpret_cast<uint8_t*>(&phminusID), sizeof(phminusID));
        file.read(reinterpret_cast<uint8_t*>(&flowID), sizeof(flowID));
        file.read(reinterpret_cast<uint8_t*>(&chlorID), sizeof(chlorID));
        file.read(reinterpret_cast<uint8_t*>(&pumpID), sizeof(pumpID));
        file.read(reinterpret_cast<uint8_t*>(&chlorinatorID), sizeof(chlorinatorID));
        // polling removed in v6.0 - always on

        file.close();            
      }
  
}



// v6.3: Login-PIN in LittleFS speichern (XOR-verschluesselt)
void write_lizenz(){
        File file = LittleFS.open(datei_lizenz, "w");
        if (!file) { Serial.println("LittleFS write error: lizenz"); return; }
        delay(100);
        Serial.println("Write Lizenz");
        size_t len = strlen(loginpassword);
        for (size_t i = 0; i < len; i++) loginpassword[i] ^= key;
        file.write(reinterpret_cast<uint8_t*>(&loginpassword), sizeof(loginpassword));
        for (size_t i = 0; i < len; i++) loginpassword[i] ^= key;  // zurueck-XOR
        file.close();
}

void read_lizenz(){
        File file = LittleFS.open(datei_lizenz, "r");
        if ( file ){
            delay(100);
            Serial.println("Read Lizenz");
            file.read(reinterpret_cast<uint8_t*>(&loginpassword), sizeof(loginpassword));
            for (size_t i = 0; i < strlen(loginpassword); i++) loginpassword[i] ^= key;
            file.close();
            Serial.print("  PIN geladen: ");
            Serial.println(loginpassword);
        }
}



void write_pool(){
        Serial.println("Write Pool");
        File file = LittleFS.open(datei_pool, "w");
        if (!file) { Serial.println("LittleFS write error: pool"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&orp_min), sizeof(orp_min));
        file.write(reinterpret_cast<uint8_t*>(&orp_max), sizeof(orp_max));
        file.write(reinterpret_cast<uint8_t*>(&phMinus), sizeof(phMinus));
        file.write(reinterpret_cast<uint8_t*>(&phPlus), sizeof(phPlus));
        //file.write(reinterpret_cast<uint8_t*>(&temp_min), sizeof(temp_min));
        //file.write(reinterpret_cast<uint8_t*>(&temp_max), sizeof(temp_max));
        file.write(reinterpret_cast<uint8_t*>(&only_one_pump_speed), sizeof(only_one_pump_speed));
        file.write(reinterpret_cast<uint8_t*>(&hostname_pumpe), sizeof(hostname_pumpe));
        file.write(reinterpret_cast<uint8_t*>(&pumpennachlaufzeit), sizeof(pumpennachlaufzeit));
        file.write(reinterpret_cast<uint8_t*>(&pumpe_aus), sizeof(pumpe_aus));
        file.write(reinterpret_cast<uint8_t*>(&pumpe_dosierung), sizeof(pumpe_dosierung));
        file.write(reinterpret_cast<uint8_t*>(&pumpe_hand), sizeof(pumpe_hand));
        file.write(reinterpret_cast<uint8_t*>(&reboot_delay), sizeof(reboot_delay));
        file.write(reinterpret_cast<uint8_t*>(&ph_fault), sizeof(ph_fault));
        file.write(reinterpret_cast<uint8_t*>(&orp_fault), sizeof(orp_fault));
        file.write(reinterpret_cast<uint8_t*>(&pumpe_temp), sizeof(pumpe_temp));
        file.write(reinterpret_cast<uint8_t*>(&winter_temp), sizeof(winter_temp));
        file.write(reinterpret_cast<uint8_t*>(&winter_shaft_temp), sizeof(winter_shaft_temp));
        file.write(reinterpret_cast<uint8_t*>(&winter_modus), sizeof(winter_modus));

        
        file.close();
   
}


void read_pool(){
  File file = LittleFS.open(datei_pool, "r");
  if ( file ) {
        Serial.println("Read Pool");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&orp_min), sizeof(orp_min));
        file.read(reinterpret_cast<uint8_t*>(&orp_max), sizeof(orp_max));
        file.read(reinterpret_cast<uint8_t*>(&phMinus), sizeof(phMinus));
        file.read(reinterpret_cast<uint8_t*>(&phPlus), sizeof(phPlus));
        //file.read(reinterpret_cast<uint8_t*>(&temp_min), sizeof(temp_min));
        //file.read(reinterpret_cast<uint8_t*>(&temp_max), sizeof(temp_max));
        file.read(reinterpret_cast<uint8_t*>(&only_one_pump_speed), sizeof(only_one_pump_speed));
        file.read(reinterpret_cast<uint8_t*>(&hostname_pumpe), sizeof(hostname_pumpe));
        file.read(reinterpret_cast<uint8_t*>(&pumpennachlaufzeit), sizeof(pumpennachlaufzeit));
        file.read(reinterpret_cast<uint8_t*>(&pumpe_aus), sizeof(pumpe_aus));
        file.read(reinterpret_cast<uint8_t*>(&pumpe_dosierung), sizeof(pumpe_dosierung));
        file.read(reinterpret_cast<uint8_t*>(&pumpe_hand), sizeof(pumpe_hand));
        file.read(reinterpret_cast<uint8_t*>(&reboot_delay), sizeof(reboot_delay));
        file.read(reinterpret_cast<uint8_t*>(&ph_fault), sizeof(ph_fault));
        file.read(reinterpret_cast<uint8_t*>(&orp_fault), sizeof(orp_fault));
        file.read(reinterpret_cast<uint8_t*>(&pumpe_temp), sizeof(pumpe_temp));
        file.read(reinterpret_cast<uint8_t*>(&winter_temp), sizeof(winter_temp));
        file.read(reinterpret_cast<uint8_t*>(&winter_shaft_temp), sizeof(winter_shaft_temp));
        file.read(reinterpret_cast<uint8_t*>(&winter_modus), sizeof(winter_modus));
        
        file.close();
  }    
               
}


void write_flow(){
        Serial.println("Write Flow");
        File file = LittleFS.open(datei_flow, "w");
        if (!file) { Serial.println("LittleFS write error: flow"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&check_flow), sizeof(check_flow));
        file.write(reinterpret_cast<uint8_t*>(&flowcontrol_delay), sizeof(flowcontrol_delay));
        file.write(reinterpret_cast<uint8_t*>(&hostname_flowcontrol), sizeof(hostname_flowcontrol));
        file.write(reinterpret_cast<uint8_t*>(&password_flowcontrol), sizeof(password_flowcontrol));
        file.close();
   
}


void read_flow(){
  File file = LittleFS.open(datei_flow, "r");
  if ( file ){
        Serial.println("Read Flow");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&check_flow), sizeof(check_flow));
        file.read(reinterpret_cast<uint8_t*>(&flowcontrol_delay), sizeof(flowcontrol_delay));
        file.read(reinterpret_cast<uint8_t*>(&hostname_flowcontrol), sizeof(hostname_flowcontrol));
        file.read(reinterpret_cast<uint8_t*>(&password_flowcontrol), sizeof(password_flowcontrol));
        file.close();
                
  }
                      
}

void write_chlorinator(){
 
        Serial.println("Write Chlorinator");
        File file = LittleFS.open(datei_chlorinator, "w");
        if (!file) { Serial.println("LittleFS write error: chlorinator"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&check_chlorinator), sizeof(check_chlorinator));
        file.write(reinterpret_cast<uint8_t*>(&check_orp_interval_delay), sizeof(check_orp_interval_delay));
        file.write(reinterpret_cast<uint8_t*>(&hostname_chlorinator), sizeof(hostname_chlorinator));
        file.write(reinterpret_cast<uint8_t*>(&orp_dblchk), sizeof(orp_dblchk));
        file.write(reinterpret_cast<uint8_t*>(&orp_chk_counter), sizeof(orp_chk_counter));
        file.write(reinterpret_cast<uint8_t*>(&ChlorInterval), sizeof(ChlorInterval));
        file.close();
   
}


void read_chlorinator(){
  File file = LittleFS.open(datei_chlorinator, "r");
  if ( file ) {
        delay(100);
        Serial.println("Read Chlorinator");
        file.read(reinterpret_cast<uint8_t*>(&check_chlorinator), sizeof(check_chlorinator));
        file.read(reinterpret_cast<uint8_t*>(&check_orp_interval_delay), sizeof(check_orp_interval_delay));
        file.read(reinterpret_cast<uint8_t*>(&hostname_chlorinator), sizeof(hostname_chlorinator));
        file.read(reinterpret_cast<uint8_t*>(&orp_dblchk), sizeof(orp_dblchk));
        file.read(reinterpret_cast<uint8_t*>(&orp_chk_counter), sizeof(orp_chk_counter));
        file.read(reinterpret_cast<uint8_t*>(&ChlorInterval), sizeof(ChlorInterval));
        file.close();
        //strcpy(check_orp_interval_delay_std, check_orp_interval_delay);
        orp_chk_counter_read = orp_chk_counter;
       

  }                  
}





void write_phminuspmp(){
    
        Serial.println("Write PHMinusPmP");
        File file = LittleFS.open(datei_phminuspmp, "w");
        if (!file) { Serial.println("LittleFS write error: phminuspmp"); return; }
        delay(100);


        file.write(reinterpret_cast<uint8_t*>(&check_phminus), sizeof(check_phminus));
        file.write(reinterpret_cast<uint8_t*>(&check_phMinus_interval_delay), sizeof(check_phMinus_interval_delay));
        file.write(reinterpret_cast<uint8_t*>(&hostname_phminus), sizeof(hostname_phminus));
        file.write(reinterpret_cast<uint8_t*>(&phminus_fuellstand), sizeof(phminus_fuellstand));
        file.write(reinterpret_cast<uint8_t*>(&phminus_dosiermenge), sizeof(phminus_dosiermenge));
        file.write(reinterpret_cast<uint8_t*>(&phminus_dblchk), sizeof(phminus_dblchk));
        file.write(reinterpret_cast<uint8_t*>(&phminus_dblchk_counter), sizeof(phminus_dblchk_counter));
        file.write(reinterpret_cast<uint8_t*>(&password_phminus), sizeof(password_phminus));

        file.close();
   
}


void read_phminuspmp(){
  File file = LittleFS.open(datei_phminuspmp, "r");
  if ( file ) {
        delay(100);
        Serial.println("Read PHMinusPmP");
        file.read(reinterpret_cast<uint8_t*>(&check_phminus), sizeof(check_phminus));
        file.read(reinterpret_cast<uint8_t*>(&check_phMinus_interval_delay), sizeof(check_phMinus_interval_delay));
        file.read(reinterpret_cast<uint8_t*>(&hostname_phminus), sizeof(hostname_phminus));
        file.read(reinterpret_cast<uint8_t*>(&phminus_fuellstand), sizeof(phminus_fuellstand));
        file.read(reinterpret_cast<uint8_t*>(&phminus_dosiermenge), sizeof(phminus_dosiermenge));
        file.read(reinterpret_cast<uint8_t*>(&phminus_dblchk), sizeof(phminus_dblchk));
        file.read(reinterpret_cast<uint8_t*>(&phminus_dblchk_counter), sizeof(phminus_dblchk_counter));
        file.read(reinterpret_cast<uint8_t*>(&password_phminus), sizeof(password_phminus));
        
        file.close();
        //strcpy(check_phMinus_interval_delay_std, check_phMinus_interval_delay);
        phminus_dblchk_counter_read = phminus_dblchk_counter;
  }                           
}

void write_heater(){
  
        Serial.println("Write Heater");
        File file = LittleFS.open(datei_heater, "w");
        if (!file) { Serial.println("LittleFS write error: heater"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&check_heizung), sizeof(check_heizung));
        file.write(reinterpret_cast<uint8_t*>(&check_fan), sizeof(check_fan));
        file.write(reinterpret_cast<uint8_t*>(&temp_min), sizeof(temp_min));
        file.write(reinterpret_cast<uint8_t*>(&temp_max), sizeof(temp_max));
        file.write(reinterpret_cast<uint8_t*>(&humidity_max), sizeof(humidity_max));
        file.write(reinterpret_cast<uint8_t*>(&humidity_min), sizeof(humidity_min));
        file.write(reinterpret_cast<uint8_t*>(&check_temp_interval_delay), sizeof(check_temp_interval_delay));
        file.write(reinterpret_cast<uint8_t*>(&hostname_heater), sizeof(hostname_heater));
        file.write(reinterpret_cast<uint8_t*>(&password_heater), sizeof(password_heater));
        file.write(reinterpret_cast<uint8_t*>(&hostname_fan), sizeof(hostname_fan));
        file.write(reinterpret_cast<uint8_t*>(&password_fan), sizeof(password_fan));
        file.close();
   
}

void read_heater(){
  File file = LittleFS.open(datei_heater, "r");
  if ( file ) {
        Serial.println("Read Heater");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&check_heizung), sizeof(check_heizung));
        file.read(reinterpret_cast<uint8_t*>(&check_fan), sizeof(check_fan));
        file.read(reinterpret_cast<uint8_t*>(&temp_min), sizeof(temp_min));
        file.read(reinterpret_cast<uint8_t*>(&temp_max), sizeof(temp_max));
        file.read(reinterpret_cast<uint8_t*>(&humidity_max), sizeof(humidity_max));
        file.read(reinterpret_cast<uint8_t*>(&humidity_min), sizeof(humidity_min));
        file.read(reinterpret_cast<uint8_t*>(&check_temp_interval_delay), sizeof(check_temp_interval_delay));
        file.read(reinterpret_cast<uint8_t*>(&hostname_heater), sizeof(hostname_heater));
        file.read(reinterpret_cast<uint8_t*>(&password_heater), sizeof(password_heater));
        file.read(reinterpret_cast<uint8_t*>(&hostname_fan), sizeof(hostname_fan));
        file.read(reinterpret_cast<uint8_t*>(&password_fan), sizeof(password_fan));

        file.close();
        
  }                            
}







// v6.1: AM2315C Offset-Kalibrierung
void write_dht_cal(){
        Serial.println("Write DHT Cal");
        File file = LittleFS.open(datei_dht_cal, "w");
        if (!file) { Serial.println("LittleFS write error: dht_cal"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&dht_temp_offset), sizeof(dht_temp_offset));
        file.write(reinterpret_cast<uint8_t*>(&dht_hum_offset), sizeof(dht_hum_offset));
        file.close();
}

void read_dht_cal(){
  File file = LittleFS.open(datei_dht_cal, "r");
  if ( file ) {
        Serial.println("Read DHT Cal");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&dht_temp_offset), sizeof(dht_temp_offset));
        file.read(reinterpret_cast<uint8_t*>(&dht_hum_offset), sizeof(dht_hum_offset));
        file.close();
        Serial.print("  Temp-Offset: ");
        Serial.print(dht_temp_offset);
        Serial.print("  Hum-Offset: ");
        Serial.println(dht_hum_offset);
  }
}


void write_timer(){
        Serial.println("Write Timer");
        File file = LittleFS.open(datei_timer, "w");
        if (!file) { Serial.println("LittleFS write error: timer"); return; }
        delay(100);
        file.write(reinterpret_cast<uint8_t*>(&summer), sizeof(summer));
        file.write(reinterpret_cast<uint8_t*>(&timers), sizeof(timers));
        file.close();
}


void read_timer(){
  File file = LittleFS.open(datei_timer, "r");
  if ( file ) {
        Serial.println("Read Timer");
        delay(100);
        file.read(reinterpret_cast<uint8_t*>(&summer), sizeof(summer));
        file.read(reinterpret_cast<uint8_t*>(&timers), sizeof(timers));
        file.close();
  }

  // Timer-Zeiten in Minuten umrechnen
  delay(100);
  for (int i = 0; i < TIMER_COUNT; i++) {
    timer_minuten[i] = String(timers[i].zeit).substring(0,2).toInt() * 60
                     + String(timers[i].zeit).substring(3,5).toInt();
  }
}
