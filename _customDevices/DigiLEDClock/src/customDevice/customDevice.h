#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"
#include "framework/Core/Clock.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "DigiLEDClock"
#define DEVICEVERSION "1.0"

#define CLOCKTICK_MINUTE 1

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
  MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
  Clock clock = Clock(topicQueue);

  String poolServerName = NTP_POOL_SERVER;

  int ledBrightness = INITIAL_BRIGHTNESS;
  bool brightnessOverride = false;
  bool powerOverride = false;
  bool powerState = true;

  void getConfig();
  void saveConfig();
  void inform();

  void displayTime();
  void displayText();

  int animDelay;
  int messageTime;
  int msgTimer = 0;
  char msgbuf[100];
  float roomBrightness();

  unsigned long lastPoll= 0;
  unsigned long TEST_lastPoll = 0;
  int informDelay = 0;

  String UTF8toISO8859_1(const char * in);
};
