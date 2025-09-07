#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <queue>
#include "FileHandler.h"
class Message
{
public:
  Message(String json) : json(json), send_counter(0) {}
  String json;
  uint8_t send_counter;
};

enum device_Type
{
  device_Type_Receiver,
  device_Type_Sender,
  device_Type_Sender_Receiver
};

class MessageHandler
{
public:
  // Device info
  ConnectionHandler connectionHandler;

  uint32_t msgCount;      // count of outgoing messages
  uint32_t send_interval; // interval between sends
  uint64_t last_send;

  std::queue<Message> message_Queue;

  MessageHandler();
  void init(ConnectionHandler &cHandle);
  void appendNewMsg(String message);
  void appendReceivedMsg(String message, String id);
  void process_list();
};

#endif