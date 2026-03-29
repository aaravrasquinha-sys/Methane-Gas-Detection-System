# Methane-Gas-Detection-System
IoT-based methane gas detection system built on Arduino Uno with an MQ-9 sensor, providing real-time LEL monitoring via a 16×2 LCD, tiered alerts through an LED and buzzer, and GSM-based SMS notifications for remote safety response — designed for infrastructure-sparse environments where internet connectivity cannot be assumed.

![Image Alt](https://github.com/aaravrasquinha-sys/Methane-Gas-Detection-System/blob/9ce68b7d877caa5200241823f22d34915affa5e0/Working%20Prototype.jpeg)
Problem Statement

Natural gas and biogas infrastructure—spanning residential pipelines, agricultural digesters, industrial plants, and confined utility spaces—presents a persistent and underappreciated safety risk. Methane (CH₄), the primary constituent of natural gas, is both odourless in its pure form and highly flammable, with a Lower Explosive Limit (LEL) of just 5% by volume in air.

Existing safety measures in low-income and rural deployments are either prohibitively expensive (catalytic combustion detectors) or entirely absent. **A single undetected leak in an enclosed space can escalate from trace concentration to an explosive atmosphere within minutes, with no automated mechanism to alert occupants or remote stakeholders.

This project addresses the gap between affordable embedded hardware and actionable real-time safety intelligence by deploying a tiered detection-and-alert pipeline capable of:

- Continuously monitoring ambient methane concentration
- Escalating through warning → alarm states at defined LEL thresholds
- Delivering SMS notifications to a remote contact without internet dependency, using the ubiquitous GSM cellular network

The system is designed to be deployable in infrastructure-sparse environments where Wi-Fi connectivity cannot be assumed, making it especially relevant for agricultural biogas installations, LPG storage facilities, and industrial kitchens in developing regions.

Bill of Materials

| Component | Specification | Role |
|-----------|--------------|------|
| **Arduino Uno** | ATmega328P, 5V | Central microcontroller |
| **MQ-9 Gas Sensor** | 300–10000 ppm CH₄/CO | Gas concentration sensing |
| **16×2 LCD Display** | I2C, 0x27 addr | Live readout (level + status) |
| **GSM Module** | SIM800L / SIM900 | Cellular SMS alerting |
| **Active Buzzer** | 5V, 2.4 kHz | Audible danger alarm |
| **LED (Orange)** | 5mm, ~2V Vf | Visual warning indicator |
| **10 kΩ Resistor** | Load resistor (RL) | MQ-9 voltage divider |
| **Power Supply** | 5V / 2A | System power 


Setup & Configuration

### 1. Install Libraries

Open Arduino IDE → **Sketch → Include Library → Manage Libraries**, then install:

```
LiquidCrystal_I2C   (by Frank de Brabander)
```

`SoftwareSerial` is bundled with the Arduino IDE.

### 2. Wire the Circuit

| Arduino Pin | Connected To |
|-------------|-------------|
| `A0` | MQ-9 `AOUT` |
| `D7` | LED (+ 220Ω series resistor) |
| `D8` | Buzzer `+` |
| `D2` | GSM Module `TX` |
| `D3` | GSM Module `RX` |
| `SDA (A4)` | LCD `SDA` |
| `SCL (A5)` | LCD `SCL` |
| `5V / GND` | All peripherals |

### 3. Configure the Sketch

Edit the top of `methane_detector.ino`:

```cpp
// Your alert phone number (E.164 format)
const char ALERT_NUMBER[] = "+91XXXXXXXXXX";

// MQ-9 R0 — calibrate in clean air before deployment
const float R0 = 10.0;  // kΩ

// Thresholds (% LEL)
#define WARN_THRESHOLD   8
#define ALERT_THRESHOLD  10


