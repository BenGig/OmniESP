#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

#include "framework/Utils/SysUtils.h"
#include "framework/Utils/Logger.h"

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
      buzzer("buzzer", logging, topicQueue, BUZZER_PIN) {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start(); // mandatory

  // ... your code here ...
  logging.info("Setup device");
  getConfig();

  dht.setup(DHT22_PIN, dht.DHT22);
  buzzer.setBuzzer(buzz_mode_silent);
  pinMode(PIR_PIN, INPUT);

//  configItem = ffs.deviceCFG.readItem("configItem").toInt();
//  logging.info("configItem is "+String(configItem));
  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

float customDevice::measureTemperature() {
  return dht.getTemperature();
}

float customDevice::measureHumidity() {
  return dht.getHumidity();
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {

  unsigned long now = millis();

  // return motion state change on each poll
  if (digitalRead(PIR_PIN) == HIGH) {
    lastMotion = millis();
    if (! motionSubmitted) {
      topicQueue.put("~/event/device/motion on");
      motionSubmitted = true;
    }
  } else if (motionSubmitted && millis() > lastMotion + motionCooldown) {
    topicQueue.put("~/event/device/motion off");
    motionSubmitted = false;
  }

  buzzer.handle();

  if (now - lastPoll >= 18000) {
    // post climate values every 3 minutes
    lastPoll = now;
    inform();
  }
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─yourItem         (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/yourItem
    return TOPIC_NO;
  if (topic.itemIs(3, "buzzer")) {
    if (topic.arg_asString() == "silent") {
      buzzer.setBuzzer(buzz_mode_silent);
      topicQueue.put("~/event/device/buzzer silent");
    } else if (topic.arg_asString() == "warn") {
      buzzer.setBuzzer(buzz_mode_warn);
      topicQueue.put("~/event/device/buzzer warn");
    } else if (topic.arg_asString() == "alert") {
      buzzer.setBuzzer(buzz_mode_alert);
      topicQueue.put("~/event/device/buzzer alert");
    } else {
      return TOPIC_NO;
    }
    return TOPIC_OK;
  } else if (topic.itemIs(3, "motionCooldown")) {
    motionCooldown = topic.getArgAsLong(0);
    setConfig();
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
   if (topic.itemIs(3, "temperature")) {
    return String(measureTemperature());
  } else if (topic.itemIs(3, "humidity")) {
    return String(measureHumidity());
  } else if (topic.itemIs(3, "motion")) {
    if (motion) {
      return String("on");
    } else {
      return String("off");
    }
  } else {
    return TOPIC_NO;
  }
}

void customDevice::inform() {
  topicQueue.put("~/event/device/temperature", measureTemperature(), "%.1f");
  topicQueue.put("~/event/device/humidity", measureHumidity(), "%g");
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

  // central business logic
}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................
String customDevice::fillDashboard() {
  //topicQueue.put("~/event/device/drawer/index 1");
  //topicQueue.put("~/set/device/drawer/index 1");

  inform();
  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//...............................................................................
//  Persistent configuration read/write
//...............................................................................
void customDevice::getConfig() {
  motionCooldown = ffs.deviceCFG.readItemLong("motionCooldown");
}

void customDevice::setConfig() {
  ffs.deviceCFG.writeItemLong("motionCooldown", motionCooldown);
  ffs.deviceCFG.saveFile();
}
