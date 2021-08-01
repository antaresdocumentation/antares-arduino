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

const char *ANTARES_URL PROGMEM = "platform.antares.id";
const int ANTARES_PORT PROGMEM = 8443;
const char *GET_CA_URL PROGMEM = "http://get-ca.vmasdani.my.id";

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
#if defined(ESP8266)
            http.begin(client, url)

#elif defined(ESP32)
            http.begin(url)
#endif
        )
        {
            auto statusCode = http.GET();

            if (statusCode > 0 && statusCode == HTTP_CODE_OK)
            {
                auto cert = http.getString();

                Serial.println("[CERT]");
                Serial.println(cert);

                _cert = cert;

                return true;
            }
            else
            {
                Serial.println("Error getting " + url + " " + String(statusCode));
                return false;
            }
        }
        else
        {
            Serial.println("GET request error");
            return false;
        }
    }

    void checkWifi()
    {

        // Attempt to connect to WiFi 5 times.
        for (auto i = 0; i < 5; ++i)
        {
            Serial.println("Connecting... attempt " + (String)(i + 1));

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
            checkWifi();
        }
        else
        {
            Serial.println("Connected to " + (String)_ssid + "!");

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

                Serial.print("Waiting for NTP time sync: ");
                time_t now = time(nullptr);
                while (now < 8 * 3600 * 2)
                {
                    delay(500);
                    Serial.print(".");
                    now = time(nullptr);
                }
                Serial.println("");
                struct tm timeinfo;
                gmtime_r(&now, &timeinfo);
                Serial.print("Current time: ");
                Serial.print(asctime(&timeinfo));
#endif
            }
            else
            {
                Serial.println("Getting certificate failed.");
                checkWifi();
            }
        }
    }

    void send(String &data)
    {
        auto m2mData = String();
        m2mDataBuilder(m2mData, data);

        WiFiClientSecure client;
        auto success = getSecureClient(client);

#ifdef ANTARES_DEBUG
        Serial.println("Getting client finished...");
#endif

        if (success)
        {
#ifdef ANTARES_DEBUG
            Serial.println("Sending data...");
            Serial.println(data);
#endif

            auto builder = String("");
            initQueryStringBuilder(builder, "POST");

            builder += "Content-Length: ";
            builder += m2mData.length();
            builder += "\r\n";

            // Add space for data;
            builder += "\r\n";

            // Build data
            builder += m2mData;

#ifdef ANTARES_DEBUG
            Serial.println(builder);
#endif

            Serial.println("Sending to Antares!");
            client.print(builder);

            while (client.connected())
            {
                String line = client.readStringUntil('\n');
                if (line == "\r")
                {
                    Serial.println("Store success!");
                    break;
                }
            }

            String line = client.readStringUntil('\n');
            Serial.println(line);
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.println("Getting secure client error.");
#endif
        }
    }

    void sendHttpClient(String &data)
    {
        auto m2mData = String();
        m2mDataBuilder(m2mData, data);

        WiFiClientSecure client;
        auto success = getSecureClient(client);

#ifdef ANTARES_DEBUG
        Serial.println("Getting client finished...");
#endif

        if (success)
        {
            HTTPClient http;

            // Build App & Device URL
            auto appDeviceUrl = String(ANTARES_URL);
            appDeviceUrlBuilder(appDeviceUrl);

#ifdef ANTARES_DEBUG
            Serial.println("Getting data with HTTP client to " + appDeviceUrl);
#endif

            http.begin(client, appDeviceUrl);

#ifdef ANTARES_DEBUG
            Serial.println("No errors here.");
#endif
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.println("Getting secure client error.");
#endif
        }
    }

    void m2mDataBuilder(String &m2mData, String &data)
    {
        // Allow no whitespaces or \r, \n
        // data.replace(" ", "");

        // Replace " with \"
        data.replace("\"", "\\\"");

        m2mData += "{";
        m2mData += "\"m2m:cin\":{";
        m2mData += "\"con\":\"" + data + "\"";
        m2mData += "}";
        m2mData += "}";
    }

    void appDeviceUrlBuilder(String &baseUrl)
    {
        baseUrl += "/~/antares-cse/antares-id/";
        baseUrl += _appName;
        baseUrl += "/";
        baseUrl += _deviceName;
    }

    void initQueryStringBuilder(String &builder, const char *method)
    {

        builder += method;
        builder += " ";

        appDeviceUrlBuilder(builder);

        builder += " HTTP/1.1\r\n";

        builder += "Host: ";
        builder += ANTARES_URL;
        builder += ":";
        builder += ANTARES_PORT;
        builder += "\r\n";

        builder += "X-M2M-Origin: ";
        builder += _key;
        builder += "\r\n";

        builder += "Content-Type: application/json;ty=4\r\n";

        builder += "Accept: application/json\r\n";

        builder += "Connection: close\r\n";
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
            Serial.println("Connecting to " + String(ANTARES_URL) + ":" + String(ANTARES_PORT) + " failed! Retrying...");
#endif

            return false;
        }
        else
        {
#ifdef ANTARES_DEBUG
            Serial.println("Connecting to " + String(ANTARES_URL) + ":" + String(ANTARES_PORT) + " successful!");
#endif

            return true;
        }

        return false;
    }
};