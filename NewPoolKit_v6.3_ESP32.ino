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
// v6.3 - Login-PIN wieder aenderbar ueber Webseite (/pw.htm)
//       - PIN wird in LittleFS gespeichert (/lizenz.cfg), XOR-verschluesselt
//       - Initial-PIN bleibt im Code als Default
// v6.1 - AM2315C wieder aktiviert + AM2315C Timeout-Fix (kein Endlos-Loop mehr)
//       - AM2315C Offset-Kalibrierung (Temp + Humidity)
//
// =====================================================================
//  KONFIGURATION - Was muss wo eingetragen werden:
// =====================================================================
//
//  1) WLAN-Zugangsdaten:
//     -> Ueber die Webseite (/wifi.htm) oder beim Erststart via Soft-AP
//     -> Wird in LittleFS gespeichert (/wifi.dat)
//
//  2) Login-PIN (Initial-Passwort fuer die Webseite):
//     -> Diese Datei, weiter unten (suche nach "HIER LOGIN-PIN EINTRAGEN")
//     -> Kann ueber die Webseite (/pw.htm) geaendert werden
//
//  3) OTA-Passwort (fuer Firmware-Update ueber WLAN):
//     -> Diese Datei, weiter unten (suche nach "ArduinoOTA.setPasswordHash")
//
//  4) Hostname (WiFi + OTA):
//     -> Ueber die Webseite (/wifi.htm) konfigurierbar
//     -> Default: "RWSPoolkit"
//
//  5) Alles andere (ThingSpeak, Timer, Chlorinator, PHMinus, Heater,
//     FlowControl, Pool&Pumpe) wird ueber die Webseite konfiguriert
//     und in LittleFS gespeichert - wie gehabt.
//
//  ACHTUNG: Beim ersten Flashen LittleFS formatieren!
// =====================================================================

#include "AM2315C.h"
#include <iot_cmd.h>
#include <WiFi.h>                                         
#include "ThingSpeak.h"                                          
#include <sequencer4.h>                                          
#include <sequencer1.h>                                          
#include <Ezo_i2c_util.h>                                        
#include <Ezo_i2c.h>                                             
#include <Wire.h>    

// OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// OTA

#include <DNSServer.h>          // v6.3: Captive Portal
#include <NTPClient.h>
#include <HTTPClient.h> // HTTP Client
#include <WebServer.h>  // Webserver
#include <LittleFS.h>          //
#include <uptime_formatter.h>

AM2315C DHT;    // AM2315C
uint8_t count = 0;  // AM2315C

// v6.3: ein Hostname fuer WiFi + OTA (konfigurierbar ueber /wifi.htm)
char hostname[60] {"RWSPoolkit"};

// v6.3: DNS-Server fuer Captive Portal im AP-Modus
const byte DNS_PORT = 53;
DNSServer dnsServer;

unsigned int polling_delay = 2000; // in ms

 
             
WiFiClient client;
WiFiClient client_ts;    // separater Client fuer ThingSpeak  


HTTPClient http;
WebServer server(80); //Server on port 80   // Webserver

WiFiUDP ntpUDP;  

int utcOffsetInSeconds = 3600;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000);  // 60s Update-Intervall intern, Timeout default

// Declaration ################################################################################
 //----------------Fill in your Wi-Fi / ThingSpeak Credentials-------
             
// Declaration von RWS


unsigned long currentMillis = millis();   

// v6.0: modus und lizenz entfernt - immer aktiv
// Login-PIN: Default-Wert, wird beim Boot aus LittleFS ueberschrieben falls gespeichert
bool login = false;
// *** HIER LOGIN-PIN EINTRAGEN (Initial-PIN) ***
char loginpassword[55] {"2203"};
constexpr char key {129};                    // XOR-Key fuer PIN-Verschluesselung
const char* datei_lizenz = "/lizenz.cfg";    // LittleFS-Datei fuer Login-PIN

// alle chars +10
bool send_to_thingspeak = false;
bool thingspeak_stored = send_to_thingspeak;
char thingspeak_delay[22] {"15"};      // Achtung ist in (s) und du musst es im Hauptprogramm in millis umrechen, also * 1000
char myChannelNumber[22] {"1234567"}; 
char myWriteAPIKey[32] {"myWriteAPIKey   "};
char myReadAPIKey[32]  {"myReadAPIKey   "};
char phminusID[12] {"4"};
char flowID[12] {"5"};
char chlorID[12] {"6"};
char pumpID[12] {"7"};
char chlorinatorID[12] {"8"};


const char* datei_thing = "/thing.cfg";

char orp_min[14] {"650"};
char orp_max[14] {"720"};
char phMinus[15] {"7.10"};            // unter  diesem Werten wird der PHMinus eingeschaltet
char phPlus[15] {"6.80"};              // Über  diesem Werten wird der PHMinus eingeschaltet
char temp_min[13] {"25"};
char temp_max[13] {"28"};
char humidity_min[13] {"70"};
char humidity_max[13] {"80"};


bool only_one_pump_speed = false;   // etwas verwirrend -> false bedeutet es ist eine CH1 Pumpe!!
char hostname_pumpe[60] {"PoolPmp"};
char pumpennachlaufzeit[14] {"15"};     // in Minuten ! - muss umgerechnet werden, wie lange soll die Pumpe nachlaufen wenn PH/ORP auslöst
char pumpe_aus[13] {"1"};             // Ausgang am Sonoff CH4 für Pumpe AUS!
char pumpe_dosierung[13] {"1"};       //Pumpenstufe die für Dosierung Chlor gewäht wird
char pumpe_hand[13] {"1"};
String check_pump_on = "";
String check_pump_off = "checked";

bool winter_modus = false;
char pumpe_temp[13] {"1"};
char winter_temp[13] {"2"};
char winter_shaft_temp[13] {"2"};


char reboot_delay[14] {"0"};  // in Stunden! 0 = kein auto-reboot
unsigned long previousMillis_reboot = 0;  


char orp_fault[16] = {"0"};
int orp_fault_counter = 0;
char ph_fault[16] = {"0"};
int ph_fault_counter = 0;

const char* datei_pool = "/pool.cfg";




bool check_flow  = false;
char flowcontrol_delay[15] {"2"};                   // in Secunden - Muss umgerechnet werden
char hostname_flowcontrol[60] {"flowcontrol_shelly"}; 
char password_flowcontrol[60] {""};  
const char* datei_flow = "/flow.cfg";

bool check_chlorinator = false;
unsigned int orp_chk_counter = 0;
unsigned int orp_chk_counter_read = 0;
char orp_dblchk[16] {"15"}; 
char check_orp_interval_delay[15] {"20"};            // Sekunden!!  - muss umgerechnet werden
//char check_orp_interval_delay_std[15] {"20"};            // Sekunden!!  - muss umgerechnet werden
char ChlorInterval[15] {"30"};                      // minuten
unsigned int ChlorInterval_counter = 0;
char hostname_chlorinator[60] {"chlorinator_shelly"};
const char* datei_chlorinator = "/chlorinator.cfg";

bool check_phminus = false;
char phminus_dblchk[15] {"4"};
unsigned int phminus_dblchk_counter = 0;
unsigned int phminus_dblchk_counter_read = 0;         // Falls zwischendurch rebootet wird:
char check_phMinus_interval_delay[15] {"15"};    // 15 Minuten!!
//char check_phMinus_interval_delay_std[15] {"15"};    // 15 Minuten!!
char hostname_phminus[60] {"phminusPmP"};
char phminus_fuellstand[16] {"0.00"};
char phminus_dosiermenge[15] {"0.00"};
char password_phminus[60] {""};
const char* datei_phminuspmp = "/phminuspmp.cfg";

bool check_heizung = false;
bool check_fan = false;
char check_temp_interval_delay[14] {"20"};    // Sekunden
char hostname_heater[50] {"Heater"};
int temp_hyst = 0;
int humidity_hyst = 0;

// v6.1: AM2315C Offset-Kalibrierung
float dht_temp_offset = 0.0;    // Temperatur-Offset in °C
float dht_hum_offset = 0.0;     // Feuchtigkeits-Offset in % rF
const char* datei_dht_cal = "/dhtcal.cfg";

// Helper: kalibrierte Werte vom AM2315C
float dht_temp() { return DHT.getTemperature() + dht_temp_offset; }
float dht_hum()  { return DHT.getHumidity() + dht_hum_offset; }

char password_heater[60] {""};
char hostname_fan[50] {"Fan"};
char password_fan[60] {""};
const char* datei_heater = "/heater.cfg";




bool summer = false;
unsigned long standard_timer_interval_delay = 30000; //(0,5m)

// v6.0 Tasmota-Style Timer: 24 Timer, jeder mit Zeit + Speed + Aktiv
#define TIMER_COUNT 24
struct TimerEntry {
  char zeit[6];       // "HH:MM" (5 Zeichen + Null)
  uint8_t speed;      // 1-4 (1 = Pumpe aus, 2-4 = Speed auf CH4)
  bool aktiv;         // true = Timer aktiv
};
TimerEntry timers[TIMER_COUNT];
unsigned int timer_minuten[TIMER_COUNT];  // zeit umgerechnet in Minuten

// Timer-Defaults setzen (wird in setup vor read_timer aufgerufen)
void init_timers() {
  for (int i = 0; i < TIMER_COUNT; i++) {
    strcpy(timers[i].zeit, "00:00");
    timers[i].speed = 1;
    timers[i].aktiv = false;
    timer_minuten[i] = 0;
  }
}




const char* datei_timer = "/timer.cfg";

bool pumpe_on = false;
bool zeit_on = false;    // nicht in config
bool orp_on = false;      // nicht in config
bool flow = false;       // Intitalisierung Abfrage Flow wird nicht in config.sys benötigt
String oops = "1";      // ->  wenn 1 dann haben wir eine Mehrkanal Pumpe und schalten ein 4ch Sonoff
                        // -> wenn 0 dann haben wir eine normale Pumpe und schalten any Device ON/Off


int chlor_hyst = 0;  // wird benötigt um den chlorinator innerhalb der Werte ORP einzuschalten:
//int temp_hyst = 0;


// Zeiten  1min=60000 15min=900.000  30min=1.800.000
unsigned long previousMillis = 0;  // brauch ich für die Pumpensteuerung 
unsigned long previousMillisFlow = 0;  // brauch ich für die Flowteuerung 
unsigned long previousMillis_check_ORP = 0;  
unsigned long previousMillis_thingspeak = 0; 
unsigned long previousMillis_phminus = 0;  
unsigned long previousMillisChlorInterval = 0;
unsigned long previousMillis_check_heater = 0;
unsigned long previousMillis_check_humidity = 0;



//unsigned long check_cpmp;
unsigned long check_phM;
unsigned long check_phP;

// Achtung timer_interval_delay darf niemals kleiner orp_delay oder ph_delay sein
unsigned long timer_interval_delay = standard_timer_interval_delay;

// Failure-Cache: nach fehlgeschlagener Verbindung 5 Min Pause
const unsigned long HOST_RETRY_DELAY = 300000;  // 5 Minuten in ms
unsigned long pumpe_fail_time = 0;
unsigned long chlorinator_fail_time = 0;
unsigned long heater_fail_time = 0;
unsigned long fan_fail_time = 0;
unsigned long phminus_fail_time = 0;
unsigned long flowcontrol_fail_time = 0;



// RWS



// EZO Board ##########################################################################################

Ezo_board PH = Ezo_board(99, "PH");           //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board ORP = Ezo_board(98, "ORP");         //create an ORP circuit object who's address is 98 and name is "ORP"
Ezo_board RTD = Ezo_board(102, "RTD");        //create an RTD circuit object who's address is 102 and name is "RTD"
Ezo_board PMPL = Ezo_board(109, "PMPL");      //create an PMPL circuit object who's address is 109 and name is "PMPL"

Ezo_board device_list[] = {   //an array of boards used for sending commands to all or specific boards
  PH,
  ORP,
  RTD,
  PMPL
};

Ezo_board* default_board = &device_list[0]; //used to store the board were talking to

//gets the length of the array automatically so we dont have to change the number every time we add new boards
const uint8_t device_list_len = sizeof(device_list) / sizeof(device_list[0]);

//enable pins for each circuit
const int EN_PH = 12;
const int EN_ORP = 27;
const int EN_RTD = 15;
const int EN_AUX = 33;


const unsigned long reading_delay = 1000;                 //how long we wait to receive a response, in milliseconds
//const unsigned long thingspeak_delay = 15000;             //how long we wait to send values to thingspeak, in milliseconds

//unsigned int poll_delay = polling_delay  - reading_delay * 2 + 300; //how long to wait between polls after accounting for the times it takes to send readings
// FIX: Overflow-Schutz - wenn polling_delay zu klein ist, mindestens 0
unsigned int poll_delay = (polling_delay > reading_delay * 2 + 300) ? (polling_delay - reading_delay * 2 - 300) : 0;

//parameters for setting the pump output
#define PUMP_BOARD        PMPL      //the pump that will do the output (if theres more than one)
#define PUMP_DOSE         10        //the dose that the pump will dispense in  milliliters
#define EZO_BOARD         PH        //the circuit that will be the target of comparison
#define IS_GREATER_THAN   true      //true means the circuit's reading has to be greater than the comparison value, false mean it has to be less than
#define COMPARISON_VALUE  7         //the threshold above or below which the pump is activated

float k_val = 0;                                          //holds the k value for determining what to print in the help menu

// Achtung:
// wird beides über die Config ausgelesen
bool polling  = false;                                     //variable to determine whether or not were polling the circuits
//bool send_to_thingspeak = true;                           //variable to determine whether or not were sending data to thingspeak

bool wifi_isconnected() {                           //function to check if wifi is connected
  return (WiFi.status() == WL_CONNECTED);
}

/*
void reconnect_wifi() {                                   //function to reconnect wifi if its not connected
  if (!wifi_isconnected()) {
    WiFi.begin(ssid, pass);
    Serial.println("connecting to wifi");
  }
}
*/

void thingspeak_send() {
  if (send_to_thingspeak == true) {                                                    //if we're datalogging
    if (wifi_isconnected()) {
      int return_code = ThingSpeak.writeFields(atol(myChannelNumber), myWriteAPIKey);
      if (return_code == 200) {                                                          //code for successful transmission
        Serial.println("sent to thingspeak");
      } else {
        Serial.println("couldnt send to thingspeak");
      }
    }
  }
}

void step1();      //forward declarations of functions to use them in the sequencer before defining them
void step2();
void step3();
void step4();
Sequencer4 Seq(&step1, reading_delay,   //calls the steps in sequence with time in between them
               &step2, 300,
               &step3, reading_delay,
               &step4, poll_delay);

//Sequencer1 Wifi_Seq(&reconnect_wifi, 10000);  //calls the wifi reconnect function every 10 seconds

Sequencer1 Thingspeak_seq(&thingspeak_send, atol(thingspeak_delay) * 1000); //sends data to thingspeak with the time determined by thingspeak delay
Sequencer1 Timer_seq(&timer, 30000); //sends data to thingspeak with the time determined by thingspeak delay
Sequencer1 check_flowcontrol(&check_flowcontrol_switch, 60 * 1000);





// END EZO Board ############################################################################


// END Declaration ########################################################################


void setup() {
  Serial.begin(9600);                     //start the Serial communication to the computer

  
  delay(1000);

  DHT.begin();    //  AM2315C
  
// LittleFS
 if(LittleFS.begin(true)){
    Serial.println("Filessystem init");
  }else{
    Serial.println("Error Filesystem");
  }

  File root = LittleFS.open("/");
  File f = root.openNextFile();
  while (f) {
    Serial.println("");
    Serial.print("File: ");
    Serial.print(f.name());
    Serial.println(f.size());
    f = root.openNextFile();
  }
// LittleFS    

connectWifi();

only_one_pump_speed = false;
send_to_thingspeak = false;
winter_modus = false;


//Alles auslesen!

// v6.3: Login-PIN aus LittleFS laden (falls vorhanden, sonst bleibt Default)
read_lizenz();
read_thing();
thingspeak_stored = send_to_thingspeak;
read_pool();
read_flow();
read_chlorinator();
read_phminuspmp();
read_dht_cal();   // v6.1: AM2315C Offset laden
init_timers();    // v6.0: Defaults setzen bevor Config geladen wird
read_timer();
read_heater();

// v6.0: polling is always on
polling = true;
Serial.println("v6.3: polling=TRUE (always on)");




Serial.print("polling_delay: ");
Serial.println(polling_delay);
Serial.print("poll_delay: ");
Serial.println(poll_delay);




Serial.print("PHMinus DoubleCheckCounterRead: ");
Serial.println(phminus_dblchk_counter_read);
Serial.print("ORP DoubleCheckCounter: ");
Serial.println(orp_chk_counter_read);


if ( summer == true ){
  utcOffsetInSeconds = 7200;
  Serial.println("Summertime");
}
else{
  utcOffsetInSeconds = 3600;  
  Serial.println("Wintertime");
}

// haben wir eine 3Speed Pumpe ?

if ( only_one_pump_speed == false ){
          
          oops = "0";  // anscheinedn ist es eine normale Pumpe und es wird kein CH4 Sonoff geschaltet
          Serial.println(" ---------  Only One Speed Pump ----  Only One Speed Pump  ----");
      }
if ( only_one_pump_speed == true ){
          
          oops = "1";  // anscheinedn ist es eine normale Pumpe und es wird kein CH4 Sonoff geschaltet
          Serial.println(" ---------  CH4-Pump ---------  CH4-Pump ----");
      }


// Webseiten
  server.begin();                     //Start server
  Serial.println("HTTP server started");

  // Generiere Webseiten
  
  server.on("/", handleRoot);      //Which routine to handle at root location
  
  server.on("/action_page", handleForm);
  server.on("/style.css", handleCss);      
  // v6.3: WiFi + PIN Seiten
  server.on("/wifi.htm", handleWifi);
  server.on("/pw.htm", handlePassword);
  server.on("/thingspeak.htm", handlePageThingspeak);
  server.on("/flow.htm", handlePageFlow);
  server.on("/pool.htm", handlePagePool);
  server.on("/chlorinator.htm", handlePageChlor);
  server.on("/phminus.htm", handlePagePHMinusPMP);
  server.on("/heater.htm", handlePageHeizung);
  server.on("/timer.htm", handlePageTimer);
  // server.on("/stats.htm", handleStats);  // v6.0: Stats entfernt (identisch mit Dashboard)
  server.on("/calibration.htm", handleCal);
  server.on("/reboot.htm", handleReboot);
  server.on("/logout.htm", handleLogout);
  
  
  /// Webserver Seiten  END


// Webseiten


      
  


  // EZO Board  
  pinMode(EN_PH, OUTPUT);                                                         //set enable pins as outputs
  pinMode(EN_ORP, OUTPUT);
  pinMode(EN_RTD, OUTPUT);
  pinMode(EN_AUX, OUTPUT);
  digitalWrite(EN_PH, LOW);                                                       //set enable pins to enable the circuits
  digitalWrite(EN_ORP, LOW);
  digitalWrite(EN_RTD, HIGH);
  digitalWrite(EN_AUX, LOW);

  Wire.begin();                           //start the I2C
 
  //WiFi.mode(WIFI_STA);                    //set ESP8266 mode as a station to be connected to wifi network
  //WiFi.hostname(hostname_ww);

  client.setTimeout(1000);                //TCP-Timeout auf 1s reduzieren (statt 5s Default)
  ThingSpeak.begin(client_ts);             //enable ThingSpeak connection (eigener Client)
  timeClient.begin();                     // v6.0: NTP Client starten
//  Wifi_Seq.reset();                       //initialize the sequencers
  Seq.reset();
  Thingspeak_seq.reset();
  Timer_seq.reset();
  check_flowcontrol.reset(); 
  
  
  // END EZO Board
     
        
   // Alles aus




  
  // OTA
  //WiFi.begin(ssid,pass);
  //reconnect_wifi();
  
  // v6.3: Hostname aus WiFi-Config (wird auch fuer OTA verwendet)
  ArduinoOTA.setHostname(hostname);
   // OTA password: Enter your MD5 hash below. Generate it with:
   //   Linux/Mac:  echo -n "YourPassword" | md5sum
   //   Online:     search for "MD5 Hash Generator"
   ArduinoOTA.setPasswordHash("9622984caa8f4a3c86df3d357fdfafa5");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  delay(1000);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // END OTA

  // End OTA

     // Serial.println("up " + uptime_formatter::getUptime());   
        
        Serial.println("");
        Serial.print("Rebooting..");
        int x = 0;
        // Achtung 20
        while ( x <= 20){ 
         x ++;
         Serial.print(".");
         delay(500);
         }
        Serial.println("");


        
        alles_aus();
        Serial.println("");
        Serial.print("PHMinus - Level: ");
        Serial.println(phminus_fuellstand);
        


    
 
}
// END Setup ####################################################################################

void loop() {

 
    
  ///Serial.println(ESP.getFreeHeap());  // Debugging
  //delay(1000);
  // OTA
  ArduinoOTA.handle();
  dnsServer.processNextRequest();  // v6.3: Captive Portal (nur aktiv im AP-Modus)
  reStation();
  
  server.handleClient();                  //Handle client requests   // Webserver


   

  // END OTA

  // EZO ##########################################################################################
  String cmd;                            //variable to hold commands we send to the kit

//  Wifi_Seq.run();                        //run the sequncer to do the polling

  if (receive_command(cmd)) {            //if we sent the kit a command it gets put into the cmd variable
    polling = false;                     //we stop polling
    send_to_thingspeak = false;          //and sending data to thingspeak
    if (!process_coms(cmd)) {            //then we evaluate the cmd for kit specific commands
      process_command(cmd, device_list, device_list_len, default_board);    //then if its not kit specific, pass the cmd to the IOT command processing function
    }
  }

  // v6.0 Status output every 10s
  static unsigned long debug_last_print = 0;
  if (millis() - debug_last_print >= 10000) {
    debug_last_print = millis();
    Serial.println("");
    Serial.println("--- v6.3 LOOP ---");
    Serial.print("polling=TRUE");
    Serial.print(" ph_fault_counter=");
    Serial.print(ph_fault_counter);
    Serial.print(" orp_fault_counter=");
    Serial.print(orp_fault_counter);
    Serial.print(" heap=");
    Serial.println(ESP.getFreeHeap());
    // v6.1: Nicht-blockierender AM2315C Read mit Timeout
    int dht_rc = DHT.requestData();
    if (dht_rc == 0) {
      unsigned long dht_start = millis();
      while (DHT.isMeasuring()) {
        if (millis() - dht_start > 100) {  // 100ms Timeout
          Serial.println("AM2315C: TIMEOUT!");
          dht_rc = -99;
          break;
        }
        yield();
      }
      if (dht_rc != -99) {
        dht_rc = DHT.readData();
        if (dht_rc > 0) DHT.convert();
      }
    }
    if (dht_rc < 0 && dht_rc != AM2315C_ERROR_LASTREAD) {
      Serial.print("AM2315C Fehler: ");
      Serial.println(dht_rc);
    }
    Serial.print("Shaft-Temp=");
    Serial.print(dht_temp(), 1);
    Serial.print("C Humidity=");
    Serial.print(dht_hum(), 1);
    Serial.println("%");
    Serial.println("--- v6.3 LOOP END ---");
  }

  if (polling == true) {                 //if polling is turned on, run the sequencer
    Seq.run();
    Thingspeak_seq.run();
  }
  // END EZO #######################################################################################
  Timer_seq.run();

  
  if (check_flow) check_flowcontrol.run();

     // Reboot 
     currentMillis = millis();
     if ( atol(reboot_delay) > 0) {
         if (currentMillis - previousMillis_reboot >= atol(reboot_delay) * 3600000UL ) { 
             previousMillis_reboot = millis();
              ESP.restart();
         }
     }

    // Reboot after Reading faults!

    
    // Achtung
    
    
    
    
    


  // v6.0: modus always true - no condition needed
  {
        // Fault-Check (nur wenn Fault-Ueberwachung aktiv, also > 0)
        if ( (atol(ph_fault) > 0 && ph_fault_counter > 0) || (atol(orp_fault) > 0 && orp_fault_counter > 0) ) {
          Serial.print("DEBUG fault: ph_fault_counter=");
          Serial.print(ph_fault_counter);
          Serial.print("/");
          Serial.print(atol(ph_fault));
          Serial.print(" | orp_fault_counter=");
          Serial.print(orp_fault_counter);
          Serial.print("/");
          Serial.println(atol(orp_fault));
        }

        if ( (atol(ph_fault) > 0 && ph_fault_counter >= atol(ph_fault)) || (atol(orp_fault) > 0 && orp_fault_counter >= atol(orp_fault)) ){
        Serial.println("");
        Serial.println("!!! REBOOTING after getting too much reading faults! !!!");
        Serial.println("");
        delay(200);
        ESP.restart();
      
    }

        
      if (thingspeak_stored != send_to_thingspeak){
        send_to_thingspeak = thingspeak_stored;
     }


     
      //##    
      if ( check_chlorinator ){
         currentMillis = millis();
         if (currentMillis - previousMillis_check_ORP >= atol(check_orp_interval_delay) * 1000) { 
             previousMillis_check_ORP = millis();
              check_orp(); 
         }
      }
      //##

      //##
       if ( check_phminus ){
              
         check_phM = atol(check_phMinus_interval_delay) * 1000 * 60;
         
         currentMillis = millis();
         if (currentMillis - previousMillis_phminus >= check_phM) { 
             previousMillis_phminus = millis();
             check_phMinus();
             ThingSpeak.setField(String(phminusID).toInt(), phminus_fuellstand);  
         } 
      }
      //##


      //##
       if ( check_heizung ){
         currentMillis = millis();
         if (currentMillis - previousMillis_check_heater >= atol(check_temp_interval_delay) * 1000) {
             previousMillis_check_heater = millis();
              check_temp();
         }
       }
       else{  // heizung aus - nur einmal ausfuehren, nicht bei jedem Loop!
            temp_hyst = 0;
       }
      // ##


      // ##
       if ( check_fan ){
         currentMillis = millis();
         if (currentMillis - previousMillis_check_humidity >=  5000) {
              previousMillis_check_humidity = millis();
              check_temp();
         }
       }
       else{  // fan aus - nur einmal ausfuehren, nicht bei jedem Loop!
           humidity_hyst = 0;
        }
       // ##
      

      
     

 



 }   // End Modus Abfrage

  

  
}

// END Loop ###################################################

// Cached connect: versucht Verbindung nur wenn der Host nicht kuerzlich fehlgeschlagen ist
// v6.3: Fallback mit .local-Suffix fuer mDNS-Aufloesung
bool cached_connect(const char* host, int port, unsigned long &fail_time) {
  if (fail_time > 0 && millis() - fail_time < HOST_RETRY_DELAY) {
    return false;  // Host war kuerzlich nicht erreichbar
  }
  client.stop();  // vorherige Verbindung sauber schliessen
  if (client.connect(host, port)) {
    return true;
  }
  // Fallback: hostname.local versuchen (mDNS)
  String mdns_host = String(host) + ".local";
  if (client.connect(mdns_host.c_str(), port)) {
    Serial.print("mDNS: ");
    Serial.println(mdns_host);
    return true;
  }
  fail_time = millis();
  Serial.print("connection failed (cached 5min): ");
  Serial.println(host);
  return false;
}

//function that controls the pumps activation and output
void pump_function(Ezo_board &pump, Ezo_board &sensor, float value, float dose, bool greater_than) {
  if (sensor.get_error() == Ezo_board::SUCCESS) {                    //make sure we have a valid reading before we make any decisions
    bool comparison = false;                                        //variable for holding the reuslt of the comparison
    if (greater_than) {                                             //we do different comparisons depending on what the user wants
      comparison = (sensor.get_last_received_reading() >= value);   //compare the reading of the circuit to the comparison value to determine whether we actiavte the pump
    } else {
      comparison = (sensor.get_last_received_reading() <= value);
    }
    if (comparison) {                                               //if the result of the comparison means we should activate the pump
      pump.send_cmd_with_num("d,", dose);                           //dispense the dose
      delay(100);                                                   //wait a few milliseconds before getting pump results
      Serial.print(pump.get_name());                                //get pump data to tell the user if the command was received successfully
      Serial.print(" ");
      char response[20];
      if (pump.receive_cmd(response, 20) == Ezo_board::SUCCESS) {
        Serial.print("pump dispensed ");
      } else {
        Serial.print("pump error ");
      }
      Serial.println(response);
    } else {
      pump.send_cmd("x");                                          //if we're not supposed to dispense, stop the pump
    }
  }
}

void step1() {
  //send a read command. we use this command instead of RTD.send_cmd("R");
  //to let the library know to parse the reading
  RTD.send_read_cmd();
}

void step2() {
  receive_and_print_reading(RTD);             //get the reading from the RTD circuit

  if ((RTD.get_error() == Ezo_board::SUCCESS) && (RTD.get_last_received_reading() > -1000.0)) { //if the temperature reading has been received and it is valid
    PH.send_cmd_with_num("T,", RTD.get_last_received_reading());
    ThingSpeak.setField(3, String(RTD.get_last_received_reading(), 2));                 //assign temperature readings to the third column of thingspeak channel
  } else {                                                                                      //if the temperature reading is invalid
    PH.send_cmd_with_num("T,", 25.0);                                                           //send default temp = 25 deg C to PH sensor
    ThingSpeak.setField(3, String(25.0, 2));                 //assign temperature readings to the third column of thingspeak channel
  }

  Serial.print(" ");
}

void step3() {
  //send a read command. we use this command instead of PH.send_cmd("R");
  //to let the library know to parse the reading
  PH.send_read_cmd();
  ORP.send_read_cmd();
}

void step4() {
  receive_and_print_reading(PH);             //get the reading from the PH circuit
  if (PH.get_error() == Ezo_board::SUCCESS) {                                          //if the PH reading was successful (back in step 1)
    ThingSpeak.setField(1, String(PH.get_last_received_reading(), 2));                 //assign PH readings to the first column of thingspeak channel
    ph_fault_counter = 0;
  }
  else if (atol(ph_fault) > 0) {
    Serial.println("----- PH Reading fault! ---- ");
    ph_fault_counter ++;
  }
  Serial.print("  ");
  receive_and_print_reading(ORP);             //get the reading from the ORP circuit
  if (ORP.get_error() == Ezo_board::SUCCESS) {                                          //if the ORP reading was successful (back in step 1)
    ThingSpeak.setField(2, String(ORP.get_last_received_reading(), 0));                 //assign ORP readings to the second column of thingspeak channel
    orp_fault_counter = 0;
  }
  else if (atol(orp_fault) > 0) {
    Serial.println("----- ORP Reading fault! ---- ");
    orp_fault_counter ++;
  }
  Serial.println();
  pump_function(PUMP_BOARD, EZO_BOARD, COMPARISON_VALUE, PUMP_DOSE, IS_GREATER_THAN);
}

void start_datalogging() {
  polling = true;                                                 //set poll to true to start the polling loop
  
  send_to_thingspeak = true;
  Thingspeak_seq.reset();
}

bool process_coms(const String &string_buffer) {      //function to process commands that manipulate global variables and are specifc to certain kits
  if (string_buffer == "HELP") {
    print_help();
    return true;
  }
  else if (string_buffer.startsWith("DATALOG")) {
    start_datalogging();
    return true;
  }
  else if (string_buffer.startsWith("POLL")) {
    polling = true;
    Seq.reset();

    int16_t index = string_buffer.indexOf(',');                    //check if were passing a polling delay parameter
    if (index != -1) {                                              //if there is a polling delay
      float new_delay = string_buffer.substring(index + 1).toFloat(); //turn it into a float

      float mintime = reading_delay * 2 + 300;
      if (new_delay >= (mintime / 1000.0)) {                                     //make sure its greater than our minimum time
        Seq.set_step4_time((new_delay * 1000.0) - mintime);          //convert to milliseconds and remove the reading delay from our wait
      } else {
        Serial.println("delay too short");                          //print an error if the polling time isnt valid
      }
    }
    return true;
  }
  return false;                         //return false if the command is not in the list, so we can scan the other list or pass it to the circuit
}

void print_help() {
  Serial.println(F("Atlas Scientific I2C pool kit                                              "));
  Serial.println(F("Commands:                                                                  "));
  Serial.println(F("datalog      Takes readings of all sensors every 15 sec send to thingspeak "));
  Serial.println(F("             Entering any commands stops datalog mode.                     "));
  Serial.println(F("poll         Takes readings continuously of all sensors                    "));
  Serial.println(F("                                                                           "));
  Serial.println(F("ph:cal,mid,7     calibrate to pH 7                                         "));
  Serial.println(F("ph:cal,low,4     calibrate to pH 4                                         "));
  Serial.println(F("ph:cal,high,10   calibrate to pH 10                                        "));
  Serial.println(F("ph:cal,clear     clear calibration                                         "));
  Serial.println(F("                                                                           "));
  Serial.println(F("orp:cal,225          calibrate orp probe to 225mV                          "));
  Serial.println(F("orp:cal,clear        clear calibration                                     "));
  Serial.println(F("                                                                           "));
  Serial.println(F("rtd:cal,t            calibrate the temp probe to any temp value            "));
  Serial.println(F("                     t= the temperature you have chosen                    "));
  Serial.println(F("rtd:cal,clear        clear calibration                                     "));
}
