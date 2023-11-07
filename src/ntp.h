
#ifndef __NTP_H__
#define __NTP_H__ 1

#include <TimeLib.h>
#include "config.h"
#include "util.h"

/*
**  ntp sync cycle
*/
#define NTPSYNC_CYCLES 100

/*
**  init the NTP functions
*/
void NtpSetup(void);

/*
**  update the NTP time
*/
void NtpUpdate(void);

/*
**  get the NTTP time
*/
time_t NtpGetTime(void);

/*
   get the uptime
*/
time_t NtpUptime(void);

/*
   get the frist received timestamp
*/
time_t NtpUpSince(void);

#endif

/**/
