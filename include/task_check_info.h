#ifndef __TASK_CHECK_INFO_H__
#define __TASK_CHECK_INFO_H__

#include <ArduinoJson.h>
#include "LittleFS.h"
#include "global.h"
#include "task_wifi.h"

extern String g_WIFI_SSID;
extern String g_WIFI_PASS;
extern String g_CORE_IOT_TOKEN;
extern String g_CORE_IOT_SERVER;
extern String g_CORE_IOT_PORT;

bool check_info_File(bool check);
void Load_info_File();
void Delete_info_File();
void Save_info_File(String in_WIFI_SSID, String in_WIFI_PASS, String in_CORE_IOT_TOKEN, String in_CORE_IOT_SERVER, String in_CORE_IOT_PORT);

#endif