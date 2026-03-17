# RWS Pool-Kit v6.3 (ESP32)

Web-based pool controller built on the **Atlas Scientific Wi-Fi Pool Kit V1.8**.

> **ESP32 version** for Pool Kit V1.8 (Adafruit HUZZAH32).
> For the older Pool Kit V1.3 (ESP8266), see: [rwspoolkit](https://github.com/ridewithoutstomach/rwspoolkit)

A non-commercial hobby project, created as an alternative to expensive off-the-shelf pool controllers. Everything is managed through a web interface — no programming knowledge required.

More info: [rws.casa-eller.de](http://rws.casa-eller.de)

## Features

- **Sensor readings**: pH, ORP, temperature (EZO I2C sensors) + air temperature/humidity (AM2315C)
- **Pool pump**: Timer control, variable speed pump with 3 levels
- **Chlorinator**: Saltwater electrolysis control
- **pH dosing**: PHMinus/PHPlus dosing pumps
- **Chlorine dosing**: Dosing pump with canister level monitoring
- **Heating**: Heater control
- **ThingSpeak**: Data logging and visualization via app
- **Tasmota**: Control all devices (smart plugs, Sonoff) via HTTP over WiFi
- **OTA updates**: Firmware update over WiFi
- **Captive portal**: WiFi configuration via Soft-AP on first boot
- **Login PIN**: Changeable via web interface

## Hardware

- **Atlas Scientific Wi-Fi Pool Kit V1.8** (Adafruit Feather HUZZAH32 / ESP32)
- EZO circuits: pH (I2C 99), ORP (I2C 98), RTD (I2C 102)
- Optional: EZO PMP-L dosing pump (I2C 109)
- AM2315C temperature/humidity sensor
- PVC electrode holder for water circuit
- Tasmota-compatible WiFi smart plugs / Sonoff devices
- 230V peristaltic dosing pumps
- Strong WiFi signal in pool area (use repeater if needed)

### Pin assignment (HUZZAH32 / V1.8)

| Sensor | GPIO | State = ON |
|--------|------|-----------|
| pH     | 12   | LOW       |
| ORP    | 27   | LOW       |
| AUX    | 33   | LOW       |
| Temp   | 15   | HIGH      |

## Setup

### 1. Flash firmware

Upload the sketch to the ESP32 using the Arduino IDE (see "Board & Libraries" section below).

### 2. Configure WiFi (Captive Portal)

After the first flash, the ESP has no WiFi credentials. It will automatically start its own access point:

1. Open **WiFi settings** on your phone
2. Connect to the network **"Soft AP RWSPoolKit"**
3. The captive portal opens automatically (if not: open `http://172.168.4.1` in your browser)
4. **Enter login PIN: `2203`**
5. Enter your home **WiFi name (SSID)** and **WiFi password**
6. Optional: Change the **hostname** (default: `RWSPoolkit`)
7. Check the copyright checkbox and save
8. The ESP reboots and connects to your WiFi network

### 3. Open web interface

After reboot, the Pool-Kit is accessible on your home network:

- In browser: **`http://rwspoolkit`** (or whatever hostname you set)
- Alternatively: Check your router for the assigned IP address
- Login with PIN `2203` (can be changed later under "PIN" in the web interface)

All further settings can be configured from here (larger screen recommended).

### 4. Switch EZO circuits to I2C

The EZO sensor circuits (pH, ORP, RTD) ship in **UART mode** by default. They must be switched to **I2C mode** for the Pool-Kit.

### 5. Calibrate sensors

Calibrate pH, ORP and RTD sensors through the web interface.

### 6. OTA password (firmware update over WiFi)

To use OTA (Over-The-Air Updates), set an MD5 hash of your password in the main sketch file. Search for `ArduinoOTA.setPasswordHash` and replace the hash with your own.

**Generate your MD5 hash:**

Linux/Mac:
```bash
echo -n "YourPassword" | md5sum
```

Windows PowerShell:
```powershell
$md5 = [System.Security.Cryptography.MD5]::Create()
$bytes = [System.Text.Encoding]::UTF8.GetBytes("YourPassword")
$hash = $md5.ComputeHash($bytes)
($hash | ForEach-Object { $_.ToString("x2") }) -join ""
```

Online: Search for "MD5 Hash Generator" and enter your password.

Paste the 32-character hash into the code:
```cpp
ArduinoOTA.setPasswordHash("your_md5_hash_here");
```

Upload the sketch once via USB. After that you can select the ESP via network under **Tools → Port** in the Arduino IDE.

### 7. Setup Tasmota devices

Assign static IP addresses to all Tasmota devices (smart plugs, Sonoff) in your router and enter them in the Pool-Kit web interface.

## Arduino IDE — Board & Libraries

### Board

- Install **ESP32 Board Package** (Board Manager → search "esp32" by Espressif)
- Select board: `Adafruit ESP32 Feather`

### Libraries (Library Manager)

| Library | Used for |
|---|---|
| `Ezo_I2c` (Atlas Scientific) | EZO sensor communication (`Ezo_i2c.h`, `Ezo_i2c_util.h`, `iot_cmd.h`, `sequencer4.h`, `sequencer1.h`) |
| `ThingSpeak` | Data logging |
| `AM2315C` | Temperature/humidity sensor |
| `NTPClient` | Time synchronization |
| `Uptime Library` | Uptime display (`uptime_formatter.h`) |

### Included in ESP32 Board Package

- `WiFi.h`, `WebServer.h`, `HTTPClient.h`, `ESPmDNS.h`
- `DNSServer.h`, `ArduinoOTA.h`, `WiFiUdp.h`
- `Wire.h`, `LittleFS.h`

## Differences from ESP8266

| | ESP8266 (V1.3) | ESP32 (V1.8) |
|---|---|---|
| Board | Adafruit Feather HUZZAH | Adafruit Feather HUZZAH32 |
| Repo | [rwspoolkit](https://github.com/ridewithoutstomach/rwspoolkit) | [rwspoolkit-esp32](https://github.com/ridewithoutstomach/rwspoolkit-esp32) |
| WiFi lib | `ESP8266WiFi.h` | `WiFi.h` |
| WebServer | `ESP8266WebServer.h` | `WebServer.h` |
| HTTP client | `ESP8266HTTPClient.h` | `HTTPClient.h` |
| mDNS | `ESP8266mDNS.h` | `ESPmDNS.h` |
| Hostname | `WiFi.hostname()` | `WiFi.setHostname()` |
| EN_PH | GPIO 14 | GPIO 12 |
| EN_ORP | GPIO 12 | GPIO 27 |
| EN_RTD | GPIO 15 | GPIO 15 |
| EN_AUX | GPIO 13 | GPIO 33 |

## File structure

The sketch consists of multiple `.ino` files in the same folder (Arduino IDE compiles them all together automatically):

- `NewPoolKit_v6.3_ESP32.ino` — Setup, loop, global variables, EZO sequencer
- `website.ino` — Web server handlers, dashboard, CSS
- `configlesen.ino` — Configuration read/write (LittleFS)
- `connect.ino` — WiFi connection, Soft-AP, captive portal
- `timer.ino` — NTP sync, timer logic
- `wwWiFi.ino` — WiFi and PIN configuration pages
- `ww*.ino` — Individual configuration pages (pool, ThingSpeak, chlorinator, etc.)
- `i2c.ino` — I2C communication with EZO boards

## License

MIT License — see [LICENSE](LICENSE) file. Free to use, copyright notice must be retained.

## Credits

- [Atlas Scientific](https://atlas-scientific.com) — Wi-Fi Pool Kit hardware & EZO library
- [Tasmota](https://tasmota.github.io) — Open-source firmware for WiFi devices
- Jens Fleischer — Captive portal implementation
