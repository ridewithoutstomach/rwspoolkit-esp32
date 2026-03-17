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

String check_flow_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String check_flow_no = " ";

void handlePageFlow(){

if ( check_flow == true ){
    
    check_flow_yes = "checked";
    check_flow_no = " ";
  }
  else{
    check_flow_yes = "";
    check_flow_no = "checked";
  }
  

String message;

 addTop(message);
    
  message += F("<h2>Flowcontrol</h2>");
  message += F("<b>Please read the Manual</b><br>");
  message += F("<small>Flow-Delay Time is the time we need to get<br>");
  message += F("a positive Flow-Control-Signal after starting the Pump<br><br>");
  message += F("<b>Attention: Flow_Delay should not be longer than Pump-follow-up-time (Pool&Pump)!<br><br>");
  message += F("Works with: Chlorinator / ChlorPmp / PHMinus / PHPlus <br><br>");
  
  message += F("<center><table><tr>");
  message += F("<td>&nbsp;Activate Flow-Control:&nbsp; </td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_flow\" value=\"Yes\" ");
  message += check_flow_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_flow\" value=\"No\" ");
  message += check_flow_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");  
  message += F("<tr><td>&nbsp;Flow-Delay (s):&nbsp; </td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"flowcontrol_delay\"  value= ");
  message += flowcontrol_delay;
  message += F(">&nbsp; (s)</td></tr>"); 
  message += F("<tr><td>&nbsp;Hostname/IP FlowControl:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_flowcontrol\" minlength=\"4\" maxlength=\"48\" value= ");
  message += hostname_flowcontrol;
  message += F("> &nbsp; Tasomta Device&nbsp;</td></tr>");   
  message += F("<tr><td>&nbsp;Password FlowControl:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" name=\"password_flowcontrol\" minlength=\"4\" maxlength=\"48\" value= ");
  message += password_flowcontrol;
  message += F("> &nbsp; leave it blank for no password &nbsp;</td></tr>");   




  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>"); 
  message += F("</center>");
    
 addBottom(message);


  
}
