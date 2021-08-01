#define ANTARES_DEBUG

#include <Arduino.h>
#include <AntaresArduino.h> // Include before AntaresArduino.h
#include <ArduinoJson.h>

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
}

void loop()
{
    // Ensure WiFi is well
    antares.checkWifi();

    auto latestData = String();

    if (antares.getLatest(latestData))
    {
        Serial.println("[Latest data]");
        Serial.println(latestData);
    }

    delay(5000);
}