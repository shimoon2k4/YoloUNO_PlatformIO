#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "LiquidCrystal_I2C.h"
#include "DHT20.h"
#include <Adafruit_NeoPixel.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// extern float glob_temperature;
// extern float glob_humidity;
struct SystemData {
    // 1. Dữ liệu cảm biến
    float temperature;
    float humidity;

    SemaphoreHandle_t xMutex;         // Bảo vệ quyền truy cập dữ liệu (Shared Resource) 
    SemaphoreHandle_t xLedSemaphore;  // Tín hiệu cho Task 1 (LED) 
    SemaphoreHandle_t xNeoSemaphore;  // Tín hiệu cho Task 2 (NeoPixel) 

    // 3. Con trỏ thiết bị (Để điều khiển từ bất kỳ Task nào)
    DHT20 *dht20;
    Adafruit_NeoPixel *pixel;
    LiquidCrystal_I2C *lcd;
};

extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;
#endif