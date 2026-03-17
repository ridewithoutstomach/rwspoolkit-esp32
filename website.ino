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



//############
void handleForm() {


  if ( server.hasArg("check_pump_on")) {

    if (login) {
      int z = strcmp(server.arg(0).c_str(), "OFF");

      if ( z == 0 ) {
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
        timer_interval_delay = 86400000;
        check_pump_on = "checked";
        check_pump_off = "";
        call_pumpe_hand();
        String message;
        addTop(message);
        message += F("<br><h1><center><a href=\"pool.htm\" class=\"button3\">Don't forget to Reset!</a>");
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
      int z = strcmp(server.arg(0).c_str(), "Yes");
      if ( z == 0 ) {
        check_flow = true;
        strcpy(flowcontrol_delay, server.arg(1).c_str());
        strcpy(hostname_flowcontrol, server.arg(2).c_str());
        strcpy(password_flowcontrol, server.arg(3).c_str());
      }
      else {
        check_flow = false;
      }

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
      int z = strcmp(server.arg(0).c_str(), "Yes");
      if ( z == 0 ) {
        check_chlorinator = true;
        strcpy(check_orp_interval_delay, server.arg(1).c_str());
        strcpy(hostname_chlorinator, server.arg(2).c_str());
        strcpy(orp_dblchk, server.arg(3).c_str());
        strcpy(ChlorInterval, server.arg(4).c_str());

      }
      else {
        check_chlorinator = false;
        ThingSpeak.setField(String(chlorinatorID).toInt(), 0);

      }

      delay(100);
      write_chlorinator();
      delay(100);
      read_chlorinator();
      orp_chk_counter = 0;
      write_chlorinator();

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
      message += F("<header>\n<h1>RWS Pool-KIT (V6.3)</h1>\n</header>\n<main>\n");
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
                   "<title>RWS POOL-Kit V6.3</title>\n"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
                   "<meta name=\"viewport\" content=\"width=device-width\">\n"
                   "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
                   "</head>\n");
      message += F("<body>\n");
      message += F("<header>\n<h1><center>RWS Pool-Kit V6.3</center></h1>\n"
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
      message += F("<br>  ESP-Zeit: <b>");
      message += String(timeClient.getHours());
      message += F(":");
      if (timeClient.getMinutes() < 10) message += F("0");
      message += String(timeClient.getMinutes());
      message += F("</b>");
      if (summer) message += F(" (Sommerzeit)");
      if (!ntp_synced) message += F(" <em>(!NTP nicht sync!)</em>");
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
      message += F("ORP_Counter: ");
      message += orp_chk_counter;
      message += F("&nbsp; (");
      message += orp_dblchk;
      message += F(" * ");
      message += check_orp_interval_delay;
      message += F("s)<br>");
      message += F("PHMinus_Counter: ");
      message += phminus_dblchk_counter;
      message += F("&nbsp; (");
      message += phminus_dblchk;
      message += F(" * ");
      message += check_phMinus_interval_delay;
      message += F("min)<br>");
      message += F("Flow:");
      message += flow;
      message += F("<br>");

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
    message += F("<br>  ESP-Zeit: <b>");
    message += String(timeClient.getHours());
    message += F(":");
    if (timeClient.getMinutes() < 10) message += F("0");
    message += String(timeClient.getMinutes());
    message += F("</b>");
    if (summer) message += F(" (Sommerzeit)");
    if (!ntp_synced) message += F(" <em>(!NTP nicht sync!)</em>");
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
    message += F("ORP_Counter: ");
    message += orp_chk_counter;
    message += F("&nbsp; (");
    message += orp_dblchk;
    message += F(" * ");
    message += check_orp_interval_delay;
    message += F("s)<br>");
    message += F("PHMinus_Counter: ");
    message += phminus_dblchk_counter;
    message += F("&nbsp; (");
    message += phminus_dblchk;
    message += F(" * ");
    message += check_phMinus_interval_delay;
    message += F("min)<br>");
    message += F("Flow:");
    message += flow;
    message += F("<br>");

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
  message += F("<header>\n<h1>RWS Pool-KIT (V6.3)</h1>\n"
               "<nav><center><p>"
               "<a href=\"/\" class=\"button3\">Dashboard</a>"
               "<a href=\"pool.htm\" class=\"button3\">Pool&Pump</a>"
               "<a href=\"thingspeak.htm\" class=\"button3\">Thingspeak</a> "
               "<a href=\"flow.htm\" class=\"button3\">Flowcontrol</a> "
               "<a href=\"chlorinator.htm\" class=\"button3\">Chlorinator</a>"
               "<a href=\"phminus.htm\" class=\"button3\">PHMinus</a>"
               "<a href=\"heater.htm\" class=\"button3\">Heater&Fan</a>"
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
  message += F("<header>\n<h1>RWS Pool-KIT (V6.3)</h1>\n"
               "</center><main>\n");
}


void addBottom(String &message) {
  message += F("</main>\n"
               "<footer>\n<p>");
  message += F("<span id='min'>");
  message += ("&nbsp; RWS Pool-KIT V6.3 - (c)2021-2026 Bernd Eller <br>");
  message += ("&nbsp; uptime: ");
  message += uptime_formatter::getUptime();
  server.send(200, "text/html", message);
}
