#include "global.h"

// float glob_temperature = 0;
// float glob_humidity = 0;

SystemData systemData;
DHT20 dht20_inst;
LiquidCrystal_I2C lcd_inst(33, 16, 2);
Adafruit_NeoPixel pixel_inst(1, 45, NEO_GRB + NEO_KHZ800);

String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String ssid = "ESP32-YOUR NETWORK HERE!!!";
String password = "12345678";
String wifi_ssid = "abcde";
String wifi_password = "123456789";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
SemaphoreHandle_t xSerialMutex;