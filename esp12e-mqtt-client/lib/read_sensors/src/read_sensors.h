#ifndef READ_SENSORS_H
#define READ_SENSORS_H
/*
  ************************** read_sensors.h *************************

  Description:  Definitions to read values from read_sensors conneted to ESP8266 NodeMcu

  ************************************************************/
  /*-----------------------------------------------------------
    Version 0.8     Yasperzee   5'19    Cleaning for Release
    Version 0.7     Yasperzee   5'19    TEMT6000 support
    Version 0.6     Yasperzee   4'19    Add vcc_batt to values
    Version 0.5     Yasperzee   4'19    Add ESP12E
    Version 0.4     Yasperzee   4'19    Functions to classes and to librarys
                                        IDE: Atom.io w/ platform
    Version 0.3     Yasperzee   4'19    Support for DHT11 and DHT22 sensors
    Version 0.2     Yasperzee   4'19    Support for BME280
    Version 0.1     Yasperzee   4'19    Support for BMP280 added, initial support for ESP01
    Version 0.0     Yasperzee   4'19    Created

  -----------------------------------------------------------
*/
#include <Adafruit_BMP085_U.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DHT.h>
#include <config.h>

// values from sensor
struct Values
    {
    double temperature  = ERROR_VALUE;
    double pressure     = ERROR_VALUE;
    double altitude     = ERROR_VALUE;
    double humidity     = ERROR_VALUE;
    double als          = 0;
    int vcc_batt        = 0;
    int fail_count      = 0;
    };

class ReadSensors {
   public:
       Values read_dhtXXX(void);
       Values read_bmp180();
       Values read_bmp280();
       Values read_bme280();
       Values read_temt6000();

   private:

};

// max retry to read sensor before reboot
#define MAX_RETRYCOUNT 50

// #define ALTITUDE 119.0 // Altitude of Tampere-Pirkkala airport, Finland. In meters
#define ALTITUDE 129.0 // Altitude of Kalkunvuori, Tampere Finland. In meters

#define DHT_PIN 2 // ESP01 and ESP12 uses gpio 2 for DHT11 / DHT22 sensor
#if defined MCU_ESP01
    const int i2c_sda = 0; // BMPXXX SDA --> gpio0 on ESP01
    const int i2c_scl = 2; // BMPXXX SCL --> gpio2 on ESP01
#elif defined MCU_ESP12E
    const int i2c_sda = D6; // BMPXXX SDA --> gpio12(D6) on ESP12E
    const int i2c_scl = D7; // BMPXXX SCL --> gpio13(D7) on ESP12E
    #define ALS_PIN A0 // A0(17) as Ambient light sensor pin
#else
    //Serial.println("No NodeMCU selected (3)");
#endif

#endif // READ_SENSORS_H
