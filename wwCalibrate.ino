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


void handleCal(){


String message;

 addTop(message);
  
  message += F("<h2>Calibration Probes</h2>");
  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("After calibration you should reboot!<br><br>");
  message += F("Only numbers allowed! exept the keyword <b>clear</b> to reset a Probe<br><br>");
  
  
  message += F("<center><table><tr>");
  message += F("<td>&nbsp; Calibrate PH Low: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"calPHlow\" pattern=\"clear|[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += PH.get_last_received_reading();
  message += F(" >");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");  
  
  message += F("<td>&nbsp; Calibrate PH Mid: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"calPHmid\" pattern=\"clear|[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += PH.get_last_received_reading();
  message += F(" >");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");  
 

  message += F("<td>&nbsp; Calibrate PH High: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"calPHhigh\" pattern=\"clear|[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += PH.get_last_received_reading();
  message += F(" >");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");  
  
  message += F("<tr><td>&nbsp;</td><td></td><td></td></tr>"); 
 
  message += F("<td>&nbsp; Calibrate ORP: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"calORP\" pattern=\"clear|[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += ORP.get_last_received_reading();
  message += F(" >");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");  
 
  message += F("<tr><td>&nbsp;</td><td></td><td></td></tr>"); 
  
  message += F("<td>&nbsp; Calibrate TEMP: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"calTemp\" pattern=\"clear|[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += RTD.get_last_received_reading();
  message += F(" >");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");

  message += F("<tr><td>&nbsp;</td><td></td><td></td></tr>");
  message += F("<tr><td colspan=\"3\"><em><b>&nbsp; AM2315C Sensor Offset:</b></em></td></tr>");

  message += F("<tr><td>&nbsp; Temp-Offset (&deg;C): &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"dht_temp_offset\" pattern=\"-?[0-9]*[.]{0,}[0-9]{0,2}\" value=\"");
  message += String(dht_temp_offset, 1);
  message += F("\">");
  message += F("&nbsp; aktuell: ");
  message += String(dht_temp(), 1);
  message += F("&deg;C");

  message += F("</td></tr><tr><td>&nbsp; Humidity-Offset (% rF): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"dht_hum_offset\" pattern=\"-?[0-9]*[.]{0,}[0-9]{0,2}\" value=\"");
  message += String(dht_hum_offset, 1);
  message += F("\">");
  message += F("&nbsp; aktuell: ");
  message += String(dht_hum(), 1);
  message += F("% rF");
  message += F("</td><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td></form></tr>");

  message += F("</table>");

/*  message += F("<tr><td>&nbsp; PH-Plus-MixTime (min): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"0\" name=\"check_phPlus_interval_delay\" value= ");
  message += check_phPlus_interval_delay;
  message += F(">&nbsp; > PoolWaterCompleteMixed &nbsp;</td></tr>"); 

  message += F("<tr><td>&nbsp; Hostname/IP PH-PlusPMP:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_phplus\" value= ");
  message += hostname_phplus;
  message += F(">&nbsp; Tasomta Device &nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp; PH-Plus volume (Liter):&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"phplus_fuellstand\" pattern=\"[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += phplus_fuellstand;
  message += F(">&nbsp; current canister volume &nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp; PH-Plus-dosage-amount (ml):&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"phplus_dosiermenge\" pattern=\"[0-9]*[.]{0,}[0-9]{1,3}\" value= ");
  message += phplus_dosiermenge;
  message += F(">&nbsp; how many ml we dosage per impuls &nbsp;</td></tr>");   
  
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>"); 
  message += F("</center>");
  */
  
 addBottom(message);


  
}
