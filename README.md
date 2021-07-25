# antares-arduino

A unified library to connect ESP8266 and ESP32 to interact with the Antares IoT platform, via these protocols:
1. HTTPS
2. Secure MQTT (WIP)

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

WIP

## Installation

### Arduino IDE

WIP

### PlatformIO

WIP
