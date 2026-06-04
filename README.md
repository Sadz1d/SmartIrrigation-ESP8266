# SmartIrrigation-ESP8266

Automated plant irrigation system built on the **ESP8266** microcontroller with **Firebase Realtime Database** integration. Supports automatic moisture-based watering, manual on/off control, and real-time monitoring via Firebase.

---

## Firebase Structure

```
test/
├── waterSensor/
│   └── nivoVode        ← moisture level (0–100 %)
└── stanjePumpe/
    └── pumpa           ← "AUTO" | "ON" | "OFF" | "ON2"
```

---

## Features

| Feature | Description |
|---|---|
| Moisture reading | Analog soil sensor on pin `A0`, mapped to 0–100 % |
| AUTO mode | Pump turns ON below threshold (default 40 %), OFF above it |
| Manual ON/OFF | Override via Firebase command |
| ON2 mode | Pulses the pump relay twice (exam task demo) |
| Firebase sync | Moisture value pushed every 5 seconds |
| Status LED | LED on `D2` mirrors pump state (ON = LOW, OFF = HIGH) |

---

## Hardware

- **ESP8266** (NodeMCU or Wemos D1 Mini)
- Soil moisture sensor (analog output → `A0`)
- Relay module → `D1`
- Status LED → `D2`
- 5 V power supply

### Wiring

```
Soil Sensor (AO) ──► A0
Relay (IN)       ──► D1
LED (+)          ──► D2  (with current-limiting resistor)
```

---

## Setup

1. Open `main.ino` and fill in your credentials at the top of the file:

```cpp
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define API_KEY       "YOUR_API_KEY"
#define DATABASE_URL  "YOUR_DATABASE_URL"
```

2. The moisture threshold (default **40 %**) can be changed here:

```cpp
const int moistureThreshold = 40;
```

---

## Dependencies

Install via **Arduino Library Manager**:

| Library | Notes |
|---|---|
| [Firebase ESP Client](https://github.com/mobizt/Firebase-ESP-Client) | by Mobitz |
| ESP8266WiFi | bundled with ESP8266 board package |

**Board package:** *Boards Manager* → search `esp8266 by ESP8266 Community` → Install

---

## Getting Started

1. Clone the repo:
   ```bash
   git clone https://github.com/your-username/SmartIrrigation-ESP8266.git
   ```

2. Open `main.ino` in Arduino IDE and fill in your credentials (see Setup above).

3. Select your board and COM port, then upload.

4. Open **Serial Monitor** at `115200 baud` to see live output.

5. In your Firebase Realtime Database, set `/test/stanjePumpe/pumpa` to one of:

| Value | Behavior |
|---|---|
| `"AUTO"` | Automatic moisture-based control |
| `"ON"` | Force pump ON |
| `"OFF"` | Force pump OFF |
| `"ON2"` | Pulse relay twice, then 10 s pause |

---

## Control Logic

```
pumpa = "AUTO"  →  moisture < 40%  → pump ON
                   moisture ≥ 40%  → pump OFF
                   moisture = 0%   → no change (sensor disconnect guard)

pumpa = "ON"    →  pump always ON
pumpa = "OFF"   →  pump always OFF
pumpa = "ON2"   →  relay HIGH→LOW→HIGH→LOW, 10 s pause (repeats each loop)
```

---

## Project Structure

```
SmartIrrigation-ESP8266/
├── main.ino      ← main Arduino sketch
└── README.md
```

---

## License

MIT — free to use, modify, and distribute.
