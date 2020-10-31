#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"
#include "framework/Core/Clock.h"

#include "modules/Buzzer.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...
#include <DHT.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "WZSensor"
#define DEVICEVERSION "1.2"

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
  DHT dht;
  Buzzer buzzer;

  int soundLevel = 0;

  long motionCooldown = 10000;
  bool motion = false;
  unsigned long lastMotion = 0;
  bool motionSubmitted = false;

  void inform();

  float measureTemperature();
  float measureHumidity();

  void getConfig();
  void setConfig();

  unsigned long lastPoll= 0;
};
