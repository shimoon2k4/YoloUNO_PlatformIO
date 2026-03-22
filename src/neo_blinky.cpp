#include "neo_blinky.h"



void neo_blinky(void *pvParameters) {
    // Cast parameter to access system resources
    SystemData *data = (SystemData *)pvParameters;

    // Output NeoPixel via struct pointer
    data->pixel->begin();
    data->pixel->setBrightness(50); // Set moderate brightness
    data->pixel->clear();
    data->pixel->show();

    while (1) {
        // Task 2: Use Semaphore to synchronize color updates
        if (xSemaphoreTake(data->xNeoSemaphore, portMAX_DELAY) == pdTRUE) {
            
            float current_humi = 0;

            // Safely access humidity data using Mutex
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY) == pdTRUE) {
                current_humi = data->humidity;
                xSemaphoreGive(data->xMutex);
            }

            // Task 2: Define at least 3 humidity levels/colors
            if (current_humi < 40.0) {
                // Level 1: Dry -> Red
                data->pixel->setPixelColor(0, data->pixel->Color(255, 0, 0));
            } 
            else if (current_humi >= 40.0 && current_humi <= 70.0) {
                // Level 2: Comfort -> Green
                data->pixel->setPixelColor(0, data->pixel->Color(0, 255, 0));
            } 
            else {
                // Level 3: Humid -> Blue
                data->pixel->setPixelColor(0, data->pixel->Color(0, 0, 255));
            }

            // Update display on LED
            data->pixel->show();
        }
        vTaskDelay(500);
        
        // This task does not need a fixed vTaskDelay as it "sleeps" waiting for Semaphore
    }
}