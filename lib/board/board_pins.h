#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#define SERIAL_DEBUG_BAUD 115200
#define BAND 868E6

#define BOARD_T_CALL
// #define BOARD_TTGO_OLED_V1
// #define BOARD_TTGO_T_BEAM

#if defined(BOARD_T_CALL)

// #define WIFI_LED 12

// TTGO T-SIM pin definitions
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_DTR 25
#define MODEM_TX 26
#define MODEM_RX 27

#define I2C_SDA 21
#define I2C_SCL 22

#define PIN_ADC_BAT 35
#define PIN_ADC_SOLAR 36
#define ADC_BATTERY_LEVEL_SAMPLES 100

// SD_Card
#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13
// LoRa
#define LORA_RST 12
#define LORA_DI0 32
#define RADIO_DIO_1 33
#define RADIO_DIO_2 34
#define LORA_SS 5
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SCK 18

#elif defined(BOARD_TTGO_OLED_V1)
// #define WIFI_LED    25
#define LORA_SCK 5   // GPIO5  -- SX1278's SCK
#define LORA_MISO 19 // GPIO19 -- SX1278's MISnO
#define LORA_MOSI 27 // GPIO27 -- SX1278's MOSI
#define LORA_SS 18   // GPIO18 -- SX1278's CS
#define LORA_RST 14  // GPIO14 -- SX1278's RESET
#define LORA_DI0 26  // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define HSD 15
#elif defined(BOARD_TTGO_T_BEAM)
// #define WIFI_LED 14
#define LORA_SCK 5   // GPIO5  -- SX1278's SCK
#define LORA_MISO 19 // GPIO19 -- SX1278's MISnO
#define LORA_MOSI 27 // GPIO27 -- SX1278's MOSI
#define LORA_SS 18   // GPIO18 -- SX1278's CS
// #define LORA_RST     14   // GPIO14 -- SX1278's RESET
#define LORA_DI0 26  // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define HSD 15
#endif

#endif
