
#ifndef __STATE_H__
#define __STATE_H__ 1

/*
   the device will reboot if there is inactivity for this period in configuration mode
*/
#define STATE_CONFIGURING_TIMEOUT   (5 * 60)

/*
   STATE handling

   if we are in state configuring NTP, MQTT, BLE and BasicAuth are disabled
*/
enum STATE {
  STATE_NONE = 0,
  STATE_SCANNING,
  STATE_PAUSING,
  STATE_CONFIGURING,
  STATE_WAIT_BEFORE_REBOOTING,
  STATE_REBOOT,
};

/*
   setup the state maschine
*/
void StateSetup(int state);

/*
   do the cyclic processing

   return the current state when ever we entered a new state
*/
int StateUpdate(void);

/*
   change the state manually
*/
void StateChange(int state);

/*
   change the timeout of a state
*/
void StateModifyTimeout(int state, unsigned int timeout);

/*
   check if we are in a certain state
*/
bool StateCheck(int state);

#endif
