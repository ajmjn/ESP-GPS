

#include <WebServer.h>
#include <Update.h>
#include "config.h"
#include "http.h"
#include "wifi2.h"
#include "mqtt.h"
#include "ntp.h"
#include "watchdog.h"
#include "led.h"
#define DBG_HTTP 1
/*
   the web server object
*/
static WebServer _WebServer(80);

/*
   this is a callback helper to push data to the web client in chunks
*/
static void HttpSendHelper(const String& content)
{
  _WebServer.sendContent(content);
}

/*
  time of the last HTTP request
*/
static unsigned long _last_request = 0;

/*
   setup the webserver
*/
void HttpSetup(void)
{
  static String _html_header PROGMEM =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'>"
    "<title>" __TITLE__ "</title>"
    "<link href='/styles.css' rel='stylesheet' type='text/css'>"
    "</head>"
    "<body>"
    "<div class=content>"
    "<div class=header>"
    "<h3>" __TITLE__ "</h3>"
    "<h2>" + String(_config.device.name) + "</h2>"
    "</div>"
    ;
  static String _html_footer PROGMEM =
    "<div class=footer>"
    "<hr>"
    "<a target='_blank' style='color:#aaa;'>" __TITLE__ " Version: " GIT_VERSION "</a>"
    "</div>"
    "</div>"
    "</body>"
    "</html>"
    ;

  LogMsg("HTTP: setting up HTTP server");

  /*
     get the current config as a duplicate
  */
  ConfigGet(0, sizeof(CONFIG_T), &_config);

  _WebServer.onNotFound( []() {
    _last_request = millis();
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<form action='/config' method='get'><button>Configuration</button></form><p>"
                    "<form action='/info' method='get'><button>Information</button></form><p>"
                    "<form action='/upgrade' method='get'><button>Firmware Upgrade</button></form><p>"
                    "<form action='/restart' method='get' onsubmit=\"return confirm('Are you sure to restart the device?');\"><button class='button redbg'>Restart</button></form><p>"
                    + (StateCheck(STATE_CONFIGURING)
                       ? ""
                       : "<p><form action='/btlist' method='get'><button>Bluetooth Scan List</button></form>")
                    + "<p>" + _html_footer);
  });

  _WebServer.on("/styles.css", []() {
    _last_request = millis();
    _WebServer.send(200, "text/css",
                    "html, body { background:#ffffff; }"
                    "body { margin:1rem; padding:0; font-familiy:'sans-serif'; color:#202020; text-align:center; font-size:1rem; }"
                    "input { width:100%; font-size:1rem; box-sizing: border-box; -webkit-box-sizing: border-box; }"
                    "input[type=radio] { width:2rem; }"
                    "button { border: 0; border-radius: 0.3rem; background: #1881ba; color: #ffffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.5s; transition-duration: 0.5s; cursor: pointer; opacity:0.8; }"
                    "button:hover { opacity: 1.0; }"
                    ".header { text-align:center; }"
                    ".content { text-align:left; display:inline-block; color:#000000; min-width:340px; }"
                    ".msg { text-align:center; color:#be3731; font-weight:bold; padding:5rem 0; }"
                    ".devinfo { padding:0; margin:0; border-spacing:0; width: 100%; }"
                    ".devinfo tr th { background: #c0c0c0; font-weight:bold; }"
                    ".devinfo tr td { font-familiy:'monospace'; }"
                    ".devinfo tr td:first-child { font-weight:bold; }"
                    ".devinfo tr td, .devinfo tr th { padding:4px; }"
                    ".devinfo tr:nth-child(even) { background: #f0f0f0; }"
                    ".devinfo tr:nth-child(odd) {background: #ffffff; }"
                    ".btscanlist { padding:0; margin:0; border-spacing:0; width: 100%; }"
                    ".btscanlist tr th { background: #c0c0c0; font-weight:bold; }"
                    ".btscanlist tr td { font-familiy:'monospace'; }"
                    ".btscanlist tr td, .btscanlist tr th { padding:4px; }"
                    ".btscanlist tr:nth-child(even) { background: #f0f0f0; }"
                    ".btscanlist tr:nth-child(odd) {background: #ffffff; }"
                    ".footer { text-align:right; }"
                    ".greenbg { background: #348f4b; }"
                    ".redbg { background: #a12828; }"
                   );
  });

  _WebServer.on("/config", []() {
    _last_request = millis();
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    /*
       handle configuration changes
    */
#if DBG_HTTP
    for (int n = 0; n < _WebServer.args(); n++ )
      DbgMsg("HTTP: args: %s=%s", _WebServer.argName(n).c_str(), _WebServer.arg(n).c_str());
#endif

    if (_WebServer.hasArg("save")) {
      /*
         take over the configuration parameters
      */
#define CHECK_AND_SET_STRING(type,name) { if (_WebServer.hasArg(#type "_" #name)) strncpy(_config.type.name,_WebServer.arg(#type "_" #name).c_str(),sizeof(_config.type.name) - 1); }
#define CHECK_AND_SET_NUMBER(type,name,minimum,maximum) { if (_WebServer.hasArg(#type "_" #name)) _config.type.name = CHECK_RANGE(atoi(_WebServer.arg(#type "_" #name).c_str()),(minimum),(maximum)); }
#define CHECK_AND_SET_BOOL(type,name) { if (_WebServer.hasArg(#type "_" #name)) _config.type.name = (atoi(_WebServer.arg(#type "_" #name).c_str())) ? true : false; }
      CHECK_AND_SET_STRING(device, name);
      CHECK_AND_SET_STRING(device, password);
      CHECK_AND_SET_STRING(wifi, ssid);
      CHECK_AND_SET_STRING(wifi, psk);
      CHECK_AND_SET_STRING(ntp, server);
      CHECK_AND_SET_NUMBER(ntp, timezone, -12, +12);
      CHECK_AND_SET_STRING(mqtt, server);
      CHECK_AND_SET_NUMBER(mqtt, port, MQTT_PORT_MIN, MQTT_PORT_MAX);
      CHECK_AND_SET_STRING(mqtt, user);
      CHECK_AND_SET_STRING(mqtt, password);
      CHECK_AND_SET_STRING(mqtt, clientID);
      CHECK_AND_SET_STRING(mqtt, topicPrefix);
      CHECK_AND_SET_NUMBER(mqtt, publish_timeout, MQTT_PUBLISH_TIMEOUT_MIN, MQTT_PUBLISH_TIMEOUT_MAX);
      CHECK_AND_SET_BOOL(mqtt, publish_absence);
      CHECK_AND_SET_STRING(mqtttopic, gps_data_pub);
      CHECK_AND_SET_STRING(mqtttopic, device_status_pub);
      CHECK_AND_SET_STRING(mqtttopic, obd_data_pub);
      CHECK_AND_SET_STRING(mqtttopic, distance_sub);
      CHECK_AND_SET_STRING(mqtttopic, command_sub);
      CHECK_AND_SET_NUMBER(configled, led_pin, LED_PIN,LED_PIN );
      CHECK_AND_SET_STRING(configgprs, gprs_apn);
      CHECK_AND_SET_STRING(configgprs, gprs_user);
      CHECK_AND_SET_STRING(configgprs, gprs_pass);
      

     
      /*
         write the config back
      */
      ConfigSet(0, sizeof(CONFIG_T), &_config);

      /*
         activate the new settings
      */
      if (!StateCheck(STATE_CONFIGURING)) {
        NtpSetup();
        MqttSetup();
       
        WatchdogSetup(_config.bluetooth.scan_time);
      }
    }

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<form action='/config/device' method='get'><button>Configure Device</button></form><p>"
                    "<form action='/config/wifi' method='get'><button>Configure WiFi</button></form><p>"
                    "<form action='/config/ntp' method='get'><button>Configure NTP</button></form><p>"
                    "<form action='/config/mqtt' method='get'><button>Configure MQTT</button></form><p>"
                    "<form action='/config/set_mqtt_topics' method='get'><button>Configure MQTT Topics</button></form><p>"
                    "<form action='/config/gprs_config' method='get'><button>Configure GPRS</button></form><p>"
                    "<form action='/config/reset' method='get' onsubmit=\"return confirm('Are you sure to reset the configuration?');\"><button class='button redbg'>Reset configuration</button></form><p>"
                    "<p><form action='/' method='get'><button>Main Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/device", []() {
    _last_request = millis();
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;Device&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>Name</b>"
                    "<br>"
                    "<input name='device_name' type='text' placeholder='Device name' value='" + String(_config.device.name) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Web Password</b>"
                    "<br>"
                    "<input name='device_password' type='password' placeholder='Device Password' value='" + String(_config.device.password) + "'>"
                    "<br>"
                    "<b>Note:</b> username for authentication is <b>" HTTP_WEB_USER "</b>"
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/wifi", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;WiFi&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>SSID</b>"
                    "<br>"
                    "<input name='wifi_ssid' type='text' placeholder='WiFi SSID' value='" + String(_config.wifi.ssid) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Password</b>"
                    "<br>"
                    "<input name='wifi_psk' type='password' placeholder='WiFi Password' value='" + String(_config.wifi.psk) + "'>"
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/ntp", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;NTP&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>Server Name or IP Address</b>"
                    "<br>"
                    "<input name='ntp_server' type='text' placeholder='NTP server' value='" + String(_config.ntp.server) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Timezone (+/- offset in hours)</b>"
                    "<br>"
                    "<input name='ntp_timezone' type='text' placeholder='Timezone' value='" + String(_config.ntp.timezone) + "'>"
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/mqtt", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;MQTT&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>Server Name or IP Address</b>"
                    "<br>"
                    "<input name='mqtt_server' type='text' placeholder='MQTT server' value='" + String(_config.mqtt.server) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Port</b>"
                    "<br>"
                    "<input name='mqtt_port' type='text' placeholder='MQTT port' value='" + String(_config.mqtt.port) + "'>"
                    "</p>"

                    "<p>"
                    "<b>User (optional)</b>"
                    "<br>"
                    "<input name='mqtt_user' type='text' placeholder='MQTT user' value='" + String(_config.mqtt.user) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Password (optional)</b>"
                    "<br>"
                    "<input name='mqtt_password' type='text' placeholder='MQTT password' value='" + String(_config.mqtt.password) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Client ID</b>"
                    "<br>"
                    "<input name='mqtt_clientID' type='text' placeholder='MQTT ClientID' value='" + String(_config.mqtt.clientID) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Topic Prefix</b>"
                    "<br>"
                    "<input name='mqtt_topicPrefix' type='text' placeholder='MQTT Topic Prefix' value='" + String(_config.mqtt.topicPrefix) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Publishing of Presence &amp; Absence</b>"
                    "<br>"
                    "<input name='mqtt_publish_absence' type='radio' value='0'" + (_config.mqtt.publish_absence ? "" : " checked") + "> Publish only presence" +
                    "<br>"
                    "<input name='mqtt_publish_absence' type='radio' value='1'" + (_config.mqtt.publish_absence ? " checked" : "") + "> Publish presence &amp; absence" +
                    "<br>"
                    "<b>Note:</b> Selecting <i>Publish only presence</i> might help if multiple scanners are publishing to the same object."
                    "</p>"

                    "<p>"
                    "<b>Publishing Timeout (" + MQTT_PUBLISH_TIMEOUT_MIN + "s - " + MQTT_PUBLISH_TIMEOUT_MAX + "s)</b>"
                    "<br>"
                    "<input name='mqtt_publish_timeout' type='text' placeholder='MQTT Publishing Timeout' value='" + String(_config.mqtt.publish_timeout) + "'>"
                    "<br>"
                    "<b>Note:</b> This timeout is for device updates on a regular base. If a device presence changes, it will be reported instantly."
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

_WebServer.on("/config/set_mqtt_topics", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;MQTT TOPICS&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>GPS data publish</b>"
                    "<br>"
                    "<input name='mqtttopic_gps_data_pub' type='text' placeholder='gps_data_pub' value='" + String(_config.mqtttopic.gps_data_pub) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Device Status Publish</b>"
                    "<br>"
                    "<input name='mqtttopic_device_status_pub' type='text' placeholder='device_status_pub' value='" + String(_config.mqtttopic.device_status_pub) + "'>"
                    "</p>"

                    "<p>"
                    "<b>OBD Data Publish</b>"
                    "<br>"
                    "<input name='mqtttopic_obd_data_pub' type='text' placeholder='obd_data_pub' value='" + String(_config.mqtttopic.obd_data_pub) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Distance GAP subscribe</b>"
                    "<br>"
                    "<input name='mqtttopic_distance_sub' type='text' placeholder='GAP_sub' value='" + String(_config.mqtttopic.distance_sub) + "'>"
                    "</p>"

                    "<p>"
                    "<b>Command Sub</b>"
                    "<br>"
                    "<input name='mqtttopic_command_sub' type='text' placeholder='command_sub' value='" + String(_config.mqtttopic.command_sub) + "'>"
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/led_config", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;LED PIN SETUP&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>Red led pin</b>"
                    "<br>"
                    "<input name='red_led_pin' type='text' placeholder='' value='" + String(_config.configled.led_pin) + "'>"
                    "</p>"
                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

   _WebServer.on("/config/gprs_config", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;GPRS CONFIG&nbsp;</b>"
                    "</legend>"
                    "<form method='get' action='/config'>"

                    "<p>"
                    "<b>Gprs APN</b>"
                    "<br>"
                    "<input name='gprs_apn' type='text' placeholder='' value='" + String(_config.configgprs.gprs_apn) + "'>"
                    "</p>"

                     "<p>"
                    "<b>Gprs APN</b>"
                    "<br>"
                    "<input name='gprs_user' type='text' placeholder='' value='" + String(_config.configgprs.gprs_user) + "'>"
                    "</p>"

                     "<p>"
                    "<b>Gprs APN</b>"
                    "<br>"
                    "<input name='gprs_pass' type='text' placeholder='' value='" + String(_config.configgprs.gprs_pass) + "'>"
                    "</p>"

                    "<button name='save' type='submit' class='button greenbg'>Submit</button>"
                    "</form>"
                    "</fieldset>"
                    "<p><form action='/config' method='get'><button>Configuration Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/config/reset", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();

    /*
       reset the config
    */
    memset(&_config, 0, sizeof(_config));
    ConfigSet(0, sizeof(CONFIG_T), &_config);

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<div class='msg'>"
                    "Configuration was reset."
                    "<p>"
                    "Wait for the device to come up with an WiFi-AccessPoint, connect to it to configure the device."
                    "</div>"
                    + _html_footer);

    /*
        trigger reboot
    */
    StateChange(STATE_WAIT_BEFORE_REBOOTING);
    
  });

  _WebServer.on("/info", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<div class='info'>"
                    "<table class='devinfo'>"

                    "<tr><th colspan=2>Device</th></tr>"
                    "<tr>"
                    "<td>SW Version</td>"
                    "<td>" GIT_VERSION "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>SW Build Date</td>"
                    "<td>" __DATE__ " " __TIME__ "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Device Name</td>"
                    "<td>" + String(_config.device.name) + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Up since</td>"
                    "<td>" + String(TimeToString(NtpUpSince())) + "</td>"
                    "</tr>"

                    "<tr><th colspan=2>WiFi</th></tr>"
                    "<tr>"
                    "<td>SSID</td>"
                    "<td>" + WifiGetSSID() + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Channel</td>"
                    "<td>" + WifiGetChannel() + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>RSSI</td>"
                    "<td>" + String(WIFI_RSSI_TO_QUALITY(WifiGetRSSI())) + "% (" + WifiGetRSSI() + "dBm)</td>"
                    "</tr>"
                    "<tr>"
                    "<td>MAC</td>"
                    "<td>" + WifiGetMacAddr() + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>IP Address</td>"
                    "<td>" + WifiGetIpAddr() + "</td>"
                    "</tr>"

                    "<tr><th colspan=2>NTP</th></tr>"
                    "<tr>"
                    "<td>Server</td>"
                    "<td>" + _config.ntp.server + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Timezone</td>"
                    "<td>" + _config.ntp.timezone + "</td>"
                    "</tr>"

                    "<tr><th colspan=2>MQTT</th></tr>"
                    "<tr>"
                    "<td>Host</td>"
                    "<td>" + _config.mqtt.server + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Port</td>"
                    "<td>" + _config.mqtt.port + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>User</td>"
                    "<td>" + _config.mqtt.user + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Password</td>"
                    "<td>" + _config.mqtt.password + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>ClientID</td>"
                    "<td>" + _config.mqtt.clientID + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Topic Prefix</td>"
                    "<td>" + _config.mqtt.topicPrefix + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>MQTT Publishing of Presence &amp; Absence</td>"
                    "<td>" + (_config.mqtt.publish_absence ? "presence &amp; absence" : "only presence") + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>MQTT Publish Timeout</td>"
                    "<td>" + _config.mqtt.publish_timeout + "</td>"
                    "</tr>"
                    "<tr>"

                    "<tr><th colspan=2>Bluetooth</th></tr>"
                    "<tr>"
                    "<td>LE Scan Time</td>"
                    "<td>" + _config.bluetooth.scan_time + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Scan Pause Time</td>"
                    "<td>" + _config.bluetooth.pause_time + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Active Scan Timeout</td>"
                    "<td>" + _config.bluetooth.activescan_timeout + "</td>"
                    "</tr>"
                    "<tr>"
                    "<td>Absence Timeout Cycle</td>"
                    "<td>" + _config.bluetooth.absence_cycles + "</td>"
                    "</tr>"
                    "<td>Battery Check Timeout</td>"
                    "<td>" + _config.bluetooth.battcheck_timeout + "</td>"
                    "</tr>"

                    "</table>"
                    "</div>"
                    "<p><form action='/' method='get'><button>Main Menu</button></form><p>"
                    + _html_footer);
  });

  _WebServer.on("/restart", []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();

    _WebServer.send(200, "text/html",
                    _html_header +
                    "<div class='msg'>"
                    "Device will restart now."
                    "</div>"
                    "<p><form action='/' method='get'><button>Main Menu</button></form><p>"
                    + _html_footer);

    /*
        trigger reboot
    */
    StateChange(STATE_WAIT_BEFORE_REBOOTING);
  });

  /*
     firmware upgrade -- form
  */
  _WebServer.on("/upgrade", HTTP_GET, []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();
    DbgMsg("HTTP: /upgrade GET");

    _WebServer.send(200, "text/html",
                    _html_header +

                    "<fieldset>"
                    "<legend>"
                    "<b>&nbsp;Upgrade by file upload&nbsp;</b>"
                    "</legend>"
                    "<form method='post' action='/upgrade' enctype='multipart/form-data'>"

                    "<p>"
                    "<b>Firmware File</b>"
                    "<br>"
                    "<input name='fwfile' type='file' placeholder='Firmware File'>"
                    "</p>"

                    "<button name='upgrade' type='submit' class='button greenbg'>Start upgrade</button>"
                    "</form>"
                    "</fieldset>"

                    "<p><form action='/' method='get'><button>Main Menu</button></form><p>"
                    + _html_footer);
  });

  /*
     firmware upgrade -- flash
  */
  _WebServer.on("/upgrade", HTTP_POST, []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();

    DbgMsg("HTTP: /upgrade POST upload");
    _WebServer.send(200, "text/html",
                    _html_header +
                    "<div class='msg'>"
                    "Upgrade " + ((Update.hasError()) ? "failed" : "succeed") +
                    "<p>"
                    "Device will restart now."
                    "</div>"
                    "<p><form action='/' method='get'><button>Main Menu</button></form><p>"
                    + _html_footer);

    /*
        trigger reboot
    */
    StateChange(STATE_WAIT_BEFORE_REBOOTING);
  }, []() {
    if (!StateCheck(STATE_CONFIGURING) && _config.device.password[0] && !_WebServer.authenticate(HTTP_WEB_USER, _config.device.password))
      return _WebServer.requestAuthentication();

    _last_request = millis();

    /*
       this is the upload handler
    */
    DbgMsg("HTTP: /upgrade POST upload");
    HTTPUpload& upload = _WebServer.upload();

    if (upload.status == UPLOAD_FILE_START) {
      /*
         initiate the upgrade with the maximum possible size
      */
      DbgMsg("HTTP: UPLOAD_FILE_START  upload file: %s", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        DbgMsg("HTTP: upgrade failure: %s", Update.errorString());
      }
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
      /*
        upload the flash file
      */
      DbgMsg("HTTP: UPLOAD_FILE_WRITE: current size=%d", upload.currentSize);
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        DbgMsg("HTTP: upload failure: %s", Update.errorString());
      }
    }
    else if (upload.status == UPLOAD_FILE_END) {
      /*
        flash the flash file
      */
      DbgMsg("HTTP: UPLOAD_FILE_END");
      if (Update.end(true)) {
        if (Update.isFinished()) {
          DbgMsg("HTTP: upgrade success: %ubytes -- rebooting...", upload.totalSize);
        }
        else {
          DbgMsg("HTTP: upgrade failure: %s", Update.errorString());
        }
      }
      else {
        DbgMsg("HTTP: upgrade failure: %s", Update.errorString());
      }
    }
  });

  _WebServer.begin();
  _last_request = millis();
  LogMsg("HTTP: server started");
}

/*
**	handle incoming HTTP requests
*/
void HttpUpdate(void)
{
  _WebServer.handleClient();
}

/*
  return the time in seconds since the last HTTP request
*/
int HttpLastRequest(void)
{
  return (millis() - _last_request) / 1000;
}/**/
