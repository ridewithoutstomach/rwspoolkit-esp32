/* *****************************************************************
   RWS Pool-Kit v7.1
   Copyright (c) 2022-2026 Ridewithoutstomach
   https://rws.casa-eller.de
   https://github.com/ridewithoutstomach/rwspoolkit-esp32
  *****************************************************************
   Hardware: Adafruit Feather HUZZAH32 ESP32 (Atlas Scientific Wi-Fi Pool Kit V1.8)
  *****************************************************************
   MIT License - see LICENSE file for details.
  *******************************************************************/

String check_alive_yes = " ";
String check_alive_no  = "checked";

void handlePageAlive(){

  if ( check_alive == true ){
    check_alive_yes = "checked";
    check_alive_no  = "";
  }
  else{
    check_alive_yes = "";
    check_alive_no  = "checked";
  }

  String message;
  addTop(message);

  message += F("<h2>Alive Heartbeat</h2>");
  message += F("<small>Notfall-Watchdog f&uuml;r die Powerbank: f&auml;llt der Heartbeat aus (Atlas tot, weil Akku leer), ");
  message += F("startet das Tasmota eine Notladung. Die regul&auml;re Nachtladung l&auml;uft unabh&auml;ngig &uuml;ber einen eigenen Timer.</small><br><br>");

  message += F("<center><table><tr>");
  message += F("<td>&nbsp;Activate Alive Heartbeat:&nbsp; </td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"av1\" name=\"check_alive\" value=\"Yes\" ");
  message += check_alive_yes;
  message += F(" >");
  message += F("<label for=\"av1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"av2\" name=\"check_alive\" value=\"No\" ");
  message += check_alive_no;
  message += F(" >");
  message += F("<label for=\"av2\">No</label>");
  message += F("</div></td></tr>");

  message += F("<tr><td>&nbsp;Interval (min):&nbsp; </td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"alive_interval\"  value= ");
  message += alive_interval;
  message += F(">&nbsp; (min)</td></tr>");

  message += F("<tr><td>&nbsp;Hostname/IP Tasmota:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_alive\" minlength=\"4\" maxlength=\"48\" value= ");
  message += hostname_alive;
  message += F("> &nbsp; Tasmota Device&nbsp;</td></tr>");

  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>");
  message += F("</center>");

  // ============== Erklaerung ==============
  message += F("<div style=\"max-width:760px; margin:1.5em auto 0; text-align:left; background:#222; padding:1em 1.5em; border-radius:0.5em\"><small>");

  message += F("<h3 style=\"text-align:center\">Wozu der Heartbeat?</h3>");
  message += F("<p>Das Atlas-Pool-Kit l&auml;uft tags&uuml;ber bewusst <b>nicht</b> am Netzteil, sondern aus einer Powerbank. ");
  message += F("Netzteile koppeln Brumm- und Schaltst&ouml;rungen ein und verf&auml;lschen die hochohmigen pH/ORP-Messungen &mdash; aus dem Akku bekommen die EZO-Boards saubere, st&ouml;rungsfreie Signale.</p>");

  message += F("<p>Die Powerbank wird im <b>Normalbetrieb</b> jede Nacht zeitgesteuert &uuml;ber einen eigenen Tasmota-Timer geladen (z.B. 02:00&ndash;05:00 Uhr) &mdash; ");
  message += F("das ist der prim&auml;re Lade-Mechanismus und l&auml;uft komplett unabh&auml;ngig von diesem Heartbeat.</p>");

  message += F("<p><b>Der Heartbeat ist nur das Sicherheitsnetz</b> f&uuml;r den Fall, dass die n&auml;chtliche Ladung einmal ausf&auml;llt oder nicht ausreicht ");
  message += F("(Stromausfall in der Ladezeit, Tasmota war offline, Akku ungew&ouml;hnlich stark entladen, defekte Zellen). Dann passiert Folgendes:</p>");
  message += F("<ol>");
  message += F("<li>Powerbank entl&auml;dt sich tags&uuml;ber bis auf 0&nbsp;% &mdash; Atlas geht aus.</li>");
  message += F("<li>Es kommt kein Heartbeat mehr beim Tasmota an.</li>");
  message += F("<li>Nach Ablauf des Watchdog-Timeouts schaltet das Tasmota das Ladeger&auml;t als <em>Notladung</em> zu und f&auml;hrt es nach fester Ladezeit automatisch wieder runter.</li>");
  message += F("<li>Sobald die Powerbank wieder genug Saft hat, bootet der Atlas, sendet wieder Heartbeats &mdash; der Watchdog ist zur&uuml;ck im Normalbetrieb, das Tasmota schaltet die Notladung nicht mehr.</li>");
  message += F("</ol>");

  message += F("<p>Im sauberen Normalbetrieb feuert dieser Notfall-Pfad also <b>nie</b>. Er ist reine R&uuml;ckfall-Sicherung, damit das System sich nach einer ausgefallenen Nachtladung selbst wieder hochzieht, ");
  message += F("ohne dass jemand vor Ort am Stecker zieht.</p>");

  message += F("<h3>Wie funktioniert es technisch?</h3>");
  message += F("<p>Das Pool-Kit sendet im eingestellten <b>Interval</b> einen einfachen HTTP-GET an das Tasmota:</p>");
  message += F("<p><code>http://&lt;tasmota-ip&gt;/cm?cmnd=Event%20alive</code></p>");
  message += F("<p>Das l&ouml;st in Tasmota das Event <code>alive</code> aus. Eine Rule reagiert darauf und setzt den internen Watchdog-Timer wieder auf seinen Startwert. ");
  message += F("L&auml;uft dieser Timer einmal ab, weil keine Events mehr kommen (= Atlas ist weg, weil Akku leer), schaltet die Rule das Power1-Relais f&uuml;r eine fest eingestellte Notlade-Zeit ein und danach automatisch wieder aus.</p>");
  message += F("<p><b>Wichtig:</b> Diese Rule darf den <b>regul&auml;ren Nacht-Ladetimer nicht st&ouml;ren</b>. Der Nacht-Timer wird unabh&auml;ngig in Tasmota (Timers-Men&uuml; oder eigene Rule) eingerichtet und schaltet Power1 zu seinen festen Zeiten ein. ");
  message += F("Solange Atlas l&auml;uft, kommt der Heartbeat regelm&auml;&szlig;ig &mdash; der Watchdog-Timer wird st&auml;ndig nachgef&uuml;hrt und l&auml;uft nie ab.</p>");

  message += F("<h3>Tasmota einrichten (Rule3)</h3>");
  message += F("<p>Im Tasmota Web-Interface die <b>Konsole</b> &ouml;ffnen und folgende Befehle nacheinander eingeben:</p>");

  message += F("<p><b>1) Rule3 leeren und neu setzen:</b></p>");
  message += F("<pre style=\"background:#111; color:#cfc; padding:0.6em; overflow:auto; white-space:pre-wrap; word-break:break-word\">");
  message += F("Rule3 0\n");
  message += F("Rule3 &quot;\n");
  message += F("Rule3 ON System#Boot DO Backlog Mem1 1; RuleTimer1 3600 ENDON ");
  message += F("ON Event#alive DO RuleTimer1 3600 ENDON ");
  message += F("ON Rules#Timer=1 DO Var1 %Mem1% ENDON ");
  message += F("ON Var1#State=1 DO Backlog Power1 1; RuleTimer2 10800 ENDON ");
  message += F("ON Rules#Timer=2 DO Backlog Power1 0; Var1 0; RuleTimer1 3600 ENDON\n");
  message += F("Rule3 1");
  message += F("</pre>");

  message += F("<p><b>2) Initialisierung:</b></p>");
  message += F("<pre style=\"background:#111; color:#cfc; padding:0.6em\">");
  message += F("Mem1 1\n");
  message += F("RuleTimer1 3600\n");
  message += F("Var1 0\n");
  message += F("PowerOnState 0");
  message += F("</pre>");

  message += F("<p><b>3) Hostname / IP des Tasmota</b> oben im Formular unter &quot;Hostname/IP Tasmota&quot; eintragen und <em>Activate Alive Heartbeat</em> auf <b>Yes</b>.</p>");

  message += F("<h3>Erkl&auml;rung der Werte (anpassbar)</h3>");
  message += F("<ul>");
  message += F("<li><b>3600</b> = Watchdog-Timeout in Sekunden. Bleibt der Heartbeat 1 h aus, wird die Notladung gestartet. Der <i>Interval</i> oben muss deutlich kleiner als dieser Wert sein (Default 5 min &lt;&lt; 60 min).</li>");
  message += F("<li><b>10800</b> = Notlade-Phase in Sekunden. 3 h Strom an, dann automatisch aus &mdash; reicht, um die Powerbank f&uuml;r den n&auml;chsten Tag zu retten, ohne sie voll zu laden (das &uuml;bernimmt wieder die n&auml;chste regul&auml;re Nachtladung).</li>");
  message += F("<li><b>Mem1</b> = Master-Schalter (1 = Automatik aktiv, 0 = aus). &Uuml;berlebt einen Tasmota-Reboot.</li>");
  message += F("<li><b>Var1</b> = interner Trigger (vom Timer aus Mem1 abgeleitet, damit ein Reset auf 0 sauber bleibt).</li>");
  message += F("</ul>");

  message += F("<h3>Bedienung am Tasmota (Konsole)</h3>");
  message += F("<pre style=\"background:#111; color:#cfc; padding:0.6em\">");
  message += F("Mem1 1       -&gt; Automatik aktivieren\n");
  message += F("Mem1 0       -&gt; Automatik deaktivieren (kein Auto-Laden mehr)\n");
  message += F("Mem1         -&gt; aktuellen Status anzeigen\n");
  message += F("RuleTimer1   -&gt; Restzeit bis Strom-Zuschaltung in Sekunden\n");
  message += F("                (springt nach jedem Heartbeat zur&uuml;ck auf den Startwert)");
  message += F("</pre>");

  message += F("<p><b>Optional:</b> eine zus&auml;tzliche Rule auf <code>ON Power1#state=1 DO ...</code> einrichten ");
  message += F("(z.B. Telegram- oder CallMeBot-WhatsApp-Push), um beim automatischen Lade-Start eine Benachrichtigung zu bekommen.</p>");

  message += F("</small></div>");
  // ============== Ende Erklaerung ==============

  addBottom(message);
}


void send_alive(){
  if (millis() - alive_fail_time < HOST_RETRY_DELAY) return;   // Failure-Cache 5min
  if (strlen(hostname_alive) == 0) return;
  if (!wifi_isconnected()) return;

  http.begin(client, "http://" + String(hostname_alive) + "/cm?cmnd=Event%20alive");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    Serial.print("Alive: sent -> ");
    Serial.println(hostname_alive);
  } else {
    alive_fail_time = millis();
    Serial.printf("[Alive] GET failed: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
