#include <TinyGPSPlus.h>
#include <string.h>
#include "util.h"
#include "gps.h"
#include <SoftwareSerial.h>

static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
//SoftwareSerial gps_serial(5, 4);
EspSoftwareSerial::UART gps_serial;
std::string gps_data = "";
void GpsInit()
{
    
   // gps_serial.begin(9600);
    gps_serial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, 5, 4);

}

// void gps_data()
// {
//     while ( gps_serial.available() > 0)
//     Serial.print(gps_serial.read());
//     if (gps.encode(gps_serial.read())){
//     displayInfo();
    
//     }
      

//   if (millis() > 5000 && gps.charsProcessed() < 10)
//   {
//     Serial.println(F("No GPS detected: check wiring."));
    
//   }
// }
void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gps_serial.available())
   
      gps.encode(gps_serial.read());
  } while (millis() - start < ms);
}
 void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

std::string add_string (float data ){
    gps_data +=std::to_string(data) + ",";
    return gps_data;
    
}



std::string get_latest_gps_data(){
  if(gps.location.isValid() &&gps.date.isValid()&&gps.time.isValid()){
        gps_data = "";
        gps_data += "*O,";
        gps_data += conver_to_epoch();
        gps_data += ",";
        add_string(gps.location.lat());
        add_string(gps.location.lng());
        add_string(gps.speed.kmph());
        add_string(gps.course.deg());
        gps_data +="I#";
    return gps_data;
  }
  else{
    return 0;
  }

}
std::string conver_to_epoch(){
    struct tm t;
    time_t t_of_day;
    t.tm_year = gps.date.year() +100;
    t.tm_mon = gps.date.month();
    t.tm_mday =gps.date.day() ;
    t.tm_hour = gps.time.hour() ;
    t.tm_min = gps.time.minute();
    t.tm_sec = gps.time.second();
    t.tm_isdst = -1;
    t_of_day = mktime(&t);
    return(std::to_string(t_of_day));
}