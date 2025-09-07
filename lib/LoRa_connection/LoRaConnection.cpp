#include "LoRaConnection.h"
#include "JsonMessage.h"
#include "board_pins.h"
#include <Arduino.h>
#include <LoRa.h>

SPIClass SPI1(HSPI);

const uint32_t minTransmittTime = 250;
const uint32_t delayConstant = 100;
const uint32_t pingWaitTime = 30000;

bool comparator(DeviceInfo lhs, DeviceInfo rhs);

void LoraConnection::init()
{
    // Set address
    char tmp[20];
    sprintf(tmp, "0x%04X%08X", (uint32_t)(ESP.getEfuseMac() >> 32), (uint32_t)ESP.getEfuseMac());
    localAddress = String(tmp);
    Serial.println(localAddress);

    // Specify pin to initialize SPI1
    SPI1.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setSPI(SPI1);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
    LoRa.setTxPower(12); // 25 mW ?
    LoRa.setSpreadingFactor(7);
    Serial.printf("Setup LoRa freq : %.0f\n", BAND);
    if (!LoRa.begin(BAND))
    {
        Serial.println("LORA Begin FAIL");
        while (1)
            ;
    }
    Serial.println("LORA Begin PASS");
}

/**
 * Establishes LoRa-Mesh connection, sends a ping message every 'pingWaitTime' period
 */
void LoraConnection::connectLoRa()
{
    // Reset connection
    lora_connection_level = 0;
    lora_dest_addr = "";
    lora_receiver_addr = "";

    lora_devices_list.clear();
    LoRa.receive();
    delay(5);
    Serial.println("connecting to custom LoRaWAN...");
    while (!isConnected())
    {
        long lastPingTime = millis();
        Serial.println("ping...");
        // Send information and wait for a response (pong)
        loraPing();
        while ((millis() - lastPingTime) < pingWaitTime)
        {
            onReceive(LoRa.parsePacket());
        }
    }
    // Send updated information again
    loraPing();
    Serial.println("connection established " + lora_receiver_addr + " " + lora_connection_level);
}

/**
 * Send a message
 * @param packet The payload
 */
void LoraConnection::send(String packet)
{
    String msg = String(packet);
    detectChannelActivity();
    Serial.println("sending: " + msg);
    LoRa.beginPacket();
    LoRa.print(msg);  // add payload
    LoRa.endPacket(); // finish packet and send it
    LoRa.receive();
    delay(2);
}

/**
 * Sends an acknowledgment for normal messages
 * @param sender Target device ID
 */
void LoraConnection::loraAck(String sender)
{
    JsonMessage jMsg;
    String outgoing = jMsg.serialize_json_msg(String(msg_typ_ack), "id", localAddress, sender, "ack", "0");
    send(outgoing);
}

/**
 * Sends a broadcast containing connection information
 */
void LoraConnection::loraPing()
{
    JsonMessage jMsg;
    String outgoing = jMsg.serialize_json_msg(String(msg_typ_ping), "id", localAddress, "all", lora_receiver_addr, String(lora_connection_level));
    send(outgoing);
}

/**
 * Sends an acknowledgment for ping messages
 * @param sender Target device ID
 */
void LoraConnection::loraPong(String sender)
{
    JsonMessage jMsg;
    String outgoing = jMsg.serialize_json_msg(String(msg_typ_pong), "id", localAddress, sender, lora_receiver_addr, String(lora_connection_level));
    send(outgoing);
}

/**
 * Waits for a period based on connection level and RSSI
 * @param rssi Received signal strength
 */
void LoraConnection::ackDelay(int rssi)
{
    int rssi_ms = delayConstant * (lora_connection_level + abs(rssi));
    if (rssi_ms < minTransmittTime)
        rssi_ms = minTransmittTime;
    Serial.println("delay:" + String(rssi_ms));
    delay(rssi_ms);
}

/**
 * Waits until the channel is free (not reliable)
 */
void LoraConnection::detectChannelActivity(void)
{
    LoRa.receive();
    delay(2);
    long t = millis();
    while (abs(LoRa.rssi()) < 80)
        delay(minTransmittTime);
    Serial.println("Activity detection, total wait time: " + String(millis() - t));
}

/**
 * This method sets the connection information.
 * The information comes from the first entry in the sorted device list.
 * If the list is empty, there is no connection.
 */
void LoraConnection::updateConnection()
{
    if (!lora_devices_list.empty())
    {
        DeviceInfo first = lora_devices_list.front();
        lora_receiver_addr = first.receiver_id;
        lora_dest_addr = first.dev_address;
        lora_connection_level = first.level + 1;
    }
    else
    {
        lora_receiver_addr = "";
        lora_dest_addr = "";
        lora_connection_level = 0;
    }
}

/*!
 * connection_level 0 = no connection
 * connection_level 1 = receiver node
 * connection_level > 1 = normal node
 */
bool LoraConnection::isConnected()
{
    return lora_connection_level > 0;
}

/**
 * Updates the device list and then the connection information
 */
void LoraConnection::updateDevlist(String r_sender_addr, String r_receiver_addr, String r_content)
{
    // If this device is a receiver, skip
    if (lora_connection_level == 1)
        return;
    bool isIgnored = false;

    int receivedLvl = atoi(r_content.c_str());
    // If the received connection level is 0 or this device has the same receiver but a lower connection level,
    // the new device is not added to the list.
    if (receivedLvl == 0 || (lora_receiver_addr.equals(r_receiver_addr) && receivedLvl >= lora_connection_level))
    {
        isIgnored = true;
    }

    bool isInlist = false;
    for (std::vector<DeviceInfo>::iterator it = lora_devices_list.begin(); it != lora_devices_list.end(); ++it)
    {
        if ((*it).dev_address.equals(r_sender_addr.c_str()))
        {
            if (isIgnored)
                lora_devices_list.erase(it);
            else
            {
                (*it).level = receivedLvl;
                (*it).receiver_id = r_receiver_addr;
                (*it).rssi = LoRa.packetRssi();
            }
            isInlist = true;
            break;
        }
    }

    if (isIgnored)
        return;
    if (!isInlist)
        lora_devices_list.push_back(DeviceInfo(r_sender_addr, r_receiver_addr, LoRa.packetRssi(), atoi(r_content.c_str())));

    // Sort the list
    std::sort(lora_devices_list.begin(), lora_devices_list.end(), comparator);
    for (std::vector<DeviceInfo>::iterator it = lora_devices_list.begin(); it != lora_devices_list.end(); ++it)
    {
        Serial.println(String((*it).level) + " " + (*it).receiver_id + " " + String((*it).rssi));
    }
    // Update receiver, status & level
    updateConnection();
}

/**
 * Processes received pong messages.
 * This method is only called when the connection is being established.
 * @param packetSize Received packet size
 */
void LoraConnection::onReceive(int packetSize)
{
    if (packetSize == 0)
        return; // If there is no packet, return

    String incoming = ""; // payload of packet

    while (LoRa.available())
    {
        incoming += (char)LoRa.read(); // Read bytes one by one
    }

    int rssi = LoRa.packetRssi();
    Serial.println("lora: " + incoming);
    Serial.print("SnR:" + String(rssi));
    Serial.println(" Length: " + String(packetSize));

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, incoming);
    JsonObject object = doc.as<JsonObject>();

    String r_type_str = object["type"];
    uint32_t r_type = r_type_str.toInt();
    String r_msg_id = object["id"];
    String r_sender_addr = object["sender"];
    String r_receiver_addr = object["target"];
    String r_contentORreceiver = object["content"];
    String r_levelORcount = object["num"];

    // If the receiver is not the target device,
    if (!r_receiver_addr.equals(localAddress.c_str()))
    {
        Serial.println("This message is not for me.");
        return; // Skip the rest of the function
    }

    Serial.println("Received: " + incoming);
    Serial.println();

    // Handle message
    switch (r_type)
    {
    case msg_typ_send:
    case msg_typ_ack:
    case msg_typ_ping:
        break;
    case msg_typ_pong:
        // Update device list
        updateDevlist(r_sender_addr, r_contentORreceiver, r_levelORcount);
        break;
    default:
        Serial.println("unknown message type");
        break;
    }
}

bool comparator(DeviceInfo lhs, DeviceInfo rhs)
{
    if (lhs.level < rhs.level)
        return false;
    else if (lhs.level > rhs.level)
        return true;
    if (lhs.rssi < rhs.rssi)
        return false;
    else if (lhs.rssi > rhs.rssi)
        return true;

    return false;
}
