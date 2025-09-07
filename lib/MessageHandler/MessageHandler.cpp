#include <Arduino.h>
#include <WiFi.h>
#include <algorithm>
#include <vector>

#include "ConnectionHandler.h"
#include "HttpHandler.h"
#include "LoRaConnection.h"
#include "MessageHandler.h"
#include "WiFiConnection.h"
#include "board_pins.h"

MessageHandler::MessageHandler() {}

void MessageHandler::init(ConnectionHandler &cHandle)
{
    connectionHandler = cHandle;
    msgCount = 0;
    send_interval = 30000; // must be 1% duty cycle
    last_send = 0;
}

/**
 * Sends the first message in the queue. The message is removed when an acknowledgment is received,
 * or after six attempts. On the fifth attempt, the connection is restored.
 */
void MessageHandler::process_list()
{
    if (message_Queue.empty())
    {
        return;
    }
    if (!connectionHandler.isConnected())
    {
        connectionHandler.connect();
    }
    Message *it = &message_Queue.front();

    switch (it->send_counter)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
        if ((millis() - last_send) > send_interval)
        {
            last_send = millis();
            ++it->send_counter;

            // if receiver
            if (connectionHandler.wifi.isConnected() && (connectionHandler.lora.lora_connection_level == 1))
            {
                if (connectionHandler.httpHandler.post(connectionHandler.lora.localAddress, "messages", it->json))
                {
                    message_Queue.pop();
                    return;
                }
            }
            else if (connectionHandler.lora.isConnected())
            {
                connectionHandler.lora.send(it->json);
                return;
            }
        }
        break;
    case 5:
        if ((millis() - last_send) > send_interval)
        {
            connectionHandler.connect();
            ++it->send_counter;
        }
        break;
    default:
        // delete from list
        message_Queue.pop();
        break;
    }
}

/**
 * Appends a new message to the queue
 * @param message Message in String format
 */
void MessageHandler::appendNewMsg(String message)
{
    if (!connectionHandler.isConnected())
    {
        Serial.println("This device is not connected - cannot take new messages");
        return;
    }
    JsonMessage jMsg;
    String json = jMsg.serialize_json_msg(
        String(msg_typ_send),
        connectionHandler.lora.localAddress + "-" + String(msgCount),
        connectionHandler.lora.localAddress,
        connectionHandler.lora.lora_dest_addr,
        message,
        String(msgCount));
    ++msgCount;
    message_Queue.push(Message(json));
}

/**
 * Appends a received message to the queue
 * @param message Message in String format
 */
void MessageHandler::appendReceivedMsg(String message, String id)
{
    if (!connectionHandler.isConnected())
    {
        Serial.println("This device is not connected - cannot take new messages");
        return;
    }
    JsonMessage jMsg;
    String json = jMsg.serialize_json_msg(
        String(msg_typ_send),
        id,
        connectionHandler.lora.localAddress,
        connectionHandler.lora.lora_dest_addr,
        message,
        String(msgCount));
    ++msgCount;
    message_Queue.push(Message(json));
}