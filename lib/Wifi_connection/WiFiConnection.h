#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <Arduino.h>

class WiFiConnection
{

  String wifi_ssid;
  String wifi_password;

public:
  void init(String _ssid, String _password);
  void connectWifi();
  bool isConnected();
};

#endif