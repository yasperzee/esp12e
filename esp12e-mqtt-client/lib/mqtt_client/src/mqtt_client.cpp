/*******************************************************************************

    Description:    MqttClient - class

*******************************************************************************/

/*----------------- Version history --------------------------------------------
    Version 1.0     Yasperzee   5'19    Explicitly set the ESP8266 to be a WiFi-client.
    Version 0.9     Yasperzee   5'19    Cleaning for Release
    Version 0.8     Yasperzee   5'19    Change SENSOR_FEAT to NODE_FEATURE
    Version 0.7     Yasperzee   5'19    TEMT6000 support
    Version 0.6     Yasperzee   4'19    publish vcc_batt
    Version 0.5     Yasperzee   4'19    #define __DEBUG__
    Version 0.4     Yasperzee   4'19    Functions to classes and to librarys
                                        IDE: Atom.io w/ platform
    Version 0.3     Yasperzee   4'19    Reads TopicInfo from node
    Version 0.2     Yasperzee   4'19    Reads NodeInfo from node
    Version 0.1     Yasperzee   3'19    Created

# TODO: Finetune wifi power consuption
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
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    #if defined TRACE_INFO
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
            #if defined TRACE_INFO
            Serial.print("\n");
            Serial.println("MQTT connected.");
            //Serial.print(topicSubscribe);
            #endif
            }
        else
            {
            int reconn = 0;
            while (reconn < 5)
                {
                #if defined TRACE_INFO
                Serial.print("\n");
                Serial.print("MQTT re-connecting ");
                Serial.print(MQTT_SERVER);
                #endif
                delay(RECONNECT_DELAY); // #TODO: Light sleep
                reconn++;
                if (client.connect(MQTT_CLIENT_ID))
                    {
                    #if defined TRACE_INFO
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
        #if defined TRACE_INFO
        Serial.println("");
        Serial.println("MQTT already connected.");
        #endif
        }
    return(client.state());
    } // mqtt_connect

void MqttClient::mqtt_publish(Values values)
    {
    // ************ publish NodeInfo **********************
    #if defined TRACE_INFO
    Serial.println("Entering mqtt_publish");
    #endif
    sprintf(FAIL_COUNT, "%s", ""); // Clean
    itoa(values.fail_count, FAIL_COUNT, 10);

    // BEST PRACTICE: Do not use leading '/'
    sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_NODEINFO);
    sprintf(MQTT_DEVICE_LABEL, "%s/%s/%s/%s/%s", MCU_ID, SENSOR_STR, NODE_ID, FAIL_COUNT );
    sprintf(payload, "{\"NodeInfo\": %s}", MQTT_DEVICE_LABEL);
    client.publish(topic, payload);
    #if defined __DEBUG__
    Serial.println("Publishing Nodeinfo.");
    //Serial.println(payload);
    #endif

    // ************ publish TopicInfo **********************
    sprintf(payload, "%s", ""); // Cleans the payload
    // BEST PRACTICE: Do not use leading '/'
    sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_TOPICINFO );
    sprintf(topic_info, "%s/%s", TOPIC_LOCATION, TOPIC_ROOM );
    sprintf(payload, "{\"TopicInfo\": %s}", topic_info);
    client.publish(topic, payload);
    #if defined __DEBUG__
    Serial.println("Publishing topic_info.");
    //Serial.println(payload);
    #endif

#if defined NODE_FEATURE_TEMP
    // ************ publish Temperature **********************
    float temperature = values.temperature;
    if (temperature == ERROR_VALUE)
        {
        Serial.print("Temperature == ERROR_VALUE! \n");
        }
    else
        {
        dtostrf(temperature, 4, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_TEMP );
        sprintf(payload, "{\"Lampotila\": %s}", str_sensor);
        client.publish(topic, payload);
        //#if defined __DEBUG__
        Serial.print("Publishing Temperature:  ");
        Serial.println(str_sensor);
        //endif

        }
#endif

#if defined NODE_FEATURE_BARO
    // ************ publish Barometer **********************
    float barometer = values.pressure;
    if (barometer == ERROR_VALUE)
        {
        Serial.print("Barometer == ERROR_VALUE!");
        Serial.println("");
        }
    else
        {
        dtostrf(barometer, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_BARO );
        sprintf(payload, "{\"Ilmanpaine\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        //#if defined __DEBUG__
        Serial.print("Publishing Barometer:  ");
        Serial.println(str_sensor);
        //#endif
        }
#endif

#if defined NODE_FEATURE_ALTI
    // ************ publish Altitude **********************
    float altitude = values.altitude;
    if (altitude == ERROR_VALUE)
        {
        Serial.print("Altitude == ERROR_VALUE!");
        Serial.println("");
        }
    else
        {
        dtostrf(altitude, 7, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_ALTIT );
        sprintf(payload, "{\"Korkeus\": %s}", str_sensor);
        client.publish(topic, payload);
        //#if defined __DEBUG__
        Serial.print("Publishing Altitude : ");
        Serial.println(str_sensor);
        //#endif
        }
#endif

#if defined NODE_FEATURE_HUMID
    // ************ publish Humidity **********************
    float humidity = values.humidity;
    if (humidity == ERROR_VALUE)
        {
        Serial.print("Humidity == ERROR_VALUE!");
        }
    else
        {
        dtostrf(humidity, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_HUMID );
        sprintf(payload, "{\"Ilmankosteus\": %s}", str_sensor); // Adds the value
        client.publish(topic, payload);
        //#if defined __DEBUG__
        Serial.print("Publishing Humidity: ");
        Serial.println(str_sensor);
        //#endif
        }
#endif

#if defined NODE_FEATURE_AMBIENT_LIGHT
    // ************ publish AmbientLight **********************
    float als = values.als;
        dtostrf(als, 6, 1, str_sensor);
        sprintf(payload, "%s", ""); // Cleans the payload
        // BEST PRACTICE: Do not use leading '/'
        sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_ALS );
        sprintf(payload, "{\"Valoisuus\": %s}", str_sensor);
        client.publish(topic, payload);
        //#if defined __DEBUG__
        Serial.print("Publishing AmbientLight:");
        Serial.println(str_sensor);
        //#endif
    #endif

    #if defined NODE_FEATURE_READ_VCC
        // ************ publish Vcc **********************M
            itoa(values.vcc_batt, str_sensor, 10);
            sprintf(payload, "%s", ""); // Cleans the payload
            // BEST PRACTICE: Do not use leading '/'
            sprintf(topic, "%s/%s/%s", TOPIC_LOCATION, TOPIC_ROOM, TOPIC_VCC );
            sprintf(payload, "{\"Vcc\": %s}", str_sensor);
            client.publish(topic, payload);
            //#if defined __DEBUG__
            Serial.print("Publishing Vcc: ");
            Serial.println(str_sensor);
            //#endif
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
