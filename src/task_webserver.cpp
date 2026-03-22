#include "task_webserver.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool webserver_isrunning = false;

int Webserver_senddata(String data)
{
    int clients = ws.count();
    if (clients > 0)
    {
        ws.textAll(data); // Send to all connected clients
    }
    return clients;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
            Serial.printf("WebSocket client #%u connected from %s\r\n", client->id(), client->remoteIP().toString().c_str());
            xSemaphoreGive(xSerialMutex);
        }
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
            Serial.printf("WebSocket client #%u disconnected\r\n", client->id());
            xSemaphoreGive(xSerialMutex);
        }
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->opcode == WS_TEXT)
        {
            String message;
            message += String((char *)data).substring(0, len);
            // parseJson(message, true);
            handleWebSocketMessage(message);
        }
    }
}

void connnectWSV()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "application/javascript"); });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });
    server.begin();
    ElegantOTA.begin(&server);
    webserver_isrunning = true;
}

void Webserver_stop()
{
    ws.closeAll();
    server.end();
    webserver_isrunning = false;
}

void Webserver_reconnect()
{
    if (!webserver_isrunning)
    {
        connnectWSV();
    }
    ElegantOTA.loop();
}
