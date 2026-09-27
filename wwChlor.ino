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

String check_chlorinator_yes = "checked";
String check_chlorinator_no = " ";
String chlor_safety_yes = "checked";
String chlor_safety_no  = " ";


void handlePageChlor(){

if ( check_chlorinator == true ){
    check_chlorinator_yes = "checked";
    check_chlorinator_no = " ";
  }
  else{
    check_chlorinator_yes = "";
    check_chlorinator_no = "checked";
  }

if ( chlor_safety_stop == true ){
    chlor_safety_yes = "checked";
    chlor_safety_no  = " ";
  }
  else{
    chlor_safety_yes = " ";
    chlor_safety_no  = "checked";
  }

String message;

 addTop(message);


  message += F("<h2>Chlorinator</h2>");
  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("<small><b>Phase logic:</b> Observe -> Dose -> Distribute -> Observe<br>");
  message += F("Distribute time covers the hydraulic dead time between chlorinator output and ORP probe.<br>");
  message += F("During Dose and Distribute the pump is forced to <b>Pump Dosage Output</b>.<br><br>");
  message += F("<b>Attention: ORP is only evaluated as a trigger while the pump is ON!</b></small><br><br>");


  // Schockchloren: Start-Formular bzw. Status + Abbrechen
  message += F("<center>");
  shock_block(message, true);
  message += F("</center><br>");

  message += F("<center><table><tr>");
  message += F("<td>&nbsp;Activate Chlorinator: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_chlorinator\" value=\"Yes\" ");
  message += check_chlorinator_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_chlorinator\" value=\"No\" ");
  message += check_chlorinator_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");

  message += F("<tr><td>&nbsp;Check Chlorinator all (s): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"check_orp_interval_delay\" value=\"");
  message += check_orp_interval_delay;
  message += F("\"></td><td>&nbsp;ORP probe interval (Observe + Dose phases)</td></tr>");

  message += F("<tr><td>&nbsp;Hostname/IP Chlorinator:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_chlorinator\" minlength=\"4\" maxlength=\"48\" value=\"");
  message += hostname_chlorinator;
  message += F("\"></td><td>&nbsp;Tasmota Device</td></tr>");

  message += F("<tr><td>&nbsp;Double-Checks: &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"orp_dblchk\" value=\"");
  message += orp_dblchk;
  message += F("\"></td><td>&nbsp;Test \"x\" times before chlorinate</td></tr>");

  message += F("<tr><td>&nbsp;Pulsetime (min): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"ChlorInterval\" value=\"");
  message += ChlorInterval;
  message += F("\"></td><td>&nbsp;Max chlorinate duration per phase</td></tr>");

  // v7.0
  message += F("<tr><td>&nbsp;<b>Distribute time (min):</b> &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"chlor_distribute_min\" value=\"");
  message += chlor_distribute_min;
  message += F("\"></td><td>&nbsp;Mandatory pause - chlor reaches probe</td></tr>");

  message += F("<tr><td>&nbsp;<b>Daily budget:</b> &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"0\" name=\"chlor_daily_budget\" value=\"");
  message += chlor_daily_budget;
  message += F("\"></td><td>&nbsp;Max chlor phases per day (0 = off)</td></tr>");

  // v7.2
  message += F("<tr><td>&nbsp;<b>Warmup time (min):</b> &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"0\" name=\"chlor_warmup_min\" value=\"");
  message += chlor_warmup_min;
  message += F("\"></td><td>&nbsp;Sensor settling after first pump-on of the day (0 = off)</td></tr>");

  message += F("<tr><td>&nbsp;<b>Safety stop:</b> &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("&nbsp;<input type=\"radio\" id=\"s1\" name=\"chlor_safety_stop\" value=\"Yes\" ");
  message += chlor_safety_yes;
  message += F("><label for=\"s1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"s2\" name=\"chlor_safety_stop\" value=\"No\" ");
  message += chlor_safety_no;
  message += F("><label for=\"s2\">No</label>");
  message += F("</div></td><td>&nbsp;Stop Dose if ORP >= ORP_Max</td></tr>");

  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>");

  // v7.1: Status-Reset (eigenes Form, damit es nicht zusammen mit Settings ausgeloest wird)
  message += F("<form action=\"/action_page\" onsubmit=\"return confirm('Chlor-Status wirklich zuruecksetzen? Phase wird auf BEOBACHTEN gestellt, Today-Counter und Last-Start werden geloescht.');\">");
  message += F("<input type=\"hidden\" name=\"chlor_status_reset\" value=\"1\">");
  message += F("<input type=\"submit\" value=\"Status Reset\" style=\"background:#c00;color:white;\">");
  message += F("</form>");
  message += F("<small>Setzt Phase, Today-Counter, Last-Start und ORP-Counter zur&uuml;ck. Nur n&ouml;tig wenn z.B. ein abgebrochener Zyklus h&auml;ngt.</small>");

  message += F("</center>");

  // ============== Erklaerung der Parameter ==============
  message += F("<div style=\"max-width:760px; margin:1.5em auto 0; text-align:left; background:#222; padding:1em 1.5em; border-radius:0.5em\"><small>");

  message += F("<h3 style=\"text-align:center\">Erkl&auml;rung der Parameter</h3>");

  message += F("<h3>Phasen-Ablauf</h3>");
  message += F("<p>Die Regelung l&auml;uft in drei Phasen ab:<br>");
  message += F("<b>BEOBACHTEN &rarr; CHLOREN &rarr; VERTEILEN &rarr; BEOBACHTEN</b></p>");
  message += F("<ul>");
  message += F("<li><b>BEOBACHTEN:</b> Normale &Uuml;berwachung. ORP wird gemessen, Chlorinator aus. Ist der ORP <i>Double-Checks</i> Messungen lang zu niedrig, wird Chloren ausgel&ouml;st.</li>");
  message += F("<li><b>CHLOREN:</b> Chlorinator l&auml;uft, Pumpe wird auf Dosier-Stufe gezwungen. ORP wird gemessen, aber nur f&uuml;r den Sicherheitsabbruch ausgewertet &mdash; nicht f&uuml;r neue Trigger.</li>");
  message += F("<li><b>VERTEILEN:</b> Pflichtpause nach Chloren. Chlorinator aus, Pumpe l&auml;uft weiter auf Dosier-Stufe. Das Chlor braucht Zeit, durchs Becken zur Sonde im Skimmer zu wandern. ORP wird in dieser Phase ignoriert. Nach Ablauf geht es zur&uuml;ck nach Beobachten &mdash; ist ORP weiter zu niedrig, l&auml;uft &uuml;ber den normalen Double-Checks-Counter ein neuer Zyklus.</li>");
  message += F("</ul>");

  message += F("<h3>Parameter</h3>");

  message += F("<p><b>Activate Chlorinator</b><br>Master-Schalter der gesamten Chlor-Regelung.</p>");
  message += F("<p><b>Check Chlorinator all (s)</b><br>Mess-Takt der ORP-Sonde. Default 59 s. Gilt in allen Phasen.</p>");
  message += F("<p><b>Hostname/IP Chlorinator</b><br>Tasmota-Ger&auml;t, das den Chlorinator schaltet.</p>");
  message += F("<p><b>Double-Checks</b><br>Anzahl aufeinanderfolgender Messungen unter ORP-Min, bevor Chloren ausgel&ouml;st wird. Default 10. Filter gegen kurze Schwankungen.</p>");
  message += F("<p><b>Pulsetime (min)</b><br>Maximale Dauer einer Chloren-Phase. Default 30.</p>");

  message += F("<p><b>Distribute time (min)</b><br>Pflichtpause nach Chloren. Default 240. In dieser Zeit wird der Chlorinator nicht erneut eingeschaltet. Soll l&auml;nger sein als die hydraulische Totzeit zwischen D&uuml;se und Sonde &mdash; bei 42 m&sup3; und 11 m&sup3;/h Pumpe ca. 4 h f&uuml;r eine Beckenrunde. Nach Ablauf geht das System zur&uuml;ck in Beobachten und der Double-Checks-Counter entscheidet wie sonst auch &uuml;ber einen neuen Zyklus.</p>");

  message += F("<p><b>Daily budget</b><br>Maximale Chlor-Phasen pro Tag (0 = unbegrenzt). Begrenzt die Tagesdosis hart. Sinnvoll f&uuml;r:</p>");
  message += F("<ul>");
  message += F("<li>Tageszehrung deckeln (wenn der Bedarf bekannt ist)</li>");
  message += F("<li>Schutz bei Sondenfehler oder Fehlkonfig</li>");
  message += F("<li>Beobachtungsmodus bei neuen Einstellungen</li>");
  message += F("<li>Urlaub / unbeaufsichtigter Betrieb</li>");
  message += F("</ul>");
  message += F("<p>Bei ersch&ouml;pftem Budget misst das System weiter, zeigt &quot;Budget ersch&ouml;pft&quot; im Dashboard, l&ouml;st aber bis Mitternacht keine neue Phase aus. Reset um 0:00 Uhr.</p>");

  message += F("<p><b>Safety stop</b><br>Bei &quot;Yes&quot; wird die laufende Chloren-Phase sofort abgebrochen, wenn der ORP &uuml;ber ORP-Max steigt. Schutz gegen &Uuml;berdosierung.</p>");

  message += F("<p><b>Warmup time</b><br>Sperrzeit nach dem <b>ersten</b> Pumpen-AN des Tages. In dieser Zeit misst die Sonde zwar weiter, wertet aber nicht aus &mdash; nach Stillstand ist die ORP-Sonde noch nicht eingependelt und w&uuml;rde sonst auf einen Phantom-Tiefstand reagieren. Wird auch nach Reboot einmal angewendet. Mittags-Pause z&auml;hlt nicht (Pumpe geht nur kurz aus, Wasser hat sich nicht beruhigt). Default 15 min, 0 = aus. Beim Pumpen-AN wird der ORP-Counter gleichzeitig zur&uuml;ckgesetzt &mdash; falls sich das Wasser &uuml;ber Nacht beruhigt hat, soll keine Vor-Last aus dem Vortag durchschlagen.</p>");

  message += F("<p><b>Nacht-Deadline</b> (automatisch aus Pumpen-Timer)<br>Das System kennt die <b>letzte Aus-Anweisung</b> deines Pumpen-Timers (sp&auml;tester aktiver Slot mit Speed=1 heute) und beachtet sie. Reicht die Zeit bis dahin nicht f&uuml;r eine komplette Chloren-Phase plus 5 min Sp&uuml;lung (= Pulsetime + 5), wird kein neuer Zyklus mehr gestartet. L&auml;uft gerade ein Zyklus und die Pumpe geht in &lt;5 min aus, wird der Chlorinator sofort abgeschaltet, DISTRIBUTE &uuml;bersprungen, und die Pumpe an den Timer zur&uuml;ckgegeben &mdash; die letzten Pumpenminuten wirken als Sp&uuml;lung. Mittags-Aus-Slots, die der Chlor sowieso durchfeuert, werden ignoriert.</p>");

  message += F("<h3>Schockchloren</h3>");
  message += F("<p>Start oben auf dieser Seite: Stunden eingeben, <i>Start</i>. F&uuml;r die ganze Dauer l&auml;uft der Chlorinator durch, ");
  message += F("die Pumpe steht fest auf <b>Pump Dosage Output</b>. Timer, Winter-Modus, Manual Pump ON, ORP-Regelung, Safety stop, Nacht-Deadline und Daily budget sind solange au&szlig;er Kraft. ");
  message += F("PH-Minus regelt normal weiter. Ist das Flow-Gate aktiv, wird nur der Chlorinator bei fehlendem Flow pausiert &mdash; die Schock-Zeit l&auml;uft weiter. ");
  message += F("Nach Ablauf folgt eine komplette <b>VERTEILEN</b>-Phase (Distribute time, auch nachts), danach &uuml;bernimmt der Timer. ");
  message += F("<i>Abbrechen</i> schaltet sofort ab und gibt die Pumpe direkt an den Timer zur&uuml;ck. Ein Reboot setzt den Schock mit der Restzeit fort. Max. 48 h.</p>");

  message += F("<h3>Woran drehen bei welchem Problem?</h3>");
  message += F("<p><b>System reagiert zu sp&auml;t:</b><br>&rarr; Double-Checks reduzieren oder ORP-Min erh&ouml;hen.</p>");
  message += F("<p><b>System chloriert zu oft / zu nerv&ouml;s:</b><br>&rarr; Double-Checks erh&ouml;hen oder Distribute time verl&auml;ngern.</p>");
  message += F("<p><b>Pool wird &uuml;berchlort (ORP-Anstieg am Folgetag):</b><br>&rarr; Pulsetime reduzieren oder Distribute time verl&auml;ngern.<br>&rarr; Oder Daily budget setzen.</p>");
  message += F("<p><b>Pool wird unterchlort:</b><br>&rarr; Pulsetime erh&ouml;hen oder Distribute time vorsichtig k&uuml;rzen.</p>");
  message += F("<p><b>Sicherheitsnetz aktivieren:</b><br>&rarr; Daily budget einschalten (z.B. 6).<br>&rarr; Safety stop = Yes.</p>");

  message += F("</small></div>");
  // ============== Ende Erklaerung ==============

 addBottom(message);
}
