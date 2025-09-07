
#include "WiFiConnection.h"
#include "WiFi.h"
#include "board_pins.h"

void WiFiConnection::init(String _ssid, String _password)
{
    wifi_ssid = String(_ssid);
    wifi_password = String(_password);
}
/**
 * Connects the device to the WiFi network
 */
void WiFiConnection::connectWifi()
{
    Serial.println("connecting to WiFi..");

    int retry = 0;
    while (!WiFi.isConnected() && retry++ < 7)
    {
        WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
        delay(2000);
        Serial.printf(".");
    }

    Serial.print("\nWiFi connected SSID: " + WiFi.SSID() + ", IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
}
/**
 * Returns the WiFi connection status
 */
bool WiFiConnection::isConnected()
{
    return WiFi.isConnected();
}
