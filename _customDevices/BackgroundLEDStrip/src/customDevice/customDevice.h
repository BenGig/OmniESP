#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

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
  long brightness=0;
  long red=255;
  long green=184;
  long blue=114;
  long animspeed=20;
  int lastPoll=0;
  void getConfig();
  void setConfig();
  void inform();
  void setRGB(long r, long g, long b);
  void setBrightness(long b);
  void setDots();
  void printValues();
  void initPattern();
  
  int patternPointer=0;
  int wave_pattern[NUM_LEDS_LEFT+NUM_LEDS_RIGHT];
};
