#define ANTARES_DEBUG

#include <Arduino.h>
#include <AntaresArduino.hpp>

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

    String json = R"(
        {
            "hello":"world!",
            "counter":1,
            "test":0.05
        }
    )";
    Serial.println(json);
    antares.send(json);

    delay(5000);
}