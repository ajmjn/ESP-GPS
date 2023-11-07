
#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__  1

/*
   timeout for the watchdog
*/
#define WATCHDOG_TIMEOUT   10 // [s]


void WatchdogSetup(int timeout);
void WatchdogUpdate(void);

#if UNIT_TEST
void WatchdogUnitTest(void);
#endif

#endif

/**/
