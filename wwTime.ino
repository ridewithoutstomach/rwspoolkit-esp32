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

String summertime_yes = "checked";
String summertime_no = " ";



void handlePageTimer(){


if (utcOffsetInSeconds == 3600 ){
    summertime_yes = " ";
    summertime_no = "checked";
  }
  else{
    summertime_yes = "checked";
    summertime_no = " ";
  }


String message;

 addTop(message);

  message += F("<h2>Timer (Tasmota-Style)</h2>");
  message += F("<small>Jeder Timer setzt ab seiner Uhrzeit den Pumpen-Speed.<br>");
  message += F("<b>Speed 1 = Pumpe aus (Power1 auf CH4 Sonoff)</b><br>");
  message += F("Speed 2-4 = Pumpen-Geschwindigkeiten (Power2-4 auf CH4 Sonoff)<br>");
  message += F("Nur aktive Timer werden ausgefuehrt.<br><br>");

  message += F("<center><table><tr>");
  message += F("<td>&nbsp; Sommerzeit:: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"utcOffsetInSeconds\" value=\"Yes\" ");
  message += summertime_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Ja</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"utcOffsetInSeconds\" value=\"No\" ");
  message += summertime_no;
  message +=F(" >");
  message += F("<label for=\"b2\">Nein</label>");
  message += F("</div></td></tr>");

  message += F("<tr><td></td><td><b>&nbsp;</b></td><td></td><td></td></tr>");
  message += F("<tr><td><em><b>&nbsp;Poolpumpe:</b></em></td><td></td><td></td><td></td></tr>");

  message += F("<tr><td><em>&nbsp; Nr.&nbsp; </td>");
  message += F("<td><em> Zeit</td><td><em> Speed</td><td><em> Aktiv</td></tr>");

  // 24 Timer-Zeilen generieren
  for (int i = 0; i < TIMER_COUNT; i++) {
    message += F("<tr><td>&nbsp; Timer ");
    message += String(i + 1);
    message += F(": </td>");

    // Zeit-Eingabe
    message += F("<td>&nbsp;<input type=\"time\" name=\"t");
    message += String(i);
    message += F("z\" value=\"");
    message += timers[i].zeit;
    message += F("\"</td>");

    // Speed-Eingabe
    message += F("<td>&nbsp; : <input type=\"number\" min=\"1\" max=\"4\" size=\"3\" name=\"t");
    message += String(i);
    message += F("s\" value=\"");
    message += String(timers[i].speed);
    message += F("\"></td>");

    // Aktiv-Checkbox
    message += F("<td>&nbsp;<input type=\"checkbox\" name=\"t");
    message += String(i);
    message += F("a\" value=\"1\"");
    if (timers[i].aktiv) message += F(" checked");
    message += F("></td></tr>");
  }

  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Speichern\"></form><br></br>");
  message += F("</center>");

 addBottom(message);


}
