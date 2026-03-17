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

String check_chlorinator_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String check_chlorinator_no = " ";


void handlePageChlor(){

if ( check_chlorinator == true ){
    
    check_chlorinator_yes = "checked";
    check_chlorinator_no = " ";
  }
  else{
    check_chlorinator_yes = "";
    check_chlorinator_no = "checked";
  }

  

String message;

 addTop(message);
  
  
  message += F("<h2>Chlorinator</h2>");
  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("<small><b>Check Chlorinator all</b> should be around 20s: Its the time we proof ORP<br>");
  message += F("<b>If the <b>Check Chlorinator</b>-time all is too long you're Chlor will be overdosed!</b><br><br>");
  message += F("<b>To avoid measurement errors we switch on after 15 low measurements!</b><br><br>");
  message += F("<b>Attention: We only measure and switch ON in the time the pump is ON (set Timer)</b><br><br>");
  
  
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
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"check_orp_interval_delay\" value= ");
  message += check_orp_interval_delay;
  message += F("> <60s!</td></tr>"); 

  message += F("<tr><td>&nbsp;Hostname/IP Chlorinator:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_chlorinator\"  minlength=\"4\" maxlength=\"48\" value= ");
  message += hostname_chlorinator;
  message += F(">&nbsp;Tasomta Device &nbsp; </td></tr>");   

  message += F("<tr><td>&nbsp;Double-Checks: &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"orp_dblchk\" value= ");
  message += orp_dblchk;
  message += F(">Test \"x\" times before chlorinate!</td></tr>"); 

  message += F("<tr><td>&nbsp;Pulsetime (min): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" name=\"ChlorInterval\" value= ");
  message += ChlorInterval;
  message += F("> Switch off Chlonator even max not reached!</td></tr>"); 
 
  
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>"); 
  message += F("</center>");
  
 addBottom(message);


  
}
