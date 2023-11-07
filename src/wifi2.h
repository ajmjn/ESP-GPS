

#ifndef __WIFI_H__
#define __WIFI_H__  1

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include "config.h"
#include "state.h"

extern WiFiClient _wifiClient;

/*
   the WiFi SSID for configuration mode
*/
#define WIFI_AP_SSID_PREFIX                __TITLE__ "-AP-"

/*
   the WiFi SSID for configuration mode uses the last digits of the own MAC
*/
#define WIFI_AP_SSID_USE_LAST_MAC_DIGITS   3

/*
   number of retries to connect to the configured Wifi

   when retrie count is reached, we will give up -- the device will enter the configuration mode
*/
#define WIFI_CONNECT_RETRIES          20

/*
   port for the DNS
*/
#define DNS_PORT  53

/*
   compute the WiFi signal strength in percent out of the RSSI
*/
#define WIFI_RSSI_TO_QUALITY(rssi)  max(0,min(100,2 * (100 + rssi)))

/*
   setup wifi
*/
bool WifiSetup(void);

/*
   do Wifi updates
*/
bool WifiUpdate(void);

/*
   return the SSID
*/
String WifiGetSSID(void);

/*
   return the SSID
*/
int WifiGetChannel(void);

/*
   return the SSID
*/
int WifiGetRSSI(void);

/*
   return the own IP address
*/
String WifiGetIpAddr(void);

/*
   return the Wifi MAC address
*/
String WifiGetMacAddr(void);

/*
   get the WiFi client object for Wifi users
*/
WiFiClient *WifiGetClient(void);

#endif

/**/
