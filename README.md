# PV-Battery Ratio Experiment (T3000)

Dieses Repository beinhaltet den Quellcode für die Entwicklung eines technischen Experiments zur Bestimmung des optimalen Verhältnisses zwischen Photovoltaikanlagenleistung und Batteriespeicherkapazität.

Das Projekt entsteht im Rahmen der Hausarbeit (Modul T3000) und Studienarbeit (T3100) im Studiengang Embedded Systems an der DHBW Stuttgart bzw. Universität Stuttgart.

## 🎯 Projektziel
Ziel ist der Aufbau eines modularen Labormodells, das eine Tag-/Nacht-Situation simuliert und es ermöglicht, PV-Module und Batteriespeicher dynamisch zu- oder abzuschalten. Die gewonnenen Messdaten dienen als didaktisches Instrument für die technische Ausbildung (u.a. in Südafrika).

## 🛠 Tech Stack & Hardware
Das System basiert auf einem **ESP32** Mikrocontroller und wird mittels **PlatformIO** entwickelt.

**Hauptkomponenten:**
* **Controller:** ESP32 (Host für Webserver & Steuerungslogik)
* **Sensorik:** INA219 (I2C) zur Messung von Strom ($I$) und Spannung ($U$)
* **Aktorik:** Transistorschaltung zur dynamischen Steuerung der Solarpanele
* **Display:** OLED-Display zur lokalen Ausgabe der Messwerte
* **Connectivity:** WiFi SoftAP/Station Mode für Web-Interface

## 🚀 Features
1.  **Web-App Steuerung:** Über einen auf dem ESP32 gehosteten Webserver können die Transistoren geschaltet (PV-Module an/aus) und die Konfiguration geändert werden.
2.  **Echtzeit-Monitoring:** Anzeige der aktuellen Erzeugung und des Verbrauchs sowohl auf dem OLED als auch im Web-Dashboard.
3.  **Datenerfassung:** Kontinuierliches Messen der Energieströme zur späteren Auswertung des PV-Batterie-Verhältnisses.

## 📂 Struktur
* `/src`: C++ Quellcode (Main Loop, Sensor-Treiber, Webserver-Logik)
* `/include`: Header-Dateien
* `/lib`: Lokale Bibliotheken (z.B. für OLED oder INA219 Wrapper)
* `platformio.ini`: Konfigurationsdatei für Board und Libraries
