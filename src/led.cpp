
#include <Arduino.h>
#include "config.h"
#include "led.h"
#include "util.h"
#include <Adafruit_NeoPixel.h>
#define DBG_LED 1

static int _led_mode = LED_MODE_OFF;
static bool _led_state = false;
static unsigned long _led_last_blink = false;
#define PIN        2 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 3 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
/*
   setup the led
*/
void LedSetup(int led_mode)
{
  pixels.begin();
     pixels.clear();
  LedMode(led_mode);
}

/*
   do the cyclic update
*/
void LedUpdate()
{
  unsigned long blink_rate = 0;
  unsigned long now = millis();
 
  switch (_led_mode) {
    case LED_MODE_BLINK_SLOW:
      blink_rate = LED_BLINK_RATE_SLOW(_led_state);
      break;
    case LED_MODE_BLINK_FAST:
      blink_rate = LED_BLINK_RATE_FAST(_led_state);
      break;
  }

#if DBG_LED
  DbgMsg("LED: _led_mode=%d  _led_last_blink=%lu  _led_state=%d  now=%lu  blink_rate=%lu", _led_mode, _led_last_blink, _led_state, now, blink_rate);
#endif

  if (blink_rate && now - _led_last_blink > blink_rate) {
    // digitalWrite(LED_PIN, _led_state = !_led_state);
    int temp_led_stat =  _led_state = !_led_state;
    if(temp_led_stat==1){
       pixels.clear();
      pixels.setPixelColor(1, pixels.Color(0, 10, 0));
       pixels.show();
     
    }
    else{
       pixels.clear();
       pixels.setPixelColor(1, pixels.Color(0, 0, 0));
        pixels.show();
    }
    _led_last_blink = now;
  
    delay(100);
  }
}

/*
   set a new led mode
*/
void LedMode(int led_mode)
{
#if DBG_LED
  DbgMsg("LED: _led_mode=%d  _led_last_blink=%lu  _led_state=%d", _led_mode, _led_last_blink, _led_state);
#endif

  switch (_led_mode = led_mode) {
    case LED_MODE_BLINK_SLOW:
    case LED_MODE_BLINK_FAST:
      _led_last_blink = 0;
      break;
  }
 // digitalWrite(LED_PIN, _led_state = (_led_mode == LED_MODE_ON) ? true : false);

#if DBG_LED
  DbgMsg("LED: _led_mode=%d  _led_last_blink=%lu  _led_state=%d", _led_mode, _led_last_blink, _led_state);
#endif

  LedUpdate();
}/**/

