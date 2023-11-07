
#ifndef __LED_H__
#define __LED_H__ 1

#include "config.h"

/*
   which oin to use?
*/
#define LED_PIN   2

/*
   define some LED states
*/
enum LED_MODE {
  LED_MODE_OFF,
  LED_MODE_ON,
  LED_MODE_BLINK_SLOW,
  LED_MODE_BLINK_FAST,
};

/*
   define the blink rates in milli seconds
*/
#define LED_BLINK_RATE_SLOW(state)    ((state) ? 1000 : 1000)
#define LED_BLINK_RATE_FAST(state)    ((state) ?  100  : 100)

/*
   setup the led
*/
void LedSetup(int led_mode);

/*
   do the cyclic update
*/
void LedUpdate();

/*
   set a new led mode
*/
void LedMode(int led_mode);

#endif

/**/
