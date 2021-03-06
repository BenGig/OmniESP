//###############################################################################
// ESP8266
//###############################################################################
/*
GPIO WeMos ESPpin Function  Connection
 2   D4    17               PullUp
 0   D3    18               PullUp
 4   D2    19     SDA
 5   D1    20     SCL
 3   RX    21
 1   TX    22
15   D8    16     SS        PullDown
13   D7     7     MOSI
12   D6     6     MISO
14   D5     5     SCK
16   D0     4     SLEEP!
ADC  A0     2     Analog
*/


//###############################################################################
// device specifis settings
//###############################################################################

// ... your defines here...
// #define ...
// Number of LEDs for left and right branch
#define NUM_LEDS_LEFT 75
#define NUM_LEDS_RIGHT 75
// Control pins
#define LEDOUT_L D4
#define LEDOUT_R D3

#define LED_TYPE WS2812B

#define WAVE_AMPLITUDE 25