
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "eeprom2.h"
#define DBG_CFG 1
CONFIG_T _config;

/*
    setup the configuration
*/
bool ConfigSetup(void)
{
  /*
     read the full config from the EEPROM
  */
  memset(&_config, 0, sizeof(CONFIG_T));
  EepromInit(sizeof(CONFIG_T));
  EepromRead(0, sizeof(CONFIG_T), &_config);

  /*
     check if the config version is ok
  */
  if (strcmp(_config.magic, CONFIG_MAGIC) || _config.version != CONFIG_VERSION) {
    /*
       set a new default configuration
    */
    LogMsg("CFG: unexpected magic or version found -- erasing config and entering config mode");
    memset(&_config, 0, sizeof(CONFIG_T));

    strcpy(_config.magic, CONFIG_MAGIC);
    _config.version = CONFIG_VERSION;

    return false;
  }

#if DBG_CFG
  dump("CFG:", &_config, sizeof(CONFIG_T));
#endif
  return true;
}

/*
   cyclic update of the configuration
*/
void ConfigUpdate(void)
{
  // nothin to do so far
}


/*
   functions to get the configuration for a subsystem
*/
void ConfigGet(int offset, int size, void *cfg)
{
#if DBG_CFG
  DbgMsg("CFG: getting config: offset:%d  size:%d  cfg:%p", offset, size, cfg);
#endif

  memcpy(cfg, (byte *) &_config + offset, size);

#if DBG_CFG
  dump("CFG:", cfg, size);
#endif
}

/*
   functions to set the configuration for a subsystem -- will be written to the EEPROM
*/
void ConfigSet(int offset, int size, void *cfg)
{
#if DBG_CFG
  DbgMsg("CFG: setting config: offset:%d  size:%d  cfg:%p", offset, size, cfg);
#endif

  memcpy((byte *) &_config + offset, cfg, size);

#if DBG_CFG
  dump("CFG:", cfg, size);
#endif

  EepromWrite(offset, size, (byte *) &_config + offset);
}/**/
