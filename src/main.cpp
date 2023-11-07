

#include "config.h"
#include "led.h"
#include "wifi2.h"
#include "ntp.h"
#include "http.h"
#include "mqtt.h"
#include "state.h"
#include "util.h"
#include "watchdog.h"
#if defined(ESP32)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "gps.h"
#include "gsm.h"
#endif

void setup()
{
#if defined(ESP32)
  /*
     disable the brownout detector (BOD)
  */
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
#elif defined(ESP8266)
  // there is no BOD in the ESP8266
#endif

  /*
     setup serial communication
  */
  Serial.begin(9600);

  Serial.println();
  LogMsg("*** " __TITLE__ " - Version " GIT_VERSION " ***");
  pinMode(16,INPUT_PULLUP);
  LedSetup(LED_MODE_ON);
  ConfigSetup();
  if(digitalRead(16)==0){
     StateChange(STATE_CONFIGURING);
    Serial.println("configure mode active");
    
    WifiSetup();
    HttpSetup();
    LedSetup(LED_MODE_BLINK_FAST);
    while(StateCheck(STATE_CONFIGURING)){
      HttpUpdate();
      LedUpdate();
    }
  }
  
   gsm_init();
   GpsInit();
   
 
}

void loop()
{
 
  smartDelay(1000);
  displayInfo();
  gsm_mqtt_update();
 
  delay(1000);
 
}
