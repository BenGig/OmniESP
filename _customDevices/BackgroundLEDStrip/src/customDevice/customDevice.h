#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"
//#include <ESP8266mDNS.h>

// modules required by device
// ...your includes here...
#include <FastLED.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "BackgroundLEDStrip"
#define DEVICEVERSION "v1.0.0"

// maintainer: Bengt Giger

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  String fillDashboard();
  void on_events(Topic &topic);

private:
  // ...your sensor objects, modules, functions here...
  int brightness=0;
  int red=255;
  int green=184;
  int blue=114;
  int lastPoll=0;
  CRGB leds_l[NUM_LEDS_LEFT];
  CRGB leds_r[NUM_LEDS_RIGHT];
  void getConfig();
  void saveConfig();
  void inform();
  void setRGB(int r, int g, int b);
  void setBrightness(int brightness);
  void setDots();
};
