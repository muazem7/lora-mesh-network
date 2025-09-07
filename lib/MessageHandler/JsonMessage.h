#ifndef JSON_MESSAGE_H
#define JSON_MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class JsonMessage
{
public:
  JsonMessage();
  String serialize_json_msg(
      String typ,
      String id,
      String local_addr,
      String target_addr,
      String contentORreceiver,
      String msgCountORlevel);
  String serialize_json_info(
      String dev_type,
      String level,
      String receiver);
};
#endif