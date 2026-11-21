# ESP32 Control Panel

A web dashboard that runs directly on an ESP32. Control an RGB LED and monitor temperature, humidity and vibration sensors from any browser on your local network. No external server or cloud service needed: the ESP32 serves the whole page itself.

Built as a learning project with PlatformIO and the Arduino framework.

<!-- Add a dashboard screenshot here later, for example:
![Dashboard](docs/screenshot.png)
-->

## Features

**RGB LED project**
- **Color sequence:** pick 2 to 10 colors and set the delay between them. The sequence runs on the ESP32, so it keeps going even if you close the browser.
- **Color picker:** choose any color and the LED changes instantly.
- **Effects:** rainbow, breathing and blinking, with adjustable speed.

**Sensors**
- Temperature and humidity with a 5 minute history chart, min / max / average values and a humidity gauge.
- Vibration sensor with an event counter and a list of recent events.

**Dashboard**
- Dark, responsive layout that works on phone and desktop.
- Live status: current LED mode and color, IP address, Wi-Fi signal and uptime.
- Data refreshes every 2 seconds.

## Hardware

| Part | Notes |
| --- | --- |
| ESP32 dev board | Tested with an ESP32 Dev Module |
| RGB LED (common cathode) | Longest leg goes to GND |
| 3 x 220 ohm resistors | One per color leg |
| DHT11 or DHT22 | Temperature and humidity (optional) |
| Vibration sensor module | Digital output (optional) |
| Breadboard and jumper wires | |

### Wiring

| Component | ESP32 pin |
| --- | --- |
| LED red (through resistor) | GPIO 21 |
| LED green (through resistor) | GPIO 22 |
| LED blue (through resistor) | GPIO 23 |
| LED common cathode | GND |
| DHT data | GPIO 4 |
| Vibration sensor output | GPIO 27 |

All pins can be changed at the top of `src/main.cpp`.

## Getting started

1. Install [VS Code](https://code.visualstudio.com/) and the [PlatformIO extension](https://platformio.org/install/ide?install=vscode).
2. Clone this repository and open the folder in VS Code.
3. Make sure `platformio.ini` looks like this:

   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   monitor_speed = 115200
   lib_deps =
       adafruit/DHT sensor library
       adafruit/Adafruit Unified Sensor
   ```

4. Open `src/main.cpp` and enter your Wi-Fi details:

   ```cpp
   const char* ssid  = "YOUR_WIFI_NAME";
   const char* sifre = "YOUR_WIFI_PASSWORD";
   ```

   The ESP32 only connects to **2.4 GHz** networks. Never commit your real password.

5. Connect the ESP32 over USB and click **Upload**. If it hangs on "Connecting...", hold the **BOOT** button on the board.
6. Open the **Serial Monitor**. When the board connects it prints an address like `http://192.168.1.45`.
7. Open that address in a browser on a device connected to the same Wi-Fi network.

### Simulation mode

If you do not have the sensors connected yet, the project runs with fake sensor data (a "SIMULATION" badge is shown on the dashboard):

```cpp
#define SIMULASYON 1   // 1 = fake data, 0 = read real sensors
```

To use real sensors, set it to `0` and adjust `DHT_PIN`, `DHT_TIP` (`DHT11` or `DHT22`), `TITRESIM_PIN` and `TITRESIM_AKTIF` (use `LOW` if your module outputs LOW on vibration).

## HTTP API

The dashboard talks to the ESP32 through simple GET endpoints, so you can also call them from a script or another app.

| Endpoint | Description |
| --- | --- |
| `/api/veri` | Returns sensor values, history and LED state as JSON |
| `/api/renk?r=255&g=0&b=0` | Sets a fixed color (0 to 255 per channel) |
| `/api/dizi?c=ff0000,00ff00,0000ff&d=1000` | Starts a color sequence (hex colors, delay in ms) |
| `/api/efekt?ad=nefes&hiz=3000&r=255&g=0&b=0` | Starts an effect: `gokkusagi`, `nefes` or `yanipsonme` |
| `/api/kapat` | Turns the LED off |

## Project structure

```
.
├── platformio.ini
├── src
│   └── main.cpp      # firmware and embedded web page
├── LICENSE
└── README.md
```

The web page (HTML, CSS and JavaScript) is embedded in `main.cpp` as a raw string, so the project is a single file to upload.

Note: variable names, code comments and the dashboard text are in Turkish.

## Ideas for next steps

- Add more projects to the dashboard (servo, relay, OLED display)
- Save sensor history to flash or send it to an MQTT broker
- Add a password to the dashboard

## License

Released under the [MIT License](LICENSE).
