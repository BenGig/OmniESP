#include "customDevice.h"
#include <Arduino.h>

// Helper function: LED brigthness is not linear, scale from standard interval 85
long scale(long raw) {
  return 0.0035*raw*raw;
}

//===============================================================================
//  Device
//===============================================================================

Adafruit_NeoPixel leds_l(NUM_LEDS_LEFT, LEDOUT_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds_r(NUM_LEDS_RIGHT, LEDOUT_R, NEO_GRB + NEO_KHZ800);

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

  logging.info("Setup device");

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  getConfig();
  initPattern();

  leds_l.begin(); 
  leds_r.begin();
  leds_l.clear();
  leds_r.clear(); 

  setDots();

  logging.info("device running");
}

void customDevice::inform() {
  topicQueue.put("~/event/device/brightness", int(brightness));
  topicQueue.put("~/event/device/red", int(red));
  topicQueue.put("~/event/device/green", int(green));
  topicQueue.put("~/event/device/blue", int(blue));
  topicQueue.put("~/event/device/animspeed", int(animspeed));
}

void customDevice::printValues() {
  logging.debug("red is "+String(red));
  logging.debug("green is "+String(green));
  logging.debug("blue is "+String(blue));
  logging.info("brightness is "+String(brightness));
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  unsigned long now = millis();
  if (now - lastPoll >= 59000 - animspeed*1000) {
    patternPointer++;
    if (patternPointer >= (NUM_LEDS_LEFT+NUM_LEDS_RIGHT)/2) {
      patternPointer = 0;
    }
    setDots();
    lastPoll = now;
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
    brightness = topic.getArgAsLong(0);
    setBrightness(brightness);
    setConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "red")) {
    red = topic.getArgAsLong(0);
    setRGB(red,green,blue);
    setConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "green")) {
    green = topic.getArgAsLong(0);
    printValues();
    setRGB(red,green,blue);
    setConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "blue")) {
    blue = topic.getArgAsLong(0);
    setRGB(red,green,blue);
    setConfig();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "animspeed")) {
    animspeed = topic.getArgAsLong(0);
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
  if (topic.itemIs(3, "brightness")) {
    return String(brightness);
  } else if (topic.itemIs(3, "red")) {
    return String(red);
  } else if (topic.itemIs(3, "green")) {
    return String(green);
  } else if (topic.itemIs(3, "blue")) {
    return String(blue);
  } else if (topic.itemIs(3, "animspeed")) {
    return String(animspeed);
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
  topicQueue.put("~/event/device/brightness", int(brightness));
  topicQueue.put("~/event/device/red", int(red));
  topicQueue.put("~/event/device/green", int(green));
  topicQueue.put("~/event/device/blue", int(blue));
  topicQueue.put("~/event/device/animspeed", int(animspeed));

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

// Loop over all LEDs and set RGB values
void customDevice::setDots() {
  // for (int dot = 0; dot < NUM_LEDS_LEFT; dot++) {
  //   leds_l.setPixelColor(dot, red*brightness/255,green*brightness/255,blue*brightness/255);
  // }
  // for (int dot = 0; dot < NUM_LEDS_RIGHT; dot++) {
  //   leds_r.setPixelColor(dot, red*brightness/255,green*brightness/255,blue*brightness/255);
  // }
  
  for (int i=0; i<NUM_LEDS_LEFT; i++) {
    leds_l.setPixelColor(i, 
        (red+sin((i+NUM_LEDS_LEFT-patternPointer-1)/2)*WAVE_AMPLITUDE)*brightness/255, 
        (green+sin((i+NUM_LEDS_LEFT-patternPointer-1)/2)*WAVE_AMPLITUDE)*brightness/255, 
        (blue+sin((i+NUM_LEDS_LEFT-patternPointer-1)/2)*WAVE_AMPLITUDE)*brightness/255);
  }
  leds_l.show();
  for (int i=0; i<NUM_LEDS_RIGHT; i++) {
    leds_r.setPixelColor(i, 
        (red+sin((i+patternPointer)/2)*WAVE_AMPLITUDE)*brightness/255, 
        (green+sin((i+patternPointer)/2)*WAVE_AMPLITUDE)*brightness/255, 
        (blue+sin((i+patternPointer)/2)*WAVE_AMPLITUDE)*brightness/255);
  }
  leds_r.show();
}

void customDevice::setRGB(long r, long g, long b) {
  red = r;
  green = g;
  blue = b;
  setDots();
}

void customDevice::setBrightness(long b) {
  brightness = b;
  setRGB(red, green, blue);
}


//...............................................................................
//  Persistent configuration read/write
//...............................................................................
void customDevice::getConfig() {
  brightness = ffs.deviceCFG.readItemLong("brightness");
  red = ffs.deviceCFG.readItemLong("red");
  green = ffs.deviceCFG.readItemLong("green");
  blue = ffs.deviceCFG.readItemLong("blue");
  animspeed = ffs.deviceCFG.readItemLong("animspeed");
  logging.debug("Values read from FS");
  printValues();
}

void customDevice::setConfig() {
  ffs.deviceCFG.writeItemLong("brightness", brightness);
  ffs.deviceCFG.writeItemLong("red", red);
  ffs.deviceCFG.writeItemLong("green", green);
  ffs.deviceCFG.writeItemLong("blue", blue);
  ffs.deviceCFG.writeItemLong("animspeed", animspeed);
  ffs.deviceCFG.saveFile();
  inform();
}

void customDevice::initPattern() {
  for (int i=0; i<((NUM_LEDS_LEFT+NUM_LEDS_RIGHT)/2); i++) {
    wave_pattern[i] = sin(i/2)*WAVE_AMPLITUDE;
  }
}
