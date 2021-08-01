#define ANTARES_DEBUG

#include <Arduino.h>
#include <ArduinoJson.h> // Include before AntaresArduino.h
#include <AntaresArduino.h>

#define SSID "my-ssid"
#define PASSWORD "my-password"
#define APP_NAME "my-app-name"
#define DEVICE_NAME "my-key-name"
#define KEY "my-credentials"

AntaresArduino antares(
    SSID,
    PASSWORD,
    APP_NAME,
    DEVICE_NAME,
    KEY);

void setup()
{
    Serial.begin(115200);
    antares.initWifi();
    randomSeed(analogRead(0));
}

void loop()
{
    // Ensure WiFi is well
    antares.checkWifi();

    // Construct new JSON document
    DynamicJsonDocument doc(1024);

    doc["temp"] = random(20, 35);
    doc["hum"] = random(80, 100);
    doc["lat"] = -6.8699498;
    doc["lng"] = 107.5892773;

    // Serialize document to String
    auto data = String();
    serializeJson(doc, data);

    antares.send(data);

    delay(5000);
}