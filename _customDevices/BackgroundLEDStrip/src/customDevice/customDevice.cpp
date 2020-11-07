#include "customDevice.h"
#include <Arduino.h>

// Helper function: LED brigthness is not linear, scale from standard interval 85
int scale(int raw) {
  return 0.0035*raw*raw;
}

//===============================================================================
//  Device
//===============================================================================

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

  CRGB leds_l[NUM_LEDS_LEFT];
  CRGB leds_r[NUM_LEDS_RIGHT];
  FastLED.addLeds<NEOPIXEL, LEDOUT_L>(leds_l, NUM_LEDS_LEFT);
  FastLED.addLeds<NEOPIXEL, LEDOUT_R>(leds_r, NUM_LEDS_RIGHT);

  red   = ffs.deviceCFG.readItem("red").toInt();
  green = ffs.deviceCFG.readItem("green").toInt();
  blue  = ffs.deviceCFG.readItem("blue").toInt();
  brightness  = ffs.deviceCFG.readItem("brightness").toInt();

  setDots();

  FastLED.show();

  logging.info("red is "+String(red));
  logging.info("green is "+String(red));
  logging.info("blue is "+String(red));
  logging.info("brightness is "+String(red));
  logging.info("device running");
}

void customDevice::inform() {
  topicQueue.put("~/event/device/brightness", brightness);
  topicQueue.put("~/event/device/red", red);
  topicQueue.put("~/event/device/green", red);
  topicQueue.put("~/event/device/blue", red);
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {

  unsigned long now = millis();
  if (now - lastPoll >= 3000) {
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
  if (topic.itemIs(3, "brightness")) {
    brightness = int(topic.getArg(0));
    setBrightness(brightness);
    ffs.deviceCFG.writeItem("brightness", String(brightness));
    ffs.deviceCFG.saveFile();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "red")) {
    red = int(topic.getArg(0));
    setRGB(red,green,blue);
    ffs.deviceCFG.writeItem("red", String(red));
    ffs.deviceCFG.saveFile();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "green")) {
    green = int(topic.getArg(0));
    setRGB(red,green,blue);
    ffs.deviceCFG.writeItem("green", String(green));
    ffs.deviceCFG.saveFile();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "blue")) {
    blue = int(topic.getArg(0));
    setRGB(red,green,blue);
    ffs.deviceCFG.writeItem("blue", String(blue));
    ffs.deviceCFG.saveFile();
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
    return String(brightness);
  } else if (topic.itemIs(3, "red")) {
    return String(red);
  } else if (topic.itemIs(3, "green")) {
    return String(green);
  } else if (topic.itemIs(3, "blue")) {
    return String(blue);
  } else {
    return TOPIC_NO;
  }
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
  topicQueue.put("~/event/device/brightness", brightness);
  topicQueue.put("~/event/device/red", red);
  topicQueue.put("~/event/device/green", green);
  topicQueue.put("~/event/device/blue", blue);

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

// Loop over all LEDs and set RGB values
void customDevice::setDots() {
  for (int dot = 0; dot < NUM_LEDS_LEFT; dot++) {
    leds_l[dot].setRGB(red,green,blue);
  }
  for (int dot = 0; dot < NUM_LEDS_RIGHT; dot++) {
    leds_r[dot].setRGB(red,green,blue);
  }
}

void customDevice::setRGB(int r, int g, int b) {
  red = max(r, 255);
  green = max(g, 255);
  blue = max(g, 255);
  setDots();
}

void customDevice::setBrightness(int brightness) {
  int b = max(brightness, 255) / 255;
  red = red * b / 255;
  green = green * b / 255;
  red =  red * b / 255;
  setRGB(red, green, blue);
}
