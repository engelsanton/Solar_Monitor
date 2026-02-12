# Solar Monitor - PV-Battery Ratio Experiment System

A comprehensive ESP32-based solar monitoring and simulation system for educational purposes. This project allows real-time monitoring and simulation of photovoltaic systems with dynamic load management and battery state tracking.

## 📋 Table of Contents
- [Overview](#overview)
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

This project is developed as part of the T3100 module at DHBW Stuttgart. The Solar Monitor system provides:

- **Educational Tool**: Demonstrates PV-battery interactions and optimal sizing ratios
- **Real-time Monitoring**: Tracks voltage, current, and power from solar panels
- **Dynamic Simulation**: Simulates day/night cycles with configurable parameters

The system can operate in two modes:
1. **Simulation Mode**: Uses mathematical models to simulate solar generation
2. **Calibration Mode**: Uses real INA219 sensor data for accurate measurements and scales them up.

## Hardware Requirements

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
| **Transistors** | 4x N-Channel MOSFETs BS170 and 4x P-Channel MOSFETs IRF9540N | Solar panel switching |
| **Solar Panels**| 4x Solar Panels 5V 100mA | For measurements| 

### Pin Configuration
```
I2C Bus:
- SDA: GPIO 9
- SCL: GPIO 8

Transistor Control (Panel Switching):
- Panel 1: GPIO 15
- Panel 2: GPIO 16
- Panel 3: GPIO 17
- Panel 4: GPIO 18
```

### I2C Addresses
- INA219 Sensor: `0x40`
- OLED Display: `0x3C`

## Software Requirements

### Development Environment
- **PlatformIO IDE** (VS Code extension recommended)
- **Python 3.x** (for PlatformIO)
- **Git** (for version control)

### Platform
- Framework: Arduino
- Platform: Espressif 32
- Board: ESP32-S3 DevKit C-1

## Installation

### Step 1: Clone the Repository
```bash
git clone https://github.com/engelsanton/Solar_Monitor.git
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
5. Open browser and navigate to: **http://192.168.4.1** (or if changed to the ip address declared in config.h)

## System Configuration

### WiFi Settings
Edit in [lib/Config/config.h](lib/Config/config.h):
```cpp
#define WIFI_SSID "Solar_Monitor"
#define WIFI_PASSWORD "12345678"
```

### Hardware Configuration
All hardware settings are configured in [lib/Config/config.h](lib/Config/config.h):

```cpp
// I2C Pin Configuration
#define OLED_SDA 9
#define OLED_SCL 8

// I2C Addresses
#define OLED_ADDR 0x3C
#define INA219_ADDR 0x40

// Transistor GPIO Pins (Panel Switching)
#define TRANSISTOR_1 15
#define TRANSISTOR_2 16
#define TRANSISTOR_3 17
#define TRANSISTOR_4 18

// WiFi Access Point Settings
#define DEFAULT_AP_IP IPAddress(192, 168, 4, 1)

// Simulation Settings
#define DEFAULT_CURRENT_MULTIPLIER 600.0
```

To modify WiFi credentials, edit in [src/main.cpp](src/main.cpp):
```cpp
WiFiManager wifiManager("Solar_Monitor", "12345678", DEFAULT_AP_IP);
```


## Usage Guide

### First Boot
1. **Serial Monitor**: Open serial monitor (115200 baud) to view boot messages
2. **I2C Scan**: System automatically scans for connected devices (OLED at 0x3C, INA219 at 0x40)
3. **Component Status**: Check if OLED and INA219 are detected
4. **WiFi AP**: System starts Access Point "Solar_Monitor" with password "12345678"
5. **IP Address**: Default is 192.168.4.1 (configurable in [config.h](lib/Config/config.h))

### Connecting to the System
1. **Connect to WiFi**: 
   - Network: `Solar_Monitor`
   - Password: `12345678`
2. **Open Web Interface**:
   - Navigate to: `http://192.168.4.1` (or your configured IP)
   - The web interface loads automatically

### Web Interface Overview

The interface has four main pages accessible via bottom navigation:

#### 1. Modules Page (Green Theme)
**Purpose**: Control solar panels and monitor power generation

**Features**:
- **Panel Buttons (P1-P4)**: Toggle individual solar panels on/off
  - Each panel: 2.5 kWp rated power
  - Click to activate/deactivate before or during simulation
  - Active panels show green highlight
- **Power Chart**: Real-time power generation in kilowatts (kW)
  - Y-axis: 0 to (active panels × 3 kW)
  - X-axis: Time from 6am to 6am (24-hour cycle)
  - Orange line shows power output
- **Play/Stop Button** (top right): Start/stop simulation
- **Report Button** (top right): View simulation summary (appears after simulation completes)

**How to Use**:
1. Select which panels to activate (at least 1 required)
2. Press Play button to start simulation
3. Watch real-time power generation curve
4. Panels automatically generate power based on sun simulation

#### 2. Battery Page (Blue Theme)
**Purpose**: Manage battery storage and monitor State of Charge

**Features**:
- **Cell Buttons (1-4)**: Toggle battery cells on/off
  - Each cell: 5 kWh capacity
  - More cells = larger total storage capacity
  - Active cells show blue highlight
- **SoC Chart**: State of Charge over 24-hour period
  - Y-axis: 0% to 100%
  - Blue line shows battery level
  - Starts at 0% and charges from solar panels
- **Dashboard**: Current SoC percentage displayed in real-time

**How to Use**:
1. Activate desired number of cells before starting simulation
2. More cells provide more storage but cost more
3. Monitor how well your battery configuration handles the daily cycle

#### 3. Load Page (Red Theme)
**Purpose**: Control household loads and energy consumption

**Features**:
- **6 Configurable Loads**:
  - **Light**: 100 W
  - **Fridge**: 150 W
  - **AC**: 2000 W
  - **Dryer**: 500 W
  - **Dishwasher**: 1000 W
  - **TV**: 300 W
- **Power Charts**: Three lines showing:
  - Green: Power produced (from panels)
  - Red: Power consumed (by loads)
  - Blue: Net power (produced - consumed)
- **Manual Control**: Click load buttons to turn on/off manually during simulation
- **Auto Toggle** (in Settings): Automatically manages loads based on available power

**How to Use**:
1. Click loads to activate them during simulation
2. Enable "Auto Toggle Loads" in Settings for automatic management
3. Watch how loads affect battery charging/discharging

#### 4. Settings Page
**Purpose**: Configure simulation parameters and view real sensor data

**Main Controls**:
- **Simulate Sun**: Enable/disable day/night cycle simulation
  - ON: Uses mathematical sun curve (default)
  - OFF: Constant irradiance
- **Auto Toggle Loads**: Automatically manage loads based on power availability
  - ON: System turns loads on/off intelligently (default)
  - OFF: Manual load control only
- **Simulation Duration**: Seconds per simulated day (1-3600s, default: 24s)
  - Lower values = faster simulation
  - Higher values = more detailed observation

**Advanced Settings**:
- **Current Multiplier**: Calibration factor for sensor readings (default: 600)
  - Range: 1-10000
  - Used when working with real hardware sensors
  - Adjust if measured current doesn't match expectations

**Real Sensor Data**:
- **Show real sensor data** button: Opens modal with live INA219 readings
  - Displays: Bus voltage (V) and current (mA)
  - Shows active panel states
  - Use for hardware calibration and verification

### OLED Display

The local OLED display (if connected) shows:
```
IP: 192.168.4.1
=== PANEL STATUS ===
P1:ON  P2:OFF
P3:ON  P4:OFF
=== SENSOR DATA ===
Current: 234.5 mA
```

**Note**: Display only shows current, not voltage. IP address is shown at the top for easy connection.

## Simulation Modes

### How the Simulation Works

The system simulates a complete 24-hour solar day cycle, from 6:00 AM to 6:00 AM the next day.

**Key Features**:
- **Day/Night Cycle**: Mathematical simulation using sine wave for realistic sun movement
- **Configurable Speed**: Complete 24-hour cycle runs in 24 seconds by default (adjustable 1-3600s)
- **Real-time Updates**: All charts and displays update continuously
- **Energy Tracking**: Records total energy produced, consumed, and grid interaction

**Simulation Flow**:
1. **Start**: Click Play button (top right) after selecting panels and cells
2. **Time Display**: Header shows current simulated time (e.g., "6 am", "12 pm")
3. **Power Generation**: 
   - Panels generate power based on sun position
   - Peak generation at solar noon (12 pm)
   - Zero generation at night (6 pm - 6 am)
4. **Battery Management**:
   - Excess solar power charges battery
   - Insufficient solar draws from battery
   - Battery starts at 0% SoC
5. **Load Management**:
   - Manual: Click load buttons to control
   - Auto: System manages loads based on available power
6. **Completion**: After 24 simulated hours, Report button appears with summary

### Configuration Options

**Simulate Sun** (Settings Page):
- **Enabled** (default): Uses day/night cycle with realistic sun curve
- **Disabled**: Constant solar irradiance for testing

**Auto Toggle Loads** (Settings Page):
- **Enabled** (default): System automatically manages loads intelligently
  - Turns loads on when sufficient power available
  - Turns loads off to prevent battery drain
- **Disabled**: Manual load control only via Load Page buttons

**Simulation Duration** (Settings Page):
- **Range**: 1-3600 seconds per 24-hour day
- **Default**: 24 seconds
- **Purpose**: Control simulation speed for observation or demonstration
- **Example**: 120 seconds = 5 minutes for complete day cycle

### Understanding the Charts

**Modules Page - Power Chart**:
- **Orange Line**: Solar power generation in kW
- **Shape**: Bell curve (sunrise → peak → sunset) when "Simulate Sun" enabled
- **Scale**: Automatic based on active panels (each panel max ~3 kW)

**Battery Page - SoC Chart**:
- **Blue Line**: Battery State of Charge (0-100%)
- **Rising**: Battery charging from excess solar
- **Falling**: Battery discharging to supply loads
- **Goal**: Keep battery in healthy range (20-80% ideally)

**Load Page - Energy Flow Chart**:
- **Green Line**: Power produced by solar panels
- **Red Line**: Power consumed by active loads
- **Blue Line**: Net power (positive = charging, negative = discharging/grid)
- **Analysis**: When red exceeds green, battery discharges or grid import needed

### Report Summary

After simulation completes, click orange Report button (top right) to view:
- **Energy Statistics**:
  - Total energy consumed (kWh)
  - Total energy from grid (kWh) - when solar + battery insufficient
  - Total energy to grid (kWh) - excess solar not stored
- **System Performance**: Understand if your panel/battery ratio is optimal

## Calibration

### Working with Real Hardware

The system can interface with real solar panels via the INA219 sensor to measure actual voltage and current. The simulation uses these measurements scaled by a calibration factor.

### Current Multiplier Explained

**Purpose**: Scales small sensor readings to meaningful simulation values

**Default Value**: 600 (configured in [config.h](lib/Config/config.h))
- Range: 1-10000
- Can be changed in Settings page under "Current multiplier (advanced settings)"
- Changes apply immediately to simulation

**Why Scaling is Needed**:
- Small solar panels produce milliamps (mA)
- Simulation represents kilowatt-scale systems
- Multiplier bridges the gap between hardware and simulation scale

### Viewing Real Sensor Data

1. **Access the Modal**:
   - Go to Settings page
   - Click "Show real sensor data" button (gray button)
   - Modal opens showing live readings

2. **Information Displayed**:
   - **Bus Voltage**: Direct voltage measurement from INA219 (Volts)
   - **Current**: Current measurement from INA219 (milliamps)
   - **Panel Status**: Shows which panels (P1-P4) are currently active
   - Note: A modal cannot be opened during an active simulation

3. **How It Works**:
   - INA219 continuously measures connected hardware
   - Values update in real-time
   - Panel switching (via transistors) is reflected in measurements

### Calibration Procedure

**If you have real solar panels connected:**

1. **Measure Reference Values**:
   - Connect multimeter to measure actual current
   - Note expected current under test conditions
   - Record INA219 raw reading from "Show real sensor data"

2. **Calculate Multiplier**:
   ```
   New Multiplier = Current Multiplier × (Expected Current / Measured Current)
   ```
   
3. **Apply Calibration**:
   - Go to Settings page
   - Locate "Current multiplier (advanced settings)"
   - Enter calculated value
   - Press Enter or click outside field to apply

4. **Verify**:
   - Click "Show real sensor data" again
   - Check if readings now match expectations
   - Repeat adjustment if needed

### Without Real Hardware

If no INA219 sensor is connected:
- System detects "INA219 not found" during boot
- Simulation still works using mathematical models
- "Show real sensor data" will show zero readings
- Current multiplier setting has no effect on pure simulation

## Troubleshooting

### ESP32 Won't Connect
**Problem**: Can't find "Solar_Monitor" WiFi network

**Solutions**:
- **Wait**: Give 10-15 seconds after boot for AP to start
- **Check Serial Monitor**: Look for "Access Point started: Solar_Monitor" and "IP Address: 192.168.4.1"
- **Power Cycle**: Disconnect and reconnect power to ESP32
- **Verify WiFi**: Ensure WiFi is enabled on your device (phone/laptop)
- **Range**: Move closer to ESP32 module

### OLED Display Not Working
**Problem**: "OLED initialization failed" in serial monitor or blank screen

**Solutions**:
- **Check Wiring**: 
  - SDA → GPIO 9
  - SCL → GPIO 8
  - VCC → 3.3V or 5V (check OLED module specs)
  - GND → GND
- **Verify Address**: OLED should be at I2C address 0x3C (check with I2C scanner)
- **Wrong Address**: If different address, update `OLED_ADDR` in [config.h](lib/Config/config.h)
- **Test I2C**: Run I2C scanner code to verify display responds
- **Power Supply**: Ensure stable power (some OLEDs need 5V, others 3.3V)

### INA219 Not Detected
**Problem**: "INA219 initialization failed" in serial monitor

**Solutions**:
- **Verify I2C Connection**: INA219 shares same I2C bus as OLED
  - SDA → GPIO 9
  - SCL → GPIO 8
- **Check Address**: Default is 0x40 (check with I2C scanner if different)
- **Power Supply**: Ensure INA219 has proper 3.3V or 5V power
- **Pull-up Resistors**: I2C bus needs 4.7kΩ pull-ups on SDA and SCL
- **Not Critical**: System works without INA219 (simulation mode only)

### Web Interface Not Loading
**Problem**: Browser can't reach 192.168.4.1

**Solutions**:
- **Verify WiFi Connection**: Must be connected to "Solar_Monitor" network
- **Use HTTP**: Type `http://192.168.4.1` (not https://)
- **Clear Cache**: Use Ctrl+F5 for hard refresh
- **Check Filesystem Upload**: 
  ```bash
  pio run --target uploadfs --environment esp32-s3-devkitc-1
  ```
- **Try Different Browser**: Chrome, Firefox, or Edge
- **Custom IP**: If you changed DEFAULT_AP_IP in config.h, use that address

### Panels Not Switching
**Problem**: Panel buttons don't control transistors

**Solutions**:
- **Check Transistor Connections**:
  - Panel 1 gate → GPIO 15
  - Panel 2 gate → GPIO 16
  - Panel 3 gate → GPIO 17
  - Panel 4 gate → GPIO 18
- **Verify Transistor Type**: N-channel MOSFETs (BS170) and P-channel (IRF9540N) on PCB
- **Check PCB Assembly**: Ensure all MOSFETs are correctly soldered
- **Measure Gate Voltage**: Should be 3.3V when button is "ON", 0V when "OFF"
- **Test GPIOs**: Use simple LED test sketch to verify GPIO pins work
- **Pull-down Resistors**: Gates should have pull-down resistors to prevent floating

### Simulation Won't Start
**Problem**: Clicking Play button does nothing

**Solutions**:
- **Check Browser Console**: Press F12, look for JavaScript errors
- **Hard Refresh**: Use Ctrl+F5 to reload page completely
- **Re-upload Filesystem**: 
  ```bash
  pio run --target uploadfs --environment esp32-s3-devkitc-1
  ```
- **Select Panels**: At least 1 panel must be active (error shown if none selected)
- **Serial Monitor**: Check for error messages from ESP32
- **Restart ESP32**: Power cycle the device

### Simulation Data Not Updating
**Problem**: Time frozen, charts not drawing, values stuck

**Solutions**:
- **Refresh Page**: Use Ctrl+F5 to clear cache and reload
- **Check Serial Monitor**: Look for errors or crash messages
- **Stop and Restart**: Click Stop, wait 2 seconds, click Play again
- **ESP32 Crash**: Power cycle device if completely frozen
- **Memory Issue**: Simulation runs continuously; restart if running for too long

### Current Readings Incorrect
**Problem**: Displayed current doesn't match real measurements

**Solutions**:
- **No Hardware Connected**: System works in simulation-only mode
- **INA219 Not Found**: Check troubleshooting section above
- **Calibration Needed**: Adjust "Current multiplier (advanced settings)" in Settings
  - Default: 600
  - Higher value = higher displayed current
  - Lower value = lower displayed current
- **Shunt Resistor**: Verify INA219 has correct shunt resistor (usually 0.1Ω)
- **Measurement Range**: INA219 limited to ±3.2A with 0.1Ω shunt

### OLED Shows Wrong IP
**Problem**: IP address on OLED doesn't match expected

**Solution**:
- **Check config.h**: Verify DEFAULT_AP_IP setting
- **Default**: Should be 192.168.4.1
- **After Changing**: Re-compile and upload firmware
- **Use Displayed IP**: Always use the IP shown on OLED display

## Project Structure

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

**main.cpp**: System initialization and main loop
- I2C bus setup (GPIO 9=SDA, GPIO 8=SCL)
- I2C device scanning (OLED at 0x3C, INA219 at 0x40)
- WiFi Access Point initialization
- Web server startup
- OLED display updates (100ms interval)
- Component integration and coordination

**simulation.cpp**: Core simulation engine
- Day/night cycle calculation (6am-6am, 24-hour)
- Solar irradiance modeling (sine wave)
- Battery State of Charge management (0-100%)
- Panel power generation calculation
- Load power consumption tracking
- Auto-toggle load management
- Energy statistics (grid import/export)
- Current multiplier scaling

**web_server.cpp**: HTTP server and API endpoints
- **GET /**: Serves main web interface (index.html)
- **GET /status**: Returns transistor states JSON
- **POST /set**: Control transistors (panels) - params: transistor, state
- **POST /simulation**: Start/stop simulation - params: action, duration, simulateSun
- **GET /simulation/data**: Get current simulation data JSON
- **POST /simulation/panel**: Set panel state - params: panel, state
- **POST /simulation/cell**: Set battery cell state - params: cell, state
- **POST /simulation/load**: Set load state - params: load, state
- **POST /simulation/autotoggle**: Enable/disable auto load management - params: enable
- **POST /simulation/currentmultiplier**: Set calibration multiplier - params: multiplier
- **GET /simulation/overview**: Get simulation summary after completion
- **GET /real/data**: Get live INA219 sensor readings

**transistor.cpp**: Hardware GPIO control
- Panel switching via MOSFETs (GPIO 15-18)
- State management for 4 panels
- Digital HIGH/LOW control

**ina.cpp**: INA219 sensor interface
- I2C communication (address 0x40)
- Voltage measurement (getBusVoltage) - returns Volts
- Current measurement (getCurrent) - returns milliamps
- Power calculation (getPower) - returns milliwatts
- Device detection (isFound)

**oled.cpp**: SSD1306 OLED display driver
- I2C communication (address 0x3C)
- 128x64 pixel resolution
- Boot splash screen
- Real-time status display:
  - IP address (top line)
  - Panel states (P1-P4 ON/OFF)
  - Current sensor reading (mA)
  - INA219 connection status
- 180° rotation for mounting orientation

**wifi_manager.cpp**: WiFi Access Point management
- Creates "Solar_Monitor" network
- Configurable password (default: "12345678")
- Configurable IP address (default: 192.168.4.1)
- Subnet: 255.255.255.0
- Returns current IP for display

## License

This project is developed as part of academic coursework at DHBW Stuttgart. See LICENSE file for details.

## Contributing

This is an educational project. If you find bugs or have suggestions:
1. Open an issue describing the problem
2. Provide serial monitor output if applicable
3. Include your hardware setup details

## Support

For issues related to:
- **Hardware Setup**: Check wiring diagrams and pin configurations
- **Software Bugs**: Enable serial monitor output for debugging
- **Simulation Questions**: Review simulation parameters in Settings
- **Calibration Issues**: Follow calibration procedure step-by-step

---

**Developed by**: Anton Engels 
**Module**: T3100 - Technical Project Work and T3000 
**Platform**: ESP32-S3 with Arduino Framework
