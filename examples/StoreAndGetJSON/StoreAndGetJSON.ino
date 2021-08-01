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
}

void loop()
{
    // Ensure WiFi is well
    antares.checkWifi();

    // Send data first
    String json = R"(
        {
            "hello":"world!",
            "counter":1,
            "test":0.05
        }
    )";
    Serial.println(json);
    antares.send(json);

    // Retrieve the sent data
    auto latestData = String();

    if (antares.getLatest(latestData))
    {
        Serial.println("[Latest data]");
        Serial.println(latestData);

        DynamicJsonDocument doc(1024);
        auto err = deserializeJson(doc, latestData);

        if (err)
        {
            Serial.println("Deserialization error: ");
            Serial.println(err.c_str());
        }
        else
        {
            Serial.println("Hello: " + (String)doc["hello"]);
            Serial.println("Counter: " + (String)doc["counter"]);
            Serial.println("Test: " + (String)doc["test"]);
        }
    }

    delay(5000);
}