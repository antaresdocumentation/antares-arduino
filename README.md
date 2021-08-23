# antares-arduino

A unified library to connect ESP8266 and ESP32 to interact with the Antares IoT platform, via these protocols:

1. HTTPS
2. MQTT

### Table of Contents

1. [Quick Start](https://github.com/antaresdocumentation/antares-arduino#quick-start)
2. [Constructing JSON with ArduinoJSON](https://github.com/antaresdocumentation/antares-arduino#constructing-json-with-arduinojson)
3. [Get Latest Data And Parse JSON](https://github.com/antaresdocumentation/antares-arduino#get-latest-data-and-parse-json)
4. [Subscribe MQTT](https://github.com/antaresdocumentation/antares-arduino#subscribe-mqtt)

### Quick Start

Working Example for ESP32 or ESP8266 to send string JSON data:

```cpp
#include <AntaresArduino.hpp>

AntaresArduino antares(
    "SSID",
    "PASSWORD",
    "APP_NAME",
    "DEVICE_NAME",
    "KEY");

void setup()
{
    Serial.begin(115200);
    antares.initWifi();
}

void loop()
{
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
```

Full example [here](https://github.com/antaresdocumentation/antares-arduino/blob/main/examples/Store/Store.ino).

## Constructing JSON with ArduinoJSON

Creating JSON data types is easy with ArduinoJSON. Just follow this example:

```cpp
#include <ArduinoJson.h> // Include before AntaresArduino.h
#include <AntaresArduino.h>

...

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

```

Full example [here](https://github.com/antaresdocumentation/antares-arduino/blob/main/examples/StoreJSON/StoreJSON.ino).

### Get Latest Data and Parse JSON

```cpp
#include <ArduinoJson.h> // Include before AntaresArduino.h
#include <AntaresArduino.h>

...

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

```

Full example [here](https://github.com/antaresdocumentation/antares-arduino/blob/main/examples/StoreAndGetJSON/StoreAndGetJSON.ino).

### Subscribe MQTT

```cpp
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
```
Full example [here](https://github.com/antaresdocumentation/antares-arduino/blob/main/examples/MQTTSubscribe/MQTTSubscribe.ino).

## Installation

### Arduino IDE

WIP

### PlatformIO

WIP
