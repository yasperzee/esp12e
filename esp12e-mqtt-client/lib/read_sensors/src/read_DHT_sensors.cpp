
/*******************************************************************************

    Description:    Read sensors and return Values struct
                    Supported sensors: DHT11 & DTH22,

*******************************************************************************/

/* ------------ Version history ------------------------------------------------
    Version 0.1     Yasperzee   6'19  DHT sensors to separate module

#TODO:
------------------------------------------------------------------------------*/
#include <config.h>
#include <read_DHT_sensors.h>

#if defined(SENSOR_DHT11) || defined(SENSOR_DHT22)
Values ReadSensors::read_dhtXXX(void)
    {
    float T,H;
    Values values;
    int RETRYCOUNT = 5;
    DHT dht(DHT_PIN, DHT_TYPE);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // Read temperature as Celsius (the default)
    do // Retry few times if readTemperature fails
        {
        dht.begin();
        T = dht.readTemperature();
        values.temperature = T;
        // Check if temp read failed and exit early (to try again).
        if (isnan(T))
            {
            Serial.println(F("Failed to read DHT TEMP!"));
            values.temperature = ERROR_VALUE;
            values.humidity = ERROR_VALUE; //let's assume HUMID will fail also if TEMP failed
            delay(500);
            values.fail_count ++;
            }
        else // TEMP read OK
            {
            break; // Go on and read HUMIDITY
            }
        }
    while (--RETRYCOUNT);

        #ifndef DHT_TEMP_ONLY
        H = dht.readHumidity();
        values.humidity = H;
        if (isnan(H))
           { // Even if HUMID sensor "ON", skip also if TEMP ok but HUMID failed
           Serial.println(F("Failed to read DHT HUMID!"));
           values.humidity = ERROR_VALUE;
           }
        #endif

    if (values.fail_count > MAX_RETRYCOUNT)
        {
        Serial.println("Reset..");
        ESP.restart();
        }
    Serial.print("ErrorCount ");
    Serial.println(values.fail_count);
    return values;
    }
#endif
