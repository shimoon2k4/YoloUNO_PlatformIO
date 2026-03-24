#include "task_check_info.h"

#include "task_check_info.h"

String g_WIFI_SSID;
String g_WIFI_PASS;
String g_CORE_IOT_TOKEN;
String g_CORE_IOT_SERVER;
String g_CORE_IOT_PORT;

void Load_info_File()
{
  File file = LittleFS.open("/info.dat", "r");
  if (!file)
  {
    return;
  }
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
  }
  else
  {
    g_WIFI_SSID = doc["WIFI_SSID"].as<String>();
    g_WIFI_PASS = doc["WIFI_PASS"].as<String>();
    g_CORE_IOT_TOKEN = doc["CORE_IOT_TOKEN"].as<String>();
    g_CORE_IOT_SERVER = doc["CORE_IOT_SERVER"].as<String>();
    g_CORE_IOT_PORT = doc["CORE_IOT_PORT"].as<String>();
  }
  file.close();
}

void Delete_info_File()
{
  if (LittleFS.exists("/info.dat"))
  {
    LittleFS.remove("/info.dat");
  }
  ESP.restart();
}

void Save_info_File(String in_WIFI_SSID, String in_WIFI_PASS, String in_CORE_IOT_TOKEN, String in_CORE_IOT_SERVER, String in_CORE_IOT_PORT)
{
  Serial.println(in_WIFI_SSID);
  Serial.println(in_WIFI_PASS);

  DynamicJsonDocument doc(4096);
  doc["WIFI_SSID"] = in_WIFI_SSID;
  doc["WIFI_PASS"] = in_WIFI_PASS;
  doc["CORE_IOT_TOKEN"] = in_CORE_IOT_TOKEN;
  doc["CORE_IOT_SERVER"] = in_CORE_IOT_SERVER;
  doc["CORE_IOT_PORT"] = in_CORE_IOT_PORT;

  File configFile = LittleFS.open("/info.dat", "w");
  if (configFile)
  {
    serializeJson(doc, configFile);
    configFile.close();
  }
  else
  {
    Serial.println("Unable to save the configuration.");
  }
  ESP.restart();
};

bool check_info_File(bool check)
{
  if (!check)
  {
    if (!LittleFS.begin(true))
    {
      Serial.println("Error initializing LittleFS!");
      return false;
    }
    Load_info_File();
  }
  
  if (g_WIFI_SSID.isEmpty() && g_WIFI_PASS.isEmpty())
  {
    if (!check)
    {
      startAP();
    }
    return false;
  }
  return true;
}