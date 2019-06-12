#ifndef READ_DHT_SENSORS_H
#define READ_DHT_SENSORS_H
/*
  ************************** read_sensors.h *************************

  Description:  Definitions to read values from read_sensors conneted to ESP8266 NodeMcu

  ************************************************************/
  /*-----------------------------------------------------------
    Version 0.1     Yasperzee   6'19  DHT sensors to separate module

  -----------------------------------------------------------
*/

#include <read_sensors.h>
#include <Wire.h>
#include <DHT.h>

// max retry to read sensor before reboot
#define MAX_RETRYCOUNT 50

#define DHT_PIN 2 // ESP01 and ESP12 uses gpio 2 for DHT11 / DHT22 sensor

#endif // READ_DHT_SENSORS_H
