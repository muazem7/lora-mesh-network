#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HttpHandler.h>

HttpHandler::HttpHandler() {}

/**
 * Initializes the HttpHandler
 * @param _servername Address of the MQTT server
 */
void HttpHandler::init(String _servername)
{
    servername = String(_servername);
    Serial.println(servername);
}

/**
 * Sends a POST request to the MQTT server (new data)
 * @param boardID Device address
 * @param path Server path
 * @param msg_json The message in JSON format
 */
String HttpHandler::post(String boardID, String path, String msg_json)
{

    String data;

    if (WiFi.status() == WL_CONNECTED)
    {

        HTTPClient http;
        String serverPath = servername + boardID + "/" + path + ".json";
        long start = millis();
        http.begin(serverPath.c_str());

        int httpResponseCode = http.POST(msg_json);

        if (httpResponseCode > 0)
        {

            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);

            if (httpResponseCode == 200)
                data = payload;
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();

        Serial.println("httpReq time: " + String(millis() - start));
    }
    else
    {
        Serial.println("Http failed : WiFi is Disconnected");
    }

    return data;
}

/**
 * Sends a PUT request to the MQTT server (overwrite data)
 * @param boardID Device address
 * @param path Server path
 * @param msg_json The message in JSON format
 */
String HttpHandler::put(String boardID, String path, String msg_json)
{
    String data;
    // Update info in Database
    HTTPClient http;
    String serverPath_info = servername + boardID + "/" + path + ".json";
    http.begin(serverPath_info.c_str());

    int httpResponseCode = http.PUT(msg_json);

    if (httpResponseCode > 0)
    {

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        if (httpResponseCode == 200)
            data = payload;
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return data;
}

/**
 * Sends a GET request to the MQTT server (retrieve data)
 * @param boardID Device address
 * @param path Server path
 */
String HttpHandler::get(String boardID, String path)
{
    String data;

    // Update info in Database
    HTTPClient http;
    String serverPath_info = servername + boardID + "/" + path + ".json";
    http.begin(serverPath_info.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        if (httpResponseCode == 200)
            data = payload;
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return data;
}