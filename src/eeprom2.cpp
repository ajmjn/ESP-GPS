

#include "eeprom2.h"
#include <EEPROM.h>

static int _eeprom_size;

/*
**  init the EEPROM handling
*/
void EepromInit(const int size)
{
  _eeprom_size = size;
  EEPROM.begin(_eeprom_size);

#if DBG_EEPROM
  EepromDump();
#endif
}

/*
**  clear the eeprom
*/
void EepromDump(void)
{
  byte *buffer;

  if ((buffer = (byte *) malloc(_eeprom_size))) {
    EepromRead(0, _eeprom_size, buffer);
#if DBG_EEPROM
    dump("EEPROM", buffer, _eeprom_size);
#endif
    free(buffer);
  }
}

/*
**  clear the eeprom
*/
void EepromClear(void)
{
  for (int n = 0; n < _eeprom_size; n++)
    EEPROM.write(n, 0xff);
  EEPROM.commit();
}

/*
**    read from the EEPROM
**
**    if all bytes were FF, zero is returned
**    and the buffer is not written
*/
int EepromRead(const int addr, const int len, void *buffer)
{
  int n;

  for (n = 0; n < len; n++) {
    if (EEPROM.read(addr + n) != 0xff)
      break;
  }
  if (n >= len)
    return 0;

  for (n = 0; n < len; n++) {
    ((byte *) buffer)[n] = EEPROM.read(addr + n);
#if DBG_EEPROM
    DbgMsg("EEPROM.read: addr[%d] = %u", addr + n, ((byte *) buffer)[n]);
#endif
  }

  return 1;
}

/*
**	write to the EEPROM
*/
void EepromWrite(const int addr, const int len, const void *buffer)
{
  for (int n = 0; n < len; n++) {
    EEPROM.write(addr + n, ((byte *) buffer)[n]);
#if DBG_EEPROM
    DbgMsg("EEPROM.write: addr[%d] = %u", addr + n, ((byte *) buffer)[n]);
#endif
  }
  EEPROM.commit();
#if DBG_EEPROM
  EepromDump();
#endif
}/**/
