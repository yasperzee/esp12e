/*******************************************************************************

    Description:    MqttClient - class

*******************************************************************************/

/*----------------- Version history --------------------------------------------

    Version 0.7     Yasperzee   5'19    TEMT6000 support
    Version 0.6     Yasperzee   4'19    publish vcc_batt
    Version 0.5     Yasperzee   4'19    #define __DEBUG__
    Version 0.4     Yasperzee   4'19    Functions to classes and to librarys
                                        IDE: Atom.io w/ platform
    Version 0.3     Yasperzee   4'19    Reads TopicInfo from node
    Version 0.2     Yasperzee   4'19    Reads NodeInfo from node
    Version 0.1     Yasperzee   3'19    Created

------------------------------------------------------------------------------*/
using namespace std;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ssid.h>  // SSID and PASS strings for local Wlan-network
#include <mqtt_client.h>
#include <config.h>

WiFiClient mqtt_client;
PubSubClient client(mqtt_client);

void MqttClient::connect_network()
    {
    // Connect to WiFi network with SSID and password
    WiFi.begin(ssid, password);
    #if defined __DEBUG__
    Serial.println("Connecting to ");
    Serial.println(ssid);
    #endif
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED)
        {
        delay(500); // #TODO: Light sleep
        Serial.print(".");
        }
    Serial.println("");
    Serial.println("WiFi connected.");
    #if defined __DEBUG__
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    #endif
    }

int MqttClient::mqtt_connect()
    {
    client.setServer(MQTT_SERVER, PORT);
    client.setCallback(callback);
    sprintf(MQTT_CLIENT_ID, "%s", MCU_ID);

    if (!client.connected())
        {
        if (client.connect(MQTT_CLIENT_ID))
            {
            #if defined __DEBUG__
            Serial.print("\n");
            Serial.println("MQTT connected.");
            //Serial.print(topicSubscribe);
            //Serial.print("\n");
            #endif
            }
        else
            {
            int reconn = 0;
            while (reconn < 5)
                {
                #if defined __DEBUG__
                Serial.print("\n");
                Serial.print("MQTT re-connecting ");
                Serial.print(MQTT_SERVER);
                #endif
                delay(RECONNECT_DELAY); // #TODO: Light sleep
                reconn++;
                if (client.connect(MQTT_CLIENT_ID))
                    {
                    #if defined __DEBUG__
                    Serial.print("\n");
                    Serial.println("MQTT re-connected.");
                    //Serial.print(topicSubscribe);
                    //Serial.print("\n");
                    #endif
                    break;
                    }
                }
            }
        }
    else
        {
        #if defined __DEBUG__
        Serial.println("");
        Serial.println("MQTT already connected.");
        #endif
        }
    return(client.state());
    } // mqtt_connect

void MqttClient::mqtt_publish(Values values)
    {
    // ************ publish NodeInfo **********************
    sprintf(FAIL_COUNT, "%s", ""); // Clean
    itoa(values.fail_count, FAIL_COUNT, 10);

    sprintf(VCC_BATT, "%s", ""); // Clean
    itoa(values.vcc_batt, VCC_BATT, 10);

    // BEST PRACTICE: Do not use leading '/'
    sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_NODEINFO);
    sprintf(MQTT_DEVICE_LABEL, "%s/%s/%s/%s/%s", MCU_ID, SENSOR_STR, NODE_ID, FAIL_COUNT, VCC_BATT);
    sprintf(payload, "{\"NodeInfo\": %s}", MQTT_DEVICE_LABEL); // Adds the value
    client.publish(topic, payload);
    #if defined __DEBUG__
    Serial.println("Publishing Nodeinfo to local mosquitto server");
    Serial.println(payload);
    #endif

    // ************ publish TopicInfo **********************
    sprintf(payload, "%s", ""); // Cleans the payload
    // BEST PRACTICE: Do not use leading '/'
    sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_TOPICINFO );
    sprintf(topic_info, "%s/%s", TOPIC_LOCATION, TOPIC_ROOM );
    sprintf(payload, "{\"TopicInfo\": %s}", topic_info); // Adds the value
    client.publish(topic, payload);
    #if defined __DEBUG__
    Serial.println("Publishing topic_info to local mosquitto server");
    Serial.println(payload);
    #endif

#if defined SENSOR_FEAT_TEMP
    // ************ publish Temperature **********************
    double temperature = values.temperature;
    if (temperature == ERROR_VALUE)
        {
        Serial.print("temperature == ERROR_VALUE \n");
        }
    else
        {
        dtostrf(temperature, 4, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_TEMP );
        sprintf(payload, "{\"Lampotila\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        #if defined __DEBUG__
        Serial.println("");
        Serial.println("Publishing Temperature to local mosquitto server");
        #endif
        }
#endif

#if defined SENSOR_FEAT_BARO
    // ************ publish Barometer **********************
    double barometer = values.pressure;
    if (barometer == ERROR_VALUE)
        {
        Serial.print("Barometer == ERROR_VALUE");
        Serial.println("");
        }
    else
        {
        //Serial.print("Barometer: ");
        //Serial.print(barometer);
        //Serial.println(" mbar");
        dtostrf(barometer, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_BARO );
        //sprintf(payload, "{\"Barometer\": %s}", str_sensor); // Adds the value
        sprintf(payload, "{\"Ilmanpaine\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        #if defined __DEBUG__
        Serial.println("Publishing Barometer to local mosquitto server");
        //Serial.println("");
        #endif
        }
#endif

#if defined SENSOR_FEAT_ALTI
    // ************ publish Altitude **********************
    double altitude = values.altitude;
    if (altitude == ERROR_VALUE)
        {
        Serial.print("Altitude == ERROR_VALUE");
        Serial.println("");
        }
    else
        {
        //Serial.print("Altitude: ");
        //Serial.print(altitude);
        //Serial.println(" meters");
        dtostrf(altitude, 7, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_ALTIT );
        sprintf(payload, "{\"Korkeus\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        #if defined __DEBUG__
        Serial.println("Publishing Altitude to local mosquitto server");
        //Serial.println("");
        #endif
        }
#endif

#if defined SENSOR_FEAT_HUMID
    // ************ publish Humidity **********************
    double humidity = values.humidity;
    if (humidity == ERROR_VALUE)
        {
        Serial.print("Humidity == ERROR_VALUE");
        }
    else
        {
        //Serial.print("Humidity: ");
        //Serial.print(humidity);
        //Serial.println(" %");
        dtostrf(humidity, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_HUMID );
        sprintf(payload, "{\"Ilmankosteus\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        #if defined __DEBUG__
        Serial.println("Publishing Humidity to local mosquitto server");
        //Serial.println("");
        #endif
        }
#endif

#if defined SENSOR_FEAT_AMBIENT_LIGHT
    // ************ publish AmbientLight **********************
    double als = values.als;
        dtostrf(als, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_ALS );
        sprintf(payload, "{\"Valoisuus\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        Serial.print(topic);
        Serial.println(payload);
        #if defined __DEBUG__
        Serial.println("Publishing AmbientLight to local mosquitto server");
        //Serial.println("");
        #endif

    #endif
    } // publish


void callback(char* topic, byte* payload, unsigned int length)
    {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = 0;
    String message(p);
    Serial.print("callback: ");
    Serial.write(payload, length);
    Serial.println();
    } //callback

/*
void MqttClient::mqtt_subscribe(char topicSubscribe[])
    {
    Serial.print("mqtt_subdsribe: ");
    Serial.print(topicSubscribe);
    Serial.print("\n");
    // BEST PRACTICE: Do not use leading '/'
    //sprintf(topicSubscribe, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_WILDCARD_MULTI );
    //client.subscribe(topicSubscribe);
    // do something with response --> callback
    }
*/
