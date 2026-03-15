#include "neo_blinky.h"



void neo_blinky(void *pvParameters) {
    // Ép kiểu tham số để truy cập tài nguyên hệ thống  
    SystemData *data = (SystemData *)pvParameters;

    // Khởi tạo NeoPixel thông qua con trỏ trong struct
    data->pixel->begin();
    data->pixel->setBrightness(50); // Đặt độ sáng vừa phải
    data->pixel->clear();
    data->pixel->show();

    while (1) {
        // Task 2: Sử dụng Semaphore để đồng bộ hóa việc cập nhật màu sắc 
        if (xSemaphoreTake(data->xNeoSemaphore, portMAX_DELAY) == pdTRUE) {
            
            float current_humi = 0;

            // Truy cập dữ liệu độ ẩm an toàn bằng Mutex 
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY) == pdTRUE) {
                current_humi = data->humidity;
                xSemaphoreGive(data->xMutex);
            }

            // Task 2: Định nghĩa ít nhất 3 mức độ ẩm/màu sắc 
            if (current_humi < 40.0) {
                // Mức 1: Khô (Dry) -> Màu Đỏ 
                data->pixel->setPixelColor(0, data->pixel->Color(255, 0, 0));
            } 
            else if (current_humi >= 40.0 && current_humi <= 70.0) {
                // Mức 2: Lý tưởng (Comfort) -> Màu Xanh lá 
                data->pixel->setPixelColor(0, data->pixel->Color(0, 255, 0));
            } 
            else {
                // Mức 3: Ẩm (Humid) -> Màu Xanh dương 
                data->pixel->setPixelColor(0, data->pixel->Color(0, 0, 255));
            }

            // Cập nhật hiển thị lên đèn
            data->pixel->show();
        }
        vTaskDelay(500);
        
        // Task này không cần vTaskDelay cố định vì nó "ngủ" đợi Semaphore 
    }
}