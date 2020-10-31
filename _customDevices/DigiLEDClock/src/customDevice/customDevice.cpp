#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

#include "framework/Utils/SysUtils.h"
#include "sys_fixed_single.h"
#include "framework/Utils/Logger.h"

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs) {

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

  clock.start(poolServerName.c_str(), 0, 60000);

  getConfig();

  P.begin();
  P.setIntensity(ledBrightness);
  P.setTextAlignment(PA_CENTER);
  P.setSpeed(animDelay);

//  configItem = ffs.deviceCFG.readItem("configItem").toInt();
//  logging.info("configItem is "+String(configItem));
  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

float customDevice::roomBrightness() {
  return analogRead(LIGHT_PIN);
}

void customDevice::displayTime() {
  // Message still on display? Show time only if no active msg
  if (msgTimer == 0) {
    String t = SysUtils::strTimeHHMM(clock.now());
    P.setFont(_sys_fixed_single);
    P.setTextAlignment(PA_CENTER);
    P.print(t.c_str());
  }
}

void customDevice::displayText() {
  P.setFont(nullptr);
  P.displayText(msgbuf, PA_CENTER, animDelay, animDelay, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  msgTimer = messageTime;
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {

  clock.handle();
  if (msgTimer > 0 && P.displayAnimate()) {
    P.displayReset();
  }
  unsigned long now = millis();
  // update display every second (clock only; animation done by display)
  if (now - lastPoll >= 1000) {
    lastPoll = now;
    if (msgTimer > 0) msgTimer--;
    displayTime();

    // update MQTT every 3 minutes
    informDelay++;
    if (informDelay >= 179) {
      inform();
      informDelay = 0;
    }
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
  // LED device settings
  if (topic.itemIs(3, "power")) {
    if (powerOverride) {
      if (topic.arg_asString() == "off" || topic.arg_asString() == "false" || topic.arg_asString() == "0") {
        P.displayShutdown(true);
        powerState = false;
        inform();
      }
      if (topic.arg_asString() == "on" || topic.arg_asString() == "true" || topic.arg_asString() == "1") {
        P.displayShutdown(false);
        powerState = true;
        inform();
      }
    }
    return TOPIC_OK;
  } else if (topic.itemIs(3, "brightness")) {
      if (brightnessOverride) {
        ledBrightness = max(0, min(15, atoi(topic.arg_asCStr())));
        P.setIntensity(ledBrightness);
        saveConfig();
      }
      return TOPIC_OK;

  // Message display
  } else if (topic.itemIs(3, "message")) {
    String buf = UTF8toISO8859_1((String("  ") + topic.arg_asString()).c_str());
    strcpy(msgbuf, buf.c_str());
    displayText();
    return TOPIC_OK;

  // Set override modes
  } else if (topic.itemIs(3, "brightnessOverride")) {
    if (topic.arg_asString() == "on" || topic.arg_asString() == "true"  || topic.arg_asString() == "1") {
      brightnessOverride = true;
    }
    if (topic.arg_asString() == "off" || topic.arg_asString() == "false"  || topic.arg_asString() == "0") {
      brightnessOverride = false;
    }
    saveConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "powerOverride")) {
    if (topic.arg_asString() == "on" || topic.arg_asString() == "true"  || topic.arg_asString() == "1") {
      powerOverride = true;
    }
    if (topic.arg_asString() == "off" || topic.arg_asString() == "false" || topic.arg_asString() == "0") {
      powerOverride = false;
    }
    saveConfig();
    return TOPIC_OK;

  // Timing config
  } else if (topic.itemIs(3, "animDelay")) {
    animDelay = topic.getArgAsLong(0);
    saveConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "messageTime")) {
    messageTime = topic.getArgAsLong(0);
    saveConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "mqttInterval")) {
    mqttInterval = topic.getArgAsLong(0);
    saveConfig();
    return TOPIC_OK;


  } else if (topic.itemIs(3, "saveconfig")) {
    saveConfig();
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
   if (topic.itemIs(3, "brightness")) {
    return String(roomBrightness());
  } else {
    return TOPIC_NO;
  }
}

void customDevice::inform() {
  topicQueue.put("~/event/device/power", powerState);
  topicQueue.put("~/event/device/brightness", ledBrightness, "%g");
  topicQueue.put("~/event/device/animDelay", animDelay, "%g");
  topicQueue.put("~/event/device/messageTime" , messageTime, "%g");
  topicQueue.put("~/event/device/powerOverride", powerOverride, "%g");
  topicQueue.put("~/event/device/brightnessOverride", brightnessOverride, "%g");
  topicQueue.put("~/event/device/mqttInterval", mqttInterval, "%g");
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
  int initialBrightness = ffs.deviceCFG.readItemLong("initialBrightness");
  if (initialBrightness == 0) {
    ledBrightness = INITIAL_BRIGHTNESS;
  } else {
    ledBrightness = initialBrightness;
  }
  if (ffs.deviceCFG.readItemLong("powerOverride") == 0)
    powerOverride = false;
  else
    powerOverride = true;
  if (ffs.deviceCFG.readItemLong("brightnessOverride") == 0)
    brightnessOverride = false;
  else
    brightnessOverride = true;

    animDelay = ffs.deviceCFG.readItemLong("animDelay");
    if (animDelay == 0) {
      animDelay = ANIM_DELAY;
    }
    mqttInterval = ffs.deviceCFG.readItemLong("mqttInterval");
    if (mqttInterval == 0) {
      mqttInterval = MQTT_INTERVAL;
    }
  messageTime = ffs.deviceCFG.readItemLong("messageTime");
  if (messageTime == 0) {
    messageTime = MESSAGE_TIME;
  }
  logging.debug("================================");
  logging.debug(String("animDelay is " + String(animDelay)));
  logging.debug(String("brightness is " + String(ledBrightness)));
  logging.debug(String("brightnessOverride is " + String(brightnessOverride)));
  logging.debug(String("messageTime is " + String(messageTime)));
  logging.debug(String("powerOverride is " + String(powerOverride)));
  logging.debug("================================");
  saveConfig();
}

void customDevice::saveConfig() {
  logging.debug(String("Saving config"));
  ffs.deviceCFG.writeItemLong("initialBrightness", ledBrightness);
  ffs.deviceCFG.writeItemLong("animDelay", animDelay);
  ffs.deviceCFG.writeItemLong("messageTime", messageTime);
  ffs.deviceCFG.writeItemLong("powerOverride", powerOverride);
  ffs.deviceCFG.writeItemLong("brightnessOverride", brightnessOverride);
  ffs.deviceCFG.writeItemLong("mqttIn", mqttInterval);
  ffs.deviceCFG.saveFile();
  inform();
}

//...............................................................................
//  UTF conversion for text display
//...............................................................................
String customDevice::UTF8toISO8859_1(const char * in)
{
    String out;
    if (in == NULL)
        return out;

    unsigned int codepoint = 0;
    while (*in != 0)
    {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            codepoint = ch;
        else if (ch <= 0xbf)
            codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            codepoint = ch & 0x0f;
        else
            codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
        {
            if (codepoint <= 255)
            {
                out.concat(static_cast<char>(codepoint));
            }
            else
            {
                // do whatever you want for out-of-bounds characters
            }
        }
    }
    return out;
}
