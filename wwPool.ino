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

String oops_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String oops_no = " ";

String winter_yes = " ";
String winter_no = "checked";


void handlePagePool(){

  

if ( only_one_pump_speed == true ){
    oops_yes = "checked";
    oops_no = " ";
  }
  else{
      oops_yes = " ";
      oops_no = "checked";
  }


if ( winter_modus == true ){
    winter_yes = "checked";
    winter_no = " ";
  }
  else{
      winter_yes = " ";
      winter_no = "checked";
  }





String message;

 addTop(message);
 
  message += F("<h2>Pool & Pump - Configuration</h2>");
  message += F("<br><b>Please read the Manual</b><br>");
  message += F("<em><small><center>If you have a 4 Channel Pump you can choose different speeds for Dosage/Manually:</center></em>");
  message += F("<b>Attention: No CH4-Pump ? -> Select <b>1</b> for all Outputs!</center></b>");
  message += F("<em><center>The pump follow-up time is the time you need until<br>");
  message += F("the medium (e.g.PHMinus) that is being dosed has been completely mixed</small></center><br></em>");
  message += F("<small><b>Pump Temperatur Output</b> and <b>Winter-Temperatur</b> is for Winter Modus</small></center><br>");
  message += F("<small>Set the Winter-Temperature to the value when the pump should run ( if its colder the Pump runs)</small>");
  
  
    
  message += F("<center><table>");
  
  message += F("<tr><td><center><b>Select ORP values:</b></center></td></tr>");
  
  message += F("<tr><td><small>&nbsp;ORP Min (mV)&nbsp; < </td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"orp_min\" min=\"100\" max=\"999\" pattern=\"[0-9]*\" value=");
  message += orp_min;
  message += F("></td><td><em><small> &nbsp;Chlorinator ON or ChlorDosePump Impuls &nbsp;</em></td></tr>");
  
  message += F("<tr><td><small>&nbsp;ORP max (mV) > &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" min=\"100\"max=\"999\" name=\"orp_max\" pattern=\"[0-9]*\" value=");
  message += orp_max;
  message += F("></td><td><em><small> &nbsp;Chlorinator switch OFF&nbsp;</small></em></td></tr>");

  message += F("<tr><td><center><b>Select PH values:</b></center></td></tr>");
  //message += F("<tr><td><em><center><b>Select PH values:</b></center></em></td><td></td><td></td></tr>");

  message += F("<tr><td><small>&nbsp;PH-Minus >&nbsp;</td>");
  //message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"phMinus\" value=");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"phMinus\" pattern=\"[0-9]*[.]{0,}[0-9]{1,2}\" value=");
  message += phMinus;
  message += F("></td><td><em><small>&nbsp;Inject <b>PH-Minus</b> (Pump)&nbsp;</em></td></tr>");

  message += F("<tr><td><small>&nbsp;PH-Plus < &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"phPlus\" pattern=\"[0-9]*[.]{0,}[0-9]{1,2}\" value=");
  message += phPlus;
  message += F("></td><td><em><small>&nbsp;Inject <b>PH-Plus</b> (Pump)&nbsp;</small></em></td></tr>");
  
  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  /*
  message += F("<tr><td><em><center><b>&nbsp;Select Temperature values:&nbsp;</b></center></em></td></tr>");
  message += F("<td><small>&nbsp;Temp Min (°C) < &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"temp_min\" pattern=\"[0-9]*\" value=");
  message += temp_min;
  message += F("></td><td><em><small>&nbsp; Heater switch ON &nbsp;</em></td></tr>");
  
  message += F("<tr><td><small>&nbsp;Temp Max (°C) > &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"temp_max\" pattern=\"[0-9]*\" value=");
  message += temp_max;
  message += F("></td><td><em><small>&nbsp; Heater switch OFF&nbsp;</em></td></tr>");
  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  */
  message += F("<tr><td><em><center><b>&nbsp;Select Pool-Pump values:&nbsp;</b></center></em></td></tr>");

  
  message += F("<tr><td><small>&nbsp;CH4 Pump: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"only_one_pump_speed\" value=\"Yes\" ");
  message += oops_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"only_one_pump_speed\" value=\"No\" ");
  message += oops_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td><td></td></tr>");  
  
    
  message += F("<tr><td><small>&nbsp;Hostname or IP Pool Pump: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"text\" name=\"hostname_pmpe\" minlength=\"4\" maxlength=\"48\" value=");
  message += hostname_pumpe;
  message += F("></td><td><em><small>&nbsp;Tasmota Device &nbsp;</em></td></tr>");
 
  message += F("<tr><td><small>&nbsp;Pump follow-up time (min): &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"pumpennachlaufzeit\" min=\"1\" pattern=\"[0-9]*\" value=");
  message += pumpennachlaufzeit;
  message += F("></td><td><em><small>&nbsp;Time to mix the \"Dosage\" (e.g.PHMinus)&nbsp; </em></td></tr>");

  message += F("<tr><td><small>&nbsp;Pump OFF Output: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"pumpe_aus\" value=");
  message += pumpe_aus;
  message += F("></td><td><small><em>&nbsp;Tasmota CH4 Ouput for OFF&nbsp; </em></td></tr>");

  message += F("<tr><td><small>&nbsp;Pump Dosage Output: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"pumpe_dosierung\" value=");
  message += pumpe_dosierung;
  message += F("></td><td><small><em>&nbsp;Tasmota CH4 Ouput for Dosage&nbsp; </em></td></tr>");

  message += F("<tr><td><small>&nbsp;Pump Manually Output: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"pumpe_hand\" value=");
  message += pumpe_hand;
  message += F("></td><td><em><small>&nbsp;Tasmota CH4 Ouput for Manually&nbsp;</em></td></tr>");
//

  message += F("<tr><td><em><center><b>&nbsp;Select Winter-Modus:&nbsp;</b></center></em></td></tr>");

  
  message += F("<tr><td><small>&nbsp;Winter-Modus: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"winter_modus\" value=\"Yes\" ");
  message += winter_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"winter_modus\" value=\"No\" ");
  message += winter_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td><td></td></tr>");  



  

  message += F("<tr><td><small>&nbsp;Pump Temp. Output: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"pumpe_temp\" value=");
  message += pumpe_temp;
  message += F("></td><td><em><small>&nbsp;Tasmota CH4 Ouput for Winter Modus&nbsp;</em></td></tr>");


  message += F("<tr><td><small>&nbsp;Winter-Water-Temp (°C)< &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"winter_temp\" value=");
  message += winter_temp;
  message += F("></td><td><em><small>&nbsp; Switch on Pump if lower&nbsp;</em></td></tr>");

  message += F("<tr><td><small>&nbsp;Winter-Shaft-Temp (°C)< &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"winter_shaft_temp\" value=");
  message += winter_shaft_temp;
  message += F("></td><td><em><small>&nbsp; Switch on Pump if lower&nbsp;</em></td></tr>");
  

  

//
  
  message += F("<tr><td></td><td><b>&nbsp;<br></td><td></td></tr>");
  message += F("<tr><td><em><center><b>&nbsp;Fault Setup:&nbsp;</b></center></em></td></tr>");
  message += F("<tr><td><small>&nbsp;Rebooting (h): &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"reboot_delay\" min=\"0\" pattern=\"[0-9]*\" value=");
  message += reboot_delay;
  message += F("></td><td><em><small>&nbsp;We reboot all \"x\" hours (0=Never)&nbsp;</em></td></tr>");

  message += F("<tr><td><small>&nbsp;PH faults: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"ph_fault\" min=\"0\" pattern=\"[0-9]*\" value=");
  message += ph_fault;
  message += F("></td><td><em><small>&nbsp;reboot after \"x\" reading faults (0=Off)&nbsp;</em></td></tr>");

  message += F("<tr><td><small>&nbsp;ORP faults: &nbsp;</td>");
  message += F("<td><form action=\"/action_page\">&nbsp;<input type=\"number\" name=\"orp_fault\" min=\"0\" pattern=\"[0-9]*\" value=");
  message += orp_fault;
  message += F("></td><td><em><small>&nbsp;reboot after \"x\" reading faults (0=Off)&nbsp;</em></td></tr>");

  
 
  message += F("</table>");
  message += F("<br><br><input type=\"submit\" value=\"Safe\"></form><br></br>"); 
  message += F("</center>");




  message += F("<h2>Switch ON the Pool PUMP manually:</h2>"); 
  message += F("<h1>Handle with care!</h1>");
  message += F("<small>Use this if you want switch ON the PUMP (e.g. wash the filter)</small>"); 
  message += F("<center><table><tr>");
  message += F("<td>Pump Manually ON or Timer (default)&nbsp;&nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_pump_on\" value=\"ON\" ");
  message += check_pump_on;
  message += F(" >");
  message += F("<label for=\"b1\">ON</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_pump_on\" value=\"OFF\" ");
  message += check_pump_off;
  message +=F(" >");
  message += F("<label for=\"b2\">Timer (default)</label>");
  message += F("</div></td>");  
  message += F("<td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Submit\"></td><td></td></tr></form><br></br>"); 
  
  message += F("</table>");
  message += F("</center>");
  
  
 addBottom(message);


  

  
}
