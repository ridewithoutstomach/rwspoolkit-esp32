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

String check_phminus_yes = "checked";  // muss mit server.arg abgefragt werden und zeigt den Button an und ändert send_to_thingspeak
String check_phminus_no = " ";


void handlePagePHMinusPMP(){


if ( check_phminus == true ){
    
    check_phminus_yes = "checked";
    check_phminus_no = " ";
  }
  else{
    check_phminus_yes = " ";
    check_phminus_no = "checked";
  }  
  

String message;

 addTop(message);
  
  message += F("<h2>PH-Minus-Pump</h2>");
  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("<small>PH-Minus-MixTime is the time in which the injected phminus has been completely mixed<br>");
  message += F("<b>ATTENTION: If you set PH-Minus-MixTime too low you're PH-Minus will be overdosed!</b><br>");
  message += F("You should approach the value slowly by observing the mixing time<br><br>");
  message += F("<b>Attention: We only measure and switch ON in the time the pump is ON (set Timer)</b><br><br>"); 
  
  message += F("<center><table><tr>");
  message += F("<td>&nbsp;Activate PH-Minus-Pump: &nbsp;</td>");
  message += F("<td><div class=\"toggle-buttons\">");
  message += F("<form action=\"/action_page\">&nbsp;<input type=\"radio\" id=\"b1\" name=\"check_phminus\" value=\"Yes\" ");
  message += check_phminus_yes;
  message += F(" >");
  message += F("<label for=\"b1\">Yes</label>");
  message += F("<input type=\"radio\" id=\"b2\" name=\"check_phminus\" value=\"No\" ");
  message += check_phminus_no;
  message +=F(" >");
  message += F("<label for=\"b2\">No</label>");
  message += F("</div></td></tr>");  

  message += F("<tr><td>&nbsp;PH-Minus-MixTime (min): &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"check_phMinus_interval_delay\" value= ");
  message += check_phMinus_interval_delay;
  message += F(">&nbsp; > PoolWaterCompleteMixed &nbsp;</td></tr>"); 

  message += F("<tr><td>&nbsp;Hostname/IP PH-MinusPMP: &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"hostname_phminus\"  minlength=\"4\" maxlength=\"48\"  value= ");
  message += hostname_phminus;
  message += F("> &nbsp; Tasomta Device &nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp;PH-Minus volume (Liter):&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"phminus_fuellstand\" pattern=\"[0-9]*[.]{0,}[0-9]{1,2}\" value= ");
  message += phminus_fuellstand;
  message += F("> &nbsp; current canister volume&nbsp;</td></tr>");   

  message += F("<tr><td>&nbsp; PH-Minus-dosage-amount (ml):</td>");
  message += F("<td>&nbsp;<input type=\"text\" name=\"phminus_dosiermenge\" pattern=\"[0-9]*[.]{0,}[0-9]{1,2}\" value= ");
  message += phminus_dosiermenge;
  message += F("> &nbsp; how many ml we dosage per impuls &nbsp;</td></tr>");   
  
  message += F("<tr><td>&nbsp;Double-Checks: &nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"number\" min=\"1\" name=\"phminus_dblchk\" min=\"1\" pattern=\"[0-9]*\" value= ");
  message += phminus_dblchk;
  message += F(">&nbsp; > How often we check before dosing &nbsp;</td></tr>");  
  
  message += F("<tr><td>&nbsp;Password PHMinus:&nbsp;</td>");
  message += F("<td>&nbsp;<input type=\"password\" name=\"password_phminus\" minlength=\"4\" maxlength=\"48\" value= ");
  message += password_phminus;
  message += F("> &nbsp; leave it blank for no password &nbsp;</td></tr>");
    
  message += F("</table>");
  //message += F("<em><b><br>Setting new Values takes around 20s. Please wait..");
  message += F("<br><br><input type=\"submit\" value=\"Submit\"></form><br></br>"); 
  message += F("</center>");
  
  
 addBottom(message);


  
}
