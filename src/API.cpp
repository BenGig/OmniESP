#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(Controller &controller) : controller(controller) {}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------

void API::start() {
  info("API started for device with chip ID "+call("~/get/esp/chipId"));
}

//...............................................................................
//  API call distributing
//...............................................................................

String API::call(Topic &topic) {

  // just a pass through
  return controller.call(topic);
}

// convenience function
String API::call(String topicsArgs) {
  Topic tmpTopic(topicsArgs);
  debug("API call "+tmpTopic.asString());
  String result= call(tmpTopic);
  if(result == nullptr) {
      result= String("no result");
  }
  debug("result: "+result);
  return result;
}

// convenience function
String API::call(string topicsArgs) {
  return call(String(topicsArgs));
}


// these are convenience functions
// a more convoluted way would be via a topic ~/log/info

void API::info(const String &msg) {
  controller.logging.info(msg);
}

void API::error(const String &msg) {
  controller.logging.error(msg);
}

void API::debug(const String &msg) {
  controller.logging.debug(msg);
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
