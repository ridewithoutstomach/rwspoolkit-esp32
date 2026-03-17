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

String thingspeak_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String thingspeak_no = " ";
// v6.0: polling toggle removed - always on


void handlePageThingspeak(){

  // String s = MAIN_page; //Read HTML contents

  if ( send_to_thingspeak == true ){
    Serial.println("thingspeak button war vor Auswahl: true");
    thingspeak_yes = "checked";
    thingspeak_no = " ";
  }
  else{
    thingspeak_yes = "";
    thingspeak_no = "checked";
  }


  // v6.0: polling is always on
 

String message;

 addTop(message);
  message += F("<h2>Thingspeak - Configuration</h2>");
  //message += F("<small>You have only to fill the changed values!<br>");
  message += F("<small>Reserved IDs are:<br> ");
  message += F("ORP_ID=1 ");
  message += F("/ PH_ID=2 ");
  message += F("/ Temp_ID=3</small>");
  message += F("<br>");
  message += F("Please read the Manual!");
  message += F("<br><b><em>Minimal Polling Time is 15s (highly recommended)<br><br></b></em>");
  message += F("<br><b><em>When you turn on/off Thingspeak, the sensor is automatically turned on/off as well<br><br></b></em>");
  
  
  message += F("<center><table><small><tr>");
  message += F("<td><small>&nbsp;Activate Thingspeak: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"send_to_thingspeak\" value=\"Yes\" ");
  message += thingspeak_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"send_to_thingspeak\" value=\"No\" ");
  message += thingspeak_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");  
  
  message += F("<tr><td><small>&nbsp;Thingspeak-Polling-Time (s): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" name=\"thingspeak_delay\" min=\"15\" value= ");
  message += atol(thingspeak_delay);
  message += F("></td></tr>");

  message += F("<tr><td><small>&nbsp;Channel-Nr.:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" name=\"myChannelNumber\" value= ");
  message += myChannelNumber;
  message += F("></td></tr>");

  message += F("<tr><td><small>&nbsp;Write-Key:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"myWriteAPIKey\" value= ");
  message += myWriteAPIKey;
  message += F("></td></tr>");

  /*message += F("<tr><td><small>&nbsp;Read-Key.:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"myReadAPIKey\" value= ");
  message += myReadAPIKey;
  message += F("></td></tr>");
  */    
  message += F("<tr><td><small>&nbsp;PH-Minus-Level-ID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"4\" max=\"8\" name=\"phminusID\" value= ");
  message += phminusID;
  message += F("></td></tr>");

  message += F("<tr><td><small>&nbsp;FlowControl-ID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"4\" max=\"8\" name=\"flowID\" value= ");
  message += flowID;
  message += F("></td></tr>");

  message += F("<tr><td><small>&nbsp;Chlor-Level-ID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"4\" max=\"8\" name=\"chlorID\" value= ");
  message += chlorID;
  message += F("></td></tr>");
  
  message += F("<tr><td><small>&nbsp;Pump-ID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"4\" max=\"8\" name=\"pumpID\" value= ");
  message += pumpID;
  message += F("></td></tr>");

  message += F("<tr><td><small>&nbsp;Chlorinator-ID:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"4\" max=\"8\" name=\"chlorinatorID\" value= ");
  message += chlorinatorID;
  message += F("></td></tr>");

  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  message += F("<tr><td><small>&nbsp;Sensor Reading:&nbsp;</td>");
  message += F("<td>&nbsp;<b>Always ON (v6.1)</b></td></tr>");
  
    
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Safe & Reboot\" ></form><br></br>"); 
  message += F("</center>");
  


  
  
 addBottom(message);


  
}
