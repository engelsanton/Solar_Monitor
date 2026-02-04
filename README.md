# Solar Monitor - PV-Battery Ratio Experiment System

A comprehensive ESP32-based solar monitoring and simulation system for educational purposes. This project allows real-time monitoring and simulation of photovoltaic systems with dynamic load management and battery state tracking.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [System Configuration](#system-configuration)
- [Usage Guide](#usage-guide)
- [Web Interface](#web-interface)
- [Simulation Modes](#simulation-modes)
- [Calibration](#calibration)
- [Troubleshooting](#troubleshooting)
- [Project Structure](#project-structure)

## 🎯 Overview

This project is developed as part of the T3000 module at DHBW Stuttgart. The Solar Monitor system provides:

- **Educational Tool**: Demonstrates PV-battery interactions and optimal sizing ratios
- **Real-time Monitoring**: Tracks voltage, current, and power from solar panels
- **Dynamic Simulation**: Simulates day/night cycles with configurable parameters
- **Load Management**: Control up to 4 independent loads with activity logging
- **Battery Tracking**: State of Charge (SoC) monitoring with visual feedback

The system can operate in two modes:
1. **Simulation Mode**: Uses mathematical models to simulate solar generation
2. **Calibration Mode**: Uses real INA219 sensor data for accurate measurements

## ✨ Features

### Hardware Control
- **4 Independent Solar Panel Controls**: Switch panels on/off via transistors
- **4 Load Outputs**: Manage connected loads individually
- **Real-time Sensor Reading**: INA219 current/voltage sensor integration
- **OLED Display**: Local status display showing panel states and measurements

### Web Interface
- **Responsive Design**: Color-coded interface (Battery: Blue, Load: Red, Modules: Green)
- **Live Data Updates**: Real-time voltage, current, and SoC display
- **Interactive Charts**: Historical data visualization for SoC and voltage
- **Activity Logging**: Timestamped load change history
- **Settings Panel**: Configure simulation parameters on-the-fly

### Simulation Engine
- **Day/Night Cycle**: Configurable cycle duration and solar intensity
- **Battery Model**: Realistic SoC calculation with charge/discharge curves
- **Irradiance Simulation**: Sin-wave based solar generation
- **Panel Switching**: Automatic panel activation based on power requirements

## 🛠 Hardware Requirements

### PCB Assembly
This system requires the **Solar_Monitor V2.0 PCB**. The PCB must be properly populated with all components according to the assembly instructions. The board integrates all necessary circuitry including:
- ESP32-S3 socket
- INA219 current sensor interface
- OLED display connector
- MOSFET transistor circuits for panel and load switching
- Power regulation and distribution

### Essential Components
| Component | Specification | Purpose |
|-----------|--------------|---------|
| **PCB** | Solar_Monitor V2.0 | Custom circuit board (must be assembled) |
| **Microcontroller** | ESP32-S3 DevKit C-1 | Main controller |
| **Current Sensor** | INA219 (I2C) | Measures voltage and current |
| **Display** | SSD1306 OLED (128x64, I2C) | Local status display |
| **Transistors** | 4x N-Channel MOSFETs | Solar panel switching |

### Pin Configuration
```
I2C Bus:
- SDA: GPIO 21
- SCL: GPIO 22

Transistor Control:
- Panel 1: GPIO 13
- Panel 2: GPIO 12
- Panel 3: GPIO 14
- Panel 4: GPIO 27

Load Control:
- Load 1: GPIO 25
- Load 2: GPIO 26
- Load 3: GPIO 32
- Load 4: GPIO 33
```

### I2C Addresses
- INA219 Sensor: `0x40`
- OLED Display: `0x3C`

## 💻 Software Requirements

### Development Environment
- **PlatformIO IDE** (VS Code extension recommended)
- **Python 3.x** (for PlatformIO)
- **Git** (for version control)

### Platform
- Framework: Arduino
- Platform: Espressif 32
- Board: ESP32-S3 DevKit C-1

## 📦 Installation

### Step 1: Clone the Repository
```bash
git clone <repository-url>
cd Solar_Monitor
```

### Step 2: Install PlatformIO
If you haven't installed PlatformIO yet:

**Via VS Code:**
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install

**Via Command Line:**
```bash
pip install platformio
```

### Step 3: Open Project
```bash
# Using VS Code
code Solar_Monitor

# Or using PlatformIO CLI
pio project init --ide vscode
```

### Step 4: Install Dependencies
PlatformIO will automatically install required libraries defined in `platformio.ini`:
- Adafruit INA219
- Adafruit SSD1306
- Adafruit GFX Library
- ESPAsyncWebServer
- AsyncTCP

### Step 5: Build the Project
```bash
# Via PlatformIO CLI
pio run

# Or use VS Code PlatformIO toolbar: Build button
```

### Step 6: Upload Firmware
**Upload the main program:**
```bash
pio run --target upload --environment esp32-s3-devkitc-1 --upload-port COM30

# Replace COM30 with your ESP32's serial port
# Windows: COM3, COM4, etc.
# Linux/Mac: /dev/ttyUSB0, /dev/cu.usbserial, etc.
```

**Upload the filesystem (HTML interface):**
```bash
pio run --target uploadfs --environment esp32-s3-devkitc-1

# This uploads the index.html file to ESP32's SPIFFS
```

### Step 7: Connect to the System
1. Power on the ESP32
2. Wait for the WiFi Access Point to start (~5 seconds)
3. Connect to WiFi network: **Solar_Monitor**
4. Password: **12345678**
5. Open browser and navigate to: **http://192.168.4.1**

## ⚙️ System Configuration

### WiFi Settings
Edit in [lib/Config/config.h](lib/Config/config.h):
```cpp
#define WIFI_SSID "Solar_Monitor"
#define WIFI_PASSWORD "12345678"
```

### Hardware Configuration
Modify pin assignments in [lib/Config/config.h](lib/Config/config.h):
```cpp
// I2C pins
#define OLED_SDA 21
#define OLED_SCL 22

// Panel control pins
#define TRANS1_PIN 13
#define TRANS2_PIN 12
#define TRANS3_PIN 14
#define TRANS4_PIN 27

// Load control pins
#define LOAD1_PIN 25
#define LOAD2_PIN 26
#define LOAD3_PIN 32
#define LOAD4_PIN 33
```

### I2C Addresses
```cpp
#define INA219_ADDR 0x40
#define OLED_ADDR 0x3C
```

## 📖 Usage Guide

### First Boot
1. **Serial Monitor**: Open serial monitor (115200 baud) to view boot messages
2. **I2C Scan**: System automatically scans for connected devices
3. **Component Status**: Check if OLED and INA219 are detected
4. **WiFi AP**: Note the IP address (usually 192.168.4.1)

### Web Interface Navigation

#### Modules Page (Green Theme)
- **Manual Control**: Click P1-P4 buttons to toggle individual panels
- **Real-time Status**: Panel buttons light up when active
- **Simulation Display**: Shows which panels are currently generating power

#### Battery Page (Blue Theme)
- **State of Charge**: Live SoC percentage in page title
- **SoC Meter**: Visual battery level indicator
- **Historical Chart**: Blue line showing SoC over time

#### Load Page (Red Theme)
- **Current Load**: Total load display in page title
- **4 Load Switches**: Toggle individual loads on/off
- **Activity Log**: Timestamped history of load changes
- **Clear Log**: Button to reset activity history

#### Settings Page
- **Simulation Control**: Start/Stop/Reset simulation
- **Mode Selection**: Toggle between simulation and calibration modes
- **Time Settings**: Adjust day/night cycle duration
- **Power Settings**: Configure battery capacity and load values
- **Solar Settings**: Set max irradiance and panel power
- **Calibration**: Set current multiplier for sensor accuracy

### OLED Display
The local display shows:
```
== PANEL STATUS ==
P1:ON  P2:OFF
P3:ON  P4:OFF
Voltage: 12.5 V
Current: 234.5 mA
```

## 🔄 Simulation Modes

### Simulation Mode (Default)
**Mathematical solar generation simulation**

**How it works:**
1. Simulates day/night cycle using sin wave
2. Automatically activates panels based on load demand
3. Calculates battery SoC based on generation vs consumption
4. Updates every 100ms

**Configuration Parameters:**
- **Day/Night Duration**: 10-300 seconds (default: 60s)
- **Max Solar Irradiance**: 100-1000 W/m² (default: 1000)
- **Panel Power**: Each panel = 10W (configurable)
- **Battery Capacity**: 1000-10000 mAh (default: 5000)
- **Load Power**: 1-50W per load (default: 5W)

**Simulation Formula:**
```
Irradiance = MaxIrradiance × sin²(π × t / CycleDuration)
PanelCurrent = (Irradiance × PanelPower × ActivePanels) / Voltage
BatteryChange = PanelCurrent - LoadCurrent
SoC += (BatteryChange / Capacity) × 100
```

### Calibration Mode
**Real sensor data acquisition**

**Purpose:**
- Read actual voltage and current from INA219 sensor
- Calibrate simulation parameters against real hardware
- Validate panel output measurements

**How to use:**
1. Connect real solar panels to INA219 sensor input
2. Go to Settings page
3. Toggle "Simulation Mode" OFF
4. Adjust "Current Multiplier" to calibrate readings
5. Click "Real Data" to view live sensor values

**Current Multiplier:**
- Range: 1-10000
- Default: 1000
- Scales INA219 current reading to match actual panel output
- Formula: `ActualCurrent = (SensorReading × Multiplier) / 1000`

## 🔧 Calibration

### Calibrating Current Readings

1. **Set up test conditions:**
   - Connect known current source or solar panel
   - Measure actual current with multimeter
   - Note the expected value

2. **Access calibration:**
   - Open web interface → Settings
   - Disable "Simulation Mode"
   - Click "Real Data" button

3. **Adjust multiplier:**
   - Compare web interface reading to multimeter
   - Calculate: `NewMultiplier = CurrentMultiplier × (ExpectedValue / DisplayedValue)`
   - Enter new value in "Current Multiplier" field
   - Save settings

4. **Verify:**
   - Check Real Data modal again
   - Reading should now match multimeter
   - If not, repeat adjustment

### Voltage Calibration
Voltage readings from INA219 are typically accurate. If needed:
- Check wiring connections
- Verify INA219 power supply is stable (3.3V or 5V)
- Replace sensor if readings are consistently wrong

## 🐛 Troubleshooting

### ESP32 Won't Connect
**Problem**: Can't find "Solar_Monitor" WiFi network
- **Solution 1**: Wait 10-15 seconds after boot
- **Solution 2**: Check serial monitor for "Access Point started" message
- **Solution 3**: Power cycle the ESP32
- **Solution 4**: Verify WiFi is enabled on your device

### OLED Display Not Working
**Problem**: "OLED initialization failed" in serial monitor
- **Solution 1**: Check I2C wiring (SDA → 21, SCL → 22)
- **Solution 2**: Verify OLED address is 0x3C (check with I2C scanner)
- **Solution 3**: Try different I2C pins if hardware is different
- **Solution 4**: Check OLED power supply (3.3V or 5V depending on module)

### INA219 Not Detected
**Problem**: "INA219 initialization failed"
- **Solution 1**: Verify I2C connections share same bus as OLED
- **Solution 2**: Check sensor address (default 0x40)
- **Solution 3**: Test with I2C scanner code
- **Solution 4**: Ensure sensor has proper power supply

### Web Interface Not Loading
**Problem**: Browser can't reach 192.168.4.1
- **Solution 1**: Verify WiFi connection to "Solar_Monitor"
- **Solution 2**: Try http://192.168.4.1 (not https)
- **Solution 3**: Clear browser cache
- **Solution 4**: Check if filesystem was uploaded (`pio run --target uploadfs`)

### Panels Not Switching
**Problem**: Panel buttons don't control transistors
- **Solution 1**: Check transistor gate connections to GPIO pins
- **Solution 2**: Verify transistor type (N-Channel MOSFET recommended)
- **Solution 3**: Check pull-down resistors on gates
- **Solution 4**: Measure gate voltage with multimeter (should be 3.3V when ON)

### Simulation Stuck
**Problem**: Time doesn't advance, values frozen
- **Solution 1**: Click "Reset Simulation" in Settings
- **Solution 2**: Refresh web page
- **Solution 3**: Restart ESP32

### Current Readings Too High/Low
**Problem**: Displayed current doesn't match expectations
- **Solution 1**: Enter Calibration Mode
- **Solution 2**: Adjust Current Multiplier (default: 1000)
- **Solution 3**: Check INA219 shunt resistor value (should be 0.1Ω)
- **Solution 4**: Verify sensor is rated for your current range

## 📁 Project Structure

```
Solar_Monitor/
├── platformio.ini          # PlatformIO configuration
├── README.md               # This file
├── LICENSE                 # License information
│
├── src/
│   └── main.cpp            # Main program entry point
│
├── data/
│   └── index.html          # Web interface (uploaded to SPIFFS)
│
├── include/                # Global header files (empty by default)
│
└── lib/                    # Project libraries
    ├── Config/
    │   └── config.h        # Hardware pin definitions
    │
    ├── INA/
    │   ├── ina.h
    │   └── ina.cpp         # INA219 sensor wrapper
    │
    ├── OLED/
    │   ├── oled.h
    │   └── oled.cpp        # SSD1306 OLED display driver
    │
    ├── Transistor/
    │   ├── transistor.h
    │   └── transistor.cpp  # Solar panel switching control
    │
    ├── WiFiManager/
    │   ├── wifi_manager.h
    │   └── wifi_manager.cpp # WiFi Access Point management
    │
    ├── WebServer/
    │   ├── web_server.h
    │   └── web_server.cpp  # HTTP server and API endpoints
    │
    ├── Simulation/
    │   ├── simulation.h
    │   └── simulation.cpp  # Solar simulation engine
    │
    └── Calibration/        # Future: Calibration data storage
```

### Key Components

**main.cpp**: System initialization, main loop coordination
- I2C bus setup and device scanning
- WiFi AP initialization
- Web server startup
- OLED display updates
- Component integration

**simulation.cpp**: Core simulation logic
- Day/night cycle calculation
- Battery SoC management
- Panel activation logic
- Power flow calculation
- Calibration mode handling

**web_server.cpp**: HTTP endpoints
- `/` - Serves main web interface
- `/status` - Returns JSON status data
- `/toggle-panel` - Panel control endpoint
- `/toggle-load` - Load control endpoint
- `/simulation/start` - Start simulation
- `/simulation/stop` - Pause simulation
- `/simulation/reset` - Reset to initial state
- `/settings` - Update configuration
- `/real/data` - Get sensor readings

**transistor.cpp**: GPIO control
- Panel ON/OFF switching
- Load ON/OFF switching
- State management

**ina.cpp**: Sensor interface
- INA219 initialization
- Voltage measurement (getBusVoltage)
- Current measurement (getCurrent)
- Device detection (isFound)

**oled.cpp**: Display management
- Boot screen
- Status display with panel states
- Voltage and current display
- Connection status

## 📄 License

This project is developed as part of academic coursework at DHBW Stuttgart. See LICENSE file for details.

## 🤝 Contributing

This is an educational project. If you find bugs or have suggestions:
1. Open an issue describing the problem
2. Provide serial monitor output if applicable
3. Include your hardware setup details

## 📞 Support

For issues related to:
- **Hardware Setup**: Check wiring diagrams and pin configurations
- **Software Bugs**: Enable serial monitor output for debugging
- **Simulation Questions**: Review simulation parameters in Settings
- **Calibration Issues**: Follow calibration procedure step-by-step

---

**Developed by**: DHBW Stuttgart Embedded Systems Program  
**Module**: T3000 - Technical Project Work  
**Platform**: ESP32-S3 with Arduino Framework
