#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
// #include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"

// include task
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"

void setup()
{
  Serial.begin(115200);
  check_info_File(0);

  // Initialize SystemData resources
  systemData.dht20 = &dht20_inst;
  systemData.lcd = &lcd_inst;
  systemData.pixel = &pixel_inst;

  systemData.xMutex = xSemaphoreCreateMutex();
  systemData.xLedSemaphore = xSemaphoreCreateBinary();
  systemData.xNeoSemaphore = xSemaphoreCreateBinary();
  xSerialMutex = xSemaphoreCreateMutex();

  xTaskCreate(led_blinky, "Task LED Blink", 4096, &systemData, 2, NULL);
  xTaskCreate(neo_blinky, "Task NEO Blink", 4096, &systemData, 2, NULL);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 4096, &systemData, 2, NULL);
  // xTaskCreate(main_server_task, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(tiny_ml_task, "Tiny ML Task", 8192, &systemData, 2, NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
  // xTaskCreate(Task_Toogle_BOOT, "Task_Toogle_BOOT", 4096, NULL, 2, NULL);
}

void loop()
{
  if (check_info_File(1))
  {
    if (!Wifi_reconnect())
    {
      Webserver_stop();
    }
    else
    {
      //CORE_IOT_reconnect();
    }
  }
  Webserver_reconnect();
}