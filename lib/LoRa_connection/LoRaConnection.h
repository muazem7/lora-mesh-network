#ifndef LORA_CONNECTION_H
#define LORA_CONNECTION_H

#include "JsonMessage.h"
#include <Arduino.h>
#include <ArduinoJson.h>

enum msg_typ
{
  msg_typ_empty,
  msg_typ_send,
  msg_typ_ack,
  msg_typ_ping,
  msg_typ_pong
};

class DeviceInfo
{
public:
  DeviceInfo(String dev_address, String receiver_id, int rssi, uint8_t level)
      : dev_address(dev_address), receiver_id(receiver_id), rssi(rssi), level(level) {}

  String dev_address;
  String receiver_id;
  int rssi;
  uint8_t level;
};

class LoraConnection
{
public:
  String lora_dest_addr;     // destination to send to
  String lora_receiver_addr; // the receiver that is the end point of the lora connection
  uint32_t lora_connection_level;
  String localAddress; // address of this device

  std::vector<DeviceInfo> lora_devices_list;

  void init();
  void connectLoRa();
  bool isConnected();
  void onReceive(int packetSize);
  void connectWAN();
  void updateConnection();
  void updateDevlist(String r_sender_addr, String r_receiver_addr, String r_content);
  void send(String json);
  void loraAck(String sender);
  void loraPing();
  void loraPong(String sender);
  void ackDelay(int rssi);
  void detectChannelActivity(void);
};

#endif
