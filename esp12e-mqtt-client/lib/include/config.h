#ifndef CONFIG_H
#define CONFIG_H

/*************************** config.h ******************************************

    Description:	Configurations for mqtt client

    Hardware:       ESP8266: ESP01, ESP12E-Dev.board, ESP12E Module

*******************************************************************************/

/*----------------  Version history  -------------------------------------------

    Version 1.6     Yasperzee   5'19    NODE_FEATURE_ seleactable separately
    Version 1.5     Yasperzee   5'19    Cleaning for Release
    Version 1.4     Yasperzee   5'19    Publish Vcc separately
                                        Change SENSOR_FEAT to NODE_FEATURE
    Version 1.3     Yasperzee   5'19    BMP280 support
    Version 1.2     Yasperzee   5'19    TEMT6000 support
    Version 1.1     Yasperzee   5'19    #define SW_VERSION
    Version 1.0     Yasperzee   4'19    #define READ_VCC
    Version 0.9     Yasperzee   4'19    #define __DEBUG__
    Version 0.8     Yasperzee   4'19    Add ESP12E
    Version 0.7     Yasperzee   4'19    DHT sensor publish Temperature only
    Version 0.6     Yasperzee   4'19    Deep sleep support
    Version 0.5     Yasperzee   4'19    Read topic_info from node.
    Version 0.4     Yasperzee   4'19    Add support for DHT11 and DHT22 sensors.
    Version 0.3     Yasperzee   4'19    Added some "automation" to definition creation.
    Version 0.2     Yasperzee   4'19    Read NodeInfo & Protocol from node.
    Version 0.1     Yasperzee   3'19    Created.


------------------------------------------------------------------------------*/

//*********************** Node specific CONGFIGURATIONS ************************

//#define __DEBUG__
//#define DEMO // if __DEBUG__ defined, DEMO gives 10sec public interval
//#define TRACE_
#define TRACE_INFO

// Activate Sleep
    //#define DEEP_SLEEP
    //#define LIGHT_SLEEP

// Select sensor connected
    //#define SENSOR_TEMT6000
    //#define SENSOR_BMP180
    #define SENSOR_BMP280
    //#define SENSOR_BME280
    //#define SENSOR_DHT22
    //#define SENSOR_DHT11

// Select measuremens to publish
    #define NODE_FEATURE_TEMP
    #define NODE_FEATURE_BARO
    #define NODE_FEATURE_ALTI
    //#define NODE_FEATURE_HUMID
    //#define NODE_FEATURE_AMBIENT_LIGHT // ESP12E only Uses same GPIO as READ_VCC!
    //#define NODE_FEATURE_READ_VCC // ESP12E only Uses same GPIO as ALS!

// DHT sensor publish Temperature only
    //#define DHT_TEMP_ONLY

// AppSW version. 1.X for nonos/ESP12E(ESP01) and v2.X for esp-idf/esp32
    #define SW_VERSION "v0.9"

// Select node-mcu in use
    //#define MCU_ESP01
    #define MCU_ESP12E

// Define number of node
    //#define NODE_NUM "01"
    //#define NODE_NUM "02"
    //#define NODE_NUM "03"
    #define NODE_NUM "04"


// Select mosquitto server
    //#define MQTT_SERVER "192.168.10.52" // Local Rpi3 with mosquitto
    //#define MQTT_SERVER "192.168.10.34" // Local W530 with mosquitto
    #define MQTT_SERVER "192.168.10.63" // Local NP-510 with mosquitto

// Select node's Location
    #define LOCATION_NODE "Koti"
    //#define LOCATION_NODE "Demo"

// Uncomment one for room or define your ownone
    //#define TOPIC_ROOM "Olohuone"  //NODE-01
    //#define TOPIC_ROOM "Ulkoilma"   //NODE-02
    #define TOPIC_ROOM "Parveke"  //NODE-03
    //#define TOPIC_ROOM "Partsi"  //NODE-04

    //#define TOPIC_ROOM "Portable"
    //#define TOPIC_ROOM "mh-1"
    //#define TOPIC_ROOM "mh-2"
    //#define TOPIC_ROOM "MH-3"
    //#define TOPIC_ROOM "Eteinen"
    //#define TOPIC_ROOM "Keittio"
    //#define TOPIC_ROOM "kph1"
    //#define TOPIC_ROOM "kph2"
    //#define TOPIC_ROOM "Liikkuva"
    //#define TOPIC_ROOM "Jaakaappi"
    //#define TOPIC_ROOM "Pakastin"

// Select port
    // TCP/IP port 1883 is reserved with IANA for use with MQTT.
    // TCP/IP port 8883 is also registered, for using MQTT over SSL.
    //#define PORT    8883
    #define PORT    1883

// Baudrate for serial port
    #define BAUDRATE 115200

// Select Language
    #define LANGUAGE_FI
    //#define LANGUAGE_EN

//***** Generated definitions **************************************************

#if defined MCU_ESP01
    #define MCU_ID "ESP01-" SW_VERSION
#elif defined MCU_ESP12E
    #define MCU_ID "ESP12E-" SW_VERSION
#else
    #define MCU_ID "Unknown"
#endif

#if defined SENSOR_DHT11
    #define SENSOR_STR "DHT11"
    #define DHT_TYPE DHT11
#elif defined SENSOR_DHT22
    #define SENSOR_STR "DHT22"
    #define DHT_TYPE DHT22

#elif defined SENSOR_BMP180
    #define SENSOR_STR "BMP180"
    #if defined NODE_FEATURE_AMBIENT_LIGHT
        #define SENSOR_STR "BMP180+ALS"
    #endif

#elif defined SENSOR_BMP280
    #define SENSOR_STR "BMP280"
#elif defined SENSOR_BME280
    #define SENSOR_STR "BME280"
#elif defined SENSOR_TEMT6000
    #define SENSOR_STR "TEMT6000"
#else
    #define SENSOR_STR "Unknown"
#endif

#ifndef LOCATION_NODE
    #define LOCATION_NODE "Unknown"
#endif

#ifndef TOPIC_ROOM
    #define TOPIC_ROOM "Unknown"
#endif

#ifndef NODE_NUM
    #define NODE_NUM "00"
#endif

const int second = 1000;
#ifdef  __DEBUG__
    #if defined DEMO
        const int PUBLISH_INTERVAL  = 10*second; //5*60*second;// 5 min
    #else
        const int PUBLISH_INTERVAL  = 38*second; // 38 second interval --> 1h cycle
    #endif
    const int RECONNECT_DELAY   = 10*second; // Try to reconnect mqtt server
#else // Release
    const int PUBLISH_INTERVAL  = 5*60*second; // 15*60 seconds interval to publish values --> 24h cycle
    const int RECONNECT_DELAY   = 30*second; // Try to reconnect mqtt server
#endif

// mqtt: topic level definitions
#ifdef LANGUAGE_FI
    #define TOPIC_LOCATION  LOCATION_NODE
    #define TOPIC_TEMP      "Lampotila"
    #define TOPIC_HUMID     "Ilmankosteus"
    #define TOPIC_BARO      "Ilmanpaine"
    #define TOPIC_ALTIT     "Korkeus"
    #define TOPIC_ALS       "Valoisuus"
    #define TOPIC_VCC       "Vcc"
#else
    #define TOPIC_LOCATION  LOCATION_NODE
    #define TOPIC_TEMP      "Temperature"
    #define TOPIC_HUMID     "Humidity"
    #define TOPIC_BARO      "Barometer"
    #define TOPIC_ALTIT     "Altitude"
    #define TOPIC_ALS       "AmbientLight"
    #define TOPIC_VCC       "Vcc"
#endif

    #define TOPIC_NODEINFO  "NodeInfo"
    #define TOPIC_TOPICINFO "TopicInfo"

    #define TOPIC_WILDCARD_SINGLE "+"
    #define TOPIC_WILDCARD_MULTI  "*"

    #define NODE_ID "NODE-" NODE_NUM

    #define ERROR_VALUE -999.9

#endif // CONFIG_H
