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
//          PHMinus Anfang        PHMinus Anfang


// Effektive Ringpuffer-Groesse aus ph_mean_window / polling_delay berechnen,
// Grenzen klemmen und Filterzustand zuruecksetzen.
void ph_filter_recalc_size(){
  long win_s = atol(ph_mean_window);
  if (win_s < 10) win_s = 10;
  long max_win_s = atol(check_phMinus_interval_delay) * 60L;
  if (max_win_s < 10) max_win_s = 10;
  if (win_s > max_win_s) win_s = max_win_s;

  long samples = (win_s * 1000L) / (long)polling_delay;
  if (samples < 5) samples = 5;
  if (samples > PH_BUF_MAX) samples = PH_BUF_MAX;

  ph_buf_size        = (uint16_t)samples;
  ph_buf_idx         = 0;
  ph_buf_count       = 0;
  ph_filtered        = 0.0;
  ph_filter_init_done = false;
  ph_spike_count     = 0;

  Serial.print("PH-Filter: window=");
  Serial.print(win_s);
  Serial.print("s, buf_size=");
  Serial.print(ph_buf_size);
  Serial.print(", spike_thr=");
  Serial.println(ph_spike_threshold);
}


// Neuen PH-Rohwert einspeisen: Plausi -> Spike-Check -> Ringpuffer -> Mittelwert
void ph_filter_update(float raw){
  // 1) Plausi
  if (raw <= 0.0 || raw >= 14.0) {
    ph_spike_count++;
    return;
  }

  // 2) Spike-Filter gegen aktuellen gleitenden Mittelwert (nur wenn Filter schon angelaufen)
  if (ph_filter_init_done) {
    float thr = atof(ph_spike_threshold);
    if (thr < 0.01) thr = 0.01;
    if (fabs(raw - ph_filtered) > thr) {
      ph_spike_count++;
      return;
    }
  }

  // 3) In Ringpuffer schreiben
  if (ph_buf_size == 0) return;
  ph_buf[ph_buf_idx] = raw;
  ph_buf_idx = (ph_buf_idx + 1) % ph_buf_size;
  if (ph_buf_count < ph_buf_size) ph_buf_count++;

  // 4) Mittelwert neu berechnen
  double sum = 0.0;
  for (uint16_t i = 0; i < ph_buf_count; i++) sum += ph_buf[i];
  ph_filtered = (float)(sum / (double)ph_buf_count);

  ph_filter_init_done = true;
}


void check_phMinus(){
  Serial.println("");
  Serial.println("######################");
  Serial.print("Check PH-Minus every: ");
  Serial.print(check_phMinus_interval_delay); 
  Serial.println(" min");
  Serial.print("PH raw: ");
  Serial.print(PH.get_last_received_reading());
  Serial.print("  PH filtered: ");
  Serial.print(ph_filtered, 2);
  Serial.print("  Setpoint: ");
  Serial.print(atof(phMinus));
  Serial.println("");
  Serial.print("PH-Filter buffer: ");
  Serial.print(ph_buf_count);
  Serial.print("/");
  Serial.print(ph_buf_size);
  Serial.print("  spikes: ");
  Serial.println(ph_spike_count);
  Serial.print("Temperature: ");
  Serial.println(RTD.get_last_received_reading());
  Serial.println("");
  Serial.print("DoubleCheckCounterRead: ");
  Serial.println(phminus_dblchk_counter_read);

  // Variante Y: Puffer muss zu 100% gefuellt sein, sonst Check komplett ueberspringen.
  // Counter wird dabei NICHT angetastet (weder ++ noch reset).
  if (ph_buf_size == 0 || ph_buf_count < ph_buf_size) {
    Serial.print("PH-Check skipped: buffer ");
    Serial.print(ph_buf_count);
    Serial.print("/");
    Serial.print(ph_buf_size);
    Serial.println(" (filter warm-up)");
    Serial.println("######################");
    Serial.println("");
    return;
  }
  
  
 // PHMinus Prüfung Anfang  -----------------------------------------------------------------------


    if (pumpe_on == true){
      Serial.println("Pumpe ist an");
    }
      else{
      Serial.println("Pumpe Aus");
      
    }

    // Test
    //if (PH.get_last_received_reading() < atof(phMinus)){      // Wenn wir über Sollwert sind dann phminus an 

    
    // Dosier-Bedingung: pH zu hoch UND Pumpe laeuft UND (Flow-Gate aus ODER Flow vorhanden)
    if (ph_filtered > atof(phMinus) && pumpe_on == true && (!check_flow || flow) ){
   
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
