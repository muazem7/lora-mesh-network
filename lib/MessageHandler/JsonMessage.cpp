#include "JsonMessage.h"
#include <Arduino.h>

JsonMessage::JsonMessage() {}

/**
 * Creates a JSON message from the given parameters (Normal Message)
 * @param typ Message type
 * @param id Message ID
 * @param local_addr Sender ID
 * @param target_addr Target device ID
 * @param contentORreceiver Contains the message if it's a normal message, or receiver ID for Ping/Pong messages
 * @param msgCountORlevel Message number for normal messages or connection level for Ping/Pong messages
 */
String JsonMessage::serialize_json_msg(
    String typ,
    String id,
    String local_addr,
    String target_addr,
    String contentORreceiver,
    String msgCountORlevel)
{
    StaticJsonDocument<1024> doc;
    // Create a JSON object
    JsonObject object = doc.to<JsonObject>();
    object["type"] = typ;
    object["id"] = id;
    object["sender"] = local_addr;
    object["target"] = target_addr;
    object["content"] = contentORreceiver;
    object["num"] = msgCountORlevel; // level or count
    String json;
    // Serialize the object to a string
    serializeJson(doc, json);
    return json;
}

/**
 * Creates a JSON message from the given parameters (Connection Info Message)
 * @param dev_type Device role: 1 - SR, 2 - SRE
 * @param level Connection level
 * @param receiver The receiver to which this device is connected
 */
String JsonMessage::serialize_json_info(
    String dev_type,
    String level,
    String receiver)
{
    StaticJsonDocument<1024> doc;
    // Create a JSON object
    JsonObject object = doc.to<JsonObject>();
    object["type"] = dev_type;
    object["level"] = level;
    object["receiver"] = receiver;
    String json;
    // Serialize the object to a string
    serializeJson(doc, json);
    return json;
}
