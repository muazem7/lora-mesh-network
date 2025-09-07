#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <Arduino.h>

#include "LoRaConnection.h"
#include "WiFiConnection.h"
#include "JsonMessage.h"
#include "HttpHandler.h"
#include "FileHandler.h"

class ConnectionHandler
{

public:
  JsonMessage jMsg;
  WiFiConnection wifi;
  LoraConnection lora;
  HttpHandler httpHandler;
  FileHandler fHandler;
  String dev_type;

  ConnectionHandler();
  void init(HttpHandler &httphandler, FileHandler &fhandler);
  void connect();
  bool isConnected();
  int getTypeIntVal(String value);
};

#endif