

#include "config.h"
#include "led.h"
#include "wifi2.h"
#include "ntp.h"
#include "http.h"
#include "mqtt.h"
#include "state.h"
#include "util.h"
#include "watchdog.h"
#include "TaskManagerIO.h"
#include "elm327.h"
#if defined(ESP32)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "gps.h"
#include "gsm.h"
#endif
TaskHandle_t blink;
TaskHandle_t rpm_task;
void get_rpm_task(void* parameters)
{
    for (;;)
    {
       obd_mqtt_update();
      vTaskDelay(pdMS_TO_TICKS(300000));
    }
}

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
  elm_init();
   xTaskCreatePinnedToCore(
        get_rpm_task, // Task function.
        "Task1",      // Name of task.
        10000,        // Stack size of task
        NULL,         // Parameter of the task
        1,            // Priority of the task
        &rpm_task,    // Task handle to keep track of created task
        0);           // Pin task to core 0

 //  xTaskCreate(&vLEDFlashTask, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  // xTaskCreate(&elmtask, "elm_task", configMINIMAL_STACK_SIZE, NULL, 6, NULL);
 
  //taskManager.schedule(repeatSeconds(1),elm_test);
  //taskid_t taskId3 = taskManager.schedule(repeatSeconds(10),gsm_mqtt_update);
 
}

void loop()
{
  
displayInfo();
gsm_mqtt_update();
 
delay(5000);
 
}
