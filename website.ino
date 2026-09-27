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



//############
void handleForm() {


  if ( server.hasArg("check_pump_on")) {

    if (login) {
      int z = strcmp(server.arg(0).c_str(), "OFF");

      if ( z == 0 ) {
        // v7.0: Manual-Mode beenden. Erst Flag loeschen, dann ausschalten,
        // sonst wuerde der Manual-Guard das call_pumpe_aus blockieren.
        pump_manual_on = false;
        pump_manual_start_ms = 0;
        timer_interval_delay = standard_timer_interval_delay;
        call_pumpe_aus();
        check_pump_on = "";
        check_pump_off = "checked";
        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"pool.htm\" class=\"button3\">Resetted to default!</a>");
        server.send(200, "text/html", message);
      }
      else {
        // v7.0: Manual ON friert Pumpenstufe auf pumpe_hand ein, Auto-Fallback nach 24h.
        // Chlor-/PH-Logik laeuft normal weiter, nur Pumpenstufenwechsel sind blockiert.
        pump_manual_on = true;
        pump_manual_start_ms = millis();
        timer_interval_delay = 86400000;
        check_pump_on = "checked";
        check_pump_off = "";
        call_pumpe_hand();
        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"pool.htm\" class=\"button3\">Manual ON - auto-fallback in 24h</a>");
        server.send(200, "text/html", message);
      }
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }


  }

  // ## PH Low Cal
  if ( server.hasArg("calPHlow")) {

    polling = false;
    send_to_thingspeak = false;
    delay(500);

    String cmd = "ph:cal,low," + server.arg(0);

    int z = strcmp(server.arg(0).c_str(), "clear");
    if ( z == 0 ) {
      cmd = "ph:cal,clear";
    }

    receive_command2(cmd);

    if ( login ) {
      if (!process_coms(cmd)) {
        process_command(cmd, device_list, device_list_len, default_board);
      }
      delay(500);
      polling = true;
      send_to_thingspeak = true;
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Calibrated.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }

  // ## PH MID Cal
  if ( server.hasArg("calPHmid")) {

    polling = false;
    send_to_thingspeak = false;
    delay(500);

    String cmd = "PH:CAL,MID," + server.arg(0);
    int z = strcmp(server.arg(0).c_str(), "clear");
    if ( z == 0 ) {
      cmd = "ph:cal,clear";
    }

    receive_command2(cmd);

    if ( login ) {
      if (!process_coms(cmd)) {
        process_command(cmd, device_list, device_list_len, default_board);
      }
      delay(500);
      polling = true;
      send_to_thingspeak = true;
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Calibrated.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }

  // ## PH HIGH Cal
  if ( server.hasArg("calPHhigh")) {

    polling = false;
    send_to_thingspeak = false;
    delay(500);

    String cmd = "PH:CAL,HIGH," + server.arg(0);
    int z = strcmp(server.arg(0).c_str(), "clear");
    if ( z == 0 ) {
      cmd = "ph:cal,clear";
    }

    receive_command2(cmd);

    if ( login ) {
      if (!process_coms(cmd)) {
        process_command(cmd, device_list, device_list_len, default_board);
      }
      delay(500);
      polling = true;
      send_to_thingspeak = true;
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Calibrated.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }

  // ## ORP  Cal
  if ( server.hasArg("calORP")) {

    polling = false;
    send_to_thingspeak = false;
    delay(500);

    String cmd = "ORP:CAL," + server.arg(0);

    receive_command2(cmd);

    if ( login ) {
      if (!process_coms(cmd)) {
        process_command(cmd, device_list, device_list_len, default_board);
      }
      delay(500);
      polling = true;
      send_to_thingspeak = true;
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Calibrated.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }

  // ## RTD Cal
  if ( server.hasArg("calTemp")) {

    polling = false;
    send_to_thingspeak = false;
    delay(500);

    String cmd = "RTD:CAL," + server.arg(0);

    receive_command2(cmd);

    if ( login ) {
      if (!process_coms(cmd)) {
        process_command(cmd, device_list, device_list_len, default_board);
      }
      delay(500);
      polling = true;
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Calibrated.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }

  // ## EZO Maintenance: I2C Scan
  if ( server.hasArg("maint_scan")) {
    if ( login ) {
      maint_i2c_scan();
      String message;
      addTop(message);
      message += F("<br><h3><center><a href=\"maintenance.htm\" class=\"button3\">Scan done.. back to page</a></h3>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }
    return;
  }

  // ## EZO Maintenance: UART -> I2C Switch
  if ( server.hasArg("maint_switch_slot") && server.hasArg("maint_confirm")) {
    if ( login ) {
      int slot_index = server.arg("maint_switch_slot").toInt();

      String log;
      bool ok = maint_switch_to_i2c(slot_index, log);

      String message;
      addTop(message);
      message += F("<h2>EZO Switch</h2>");
      message += log;
      if (ok) {
        message += F("<br><center><b>ESP is rebooting now...</b></center>");
        message += F("<br><center><small>wait ~10s, then open <a href=\"maintenance.htm\">maintenance.htm</a> again and run 'Scan I2C Bus' to verify.</small></center>");
      } else {
        message += F("<br><center><a href=\"maintenance.htm\" class=\"button3\">back to maintenance</a></center>");
      }
      addBottom(message);
      server.send(200, "text/html", message);

      if (ok) {
        delay(5000);
        ESP.restart();
      }
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }
    return;
  }


  // ## AM2315C Offset Kalibrierung
  if ( server.hasArg("dht_temp_offset")) {

    if ( login ) {
      dht_temp_offset = server.arg("dht_temp_offset").toFloat();
      dht_hum_offset = server.arg("dht_hum_offset").toFloat();
      write_dht_cal();
      read_dht_cal();

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"calibration.htm\" class=\"button3\">Offset saved.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }


  // -------------------------------------  cmd End


  // ---------------------------------------Thingspeak  Abfrage Anfang

  if (server.hasArg("send_to_thingspeak")) {
    Serial.println(" ----------------- Server.has Arg  Thingspeak .-........!");
    delay(100);
    Serial.println(server.arg(0));

    if ( login ) {

      int z = strcmp(server.arg(0).c_str(), "Yes");

      if ( z == 0 ) {
        send_to_thingspeak = true;
      }
      else {
        send_to_thingspeak = false;
      }

      strcpy(thingspeak_delay, server.arg(1).c_str());
      strcpy(myChannelNumber, server.arg(2).c_str());
      strcpy(myWriteAPIKey, server.arg(3).c_str());
      strcpy(phminusID, server.arg(4).c_str());
      strcpy(flowID, server.arg(5).c_str());
      strcpy(chlorID, server.arg(6).c_str());
      strcpy(pumpID, server.arg(7).c_str());
      strcpy(chlorinatorID, server.arg(8).c_str());

      // v6.0: polling immer an, polling Radio-Button ignoriert

      server.send(200, "text/html", " <body style=\"background-color:black;\"> ​ <br><h1><center><a href=\"/\" style=\"color:#FF0000;\" >Safed... we reboot now...</a>");
      delay(100);
      write_thing();
      delay(100);
      read_thing();
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Rebooting...</a>");
      server.send(200, "text/html", message);
      delay(500);
      ESP.restart();

    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first</a>");
      server.send(200, "text/html", message);
    }

  }


  // --------------------------------------------Thingspeak Abfrage Ende



  // ---------------------------------------------Pool Abfrage Anfang



  else if (server.hasArg("orp_min" )) {
    Serial.println("");

    delay(100);
    if ( login ) {
      strcpy(orp_min, server.arg(0).c_str());
      strcpy(orp_max, server.arg(1).c_str());
      strcpy(phMinus, server.arg(2).c_str());
      strcpy(phPlus, server.arg(3).c_str());

      int z = strcmp(server.arg(4).c_str(), "Yes");
      if ( z == 0 ) {
        only_one_pump_speed = true;
      }
      else {
        only_one_pump_speed = false;
      }

      strcpy(hostname_pumpe, server.arg(5).c_str());
      strcpy(pumpennachlaufzeit, server.arg(6).c_str());
      strcpy(pumpe_aus, server.arg(7).c_str());
      strcpy(pumpe_dosierung, server.arg(8).c_str());
      strcpy(pumpe_hand, server.arg(9).c_str());

      z = strcmp(server.arg(10).c_str(), "Yes");
      if ( z == 0 ) {
        winter_modus = true;
      }
      else {
        winter_modus = false;
      }

      strcpy(pumpe_temp, server.arg(11).c_str());
      strcpy(winter_temp, server.arg(12).c_str());
      strcpy(winter_shaft_temp, server.arg(13).c_str());

      strcpy(reboot_delay, server.arg(14).c_str());
      strcpy(ph_fault, server.arg(15).c_str());
      strcpy(orp_fault, server.arg(16).c_str());

      delay(100);
      write_pool();
      delay(100);
      read_pool();
      pumpe_fail_time = 0;  // v6.3: Fail-Cache zuruecksetzen nach Hostname-Aenderung
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"pool.htm\"  class=\"button3\">Safed.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }

  }


  // ---------------------------------------------Pool Abfrage Ende

  // ---------------------------------------------Flowcontrol Abfrage Anfang

  else if (server.hasArg("check_flow" )) {

    delay(100);
    if ( login ) {
      // Namens-basierter Zugriff, robust gegen Feld-Reihenfolge
      check_flow = (server.arg("check_flow") == "Yes");
      flow_show  = (server.arg("flow_show")  == "Yes");
      // Regel: Dosier-Gate an -> Dashboard-Anzeige automatisch auch an
      if (check_flow) flow_show = true;
      if (server.hasArg("flowcontrol_delay"))   strcpy(flowcontrol_delay,   server.arg("flowcontrol_delay").c_str());
      if (server.hasArg("hostname_flowcontrol")) strcpy(hostname_flowcontrol, server.arg("hostname_flowcontrol").c_str());
      if (server.hasArg("password_flowcontrol")) strcpy(password_flowcontrol, server.arg("password_flowcontrol").c_str());

      delay(100);
      write_flow();
      delay(100);
      read_flow();

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"flow.htm\"  class=\"button3\">Safed.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }


  }

  // ---------------------------------------------Flowcontrol Abfrage Ende


  // ---------------------------------------------Chlorinator Abfrage Anfang

  else if (server.hasArg("check_chlorinator" )) {
    Serial.println(" ----------------- Server.has Arg  Chlorinator-........!");
    delay(100);
    if ( login ) {
      check_chlorinator = (server.arg("check_chlorinator") == "Yes");
      if (check_chlorinator) {
        if (server.hasArg("check_orp_interval_delay")) strcpy(check_orp_interval_delay, server.arg("check_orp_interval_delay").c_str());
        if (server.hasArg("hostname_chlorinator"))     strcpy(hostname_chlorinator,     server.arg("hostname_chlorinator").c_str());
        if (server.hasArg("orp_dblchk"))               strcpy(orp_dblchk,               server.arg("orp_dblchk").c_str());
        if (server.hasArg("ChlorInterval"))            strcpy(ChlorInterval,            server.arg("ChlorInterval").c_str());
        // v7.0
        if (server.hasArg("chlor_distribute_min"))     strcpy(chlor_distribute_min,     server.arg("chlor_distribute_min").c_str());
        if (server.hasArg("chlor_daily_budget"))       strcpy(chlor_daily_budget,       server.arg("chlor_daily_budget").c_str());
        chlor_safety_stop = (server.arg("chlor_safety_stop") == "Yes");
        // v7.2
        if (server.hasArg("chlor_warmup_min"))         strcpy(chlor_warmup_min,         server.arg("chlor_warmup_min").c_str());
      }
      else {
        ThingSpeak.setField(String(chlorinatorID).toInt(), 0);
      }

      delay(100);
      write_chlorinator();
      delay(100);
      read_chlorinator();
      // v7.0: Phasen-Reset bei jedem Save - Phase laeuft sauber neu an
      // v7.1: ueber chlor_phase_set() damit auch chlor_phase_start_epoch korrekt gesetzt
      // und die Phase persistiert wird (sonst wuerde der Resume nach naechstem Reboot
      // die alte Phase wieder einlesen).
      orp_chk_counter      = 0;
      chlor_last_tick_ms   = 0;
      timer_interval_delay = standard_timer_interval_delay;
      chlor_phase_set(CHLOR_PHASE_OBSERVE);

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"chlorinator.htm\" class=\"button3\">Safed.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }

      alles_aus();
  }


  // ---------------------------------------------Chlorinator Abfrage Ende


  // ---------------------------------------------Chlorinator Status-Reset (v7.1)
  else if (server.hasArg("chlor_status_reset")) {
    Serial.println(" ----------------- Server.has Arg  chlor_status_reset .-........!");
    if ( login ) {
      // Hardware sicherheitshalber abschalten, falls wir aus DOSE/DISTRIBUTE kommen
      if (chlor_phase == CHLOR_PHASE_DOSE || chlor_phase == CHLOR_PHASE_DISTRIBUTE) {
        chlorinator_off();
      }
      orp_chk_counter         = 0;
      orp_chk_counter_read    = 0;
      chlor_today_count       = 0;
      chlor_last_start_minute = -1;
      chlor_last_start_epoch  = 0;
      chlor_last_tick_ms      = 0;
      timer_interval_delay    = standard_timer_interval_delay;
      // chlor_phase_set persistiert phase + start_epoch + den oben gesetzten Rest in einem Rutsch
      chlor_phase_set(CHLOR_PHASE_OBSERVE);

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"chlorinator.htm\" class=\"button3\">Chlor-Status zur&uuml;ckgesetzt</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\" class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }
  }
  // ---------------------------------------------Chlorinator Status-Reset Ende



  // -------------------------------------------   PH-Minus Abfrage Anfang
  else if (server.hasArg("check_phminus" ) ) {
    Serial.println(" ----------------- Server.has Arg  phminus .-........!");
    delay(100);
    if ( login ) {
      int z = strcmp(server.arg(0).c_str(), "Yes");
      if ( z == 0 ) {
        check_phminus = true;
      }
      else {
        check_phminus = false;
      }
      strcpy(check_phMinus_interval_delay, server.arg(1).c_str());
      strcpy(hostname_phminus, server.arg(2).c_str());
      strcpy(phminus_fuellstand, server.arg(3).c_str());
      strcpy(phminus_dosiermenge, server.arg(4).c_str());
      strcpy(phminus_dblchk, server.arg(5).c_str());
      strcpy(password_phminus, server.arg(6).c_str());

      // PH-Filter: Fensterlaenge (Sekunden) mit Validierung
      if (server.hasArg("ph_mean_window")) {
        long win_s = server.arg("ph_mean_window").toInt();
        long max_win_s = atol(check_phMinus_interval_delay) * 60L;
        if (win_s < 10) win_s = 10;
        if (max_win_s < 10) max_win_s = 10;
        if (win_s > max_win_s) win_s = max_win_s;   // Fenster <= MixTime*60
        snprintf(ph_mean_window, sizeof(ph_mean_window), "%ld", win_s);
      }
      // PH-Filter: Spike-Schwelle (pH) mit Plausi
      if (server.hasArg("ph_spike_threshold")) {
        float thr = server.arg("ph_spike_threshold").toFloat();
        if (thr < 0.05) thr = 0.05;
        if (thr > 2.0)  thr = 2.0;
        snprintf(ph_spike_threshold, sizeof(ph_spike_threshold), "%.2f", thr);
      }

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"phminus.htm\" class=\"button3\">Safed.. back to page</a>");
      server.send(200, "text/html", message);

      delay(100);
      write_phminuspmp();
      delay(100);
      read_phminuspmp();
      phminus_dblchk_counter = 0;
      write_phminuspmp();
      // Filter-Ringpuffer anhand der (ggf. geaenderten) Fensterlaenge neu dimensionieren
      ph_filter_recalc_size();

      ThingSpeak.setField(String(phminusID).toInt(), phminus_fuellstand);

    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }

    alles_aus();


  }
  // -------------------------------------------   PH-Minus Abfrage Ende

  // -------------------------------------------   Heater Abfrage Anfang
  else if (server.hasArg("check_heizung" ) ) {
    Serial.println("");
    Serial.println(" ----------------- Server.has Arg  HEIZUNG .-........!");
    Serial.println("");
    delay(100);

    if ( login ){

          int z = strcmp(server.arg(0).c_str(),"Yes");
          if ( z == 0 ){
            check_heizung = true;
          }
          else {
            check_heizung = false;
          }
          int zz = strcmp(server.arg(1).c_str(),"Yes");
          if ( zz == 0 ){
            check_fan = true;
          }
          else {
            check_fan = false;
          }

          strcpy(temp_min, server.arg(2).c_str());
          strcpy(temp_max, server.arg(3).c_str());
          strcpy(humidity_max, server.arg(4).c_str());
          strcpy(humidity_min, server.arg(5).c_str());
          strcpy(check_temp_interval_delay, server.arg(6).c_str());
          strcpy(hostname_heater, server.arg(7).c_str());
          strcpy(password_heater, server.arg(8).c_str());
          strcpy(hostname_fan, server.arg(9).c_str());
          strcpy(password_fan, server.arg(10).c_str());

          delay(100);
          write_heater();
          delay(100);
          read_heater();

          String message;
          addTop(message);
          message += F("<br><h1><center><a href=\"heater.htm\"  class=\"button3\">Safed.. back to page</a>");
          server.send(200, "text/html", message);
    }
    else{
          String message;
          addTop(message);
          message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
          server.send(200, "text/html", message);
    }


  }
  // -------------------------------------------   Heater Abfrage Ende


  // -------------------------------------------   Alive Abfrage Anfang
  else if (server.hasArg("check_alive")) {
    Serial.println(" ----------------- Server.has Arg  Alive -.........!");
    delay(100);
    if ( login ) {
      check_alive = (server.arg("check_alive") == "Yes");
      if (server.hasArg("alive_interval")) strcpy(alive_interval, server.arg("alive_interval").c_str());
      if (server.hasArg("hostname_alive")) strcpy(hostname_alive, server.arg("hostname_alive").c_str());

      delay(100);
      write_alive();
      delay(100);
      read_alive();
      alive_fail_time = 0;          // Failure-Cache zuruecksetzen
      previousMillis_alive = 0;     // sofort senden beim naechsten Loop

      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"alive.htm\"  class=\"button3\">Safed.. back to page</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }
  }
  // -------------------------------------------   Alive Abfrage Ende


  // -------------------------------------------   WiFi Abfrage Anfang (v6.3)
  else if (server.hasArg("wifi_ssid" ) ) {
    Serial.println(" --- Server.has Arg WiFi ---");
    delay(100);

    if ( login ) {
      if (server.hasArg("copyright")) {
        strcpy(wifi_ssid, server.arg("wifi_ssid").c_str());
        strcpy(wifi_pass, server.arg("wifi_pass").c_str());
        if (server.hasArg("hostname")) {
          strcpy(hostname, server.arg("hostname").c_str());
        }

        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"/\"  class=\"button3\">WiFi gespeichert - Rebooting...</a>");
        server.send(200, "text/html", message);
        server.client().flush();

        delay(100);
        write_wifi();
        delay(500);
        ESP.restart();
      }
      else {
        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"wifi.htm\"  class=\"button3\">Copyright muss akzeptiert werden!</a>");
        server.send(200, "text/html", message);
      }
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }
  }
  // -------------------------------------------   WiFi Abfrage Ende


  // -------------------------------------------   Passwort Abfrage Anfang (v6.3)
  else if (server.hasArg("loginpassword" ) ) {
    Serial.println(" --- Server.has Arg Loginpassword ---");
    delay(100);

    if (server.arg("loginpassword") == server.arg("loginpassword2")) {
      if ( login ) {
        Serial.println(server.arg("loginpassword"));
        strcpy(loginpassword, server.arg("loginpassword").c_str());

        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"/\"  class=\"button3\">PIN gespeichert - Rebooting...</a>");
        server.send(200, "text/html", message);
        server.client().flush();

        delay(100);
        write_lizenz();
        delay(100);
        read_lizenz();
        delay(500);
        ESP.restart();
      }
      else {
        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
        server.send(200, "text/html", message);
      }
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"pw.htm\"  class=\"button3\">PIN stimmt nicht ueberein!</a>");
      server.send(200, "text/html", message);
    }
  }
  // -------------------------------------------   Passwort Abfrage Ende


  // -------------------------------------------   Timer Abfrage Anfang (v6.0 Tasmota-Style)

  else if (server.hasArg("utcOffsetInSeconds" ) ) {
    Serial.println("");
    Serial.println(" --- Server.has Arg Timer (Tasmota-Style) ---");
    Serial.println("");
    delay(100);

    // Sommerzeit
    int z = strcmp(server.arg("utcOffsetInSeconds").c_str(), "Yes");
    if ( z == 0 ) {
      summer = true;
      utcOffsetInSeconds = 7200;
    }
    else {
      summer = false;
      utcOffsetInSeconds = 3600;
    }
    timeClient.setTimeOffset(utcOffsetInSeconds);

    // 24 Timer auslesen
    // Checkboxen: nicht angehakte werden NICHT gesendet, daher erstmal alle auf false
    for (int i = 0; i < TIMER_COUNT; i++) {
      timers[i].aktiv = false;
    }
    for (int i = 0; i < TIMER_COUNT; i++) {
      String idx = String(i);
      String argZeit = "t" + idx + "z";
      String argSpeed = "t" + idx + "s";
      String argAktiv = "t" + idx + "a";

      if (server.hasArg(argZeit)) {
        strncpy(timers[i].zeit, server.arg(argZeit).c_str(), 5);
        timers[i].zeit[5] = '\0';
      }
      if (server.hasArg(argSpeed)) {
        timers[i].speed = server.arg(argSpeed).toInt();
        if (timers[i].speed < 1) timers[i].speed = 1;
        if (timers[i].speed > 4) timers[i].speed = 4;
      }
      if (server.hasArg(argAktiv)) {
        timers[i].aktiv = true;
      }
    }

    if ( login ) {
      delay(100);
      write_timer();
      delay(100);
      read_timer();
      pumpe_fail_time = 0;  // Fail-Cache loeschen bei neuen Timer-Einstellungen
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"timer.htm\"  class=\"button3\">Gespeichert.. zurueck</a>");
      server.send(200, "text/html", message);
    }
    else {
      String message;
      addTop(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login first!</a>");
      server.send(200, "text/html", message);
    }

  }
  // -------------------------------------------   Timer Abfrage Ende



  //   LOGIN - PIN Abfrage

  if ( server.hasArg("login" )) {

    if ( String(loginpassword) == server.arg(0)) {

      login = true;
      String message;
      addTop2(message);
      message += F("<br><h1><center><a href=\"/\"  class=\"button3\">Login OK!</a>");
      server.send(200, "text/html", message);
      handleRoot();

    }

  }



  if ( !login ) {
    if (server.arg("login") != String(loginpassword)) {

      String message;
      addTop2(message);
      message += F("<center><br><br><h1><a href=\"/\"  class=\"button3\">Wrong! Try again</a></h1></center></h1>");
      server.send(200, "text/html", message);

      login = false;
      handleRoot();
    }
  }


}




// v6.3: handleRoot - AP-Modus: WiFi-Setup, sonst Login/Dashboard
void handleRoot() {

    // v6.3: AP-Modus -> WiFi-Setup Captive Portal
    if ( ap_mode ) {
      String message;
      message =  F("<!DOCTYPE html>\n"
                   "<html lang='en'>\n"
                   "<head>\n"
                   "<title>RWS Pool-Kit - WiFi Setup</title>\n"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
                   "<meta name=\"viewport\" content=\"width=device-width\">\n"
                   "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
                   "</head>\n");
      message += F("<body>\n");
      message += F("<header>\n<h1>RWS Pool-KIT (V7.0)</h1>\n</header>\n<main>\n");
      message += F("<h2><center>WiFi Konfiguration</h2>");
      message += F("<center><form method='POST' action='/wifisave'><table>");

      message += F("<tr><td>&nbsp;SSID:&nbsp;</td>");
      message += F("<td>&nbsp;<input type='text' name='wifi_ssid' value='' required></td></tr>");

      message += F("<tr><td>&nbsp;WiFi Passwort:&nbsp;</td>");
      message += F("<td>&nbsp;<input type='password' name='wifi_pass' value='' required></td></tr>");

      message += F("<tr><td>&nbsp;Hostname:&nbsp;</td>");
      message += F("<td>&nbsp;<input type='text' name='hostname' value='");
      message += hostname;
      message += F("'></td></tr>");

      message += F("</table><br><br>");

      message += F("<small><b>THE SOFTWARE IS PROVIDED \"AS IS\",<br>");
      message += F("WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,<br>");
      message += F("INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF<br>");
      message += F("MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE<br>");
      message += F("AND NONINFRINGEMENT.<br>");
      message += F("INCORRECT USE CAN LEAD TO CONSIDERABLE DAMAGE<br>");
      message += F("TO HEALTH AND BUILDINGS/TECHNICS!</b></small><br><br>");

      message += F("<input type='checkbox' name='copyright' value='yes' required>");
      message += F(" <b>I accept the terms of use</b><br><br>");

      message += F("<input type='submit' value='Save & Reboot'></form>");
      message += F("</center>");

      addBottom(message);
      return;
    }

    // Nicht eingeloggt -> Login-Formular + Sensorwerte anzeigen
    if ( !login ) {
      String message;
      message =  F("<!DOCTYPE html>\n"
                   "<html lang='en'>\n"
                   "<head>\n"
                   "<title>RWS POOL-Kit V7.0</title>\n"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
                   "<meta name=\"viewport\" content=\"width=device-width\">\n"
                   "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
                   "</head>\n");
      message += F("<body>\n");
      message += F("<header>\n<h1><center>RWS Pool-Kit V7.0</center></h1>\n"
                   "<nav><p></p></nav>\n</header>\n"
                   "<main>\n");
      message += F("<h2><center>Login!</h2>");
      message += F("<center><table><tr>");
      message += F("<td>&nbsp; Login (PIN) &nbsp;</td>");
      message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"password\" name=\"login\" value=\"\"></td>");
      message += F("<td><em>");
      message += F("</td></em></tr>");
      message += F("</table>");
      message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>");
      message += F("PH: ");
      message += PH.get_last_received_reading();
      message += F("&nbsp; &Oslash;");
      if (ph_buf_size > 0 && ph_buf_count >= ph_buf_size) {
        message += String(ph_filtered, 2);
      } else {
        message += F("warm-up ");
        message += ph_buf_count;
        message += F("/");
        message += ph_buf_size;
      }
      message += F("&nbsp; (");
      message += phMinus;
      message += F(") ");
      message += F("&nbsp; [");
      message += phminus_fuellstand;
      message += F(" l ]");
      message += F("<br>  ORP: ");
      message += ORP.get_last_received_reading();
      message += F("&nbsp; (");
      message += orp_min;
      message += F("/");
      message += orp_max;
      message += F(")<br>  WaterTemp: ");
      message += RTD.get_last_received_reading();
      message += F("<br>  <small style=\"color:#888\">ESP-Zeit: ");
      message += String(timeClient.getHours());
      message += F(":");
      if (timeClient.getMinutes() < 10) message += F("0");
      message += String(timeClient.getMinutes());
      if (!ntp_synced) message += F(" <em>(!NTP nicht sync!)</em>");
      message += F("</small>");
      message += F("<br>  Shaft-Temp: ");
      message += dht_temp();
      message += F("&deg;C");
      message += F("<br>");
      message += F("Humidity: ");
      message += dht_hum();
      message += F("% rF [");
      message += humidity_max;
      message += F(" / ");
      message += humidity_min;
      message += F(" ]");
      message += ("<br>");

      // v7.0: Manual-Pump-ON Status (gibt nichts aus wenn nicht aktiv)
      pump_manual_dashboard_block(message);

      // Counter direkt nach Humidity - eigene Zeilen
      message += (check_phminus ? F("<span style=\"color:#0c0\">&#x2714;</span> ") : F("<span style=\"color:#c00\">&#x2716;</span> "));
      message += F("PHMinus_Counter: ");
      message += phminus_dblchk_counter;
      message += F("&nbsp; (");
      message += phminus_dblchk;
      message += F(" * ");
      message += check_phMinus_interval_delay;
      message += F("min)<br>");
      message += F("ORP-Counter: ");
      message += orp_chk_counter;
      message += F("/");
      message += orp_dblchk;
      message += F("<br>");

      // Chlor-Phase + Today/last/next + Warmup + Pumpe-Aus
      chlor_dashboard_block(message);

      if (flow_show || check_flow) {
        message += (check_flow ? F("<span style=\"color:#0c0\">&#x2714;</span> ") : F("<span style=\"color:#ec0\">&#x2716;</span> "));
        message += F("Flow: ");
        message += flow;
        message += F("<br>");
      }

      message += ("<br>");

      message += uptime_formatter::getUptime();

      message += F("</center>");
      addBottom(message);
      return;
    }

    // Eingeloggt -> Dashboard mit Navigation
    String message;
    addTop(message);

    message += F("<h2><center>Dashboard</h2>");
    message += F("<center>");
    message += F("PH: ");
    message += PH.get_last_received_reading();
    message += F("&nbsp; &Oslash;");
    if (ph_buf_size > 0 && ph_buf_count >= ph_buf_size) {
      message += String(ph_filtered, 2);
    } else {
      message += F("warm-up ");
      message += ph_buf_count;
      message += F("/");
      message += ph_buf_size;
    }
    message += F("&nbsp; (");
    message += phMinus;
    message += F(") ");
    message += F("&nbsp; [");
    message += phminus_fuellstand;
    message += F(" l ]");
    message += F("<br>  ORP: ");
    message += ORP.get_last_received_reading();
    message += F("&nbsp; (");
    message += orp_min;
    message += F("/");
    message += orp_max;
    message += F(")<br>  WaterTemp: ");
    message += RTD.get_last_received_reading();
    message += F("<br>  <small style=\"color:#888\">ESP-Zeit: ");
    message += String(timeClient.getHours());
    message += F(":");
    if (timeClient.getMinutes() < 10) message += F("0");
    message += String(timeClient.getMinutes());
    if (!ntp_synced) message += F(" <em>(!NTP nicht sync!)</em>");
    message += F("</small>");
    message += F("<br>  Shaft-Temp: ");
    message += dht_temp();
    message += F("&deg;C");
    message += F("<br>");
    message += F("Humidity: ");
    message += dht_hum();
    message += F("% rF [");
    message += humidity_max;
    message += F(" / ");
    message += humidity_min;
    message += F(" ]");
    message += ("<br>");

    // v7.0: Manual-Pump-ON Status (gibt nichts aus wenn nicht aktiv)
    pump_manual_dashboard_block(message);

    // Counter direkt nach Humidity - eigene Zeilen
    message += (check_phminus ? F("<span style=\"color:#0c0\">&#x2714;</span> ") : F("<span style=\"color:#c00\">&#x2716;</span> "));
    message += F("PHMinus_Counter: ");
    message += phminus_dblchk_counter;
    message += F("&nbsp; (");
    message += phminus_dblchk;
    message += F(" * ");
    message += check_phMinus_interval_delay;
    message += F("min)<br>");
    message += F("ORP-Counter: ");
    message += orp_chk_counter;
    message += F("/");
    message += orp_dblchk;
    message += F("<br>");

    // Chlor-Phase + Today/last/next + Warmup + Pumpe-Aus
    chlor_dashboard_block(message);

    if (flow_show || check_flow) {
      message += (check_flow ? F("<span style=\"color:#0c0\">&#x2714;</span> ") : F("<span style=\"color:#ec0\">&#x2716;</span> "));
      message += F("Flow: ");
      message += flow;
      message += F("<br>");
    }

    message += ("<br>");

    message += uptime_formatter::getUptime();

    message += F("</center>");
    addBottom(message);

}

//################




//###########
void handleCss()
{
  // output of stylesheet
  String message;
  message += F("*{font-family:sans-serif}\n"
               "body{margin:10px;background:black;color:white}\n"
               "h1,h2{color:white;background:black;text-align:center}\n"
               "h1{font-size:1.2em;margin:1px;padding:5px}\n"
               "h2{font-size:1.8em}\n"
               "h3{font-size:0.9em}\n"
               "a{text-decoration:none;color:white;text-align:center}\n"
               "main{text-align:center}\n"
               "table{border-collapse:separate;border-spacing:0.1em 0.1em;border-color:white; border-width:3px; border-style:solid}\n"
               "tr, th, td{font-size:1.0em;margin:0px;padding:0px;background-color:#6E6E6E}\n"
               "button{margin-top:0.3em}\n"
               "footer p{font-size:0.8em;color: dimgrey;background:black;text-align:center;margin-bottom:5px}\n"
               "nav{background-color:black;margin:1px;padding:5px;font-size:0.8em}\n"
               "nav a{color:dimgrey;padding:10px;text-decoration:none}\n"
               "nav p{margin:0px;padding:0px}\n"
               "\n");
  message += F("a.button3{");
  message += F("display:inline-block;");
  message += F("padding:0.5em 1.8em;");
  message += F("margin:0 0.3em 0.3em 0;");
  message += F("border-radius:2em;");
  message += F("box-sizing: border-box;");
  message += F("text-decoration:none;");
  message += F("font-family:'Roboto',sans-serif;");
  message += F("font-weight:300;");
  message += F("color:#FFFFFF;");
  message += F("background-color:blue;");
  message += F("text-align:center;");
  message += F("transition: all 0.2s;");
  message += F("}");
  message += F("a.button3:hover{");
  message += F("background-color:#4095c6;");
  message += F("}");
  message += F("@media all and (max-width:30em){");
  message += F("a.button3{");
  message += F("display:block;");
  message += F("margin:0.2em auto;");
  message += F("}");
  message += F("}");

  message += F("input[type=submit]{");
  message += F("display:inline-block;");
  message += F("padding:0.6em 4.2em;");
  message += F("margin:0 0.3em 0.3em 0;");
  message += F("border-radius:1em;");
  message += F("box-sizing: border-box;");
  message += F("text-decoration:none;");
  message += F("font-family:'Roboto',sans-serif;");
  message += F("font-weight:300;");
  message += F("color:black;");
  message += F("background-color:tomato;");
  message += F("text-align:center;");
  message += F("transition: all 0.2s;");
  message += F("}");
  message += F("input[type=submit]:hover{");
  message += F("background-color:orange;");
  message += F("}");
  message += F("@media all and (max-width:30em){");
  message += F("input[type=submit]{");
  message += F("display:block;");
  message += F("margin:0.2em auto;");
  message += F("}");
  message += F("}");



  server.send(200, "text/css", message);
}




// v6.0: Navigation ohne WiFi, PIN, Logout - dafuer mit Reboot
void addTop(String &message)
{
  message =  F("<!DOCTYPE html>\n"
               "<html lang='en'>\n"
               "<head>\n"
               "<title>RWS Pool-Kit</title>\n"
               "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
               "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n"
               "<meta http-equiv=\"cache-control\" content=\"no-cache\" />\n"
               "<meta http-equiv=\"Expires\" content=\"0\" />\n"
               "<meta name=\"viewport\" content=\"width=device-width\">\n"
               "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
               "</head>\n");
  message += F("<body>\n");
  message += F("<header>\n<h1>RWS Pool-KIT (V7.0)</h1>\n"
               "<nav><center><p>"
               "<a href=\"/\" class=\"button3\">Dashboard</a>"
               "<a href=\"pool.htm\" class=\"button3\">Pool&Pump</a>"
               "<a href=\"thingspeak.htm\" class=\"button3\">Thingspeak</a> "
               "<a href=\"flow.htm\" class=\"button3\">Flowcontrol</a> "
               "<a href=\"chlorinator.htm\" class=\"button3\">Chlorinator</a>"
               "<a href=\"phminus.htm\" class=\"button3\">PHMinus</a>"
               "<a href=\"heater.htm\" class=\"button3\">Heater&Fan</a>"
               "<a href=\"alive.htm\" class=\"button3\">Alive</a>"
               "<a href=\"timer.htm\" class=\"button3\">Timer</a>"
               "<a href=\"calibration.htm\" class=\"button3\">Calibration</a>"
               "<a href=\"pw.htm\" class=\"button3\">PIN</a>"
               "<a href=\"wifi.htm\" class=\"button3\">WiFi</a>"
               "&nbsp;&nbsp;&nbsp;<a href=\"reboot.htm\" class=\"button3\">Reboot</a>"
               "&nbsp;<a href=\"logout.htm\" class=\"button3\">Logout</a>"
               "</p></nav>\n</header>\n"
               "</center><main>\n");
}

void addTop2(String &message)
{
  message =  F("<!DOCTYPE html>\n"
               "<html lang='en'>\n"
               "<head>\n"
               "<title>RWS Pool-Kit</title>\n"
               "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
               "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n"
               "<meta http-equiv=\"cache-control\" content=\"no-cache\" />\n"
               "<meta http-equiv=\"Expires\" content=\"0\" />\n"
               "<meta name=\"viewport\" content=\"width=device-width\">\n"
               "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
               "</head>\n");
  message += F("<body>\n");
  message += F("<header>\n<h1>RWS Pool-KIT (V7.0)</h1>\n"
               "</center><main>\n");
}


void addBottom(String &message) {
  message += F("</main>\n"
               "<footer>\n<p>");
  message += F("<span id='min'>");
  message += ("&nbsp; RWS Pool-KIT V7.0 - (c)2021-2026 Bernd Eller <br>");
  message += ("&nbsp; uptime: ");
  message += uptime_formatter::getUptime();
  server.send(200, "text/html", message);
}
