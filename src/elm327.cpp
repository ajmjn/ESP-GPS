#include <SoftwareSerial.h>
#include "ELMduino.h"
#include "elm327.h"

//SoftwareSerial mySerial(7, 21); // RX, TX
EspSoftwareSerial::UART mySerial;
#define ELM_PORT mySerial
#define SerialMon Serial
#define WAIT_TIME 2000 
bool elm_staus;
ELM327 myELM327;
uint32_t rpm = 0;
uint32_t fp = 0;
uint32_t temp2 =0;
unsigned long startTime;
std::string obd_data = "";
void elm_init()
{

 
  ELM_PORT.begin(38400, EspSoftwareSerial::SWSERIAL_8N1, 7, 21);

  SerialMon.println("Attempting to connect to ELM327...");

  if (!myELM327.begin(ELM_PORT, false, 2000,'6'))
  {
    SerialMon.println("Couldn't connect to OBD scanner");
    elm_staus = 0;
    return;
  }
  elm_staus = 1;
  SerialMon.println("Connected to ELM327");
}

float fuel_pressure()
{
  startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float fuelpressure_read = myELM327.fuelPressure();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return fuelpressure_read;    
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float fuel_tank_level_input()
{
  startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float fuel_level = myELM327.fuelLevel();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return fuel_level;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float vehicle_speed()
{
startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float speed = myELM327.mph();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return speed;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float engine_load()
{
startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float engineload = myELM327.engineLoad();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return engineload;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float short_fuel_trim_1()
{
  startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float stftb1 = myELM327.shortTermFuelTrimBank_1();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return  stftb1;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}
float short_fuel_trim_2()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float stftb2 = myELM327.shortTermFuelTrimBank_2();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return  stftb2;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}
float long_fuel_trim_1()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float ltftb1 = myELM327.longTermFuelTrimBank_1();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return  ltftb1;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float long_fuel_trim_2()
{
  startTime = millis();
  while (millis() - startTime < WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float ltftb2 = myELM327.longTermFuelTrimBank_2();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return  ltftb2;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  }
}

float fuel_trim()
{
  float stftb1 = short_fuel_trim_1();
  float stftb2 = short_fuel_trim_2();
  float ltftb1 = long_fuel_trim_1();
  float ltftb2 = long_fuel_trim_2();
  return((stftb1+stftb2+ltftb1+ltftb2)/4);
}

float air_intake_temperature()
{
 startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float airintaketemp = myELM327.intakeAirTemp();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return  airintaketemp;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

float fuel_consumption_rate()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float fuelrate = myELM327.fuelRate();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return fuelrate;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

float five_min_average_fuel_efficiency()
{
  float engineload= engine_load();
  float fueltrim =fuel_trim();
  float airintaketemp = air_intake_temperature();
  float fuelrate =fuel_consumption_rate();
  return((engineload+fueltrim+airintaketemp)/fuelrate);
}

float engine_rpm()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float enrpm = myELM327.rpm();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return enrpm;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}


float five_min_average_engine_coolant_temperature()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float encotemp = myELM327.engineCoolantTemp();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return encotemp;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

float throttle_position()
{
   startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float thposition = myELM327.throttle();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return thposition;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

float accelerator_pedal_position()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float acpepostion = myELM327.relativePedalPos();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return acpepostion;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}




float five_min_average_driver_efficiency()
{
  float speed = vehicle_speed();
  float thpostion = throttle_position();
  float accpepostion = accelerator_pedal_position();
  float enrpm = engine_rpm();
  float fucorate = fuel_consumption_rate();
  return(((speed*thpostion*accpepostion)/(fucorate*enrpm))/0.02*100);
}

float oxygen_sensor_readings()
{
  float oxreading1;
  float oxreading2;

  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   oxreading1 = myELM327.oxygenSensorsPresent_2banks();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    break;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      oxreading1=0;
    }
  } 
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   oxreading2 = myELM327.oxygenSensorsPresent_4banks();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    break;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      oxreading2=0;
    }
  } 
  return((oxreading1+oxreading2)/2);
}

float intake_manifold_absolute_pressure()
{
startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float inmanabpressure = myELM327.manifoldPressure();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return inmanabpressure;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

float maf_rate()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   float mafrate = myELM327.mafRate();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return mafrate;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}


float fuel_air_equivalence_ratio()
{
  float fuelrate = fuel_consumption_rate();
  float mafrate = maf_rate();
  return ((fuelrate/mafrate)*14.7);
}

float five_min_average_vehicle_performance()
{
float enrpm = engine_rpm();
float enload = engine_load();
float thposition = throttle_position();
float airintemp = air_intake_temperature();
float oxreading =  oxygen_sensor_readings();
float feaieqratio = fuel_air_equivalence_ratio();
float fuelpress = fuel_pressure();
float intakemfabpresure = intake_manifold_absolute_pressure();
return(((enrpm*enload*thposition)/(airintemp*oxreading*feaieqratio*fuelpress*intakemfabpresure)/10*100));

}

uint16_t freeze_dtc()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   uint16_t frdtc = myELM327.freezeDTC();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return frdtc;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}
uint16_t engine_run_time()
{
   startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   uint16_t enruntime = myELM327.runTime();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return enruntime;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}


float distance_traveled()
{
  startTime = millis();
  while (millis() - startTime <  WAIT_TIME)  // Wait for up to 5 seconds (adjust the timeout as needed)
  {
   uint16_t dttv = myELM327.distSinceCodesCleared();
    
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
    return dttv;     
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    {
      myELM327.printError();
      delay(100);
      return 0;
    }
  } 
}

std::string add_string_flaot (float data ){
    obd_data +=std::to_string(data) + ",";
    return obd_data;
    
}

std::string add_string_int(uint16_t data)
{
  obd_data +=std::to_string(data) + ",";
  return obd_data;
}

std::string send_obd_data()
{
  obd_data = "";
  obd_data += "*O,";
  obd_data += ",";
  add_string_flaot(fuel_pressure());
  add_string_flaot(fuel_tank_level_input());
  add_string_flaot(vehicle_speed());
  add_string_flaot(five_min_average_fuel_efficiency());
  add_string_flaot(engine_rpm());
  add_string_flaot(five_min_average_engine_coolant_temperature());
  add_string_flaot(five_min_average_driver_efficiency());
  add_string_flaot(five_min_average_vehicle_performance());
  add_string_int(freeze_dtc());
  add_string_int(engine_run_time());
  add_string_int(distance_traveled());
  obd_data += "#";
  return obd_data;
}