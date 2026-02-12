#ifndef CONFIG_H
#define CONFIG_H

// I2C Pin Configuration
#define OLED_SDA 9
#define OLED_SCL 8

// I2C Addresses
#define OLED_ADDR 0x3C
#define INA219_ADDR 0x40

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Transistor GPIO Pins
#define TRANSISTOR_1 15
#define TRANSISTOR_2 16
#define TRANSISTOR_3 17
#define TRANSISTOR_4 18

// WiFi Access Point Settings
#define DEFAULT_AP_IP IPAddress(192, 168, 4, 1)

// Simulation Settings
#define DEFAULT_CURRENT_MULTIPLIER 600.0

#endif // CONFIG_H
