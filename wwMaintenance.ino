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
// EZO Maintenance: I2C-Scan + UART->I2C Switch fuer PH/ORP/RTD/AUX Slot.
// Wird nur bei eingeloggtem User freigegeben.

// Puffer fuer die letzte Scan-Ausgabe, damit sie auch nach Reload sichtbar bleibt
String last_i2c_scan_result = "";

// Slot-Tabelle: Name, Enable-Pin, Invert-Flag (true=HIGH enabled, false=LOW enabled),
// Default I2C-Adresse fuer Atlas EZO.
struct EzoSlotDef {
  const char* name;
  int         en_pin;
  bool        invert;
  uint8_t     default_addr;
};
const EzoSlotDef EZO_SLOTS[] = {
  { "PH",  EN_PH,  false,  99 },
  { "ORP", EN_ORP, false,  98 },
  { "RTD", EN_RTD, true,  102 },
  { "AUX", EN_AUX, false,  99 },
};
const int EZO_SLOT_COUNT = sizeof(EZO_SLOTS) / sizeof(EZO_SLOTS[0]);

// ESP32: default I2C-Pins = SDA 21, SCL 22 (siehe Wire.begin() ohne Argumente)
// Diese Pins werden fuer SoftwareSerial verwendet, weil EZO UART und I2C
// physikalisch auf denselben Pads am Circuit liegen.
#define EZO_UART_RX_PIN 21   // = SDA
#define EZO_UART_TX_PIN 22   // = SCL


// Alle EZO-Slots abschalten
static void maint_allSlotsOff() {
  digitalWrite(EN_PH,  HIGH);
  digitalWrite(EN_ORP, HIGH);
  digitalWrite(EN_RTD, LOW);
  digitalWrite(EN_AUX, HIGH);
}

// Genau einen Slot einschalten, alle anderen aus
static void maint_enableOnly(int en_pin, bool invert) {
  maint_allSlotsOff();
  delay(500);
  digitalWrite(en_pin, invert ? HIGH : LOW);
  delay(1500);
}

// Standard-Slotbelegung wiederherstellen (identisch zu setup())
static void maint_restoreAllSlotsOn() {
  digitalWrite(EN_PH,  LOW);
  digitalWrite(EN_ORP, LOW);
  digitalWrite(EN_RTD, HIGH);
  digitalWrite(EN_AUX, LOW);
}


// I2C-Bus-Scan - baut einen HTML-Report und speichert ihn in last_i2c_scan_result
void maint_i2c_scan() {
  String s;
  s += F("<pre>");
  s += F("I2C Bus Scan\n");
  s += F("----------------------------------------\n");

  maint_restoreAllSlotsOn();
  delay(1500);

  Wire.begin();
  delay(200);

  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      found++;
      s += F("  [OK] 0x");
      if (addr < 16) s += F("0");
      s += String(addr, HEX);
      s += F(" (");
      s += String((int)addr);
      s += F(")  ");
      switch (addr) {
        case 56:  s += F("AM2315C (Temp/Humidity)"); break;
        case 97:  s += F("EZO DO");  break;
        case 98:  s += F("EZO ORP"); break;
        case 99:  s += F("EZO PH");  break;
        case 100: s += F("EZO EC");  break;
        case 102: s += F("EZO RTD"); break;
        case 109: s += F("EZO PMP"); break;
        default:  s += F("unknown"); break;
      }
      s += F("\n");
    }
  }
  s += F("----------------------------------------\n");
  s += String(found);
  s += F(" device(s) found\n");
  s += F("</pre>");

  last_i2c_scan_result = s;
  Serial.println("Maintenance I2C scan done");
}


// UART -> I2C Switch fuer einen Slot.
// Gibt true zurueck wenn der Switch-Befehl erfolgreich gesendet wurde (ESP sollte rebooten),
// false wenn kein UART-Response kam (Circuit schon I2C oder nicht gesteckt).
// Das HTML-Log wird in 'out' angehaengt.
bool maint_switch_to_i2c(int slot_index, String &out) {
  if (slot_index < 0 || slot_index >= EZO_SLOT_COUNT) {
    out += F("<p><b>ERROR:</b> invalid slot index</p>");
    return false;
  }
  const EzoSlotDef &slot = EZO_SLOTS[slot_index];

  out += F("<pre>");
  out += F("Switching slot ");
  out += slot.name;
  out += F(" -> I2C address ");
  out += String((int)slot.default_addr);
  out += F("\n----------------------------------------\n");

  // 1) Polling stoppen, damit Seq nicht mehr in den Bus gräetscht
  polling = false;
  send_to_thingspeak = false;
  delay(500);

  // 2) Nur Zielslot aktiv, alle anderen stumm
  maint_enableOnly(slot.en_pin, slot.invert);
  out += F("Slot isolated (others disabled)\n");

  // 3) HardwareSerial (UART1) mit Pin-Remapping auf SDA/SCL starten.
  //    ESP32 kann jede UART auf beliebige Pins mappen - deutlich stabiler
  //    als SoftwareSerial und ohne externe Library.
  HardwareSerial ezoSerial(1);
  ezoSerial.begin(9600, SERIAL_8N1, EZO_UART_RX_PIN, EZO_UART_TX_PIN);
  delay(1000);

  // Hilfsfunktion inline: UART-Kommando senden + Antwort einsammeln
  auto sendCmd = [&](const char* cmd) -> String {
    while (ezoSerial.available()) ezoSerial.read();
    ezoSerial.print(cmd);
    ezoSerial.print('\r');
    delay(1500);
    String resp = "";
    while (ezoSerial.available()) {
      char c = ezoSerial.read();
      if (c >= 32 && c <= 126) resp += c;
      delay(5);
    }
    return resp;
  };

  // 4) Status-Ping
  String resp = sendCmd("Status");
  out += F("Status response: [");
  out += resp;
  out += F("]\n");

  if (resp.length() == 0) {
    out += F("\nNo UART response!\n");
    out += F("Circuit is either already in I2C mode, not plugged in,\n");
    out += F("or the other EZO circuits are still attached.\n");
    out += F("</pre>");
    ezoSerial.end();
    // Slots wieder in Normalzustand, Polling wieder an
    maint_restoreAllSlotsOn();
    polling = true;
    send_to_thingspeak = true;
    return false;
  }

  // 5) Info
  resp = sendCmd("i");
  out += F("Info response: [");
  out += resp;
  out += F("]\n");

  // 6) Eigentlicher Switch
  char cmd[16];
  snprintf(cmd, sizeof(cmd), "I2C,%d", (int)slot.default_addr);
  out += F("Sending: ");
  out += cmd;
  out += F("\n");

  resp = sendCmd(cmd);
  out += F("Switch response: [");
  out += resp;
  out += F("]\n");
  out += F("\nSwitch command sent. EZO circuit will reboot in I2C mode.\n");
  out += F("ESP will restart in 5 seconds to reinitialize the bus.\n");
  out += F("</pre>");

  ezoSerial.end();
  return true;
}


void handlePageMaintenance(){
  String message;
  addTop(message);

  message += F("<h2>EZO Maintenance</h2>");

  if (!login) {
    message += F("<br><h3><center><a href=\"/\" class=\"button3\">Login first!</a></h3>");
    addBottom(message);
    return;
  }

  message += F("<b><em>Attention! -> Please read the Manual <- Attention!</em></b><br><br>");
  message += F("<small>This page lets you scan the I2C bus and switch a new EZO circuit ");
  message += F("from UART to I2C mode without reflashing or dismantling the pool kit.</small><br><br>");

  // ---- Block 1: I2C Scan ----
  message += F("<center><h3>I2C Bus Scan</h3></center>");
  message += F("<center><form action=\"/action_page\">");
  message += F("<input type=\"hidden\" name=\"maint_scan\" value=\"1\">");
  message += F("<input type=\"submit\" value=\"Scan I2C Bus\">");
  message += F("</form></center><br>");

  if (last_i2c_scan_result.length() > 0) {
    message += F("<center>");
    message += last_i2c_scan_result;
    message += F("</center>");
  }

  // ---- Block 2: UART -> I2C Switch ----
  message += F("<hr><center><h3>Switch EZO Circuit: UART &rarr; I2C</h3></center>");
  message += F("<div style=\"background:#5a4a00;padding:10px;margin:10px;border:2px solid yellow;\">");
  message += F("<b>&#9888; IMPORTANT:</b><br>");
  message += F("Before you start the switch, <b>unplug all other EZO circuits</b> from the board.<br>");
  message += F("Only the circuit you want to program must remain in its slot.<br>");
  message += F("Otherwise multiple circuits answer on the UART bus and the switch will fail.");
  message += F("</div>");

  message += F("<center><form action=\"/action_page\" onsubmit=\"return confirm('Really switch the selected circuit? The ESP will reboot.');\">");
  message += F("<table>");

  for (int i = 0; i < EZO_SLOT_COUNT; i++) {
    message += F("<tr><td>");
    message += F("<input type=\"radio\" name=\"maint_switch_slot\" value=\"");
    message += String(i);
    message += F("\" id=\"slot");
    message += String(i);
    message += F("\" required>");
    message += F("<label for=\"slot");
    message += String(i);
    message += F("\">&nbsp;");
    message += EZO_SLOTS[i].name;
    message += F(" slot &rarr; I2C ");
    message += String((int)EZO_SLOTS[i].default_addr);
    message += F("</label>");
    message += F("</td></tr>");
  }

  message += F("<tr><td><br>");
  message += F("<input type=\"checkbox\" name=\"maint_confirm\" value=\"1\" id=\"maint_confirm\" required>");
  message += F("<label for=\"maint_confirm\">&nbsp;I have unplugged all other EZO circuits</label>");
  message += F("</td></tr>");

  message += F("<tr><td><br>");
  message += F("<input type=\"submit\" value=\"Switch to I2C (ESP will reboot)\" style=\"background:#c00;color:white;\">");
  message += F("</td></tr>");

  message += F("</table></form></center>");

  message += F("<br><small><em>Tip: after the switch the ESP reboots. Come back to this page ");
  message += F("and run 'Scan I2C Bus' to verify that the new circuit is detected on its address.</em></small>");

  addBottom(message);
}
