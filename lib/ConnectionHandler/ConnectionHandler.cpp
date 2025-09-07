#include <Arduino.h>
#include <WiFi.h>

#include "ConnectionHandler.h"
#include "FileHandler.h"
#include "JsonMessage.h"

String ssid = "";
String password = "";
String wifiSettings = "wifi.txt";

ConnectionHandler::ConnectionHandler() {}

/**
 * Initializes the connection handler,
 * reads the WiFi SSID and password from the SD card and then initializes the LoRa connection
 */
void ConnectionHandler::init(HttpHandler &httphandler, FileHandler &fhandler)
{

    httpHandler = httphandler;
    fHandler = fhandler;
    String ssid_sd = fHandler.readLine(wifiSettings, 1);
    String pass_sd = fHandler.readLine(wifiSettings, 2);

    if (fHandler.isInitialised)
        wifi.init(ssid_sd, pass_sd);
    else
        wifi.init(ssid, password);

    lora.init();
}

/**
 * Connects the device to WiFi or the LoRa network
 * and reads the connection information from the database
 */
void ConnectionHandler::connect()
{

    wifi.connectWifi();

    if (wifi.isConnected())
    {
        bool typeIsSet = false;
        Serial.println("getting device type");
        while (!typeIsSet)
        {

            // Check if the device is in the database
            dev_type = httpHandler.get(lora.localAddress, "info");
            StaticJsonDocument<1024> doc;
            deserializeJson(doc, dev_type);
            JsonObject object = doc.as<JsonObject>();

            dev_type = object["type"].as<String>();
            // Serial.println(type);

            String json_msg;
            switch (getTypeIntVal(dev_type))
            {
            case -1: // not in database yet
                Serial.println("new device");

                json_msg = jMsg.serialize_json_info("0", String(lora.lora_connection_level), lora.lora_receiver_addr);
                httpHandler.put(lora.localAddress, "info", json_msg);
                break;
            case 1: // bridge
                lora.connectLoRa();
                typeIsSet = true;

                json_msg = jMsg.serialize_json_info("1", String(lora.lora_connection_level), lora.lora_receiver_addr);
                httpHandler.put(lora.localAddress, "info", json_msg);
                break;
            case 2: // receiver
                lora.lora_connection_level = 1;
                lora.lora_receiver_addr = lora.localAddress;
                typeIsSet = true;

                json_msg = jMsg.serialize_json_info("2", String(lora.lora_connection_level), lora.lora_receiver_addr);
                httpHandler.put(lora.localAddress, "info", json_msg);
                break;
            default:
                Serial.println("device type is not set, please set the type from database for the device: " + lora.localAddress);
            }

            delay(3000);
        }
        Serial.println("type was set successfully");
    }
    else
    {
        lora.connectLoRa();
        dev_type = 1;
    }
}
/**
 * Checks if there is a WiFi or LoRa connection
 */
bool ConnectionHandler::isConnected()
{
    return wifi.isConnected() || lora.isConnected();
}
/**
 * Returns the connection type read from the database.
 * 0 - no connection
 * 1 - SR node
 * 2 - receiver
 */
int ConnectionHandler::getTypeIntVal(String value)
{

    if (value.equals("null"))
        return -1;

    return atoi(value.c_str());
}