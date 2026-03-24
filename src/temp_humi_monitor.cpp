#include "temp_humi_monitor.h"
#include "task_webserver.h"
#include "task_core_iot.h"



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
    // 1. Cast parameters to SystemData struct
    SystemData *data = (SystemData *)pvParameters;

    // Initialize I2C and Sensor
    Wire.begin(11, 12); // Configure SDA = 11, SCL = 12
    data->dht20->begin();
    
    // Initialize LCD
    data->lcd->begin();
    data->lcd->backlight();

    while (1) {
        // Read data from DHT20
        data->dht20->read();
        float temp = data->dht20->getTemperature();
        float humi = data->dht20->getHumidity();

        if (isnan(temp) || isnan(humi)) {
            Serial.println("Error: Failed to read data from DHT20!");
        } else {
            // 2. Use Mutex to safely update data (Prevent resource synchronization issues)
            if (xSemaphoreTake(data->xMutex, portMAX_DELAY)) {
                data->temperature = temp;
                data->humidity = humi;
                xSemaphoreGive(data->xMutex);
            }

            // 3. LCD display logic (Task 3 requirement: at least 3 states)
            data->lcd->clear();
            data->lcd->setCursor(0, 0);
            if (temp < 30) {
                data->lcd->print("State: Normal"); // Output normal mode
            } else if (temp < 35) {
                data->lcd->print("State: Warning"); // Output warning mode
            } else {
                data->lcd->print("State: Critical"); // Output critical mode
            }

            data->lcd->setCursor(0, 1);
            data->lcd->printf("T:%.1fC H:%.1f%%", temp, humi);

            // 4. Release Semaphore to "wake up" other Tasks
            xSemaphoreGive(data->xLedSemaphore);
            xSemaphoreGive(data->xNeoSemaphore);

            // Print log for verification and send data, all protected by mutex
            if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
                Serial.printf("Temp: %.2fC, Humi: %.2f%%\r\n", temp, humi);
                CORE_IOT_sendata("telemetry", "temperature", String(temp, 1));
                CORE_IOT_sendata("telemetry", "humidity", String(humi, 1));
                
                // Send updated data to Web Component via WebSocket
                String sensorJSON = "{\"page\":\"dashboard\",\"temp\":" + String(temp, 1) + ",\"humi\":" + String(humi, 1) + "}";
                if (Webserver_senddata(sensorJSON) == 0) {
                    Serial.println("No WebSocket clients connected!");
                } else {
                    Serial.println("Sent data to WebSocket clients.");
                }
                xSemaphoreGive(xSerialMutex);
            }
        }

        // Delay task 5 seconds per read
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}