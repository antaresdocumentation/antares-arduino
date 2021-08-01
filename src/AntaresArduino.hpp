#include <Arduino.h>

#if defined(ESP32)

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>

#endif

#if defined(ARDUINOJSON_VERSION)
#include <ArduinoJson.h>
#endif

#include "AntaresStringBuilder.hpp"
#include "constants.h"

class AntaresArduino
{
private:
    const char *_ssid;
    const char *_password;
    const char *_appName;
    const char *_deviceName;
    const char *_key;
    String _cert = "";

public:
    AntaresArduino(const char *ssid, const char *password, const char *appName, const char *deviceName, const char *key)
    {
        _ssid = ssid;
        _password = password;
        _appName = appName;
        _deviceName = deviceName;
        _key = key;
    }

    void initWifi()
    {
        WiFi.begin(_ssid, _password);
    }

    bool getCaCertificate()
    {

        WiFiClient client;
        HTTPClient http;

        String url = "";
        url += String(GET_CA_URL);
        url += String("/?url=");
        url += String(ANTARES_URL);
        url += String("&port=");
        url += String(ANTARES_PORT);

        if (
            http.begin(client, url))
        {
            auto statusCode = http.GET();

            if (statusCode > 0 && statusCode == HTTP_CODE_OK)
            {
                auto cert = http.getString();

                Serial.println(F("[CERT]"));
                Serial.println(cert);

                _cert = cert;

                return true;
            }
            else
            {
                Serial.print(F("Error getting "));
                Serial.print(url);
                Serial.print(" ");
                Serial.println(statusCode);

                return false;
            }
        }
        else
        {
            Serial.println(F("GET request error"));
            return false;
        }
    }

    void checkWifi()
    {

        // Attempt to connect to WiFi 5 times.
        for (auto i = 0; i < 5; ++i)
        {
            Serial.print(F("Connecting... attempt "));
            Serial.println(i + 1);

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
            Serial.print(F("Failed connecting to "));
            Serial.println(_ssid);
            checkWifi();
        }
        else
        {
            Serial.print(F("Connected to "));
            Serial.println(_ssid);
            // Try getting cert
            auto success = getCaCertificate();

            if (success)
            {
#if defined(ESP32)
                // ESP32, does not need to synchronise NTP.

#elif defined(ESP8266)
                // ESP8266, need to synchronize NTP
                // See https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino

                // Set time via NTP, as required for x.509 validation
                configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

                Serial.print(F("Waiting for NTP time sync: "));
                time_t now = time(nullptr);
                while (now < 8 * 3600 * 2)
                {
                    delay(500);
                    Serial.print(".");
                    now = time(nullptr);
                }
                Serial.println(F(""));
                struct tm timeinfo;
                gmtime_r(&now, &timeinfo);
                Serial.print(F("Current time: "));
                Serial.print(asctime(&timeinfo));
#endif
            }
            else
            {
                Serial.println(F("Getting certificate failed."));
                checkWifi();
            }
        }
    }

    bool send(String &data)
    {
        WiFiClientSecure client;
        auto success = getSecureClient(client);

#if defined(ESP8266)
        // ESP8266 setInsecure temporarily. Will fix later
        client.setInsecure();
#endif

#ifdef ANTARES_DEBUG
        Serial.println(F("Getting client finished..."));
#endif

        if (success)
        {
            HTTPClient http;

            // Build App & Device URL
            auto appDeviceUrl = "https://" + String(ANTARES_URL) + ":" + ANTARES_PORT;
            AntaresStringBuilder::appDeviceUrlBuilder(appDeviceUrl, _appName, _deviceName);

#ifdef ANTARES_DEBUG
            Serial.print(F("Sending data with HTTPS client to "));
            Serial.println(appDeviceUrl);
#endif

            http.begin(client, appDeviceUrl);
            http.addHeader("X-M2M-Origin", _key);
            http.addHeader("Content-Type", "application/json;ty=4");
            http.addHeader("Accept", "application/json");
            http.addHeader("Connection", "close");

            auto m2mData = String();
            AntaresStringBuilder::m2mDataBuilder(m2mData, data);

#ifdef ANTARES_DEBUG
            Serial.println(F("m2m data:"));
            Serial.println(m2mData);
#endif

            auto httpCode = http.POST(m2mData);

            Serial.print(F("Response code: "));
            Serial.println(httpCode);

            if (httpCode == 201)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.println(F("Getting secure client error."));
#endif
            return false;
        }

        return false;
    }

    bool getLatest(String &responseString)
    {
#if defined(ARDUINOJSON_VERSION)
        WiFiClientSecure client;
        auto success = getSecureClient(client);

#if defined(ESP8266)
        // ESP8266 setInsecure temporarily. Will fix later
        client.setInsecure();
#endif

#ifdef ANTARES_DEBUG
        Serial.println(F("Getting client finished..."));
#endif

        if (success)
        {
            HTTPClient http;

            // Build App & Device URL
            auto appDeviceUrl = "https://" + String(ANTARES_URL) + ":" + ANTARES_PORT;
            AntaresStringBuilder::appDeviceUrlBuilder(appDeviceUrl, _appName, _deviceName);
            appDeviceUrl += "/la";

#ifdef ANTARES_DEBUG
            Serial.print(F("Getting data with HTTP client to "));
            Serial.println(appDeviceUrl);
#endif

            http.begin(client, appDeviceUrl);
            http.addHeader("X-M2M-Origin", _key);
            http.addHeader("Content-Type", "application/json;ty=4");
            http.addHeader("Accept", "application/json");
            http.addHeader("Connection", "close");

            auto httpCode = http.GET();

            Serial.print(F("Response code: "));
            Serial.println(httpCode);

            if (httpCode == 200)
            {
                // 16kb max
                DynamicJsonDocument doc(16384);

                auto err = deserializeJson(doc, http.getString());

                if (err)
                {
                    Serial.println(F("Deserialization error"));
                    return false;
                }
                else
                {
                    responseString = doc["m2m:cin"]["con"].as<String>();
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.println(F("Getting secure client error."));
#endif

            return false;
        }

        return false;
#else
        Serial.println(F("[getLatest error] ArduinoJSON v6 not installed or not included. Please install and include ArduinoJSON v6 first."));
#endif

        return false;
    }

    bool getSecureClient(WiFiClientSecure &client)
    {

#if defined(ESP32)
        client.setCACert(_cert.c_str());

#elif defined(ESP8266)
        // Use BearSSL
        X509List cert(_cert.c_str());
        client.setTrustAnchors(&cert);

#endif

        if (!client.connect(ANTARES_URL, ANTARES_PORT))
        {
#ifdef ANTARES_DEBUG
            Serial.print(F("Connecting to "));
            Serial.print(ANTARES_URL);
            Serial.print(F(":"));
            Serial.print(ANTARES_PORT);
            Serial.println(F(" failed! Retrying..."));

#endif

            return false;
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.print(F("Connecting to "));
            Serial.print(ANTARES_URL);
            Serial.print(F(":"));
            Serial.print(ANTARES_PORT);
            Serial.println(F(" successful!"));
#endif

            return true;
        }

        return false;
    }
};