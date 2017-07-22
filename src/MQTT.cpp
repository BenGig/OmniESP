#include "MQTT.h"
#include "SysUtils.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

//###############################################################################
//  MQTT client
//###############################################################################
MQTT::MQTT(API& api):
    api(api),
    espClient(),
    client(espClient)
{

//callbacks
    client.setCallback(std::bind(&MQTT::on_incommingSubcribe, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

//-------------------------------------------------------------------------------
//  MQTT public
//-------------------------------------------------------------------------------
//...............................................................................
//  WiFi start connection
//...............................................................................
bool MQTT::start(){
  bool MQTTOK = false;

  String strIP = api.call("~/get/ffs/cfg/item/mqttServer");
  IPAddress IP = SysUtils::strToIP(strIP);
  int port = api.call("~/get/ffs/cfg/item/mqttPort").toInt();
  String deviceName = api.call("~/get/ffs/cfg/item/mqttDeviceName");
  String lastWillTopic = "Devices/" + deviceName;

  api.info("MQTT client connecting to " + strIP + ":" + String(port));
  /*
  i2c.lcd.clear();
  i2c.lcd.println("MQTTBroker:", ArialMT_Plain_10, 0);
  i2c.lcd.println(strIP + ":" + String(port), ArialMT_Plain_16,  10);
  */

  client.disconnect();
  client.setServer(IP, port);
  if (client.connect(deviceName.c_str(), lastWillTopic.c_str() , 0, false, "Dead")) {
    MQTTOK = true;
    api.info("MQTT client connected to MQTT broker");
    /*
    i2c.lcd.println("...connected", ArialMT_Plain_10, 31); */
    client.publish(lastWillTopic.c_str(), "Alive");

    String root;

    //global subscribe
    DynamicJsonBuffer global_JsonBuffer;
    root= api.call("~/get/ffs/subGlobal/root");
    JsonObject& global_rootObject = global_JsonBuffer.parseObject(root);
    if (global_rootObject.success()) {
      for (auto &element : global_rootObject){
        //String strKey = element.key;
        String strValue = element.value; strValue += "/#";
        //Serial.println(strValue);
        client.subscribe(strValue.c_str());
        client.loop();
      }
    }else{
      api.error("reading ffs.subGlobal.root failed");
    }
    //device subscribe
    DynamicJsonBuffer device_JsonBuffer;
    root= api.call("~/get/ffs/sub/root");
    JsonObject& device_rootObject = device_JsonBuffer.parseObject(root);
    if (device_rootObject.success()) {
      for (auto &element : device_rootObject){
        //String strKey = element.key;
        String value = element.value;
        String strValue = deviceName; strValue += "/"; strValue += value;
        //Serial.println(strValue);
        client.subscribe(strValue.c_str());
        client.loop();
      }
    }else{
      api.error("reading ffs.sub.root failed");
    }
  }
  return MQTTOK;
}

//...............................................................................
//  WiFi start connection
//...............................................................................
bool MQTT::handle(){
  client.loop();
  if (client.connected())
    return true;
  else
    return false;
}

//...............................................................................
//  EVENT incomming MQTT subcribe
//...............................................................................
void MQTT::on_incommingSubcribe(char* topics, byte* payload, unsigned int length){
  //api.debugMem_start();

  char* args = new char[length+1];
  strncpy(args, (char*)payload, length+1);
  args[length] = '\0';

  api.debug("MQTT incoming subscribe: "+ String(topics) + " " + String(args));

  //String str = String(topics) + " " + String(args);
  //Topic tmpTopic(str);
  Topic tmpTopic(topics, args);
  String tmp = api.call(tmpTopic);    //API verändert tmpTopic!!
  pub(tmpTopic.modifyTopic(1), tmp);

  if (args != NULL) delete[] args;
}

//...............................................................................
//  MQTT publish
//...............................................................................
void MQTT::pub(String topic, String value){
  client.publish(topic.c_str(), value.c_str());
  client.loop();
}

//-------------------------------------------------------------------------------
//  MQTT API
//-------------------------------------------------------------------------------
//...............................................................................
//  MQTT SET
//...............................................................................
/*
mqtt
  └─connect
*/
/*
String MQTT::set(Topic& topic){
  if (topic.itemIs(3, "connect")){
    start();
    if (handle()){
      return "connected";
    }else{
      return "disconnected";
    }

  }
}
*/

//...............................................................................
//  MQTT GET
//...............................................................................
/*
mqtt
  └─status
*/
/*
String MQTT::get(Topic& topic){
  String str = "NIL";
//status
  if (topic.itemIs(3, "status")){
    if (handle()){
      return "connected";
    }else{
      return "disconnected";
    }
  }
}
*/
//-------------------------------------------------------------------------------
//  MQTT private
//-------------------------------------------------------------------------------
