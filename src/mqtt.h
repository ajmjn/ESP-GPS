

#ifndef __MQTT_H__
#define __MQTT_H__ 1

#include <PubSubClient.h>
#include "config.h"

/*
   MQTT settings
*/
#define MQTT_PORT_DEFAULT         1883
#define MQTT_PORT_MIN             1024
#define MQTT_PORT_MAX             65535


#define MQTT_TOPIC_ANNOUNCE       "/status"
#define MQTT_TOPIC_CONTROL        "/control"
#define MQTT_TOPIC_DEVICE         "/device"

#define MQTT_PUBLISH_TIMEOUT_MIN  10            // seconds
#define MQTT_PUBLISH_TIMEOUT_MAX  (60 * 60)

/*
   if the MQTT connection failed, wait this time before retrying
*/
#define MQTT_WAIT_TO_RECONNECT  10

/*
   time for cyclic status update
*/
#define MQTT_STATUS_UPDATE_CYCLE  (5*60)

/*
   initialize the MQTT context
*/
void MqttSetup(void);

/*
   cyclic update of the MQTT context
*/
void MqttUpdate(void);

/*
   return true if we should publish all devices

   this will be done after a reconnect and from time to time
*/
bool MqttPublishAll(void);

/*
   publish the given message
*/
void MqttPublish(String suffix, String msg);

#endif

/**/
