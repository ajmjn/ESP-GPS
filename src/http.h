

#ifndef __HTTP_H__
#define __HTTP_H__ 1

#include "config.h"
#include "util.h"

/*
   user for the Basic Auth
*/
#define HTTP_WEB_USER   "admin"

/*
**  setup the HTTP web server:w
*/
void HttpSetup(void);

/*
**	handle incoming HTTP requests
*/
void HttpUpdate(void);

/*
  return the time in seconds since the last HTTP request
*/
int HttpLastRequest(void);

#endif

/**/
