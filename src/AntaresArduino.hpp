#include <Arduino.h>

#if defined(ESP32)

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#endif

const char *ANTARES_URL PROGMEM = "platform.antares.id";
const char *ANTARES_PORT PROGMEM = "8443";
const char *GET_CA_URL PROGMEM = "http://get-ca.vmasdani.my.id";

class AntaresArduino
{
private:
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

    void initWifi()
    {
        WiFi.begin(_ssid, _password);
    }

    void checkWifi()
    {

        // Attempt to connect to WiFi 5 times.
        for (auto i = 0; i < 5; ++i)
        {
            Serial.println("Connecting... attempt " + (String)i);

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
            Serial.println("Failed connecting to " + (String)_ssid + "!");
            initWifi();
        }
        else
        {
            Serial.println("Connected to " + (String)_ssid + "!");

            WiFiClient client;
            HTTPClient http;

            String url = "";
            url += String(GET_CA_URL);
            url += String("/?url=");
            url += String(ANTARES_URL);
            url += String("&port=");
            url += String(ANTARES_PORT);

            if (http.begin(url))
            {
                auto statusCode = http.GET();

                if (statusCode > 0 && statusCode == HTTP_CODE_OK)
                {
                    auto res = http.getString();

                    Serial.println("[CERT]");
                    Serial.println(res);
                }
                else
                {
                    Serial.println("Error getting " + url);
                }
            }
        }
    }
};