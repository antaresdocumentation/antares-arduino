#include <Arduino.h>

class AntaresStringBuilder
{
public:
    static void m2mDataBuilder(String &m2mData, String &data)
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

    static void appDeviceUrlBuilder(String &baseUrl, const char *_appName, const char *_deviceName)
    {
        baseUrl += "/~/antares-cse/antares-id/";
        baseUrl += _appName;
        baseUrl += "/";
        baseUrl += _deviceName;
    }

    static void initQueryStringBuilder(String &builder,
                                const char *method,
                                const char *_appName,
                                const char *_deviceName,
                                const char *ANTARES_URL,
                                const char *ANTARES_PORT,
                                const char *_key)
    {

        builder += method;
        builder += " ";

        appDeviceUrlBuilder(builder, _appName, _deviceName);

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
};