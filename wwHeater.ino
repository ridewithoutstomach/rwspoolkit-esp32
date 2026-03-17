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




String check_heizung_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String check_heizung_no = " ";

String check_fan_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String check_fan_no = " ";



void handlePageHeizung(){


if ( check_heizung == true ){
    
    check_heizung_yes = "checked";
    check_heizung_no = " ";
  }
  else{
    check_heizung_yes = " ";
    check_heizung_no = "checked";
  }  

if ( check_fan == true ){
    
    check_fan_yes = "checked";
    check_fan_no = " ";
  }
  else{
    check_fan_yes = " ";
    check_fan_no = "checked";
  }  
  

String message;

 addTop(message);
  
  message += F("<h2>Shaft Heater & Fan</h2>");
  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("<small><b>Check Temperatur all</b> is the time we wait to check the Temperature and switch ON/OFF<br><br>");
 
    
  message += F("<center><table><tr>");
  message += F("<td>&nbsp; Activate Heater: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_heizung\" value=\"Yes\" ");
  message += check_heizung_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_heizung\" value=\"No\" ");
  message += check_heizung_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");  

  
  message += F("<td>&nbsp; Activate Fan: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_fan\" value=\"Yes\" ");
  message += check_fan_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_fan\" value=\"No\" ");
  message += check_fan_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");  
  

  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  message += F("<tr><td><em><center><b>&nbsp;Select Temperature values:&nbsp;</b></center></em></td></tr>");
  message += F("<td><small>&nbsp;Temp Min (°C) < &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"temp_min\" pattern=\"[0-9]*\" value=");
  message += temp_min;
  message += F("> &nbsp; Heater switch ON &nbsp;</em></td></tr>");
  
  message += F("<tr><td><small>&nbsp;Temp Max (°C) > &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"temp_max\" pattern=\"[0-9]*\" value=");
  message += temp_max;
  message += F("> &nbsp; Heater switch OFF&nbsp;</em></td></tr>");   

//
  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  message += F("<tr><td><em><center><b>&nbsp;Select Humidity values:&nbsp;</b></center></em></td></tr>");
  message += F("<tr><td><small>&nbsp;Humidity Max (% rF) > &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"humidity_max\" pattern=\"[0-9]*\" value=");
  message += humidity_max;
  message += F("> &nbsp; Fan switch ON&nbsp;</em></td></tr>");
  message += F("<td><small>&nbsp;Humidity Min (% rF) < &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"humidity_min\" pattern=\"[0-9]*\" value=");
  message += humidity_min;
  message += F("> &nbsp; Fan switch OFF!! &nbsp;</em></td></tr>");
  
  




//
  
  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  message += F("<tr><td><em><center><b>&nbsp;Select Heater / Fan  values:&nbsp;</b></center></em></td></tr>");

  message += F("<tr><td>&nbsp; Check Temperatur/Humidity all (s): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"check_temp_interval_delay\" value= ");
  message += check_temp_interval_delay;
  message += F("></td></tr>"); 

  message += F("<tr><td>&nbsp; Hostname/IP Heater:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_heater\" value= ");
  message += hostname_heater;
  message += F("> &nbsp; Tasomta Device &nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp;Password Heater:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" name=\"password_heater\" minlength=\"4\" maxlength=\"48\" value= ");
  message += password_heater;
  message += F("> &nbsp; leave it blank for no password &nbsp;</td></tr>");


  message += F("<tr><td>&nbsp; Hostname/IP Fan:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_fan\" value= ");
  message += hostname_fan;
  message += F("> &nbsp; Tasomta Device &nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp;Password Fan:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" name=\"password_fan\" minlength=\"4\" maxlength=\"48\" value= ");
  message += password_fan;
  message += F("> &nbsp; leave it blank for no password &nbsp;</td></tr>");
  
 
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>"); 
  message += F("</center>");
  
  
 addBottom(message);


  
}
