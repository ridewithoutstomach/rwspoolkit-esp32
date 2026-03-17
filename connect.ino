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

// v6.3: WiFi-Credentials + Hostname in LittleFS gespeichert
char wifi_ssid[33] {""};
char wifi_pass[65] {""};
bool ap_mode = false;

IPAddress apIP(172, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


void write_wifi(){
        Serial.println("Write WiFi");
        File file = LittleFS.open("/wifi.dat", "w");
        if (!file) { Serial.println("LittleFS write error: wifi"); return; }
        delay(100);
        size_t len_s = strlen(wifi_ssid);
        size_t len_p = strlen(wifi_pass);
        size_t len_h = strlen(hostname);
        for (size_t i = 0; i < len_s; i++) wifi_ssid[i] ^= key;
        for (size_t i = 0; i < len_p; i++) wifi_pass[i] ^= key;
        for (size_t i = 0; i < len_h; i++) hostname[i] ^= key;
        file.write(reinterpret_cast<uint8_t*>(&wifi_ssid), sizeof(wifi_ssid));
        file.write(reinterpret_cast<uint8_t*>(&wifi_pass), sizeof(wifi_pass));
        file.write(reinterpret_cast<uint8_t*>(&hostname), sizeof(hostname));
        file.close();
        for (size_t i = 0; i < len_s; i++) wifi_ssid[i] ^= key;
        for (size_t i = 0; i < len_p; i++) wifi_pass[i] ^= key;
        for (size_t i = 0; i < len_h; i++) hostname[i] ^= key;
}

void read_wifi(){
        File file = LittleFS.open("/wifi.dat", "r");
        if ( file ){
            Serial.println("Read WiFi");
            delay(100);
            file.read(reinterpret_cast<uint8_t*>(&wifi_ssid), sizeof(wifi_ssid));
            file.read(reinterpret_cast<uint8_t*>(&wifi_pass), sizeof(wifi_pass));
            file.read(reinterpret_cast<uint8_t*>(&hostname), sizeof(hostname));
            file.close();
            for (size_t i = 0; i < strlen(wifi_ssid); i++) wifi_ssid[i] ^= key;
            for (size_t i = 0; i < strlen(wifi_pass); i++) wifi_pass[i] ^= key;
            for (size_t i = 0; i < strlen(hostname); i++) hostname[i] ^= key;
            Serial.print("  SSID: "); Serial.println(wifi_ssid);
            Serial.print("  Hostname: "); Serial.println(hostname);
        }
}


void startAP() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  Serial.println("\nStarte Soft AP");
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP("Soft AP RWSPoolKit");
  dnsServer.start(DNS_PORT, "*", apIP);
  ap_mode = true;
  server.on("/wifisave", HTTP_POST, handleWifiSave);
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}


void handleWifiSave() {
  if (server.hasArg("wifi_ssid") && server.hasArg("wifi_pass") && server.hasArg("copyright")) {
    strcpy(wifi_ssid, server.arg("wifi_ssid").c_str());
    strcpy(wifi_pass, server.arg("wifi_pass").c_str());
    if (server.hasArg("hostname")) {
      strcpy(hostname, server.arg("hostname").c_str());
    }
    String s = F("<body style='background:black;color:white;text-align:center;font-family:sans-serif'>"
                 "<br><h1>WiFi gespeichert - Rebooting...</h1>");
    server.send(200, "text/html", s);
    server.client().flush();
    delay(100);
    write_wifi();
    delay(500);
    ESP.restart();
  } else {
    String s = F("<body style='background:black;color:white;text-align:center;font-family:sans-serif'>"
                 "<br><h1><a href='/' style='color:red'>Copyright muss akzeptiert werden!</a></h1>");
    server.send(200, "text/html", s);
  }
}


void connectWifi() {
  read_wifi();

  if (strlen(wifi_ssid) == 0) {
    Serial.println("Kein WiFi gespeichert - starte AP");
    startAP();
    return;
  }

  WiFi.disconnect();
  // WiFi.persistent(false);  // nicht verfuegbar auf ESP32
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);  // ESP32: setHostname() statt hostname()
  WiFi.begin(wifi_ssid, wifi_pass);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 0);
    delay(500);
    digitalWrite(LED_BUILTIN, 1);
    delay(500);
    Serial.printf(" %i sek\n", ++i);
    if (WiFi.status() == WL_NO_SSID_AVAIL || i > 30) {
      Serial.println("\nWiFi Verbindung fehlgeschlagen - starte AP");
      startAP();
      return;
    }
  }
  ap_mode = false;
  Serial.printf("\nVerbunden mit: %s\nIP: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}


void reStation() {
  static unsigned long previousMillis2;
  constexpr unsigned long INTERVAL (3e5);               // 5 Minuten
  if (millis() - previousMillis2 >= INTERVAL) {
    Serial.println("\n check Wifi Connection\n ");
    previousMillis2 += INTERVAL;
    if (WiFi.status() != WL_CONNECTED) connectWifi();
  }
}
