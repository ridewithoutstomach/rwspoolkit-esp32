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


// v6.0: Reboot (ersetzt Logout - setzt login zurueck und startet ESP neu)
void handleReboot(){
  login = false;
  String s = "<br><h1><center><a href=\"/\">Rebooting..</a>";
  server.send(200, "text/html", s);
  delay(2000);
  ESP.restart();
}

// v6.3: Logout ohne Reboot
void handleLogout(){
  login = false;
  String message;
  addTop2(message);
  message += F("<br><h1><center><a href=\"/\" class=\"button3\">Logged out!</a>");
  server.send(200, "text/html", message);
}

// v6.3: Passwort-Aendern-Seite
void handlePassword(){
  String message;
  addTop(message);

  message += F("<h2><center>Login-PIN aendern</h2>");
  message += F("<b>max 54 Zeichen erlaubt!</b><br><br>");
  message += F("<small>Nicht vergessen! Ohne PIN kommst du nur per Hard-Reset rein!</small><br><br>");
  message += F("<center><table><tr>");
  message += F("<td>&nbsp;Neuer PIN:&nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"password\" pattern=\".{4,54}\" name=\"loginpassword\" value=\"\"></td>");
  message += F("<td></td></tr>");
  message += F("<tr><td>&nbsp;PIN wiederholen:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" pattern=\".{4,54}\" name=\"loginpassword2\" value=\"\"></td>");
  message += F("<td></td></tr>");
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>");
  message += F("</center>");

  addBottom(message);
}

// v6.3: WiFi-Konfiguration (ueber Navigation erreichbar, Login erforderlich)
void handleWifi(){
  String message;
  addTop(message);

  message += F("<h2><center>WiFi Konfiguration</h2>");
  message += F("<center><form action=\"/action_page\"><table>");

  message += F("<tr><td>&nbsp;SSID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"wifi_ssid\" value=\"");
  message += wifi_ssid;
  message += F("\"></td></tr>");

  message += F("<tr><td>&nbsp;WiFi Passwort:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" name=\"wifi_pass\" value=\"");
  message += wifi_pass;
  message += F("\"></td></tr>");

  message += F("<tr><td>&nbsp;Hostname:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname\" value=\"");
  message += hostname;
  message += F("\"></td></tr>");

  message += F("</table><br><br>");

  message += F("<small><b>THE SOFTWARE IS PROVIDED \"AS IS\",<br>");
  message += F("WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,<br>");
  message += F("INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF<br>");
  message += F("MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE<br>");
  message += F("AND NONINFRINGEMENT.<br>");
  message += F("INCORRECT USE CAN LEAD TO CONSIDERABLE DAMAGE<br>");
  message += F("TO HEALTH AND BUILDINGS/TECHNICS!</b></small><br><br>");

  message += F("<input type=\"checkbox\" name=\"copyright\" value=\"yes\" required>");
  message += F(" <b>I accept the terms of use</b><br><br>");

  message += F("<input type=\"submit\" value=\"Save & Reboot\"></form>");
  message += F("</center>");

  addBottom(message);
}
