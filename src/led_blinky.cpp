#include "led_blinky.h"
void led_blinky(void *pvParameters) {
    SystemData *data = (SystemData *)pvParameters;
    
    pinMode(LED_GPIO, OUTPUT);
    uint32_t blink_delay = 1000; // Default value

    while (1) {
        // Wait for synchronization signal from Sensor Task (Task 1)
        if (xSemaphoreTake(data->xLedSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Safely access temperature data using Mutex
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY) == pdTRUE) {
                float current_temp = data->temperature;
                xSemaphoreGive(data->xMutex);

                // Task 1: Define 3 blinking behaviors based on temperature
                if (current_temp < 30.0) {
                    blink_delay = 2000; 
                } 
                else if (current_temp >= 30.0 && current_temp < 35.0) {
                    blink_delay = 1000;
                } 
                else {
                    blink_delay = 200;
                }
            }
        }

        // Execute blinking behavior
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
    }
}