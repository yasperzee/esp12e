/*******************************************************************************

    Description:    mqtt client/publisher for nodemcu esp8266.
                    Reads temperature & pressure from bmp180/bmp280 sensor
                    and publish values to mqtt server.


    IDE & tools:    - Ubuntu 18.10, Atom.io -IDE with platformio package

    Components:     - ESP01 or ESP12 node mcu
                    - BMP180, BMP280, BME280, DHT11, DTH22 or TEMT6000 sensor

    The circuit:    - ESP12 with   BMP180, BMP280 or BME280 sensor:
                                    i2c:    SDA -> gpio4, SCL -> gpio5
                                    BMP280 and BME280:
                                    - pin 6(SDO) to GND sets I2C address to 0X76
                                    - pin 6(SDO) to Vcc sets I2C address to 0X77

                    - ESP01 with   BMP180, BMP280 or BME280 sensor:
                                    i2c:    SDA -> gpio 0, SCL -> gpio2
                    - ESP01/ESP12 --> DHTxx Sensor: gpio2 --> DHTxx data
                    - ESP12E with   TEMT6000 sensor:
                                    TEMT6000 Signal --> ESP12E A0

    Librarys:       https://github.com/adafruit/Adafruit_Sensor
                    https://github.com/adafruit/Adafruit_BME280_Library
                    https://github.com/adafruit/Adafruit_BMP280_Library
                    https://github.com/adafruit/Adafruit_BMP085_Unified
            REMOVE use adafruit lib instead https://github.com/sparkfun/BMP180_Breakout
                    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
                    https://github.com/knolleary/pubsubclient
                    https://github.com/adafruit/DHT-sensor-library

    References:   -

*******************************************************************************/

/* -------------------- Version history ----------------------------------------

# TODO: Support for BME280 sensor w/ temperature, humidity & pressure.
        - git clone https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
# TODO: use Adafruit_BMP085_Unified lib instead of SparkFun one for BMP180

    Version 1.4    Yasperzee   5'19     Cleaning for Release
    Version 1.3    Yasperzee   5'19     TEMT6000 support
    Version 1.2    Yasperzee   4'19     ESP.getVcc

    Version 1.1    Yasperzee   4'19     Functions to classes and to librarys
                                        IDE: Atom.io w/ platform

    Version 1.0    Yasperzee   4'19     DEEP_SLEEP: Some workaround for DHT22 slow wakeup

    Version 0.9    Yasperzee   4'19     Deep sleep support

    Version 0.8    Yasperzee   4'19     Add support for DHT11 and DHT22 sensors

    Version 0.7    Yasperzee   4'19     Support for ESP01 NodeMcu

    Version 0.6    Yasperzee   4'19     Support for BMP280 sensor

    Version 0.5    Yasperzee   3'19     #TODO: Read SENSOR and NODEMCU from node for SHEET_NAME
                mqtt client related functions to separate .cpp file.

    Version 0.4    Yasperzee   3'/ temperature, humidity & pressure.
        - git clone https://github.com/sparkfun/SparkFun_BME280_Arduino_Library

    V0.8    Yasperzee   4'19    add support for DHT11 and DHT22 sensors

    V0.7    Yasperzee   4'19    Support for ESP01 NodeMcu

    V0.6    Yasperzee   4'19    Support for BMP280 sensor

    V0.5    Yasperzee   3'19
                mqtt client related functions to separate .cpp file.

    V0.4    Yasperzee   3'19
                read_bmp180 -function to separate .cpp file.
                publish calculated altitude

    V0.3    Yasperzee   3'19
                nodemcu-weather-mqtt.ino --> C++ & Atom platformer project
                so no arduino-ide in use anymore

    V0.2    Yasperzee   3'19
                topic levels defined
                koti/oh/temperature
                koti/oh/barometer

    0.1     Yasperzee   2'19
                mqtt client functionality tested.

------------------------------------------------------------------------------*/
// includes
#include <config.h>
#include <read_sensors.h>
#include <mqtt_client.h>

MqttClient mqttClient;
ReadSensors readSensors;
Values values;

#if defined NODE_FEATURE_READ_VCC
ADC_MODE(ADC_VCC);
#endif

void setup()
    {
    Serial.begin(BAUDRATE);

#if defined __DEBUG__
    Serial.print("\nChipID ");
    Serial.println(ESP.getChipId());
    Serial.print("CoreVersion ");
    Serial.println(ESP.getCoreVersion());
    Serial.print("FlashChipRealSize ");
    Serial.println(ESP.getFlashChipRealSize());
    Serial.print("FlashChipSize ");
    Serial.println(ESP.getFlashChipSize());
    #if defined NODE_FEATURE_READ_VCC
    Serial.print("Vcc ");
    Serial.println(ESP.getVcc());
    #endif
#endif
    mqttClient.connect_network();
    }

void loop()
    {
    if(mqttClient.mqtt_connect())
        {
        Serial.println("mqtt_connect FAILED!");
        mqttClient.connect_network();
        }

    #if defined SENSOR_DHT11 or defined SENSOR_DHT22
        values = readSensors.read_dhtXXX();
    #elif defined(SENSOR_BMP180)
        values = readSensors.read_bmp180();
    #elif defined(SENSOR_BMP280)
        values = readSensors.read_bmp280();
    #elif defined(SENSOR_BME280)
        values = readSensors.read_bme280();
    #elif defined(SENSOR_TEMT6000_ALONE)
        values = readSensors.read_temt6000();
    #else
        Serial.println("No Sensor selected!");
        values.temperature = ERROR_VALUE;
        values.humidity = ERROR_VALUE;
        values.pressure = ERROR_VALUE;
        values.altitude = ERROR_VALUE;
        values.als      = ERROR_VALUE;
    #endif

    #if defined NODE_FEATURE_READ_VCC
    values.vcc_batt = ESP.getVcc();
    #endif

    // #TODO: publish only if valid sensor values.
    // We always read atleast temperature, so if temp == ERROR_VALUE, do not publish
    if(values.temperature != ERROR_VALUE)
    {
    mqttClient.mqtt_publish(values);
    //client.loop();
    }
    // delay to next publish
    #if defined(DEEP_SLEEP) // #TODO: some semafore here
        // ToDo: better solution required!
        delay(1000); // give mqtt_publish() time to publish all
        //Serial.println("DeepSleep...");
        ESP.deepSleep(PUBLISH_INTERVAL * 1000); // microseconds
    #elif defined(LIGHT_SLEEP)
        //Serial.println("LightSleep...");
        // light_sleep(PUBLISH_INTERVAL); // milliseconds
    #else
        //Serial.println("Delay...");
        delay(PUBLISH_INTERVAL); // milliseconds
    #endif
    }
