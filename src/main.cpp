#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h> // include libraries

#include "ConnectionHandler.h"
#include "FileHandler.h"
#include "JsonMessage.h"
#include "MessageHandler.h"
#include "board_pins.h"

void onReceive(int packetSize);

ConnectionHandler connectionHandler;
MessageHandler messageHandler;
HttpHandler httphandler;
JsonMessage jMsg;
FileHandler fHandler;

String serverName = "https://<url>.firebasedatabase.app/device/";

long lastSendTime = 0; // last send time

void setup() {
    // Initialize serial
    Serial.begin(SERIAL_DEBUG_BAUD);
    while (!Serial)
        ;

    fHandler.init();
    httphandler.init(serverName);
    connectionHandler.init(httphandler, fHandler);
    connectionHandler.connect();
    messageHandler.init(connectionHandler);
}

void loop() {
    // Send a message if the sender has an internet connection and is of type SR.
    if ((millis() - lastSendTime) > (2 * messageHandler.send_interval) &&
        connectionHandler.isConnected() &&
        connectionHandler.dev_type.equals("1")) {
        String message = "HeLoRa World!"; // The message
        messageHandler.appendNewMsg(message);
        lastSendTime = millis(); // Save send time
    }

    messageHandler.process_list();
    long time = millis();
    while ((millis() - time) < 500)
        onReceive(LoRa.parsePacket());
}

/**
 * Processes received messages.
 * This method is only called when the connection is already established.
 * @param packetSize Received packet size
 */
void onReceive(int packetSize) {
    if (packetSize == 0)
        return; // If there is no packet, return

    String incoming = ""; // payload of packet

    while (LoRa.available()) {
        incoming += (char)LoRa.read(); // Add bytes one by one
    }

    Serial.println("Main: " + incoming);

    int rssi = LoRa.packetRssi();
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

    // If the recipient isn't this device or broadcast,
    if (!r_receiver_addr.equals(connectionHandler.lora.localAddress.c_str()) && (r_type != msg_typ_ping)) {
        Serial.println("This message is not for me.");
        return; // Skip rest of function
    }

    Serial.println("Received: " + incoming);

    // Handle message
    switch (r_type) {
    case msg_typ_send:
        // Send ACK
        connectionHandler.lora.loraAck(r_sender_addr);
        messageHandler.appendReceivedMsg(r_contentORreceiver, r_msg_id);
        // Check free heap space
        Serial.println(ESP.getFreeHeap());
        break;
    case msg_typ_ack:
        // Find on Queue and delete
        if (connectionHandler.lora.isConnected()) {
            messageHandler.message_Queue.pop();
            Serial.println("ACK received - msg deleted");
        }
        break;
    case msg_typ_ping:
        // Update device list and send a pong message
        connectionHandler.lora.updateDevlist(r_sender_addr, r_contentORreceiver, r_levelORcount);
        // Send pong
        if (connectionHandler.lora.isConnected()) {
            connectionHandler.lora.ackDelay(rssi);
            connectionHandler.lora.loraPong(r_sender_addr);
        }
        break;
    case msg_typ_pong:
        // Update device list (msg_id is the d_receiver_id)
        connectionHandler.lora.updateDevlist(r_sender_addr, r_contentORreceiver, r_levelORcount);
        break;
    default:
        Serial.println("unknown message type");
        break;
    }
}
