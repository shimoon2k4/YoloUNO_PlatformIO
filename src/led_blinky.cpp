#include "led_blinky.h"
void led_blinky(void *pvParameters) {
    SystemData *data = (SystemData *)pvParameters;
    
    pinMode(LED_GPIO, OUTPUT);
    uint32_t blink_delay = 1000; // Giá trị mặc định

    while (1) {
        // Đợi tín hiệu đồng bộ từ Task cảm biến (Task 1) 
        if (xSemaphoreTake(data->xLedSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Truy cập dữ liệu nhiệt độ an toàn bằng Mutex 
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY) == pdTRUE) {
                float current_temp = data->temperature;
                xSemaphoreGive(data->xMutex);

                // Task 1: Định nghĩa 3 hành vi chớp tắt theo nhiệt độ 
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

        // Thực hiện hành vi chớp tắt
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
    }
}