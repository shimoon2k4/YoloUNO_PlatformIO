#include "global.h"
// float glob_temperature = 0;
// float glob_humidity = 0;
struct SystemData {
    float temperature;
    float humidity;

    // Semaphores để đồng bộ hóa 
    SemaphoreHandle_t xMutex;         // Bảo vệ dữ liệu temp/humi
    SemaphoreHandle_t xLedSemaphore;  // Tín hiệu cho Task 1
    SemaphoreHandle_t xNeoSemaphore;  // Tín hiệu cho Task 2

    // Con trỏ thiết bị
    DHT20 *dht20;
    Adafruit_NeoPixel *pixel;
};

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