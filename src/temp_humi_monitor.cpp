#include "temp_humi_monitor.h"
DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);


// void temp_humi_monitor(void *pvParameters){

//     Wire.begin(11, 12);
//     Serial.begin(115200);
//     dht20.begin();

//     while (1){
//         /* code */
        
//         dht20.read();
//         // Reading temperature in Celsius
//         float temperature = dht20.getTemperature();
//         // Reading humidity
//         float humidity = dht20.getHumidity();

        

//         // Check if any reads failed and exit early
//         if (isnan(temperature) || isnan(humidity)) {
//             Serial.println("Failed to read from DHT sensor!");
//             temperature = humidity =  -1;
//             //return;
//         }

//         //Update global variables for temperature and humidity
//         glob_temperature = temperature;
//         glob_humidity = humidity;

//         // Print the results
        
//         Serial.print("Humidity: ");
//         Serial.print(humidity);
//         Serial.print("%  Temperature: ");
//         Serial.print(temperature);
//         Serial.println("°C");
        
//         vTaskDelay(5000);
//     }
    
// }
void temp_humi_monitor(void *pvParameters) {
    // 1. Ép kiểu tham số (pvParameters) về struct SystemData
    SystemData *data = (SystemData *)pvParameters;

    // Khởi tạo cảm biến (Sử dụng con trỏ từ struct)
    data->dht20->begin();

    while (1) {
        // Đọc dữ liệu từ DHT20
        data->dht20->read();
        float temp = data->dht20->getTemperature();
        float humi = data->dht20->getHumidity();

        if (isnan(temp) || isnan(humi)) {
            Serial.println("Lỗi: Không đọc được dữ liệu từ DHT20!");
        } else {
            // 2. Sử dụng Mutex để cập nhật dữ liệu an toàn (Tránh xung đột tài nguyên)
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY)) {
                data->temperature = temp;
                data->humidity = humi;
                xSemaphoreGive(data->xMutex);
            }

            // 3. Logic hiển thị LCD (Yêu cầu Task 3: ít nhất 3 trạng thái) 
            data->lcd->clear();
            data->lcd->setCursor(0, 0);
            if (temp < 30) {
                data->lcd->print("State: Normal"); // Trạng thái bình thường
            } else if (temp < 35) {
                data->lcd->print("State: Warning"); // Trạng thái cảnh báo
            } else {
                data->lcd->print("State: Critical"); // Trạng thái nguy cấp
            }

            data->lcd->setCursor(0, 1);
            data->lcd->printf("T:%.1fC H:%.1f%%", temp, humi);

            // 4. Giải phóng Semaphore để "đánh thức" các Task khác 
            xSemaphoreGive(data->xLedSemaphore);
            xSemaphoreGive(data->xNeoSemaphore);

            // Log để kiểm tra
            Serial.printf("Nhiet do: %.2f, Do am: %.2f\n", temp, humi);
        }

        // Delay task 5 giây mỗi lần đọc
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}