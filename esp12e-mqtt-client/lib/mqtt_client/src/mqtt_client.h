#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

/*************************** mqtt_client.h *************************

    Description:	Functions for mqtt client

    Hardware:       ESP8266: ESP01, ESP12E-Dev.board, ESP12E Module

***********************************************************/
/*-----------------------------------------------------------
    Version 0,3     Yasperzee   5'19    Cleaning for Release
    Version 0.2     Yasperzee   4'19    Functions to classes and to librarys
                                        IDE: Atom.io w/ platform
    Version 0.1     Yasperzee   3'19    Created
  -----------------------------------------------------------*/

#include <read_sensors.h>

void callback(char* topic, byte* payload, unsigned int length);

class MqttClient {
   public:
      void connect_network();
      int mqtt_connect();
      void mqtt_publish(Values values);

   private:
       char FAIL_COUNT[10];
       char payload[100];
       char topic[150];
       char topicSubscribe[100];
       // Space to store values to send
       char str_sensor[10];
       char MQTT_DEVICE_LABEL[20];
       char MQTT_CLIENT_ID[20];
       char topic_info[150];
};

#define MAX_ALS 20000
#define MAX_VCC 5000

#endif //MQTT_CLIENT_H
