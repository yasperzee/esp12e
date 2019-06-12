#ifndef READ_SENSORS_H
#define READ_SENSORS_H
/*
  ************************** read_sensors.h *************************

  Description:  Definitions to read values from read_sensors conneted to ESP8266 NodeMcu

  ************************************************************/
  /*-----------------------------------------------------------
    Version 0.9     Yasperzee   6'19    DHT sensors to separate module
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
#include <config.h>

// values from sensor
struct Values
    {
    double temperature  = ERROR_VALUE;
    double pressure     = ERROR_VALUE;
    double altitude     = ERROR_VALUE;
    double humidity     = ERROR_VALUE;
    double als          = ERROR_VALUE;
    double vcc_batt     = ERROR_VALUE;
    int fail_count      = 0;
    };

class ReadSensors {
   public:
       #if defined(SENSOR_DHT11) || defined(SENSOR_DHT22)
       Values read_dhtXXX(void);
       #endif
       Values read_bmp180();
       Values read_bmp280();
       Values read_bme280();
       Values read_temt6000();

   private:

};


//#define BMP280_ADDR BMP280_ADDRESS //(0x77) SDO = HIGH
#define BMP280_ADDR BMP280_ADDRESS_ALT  //(0x76) SDO = LOW

// #define ALTITUDE 119.0 // Altitude of Tampere-Pirkkala airport, Finland. In meters
#define ALTITUDE 129.0 // Altitude of Kalkunvuori, Tampere Finland. In meters

#ifdef MCU_ESP01
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
