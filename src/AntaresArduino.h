#include <Arduino.h>

#if defined(ESP32)

#include <WiFi.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#endif

class AntaresArduino
{
private:
    char *_url PROGMEM = "https://platform.antares.id";
    char *_getCaUrl PROGMEM = "http://get-ca.vmasdani.my.id";
    char *_ssid;
    char *_password;
    char *_appId;
    char *_deviceId;

public:
    AntaresArduino(char *ssid, char *password, char *appId, char *deviceId)
    {
        _ssid = ssid;
        _password = password;
        _appId = appId;
        _deviceId = deviceId;
    }

    static void initWifi(char *ssid, char *password)
    {

        WiFi.begin(ssid, password);
    }

    static void checkWifi()
    {

        // Attempt to connect to WiFi 5 times.
        for (auto i = 0; i < 5; ++i)
        {
            Serial.println("Connecting... attempt " + i);

            for (auto j = 0; j < 5; ++j)
            {
                Serial.print(".");
                delay(1000);

                if (WiFi.status() == WL_CONNECTED)
                {
                    break;
                }
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                break;
            }
        }

        // If unable, repeat.
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Failed connecting to " + ssid + "!");
            initWifi(ssid, password);
        }
        else
        {
            Serial.println("Connected to " + ssid + "!");
        }
    }
}