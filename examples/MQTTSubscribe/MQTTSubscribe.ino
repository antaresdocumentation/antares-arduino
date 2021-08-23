#define ANTARES_DEBUG

#include <Arduino.h>
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
    antares.initWifi(true);
    antares.initMqtt();
    antares.mqttCallback([](String topic, String payload)
                         {
                             Serial.println(F("Topic:"));
                             Serial.println(topic);
                             Serial.println(F("Payload:"));
                             Serial.println(payload);
                         });
}

void loop()
{
    antares.loop();
}