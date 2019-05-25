
/*******************************************************************************

    Description:    read sensors and return Values struct
                    Supported sensors: DHT11 & DTH22,
                    BMB180 & BMP280, BME280, TEMT6000

*******************************************************************************/

/* ------------ Version history ------------------------------------------------
    Version 1.2     Yasperzee   5'19    Cleaning for Release
    Version 1.1     Yasperzee   5'19    BMP280 support
    Version 1.0     Yasperzee   5'19    TEMT6000 support
    Version 0.9     Yasperzee   5'19    Add some retry if sensor read failed
    Version 0.8     Yasperzee   4'19    #define __DEBUG__
    Version 0.7     Yasperzee   4'19    Add few retrys if DHT sensor read faila
    Version 0.6     Yasperzee   4'19    Functions to classes and to librarys
                                        IDE: Atom.io w/ platform
    Version 0.5     Yasperzee   4'19    DHT sensor publish Temperature only
    Version 0.4     Yasperzee   4'19    add support for DHT11 / DHT22 sensors
    Version 0.3     Yasperzee   4'19    Get info from node
    Version 0.2     Yasperzee   4'19    Added bmp280 support
    Version 0.1     Yasperzee   3'19    Created
                    * Read_bmp180 so temperature, pressure and calculated altitude

#TODO:
------------------------------------------------------------------------------*/
#include <read_sensors.h>
/*
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
*/

#if defined SENSOR_DHT11 or defined SENSOR_DHT22
Values ReadSensors::read_dhtXXX(void)
    {
    double T,H;
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
            delay(500); // #TODO: Light sleep or optimize / remove delay
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

#if defined SENSOR_BMP180
Values ReadSensors::read_bmp180()
    {
    char bmp180_status;
    double T,P,p0;
    Values values;

    Adafruit_BMP085_Unified bmp180 = Adafruit_BMP085_Unified(10085);
    Wire.begin(i2c_sda, i2c_scl);

    if (!bmp180.begin())
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BMP180 init FAIL!!");
        values.temperature = ERROR_VALUE;
        values.pressure = ERROR_VALUE;
        values.altitude = ERROR_VALUE;
        values.fail_count ++;
        return values;
        }
    else
        {
        /* Get a new sensor event */
        sensors_event_t event;
        bmp180.getEvent(&event);
        /* Display some basic information on this sensor */
        #if defined __DEBUG__
        //displaySensorDetails()
        sensor_t sensor;
        bmp180.getSensor(&sensor);
        Serial.println("------------------------------------");
        Serial.print  ("Sensor:       "); Serial.println(sensor.name);
        Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
        Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
        Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
        Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
        Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");
        Serial.println("------------------------------------");
        Serial.println("");
        #endif

    /* Display the results (barometric pressure is measure in hPa) */
        if (!event.pressure)
            {
            values.temperature = ERROR_VALUE;
            values.pressure = ERROR_VALUE;
            values.altitude = ERROR_VALUE;
            Serial.println("Sensor error");
            }
        else
            {
            /* Display atmospheric pressue in hPa */
            values.pressure = event.pressure;
            #if defined __DEBUG__
            Serial.print("Pressure:    ");
            Serial.print(event.pressure);
            Serial.println(" hPa");
            #endif
            /* First we get the current temperature from the BMP085 */
            float temperature;
            bmp180.getTemperature(&temperature);
            values.temperature = temperature;
            #if defined __DEBUG__
            Serial.print("Temperature: ");
            Serial.print(values.temperature );
            Serial.println(" C");
            #endif
            /* Then convert the atmospheric pressure, and SLP to altitude         */
            /* Update this next line with the current SLP for better results      */
            float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
            values.altitude = (bmp180.pressureToAltitude(seaLevelPressure, event.pressure));
            #if defined __DEBUG__
            Serial.print("Altitude:    ");
            Serial.print(values.altitude);
            Serial.println(" m");
            #endif
            pinMode(ALS_PIN, INPUT);
            double reading = analogRead(ALS_PIN); //Read light level
            //double square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
            //square_ratio = pow(square_ratio, 2.0);
            values.als = reading;
            #if defined __DEBUG__
            Serial.print("Ambient Light: ");
            Serial.println(values.als);
            #endif
            }
        }
    } //read_bmp180
#endif

#if defined SENSOR_TEMT6000_ALONE
Values ReadSensors::read_temt6000()
    {
    Values values;
    pinMode(ALS_PIN, INPUT);
    double reading = analogRead(ALS_PIN); //Read light level
    double square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
    square_ratio = pow(square_ratio, 2.0);
    values.als = reading;
    #if defined __DEBUG__
    Serial.print("read_temt6000: ");
    Serial.print(values.als);
    Serial.print("\n");
    #endif
    return values;
    } //read_temt6000
#endif

#if defined SENSOR_BMP280
Values ReadSensors::read_bmp280()
    {
    char bmp280_status;
    Values values;

    Adafruit_BMP280 bmp280;
    Wire.begin(i2c_sda, i2c_scl);
    if (!bmp280.begin())
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BMP280 init FAIL!!");
        values.temperature = ERROR_VALUE;
        values.pressure = ERROR_VALUE;
        values.altitude = ERROR_VALUE;
        values.fail_count ++;
        }
    else
        {
        values.temperature = bmp280.readTemperature();
        values.pressure = bmp280.readPressure();
        //values.altitude = bmp280.readAltitude(1013.25);
        values.altitude = bmp280.readAltitude();
        #if defined __DEBUG__
        Serial.println("BMP280: Temperature OK: ");
        Serial.println(values.temperature);
        Serial.println("BMP280: Pressure OK: ");
        Serial.println(values.pressure);
        Serial.println("BMP280: Altitude OK: ");
        Serial.println(values.altitude);
        #endif

        pinMode(ALS_PIN, INPUT);
        double reading = analogRead(ALS_PIN); //Read light level
        //double square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
        //square_ratio = pow(square_ratio, 2.0);
        values.als = reading;
        #if defined __DEBUG__
        Serial.print("temt6000: ");
        Serial.print(values.als);
        Serial.print("\n");
        #endif
        }
    return values;
    } //read_bmp280
#endif

#if defined SENSOR_BME280
Values ReadSensors::read_bme280()
    {
    char bme280_status;
    double T,P,H,p0;
    Values values;

    Adafruit_BME280 bme280;
    Wire.begin(i2c_sda, i2c_scl);
    //Wire.setClock(100000);
    //uint8_t BME280_i2caddr = BMP280_ADDRESS;
    uint8_t BME280_i2caddr = BMP280_ADDRESS_ALT;

    bme280_status = bme280.begin(BME280_i2caddr);
    if(!bme280_status)
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BME280 init FAIL!!");
        values.temperature  = ERROR_VALUE;
        values.pressure     = ERROR_VALUE;
        values.altitude     = ERROR_VALUE;
        values.humidity     = ERROR_VALUE;
        values.fail_count ++;
        }
    else
        {
        values.temperature = bme280.readTemperature();
        values.humidity = bme280.readHumidity();
        values.pressure = bme280.readPressure();
        //values.altitude = bme280.readAltitude();

        #if defined __DEBUG__
        Serial.println("BME280: Temperature: ");
        Serial.print(values.temperature);
        Serial.print("\n");
        Serial.println("BME280: Humidity: ");
        Serial.print(values.humidity);
        Serial.print("\n");
        Serial.println("BME280: Pressure: ");
        Serial.print(values.pressure);
        Serial.print("\n");
        Serial.println("BME280: Altitude: ");
        Serial.print(values.altitude);
        Serial.print("\n");


        #endif
        }
    return values;
} //read_bme280
#endif
