#include <task_handler.h>

void handleWebSocketMessage(String message)
{
    Serial.println(message);
    StaticJsonDocument<256> doc;

        DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        Serial.println("JSON parsing error!");
        return;
    }
    JsonObject value = doc["value"];
    if (doc["page"] == "device")
    {
        if (!value.containsKey("gpio") || !value.containsKey("status"))
        {
            Serial.println("JSON missing gpio or status info");
            return;
        }

        int gpio = value["gpio"];
        String status = value["status"].as<String>();

        Serial.printf("Controlling GPIO %d -> %s\r\n", gpio, status.c_str());
        pinMode(gpio, OUTPUT);
        if (status.equalsIgnoreCase("ON"))
        {
            digitalWrite(gpio, HIGH);
            Serial.printf("GPIO %d ON\r\n", gpio);
        }
        else if (status.equalsIgnoreCase("OFF"))
        {
            digitalWrite(gpio, LOW);
            Serial.printf("GPIO %d OFF\r\n", gpio);
        }
    }
    else if (doc["page"] == "setting")
    {
        String l_WIFI_SSID = doc["value"]["ssid"].as<String>();
        String l_WIFI_PASS = doc["value"]["password"].as<String>();
        String l_CORE_IOT_TOKEN = doc["value"]["token"].as<String>();
        String l_CORE_IOT_SERVER = doc["value"]["server"].as<String>();
        String l_CORE_IOT_PORT = doc["value"]["port"].as<String>();

        Serial.println("Received configuration from WebSocket:");
        Serial.println("SSID: " + l_WIFI_SSID);
        Serial.println("PASS: " + l_WIFI_PASS);
        Serial.println("TOKEN: " + l_CORE_IOT_TOKEN);
        Serial.println("SERVER: " + l_CORE_IOT_SERVER);
        Serial.println("PORT: " + l_CORE_IOT_PORT);

        // Call function to save configuration
        Save_info_File(l_WIFI_SSID, l_WIFI_PASS, l_CORE_IOT_TOKEN, l_CORE_IOT_SERVER, l_CORE_IOT_PORT);

        // Respond to client (optional)
        String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
        ws.textAll(msg);
    }
    else if (doc["page"] == "wifi_change")
    {
        String l_WIFI_SSID = doc["value"]["ssid"].as<String>();
        String l_WIFI_PASS = doc["value"]["password"].as<String>();

        Serial.println("=== WIFI CHANGE REQUESTED ===");
        Serial.println("New SSID: " + l_WIFI_SSID);
        Serial.println("New Password: " + l_WIFI_PASS);

        // Update only WiFi, keep CoreIOT settings
        g_WIFI_SSID = l_WIFI_SSID;
        g_WIFI_PASS = l_WIFI_PASS;
        // Keep existing CoreIOT settings
        // g_CORE_IOT_TOKEN, g_CORE_IOT_SERVER, g_CORE_IOT_PORT unchanged

        // Save to file (preserves CoreIOT settings)
        Save_info_File(g_WIFI_SSID, g_WIFI_PASS, g_CORE_IOT_TOKEN, g_CORE_IOT_SERVER, g_CORE_IOT_PORT);

        Serial.println("WiFi configuration updated! Device will reconnect...");
    }
    else if (doc["page"] == "reset")
    {
        Serial.println("=== RESET CONFIGURATION REQUESTED ===");
        Serial.println("Deleting configuration file...");
        Delete_info_File();
    }
}
