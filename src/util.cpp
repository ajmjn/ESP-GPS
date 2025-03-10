

#include "util.h"
#include "config.h"
/*
   convert an IP address to a field of bytes
*/
const byte *IPAddressToBytes(IPAddress &addr)
{
  static uint8_t bytes[4];

  for (int n = 0; n < 4; n++)
    bytes[n] = addr[n];

  return bytes;
}

/*
   convert a field of bytes to an IP address
*/
IPAddress BytesToIPAddress(uint8_t *bytes)
{
  IPAddress addr(0, 0, 0, 0);

  for (int n = 0; n < 4; n++)
    addr[n] = bytes[n];

  return addr;
}

/*
**    generic conversion
*/
const char *AddressToString(byte *addr, int addrlen, bool dec, char sep)
{
#define ROTATE_BUFFER 4
  static int rotate = -1;
  static char rotate_buffer[ROTATE_BUFFER][25];
  char *str = rotate_buffer[++rotate % ROTATE_BUFFER];
#undef ROTATE_BUFFER
  int len = 0;

  for (int n = 0; n < addrlen; n++) {
    len += sprintf(&str[len], (dec) ? "%d" : "%02X", addr[n]);
    if (sep)
      str[len++] = sep;
  }
  str[len - 1] = '\0';

  return str;
}

const byte *StringToAddress(const char *str, int addrlen, bool dec)
{
  static byte addr[10];
  char *end;

  for (int n = 0; n < addrlen; n++) {
    addr[n] = strtoul(str, &end, (dec) ? 10 : 16);
    if (end)
      str = end + 1;
  }
  return addr;
}

/*
   dump data from an address
*/
void dump(String title, const void *addr, const int len)
{
#define BYTES_PER_ROW 16

  for (int row = 0; row * BYTES_PER_ROW < len; row++) {
    String dump_offset = "";
    String dump_hex = "";
    String dump_ascii = "";
    char tmp[10];

    sprintf(tmp, "%04x:", row * BYTES_PER_ROW);
    dump_offset = tmp;
    for (int col = 0; col < BYTES_PER_ROW && row * BYTES_PER_ROW + col < len; col++) {
      if (col > 0 && col % (BYTES_PER_ROW / 2) == 0) {
        dump_hex += " ";
        dump_ascii += " ";
      }
      byte x = ((byte *) addr)[row * BYTES_PER_ROW + col];
      sprintf(tmp, " %02x", x);
      dump_hex += tmp;
      sprintf(tmp, "%c", (isprint(x)) ? x : '.');
      dump_ascii += tmp;
    }

    while (dump_hex.length() < BYTES_PER_ROW * 3 + 1)
      dump_hex += " ";
    String dump = title + ": " + dump_offset + dump_hex + "  " + dump_ascii;
    LogMsg(dump.c_str());
  }
}


/*
   get a time in ascii
*/
const char *TimeToString(time_t t)
{
#define ROTATE_BUFFER 4
#define BUFFER_SIZE   25
  static int rotate = -1;
  static char rotate_buffer[ROTATE_BUFFER][BUFFER_SIZE];
  char *buffer = rotate_buffer[++rotate % ROTATE_BUFFER];

  t += (long) _config.ntp.timezone * SECS_PER_HOUR;
  strftime(buffer, BUFFER_SIZE, "%H:%M:%S %d.%m.%Y", gmtime(&t));
  return buffer;
#undef BUFFER_SIZE
#undef ROTATE_BUFFER
}

/*
**  log a smessage to serial
*/
void LogMsg(const char *fmt, ...)
{
  va_list args;
  char msg[256];

  va_start(args, fmt);
  vsnprintf(msg, sizeof(msg) - 1, fmt, args);
  va_end(args);

  if (Serial) {
    Serial.print(TimeToString(now()));
    Serial.print(": ");
    Serial.println(msg);
    Serial.flush();
  }
}/**/
