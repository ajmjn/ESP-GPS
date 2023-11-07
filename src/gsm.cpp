#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial

#include "gsm.h"
#include <PubSubClient.h>
#include "config.h"
#include "gps.h"
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#define TINY_GSM_DEBUG SerialMon
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 57600
#define NCELL_APN "web"
#define BROKER_URL "mqtt://4.240.48.93"
#define MQTT_PORT 1883
#define MQTT_USER "654321"
#define MQTT_CLIENT_ID "654321"
#define MQTT_PASSWORD "123456789"
#define MQTT_PUBLISH_TOPIC "G"
#define MQTT_SUB_ 
#define GSM_RX 11
#define GSM_TX 12
#define GPS_RX 5
#define GPS_TX 4
const char apn[]      = "web";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char* broker = "broker.mqtt.cool";
const char* get_gps_data = "";

const char* topicLed       = "GsmClientTest/led";
const char* topicInit      = "GsmClientTest/init";
const char* topicLedStatus = "GsmClientTest/ledStatus";
SoftwareSerial SerialAT(11, 12);
TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient  mqtt(client);
uint32_t lastReconnectAttempt = 0;
void mqttCallback(char* topic, byte* payload, unsigned int len);
boolean mqttConnect();
void gsm_init()
{
    //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
    SerialAT.begin(9600);

     DBG("Initializing modem...");
   
  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);
  DBG("Waiting for network...");
  if (!modem.waitForNetwork(6000L, true)) {
    DBG("Unable to connect to the Network");
  }

  if (modem.isNetworkConnected()) { DBG("Network connected"); }

   SerialMon.print(F("Connecting to "));
   SerialMon.print(_config.configgprs.gprs_apn);
  if (!modem.gprsConnect(_config.configgprs.gprs_apn)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

   if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
    mqtt.setServer(_config.mqtt.server, _config.mqtt.port);
  mqtt.setCallback(mqttCallback);
   mqttConnect();
  
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();

  // Only proceed if incoming message's topic matches
  
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(_config.mqtt.server);
 SerialMon.print(",");
 SerialMon.print(_config.mqtt.port);
 SerialMon.print(",");
 SerialMon.print(_config.mqtt.clientID);
 SerialMon.print(",");
 SerialMon.print(_config.mqtt.user);
 SerialMon.print(",");
 SerialMon.print(_config.mqtt.password);
 

  // Connect to MQTT Broker
  boolean status = mqtt.connect(_config.mqtt.clientID,
                              _config.mqtt.user,
                              _config.mqtt.password);

  // Or, if you want to authenticate MQTT:
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void gsm_mqtt_update() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }
  }
  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    delay(100);
    return;
  }


SerialMon.println("mqtt update called");

 get_gps_data = get_latest_gps_data().c_str();
 if(get_gps_data>0){
 SerialMon.println("mqtt update");
 SerialMon.println(get_gps_data);
 mqtt.publish("G",get_gps_data);
 }
 else
 SerialMon.println("gps data invalid");
  mqtt.loop();
}
