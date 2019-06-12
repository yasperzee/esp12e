
/*******************************************************************************

    Description:    read sensors and return Values struct
                    Supported sensors: DHT11 & DTH22,
                    BMB180 & BMP280, BME280, TEMT6000

*******************************************************************************/

/* ------------ Version history ------------------------------------------------
    Version 1.4     Yasperzee   6'19    DHT sensors to separate module
    Version 1.3     Yasperzee   5'19    BMP280 Pressure from Pa to mBar(hPa)
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

#ifdef SENSOR_BMP180
void displaySensorDetails(Adafruit_BMP085_Unified bmp180)
    {
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
    }
#endif

#ifdef SENSOR_BMP180
Values ReadSensors::read_bmp180()
    {
    char bmp180_status;
    float T,P,p0;
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
        #ifdef TRACE_INFO
        displaySensorDetails(Adafruit_BMP085_Unified bmp180);
        #endif

        /* Display the results (barometric pressure is measure in hPa(mBar)) */
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
            //Serial.print("values.pressure: ");
            //Serial.println(values.pressure);
            /* First we get the current temperature from the BMP085 */
            float temperature;
            bmp180.getTemperature(&temperature);
            values.temperature = temperature;
            /* Then convert the atmospheric pressure, and SLP to altitude         */
            /* Update this next line with the current SLP for better results      */
            //float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
            //values.altitude = (bmp180.pressureToAltitude(ALTITUDE, event.pressure));
            values.altitude = (bmp180.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, event.pressure));

            //Serial.print("values.altitude: ");
            //Serial.println(values.altitude);
            #ifdef NODE_FEATURE_AMBIENT_LIGHT
                pinMode(ALS_PIN, INPUT);
                float reading = analogRead(ALS_PIN); //Read light level
                //float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
                //square_ratio = pow(square_ratio, 2.0);
                values.als = reading;
            #endif
            }
        }
        return values;
    } //read_bmp180
#endif

#ifdef SENSOR_BMP280
Values ReadSensors::read_bmp280()
    {
    char bmp280_status;
    Values values;

    Adafruit_BMP280 bmp280;

    Wire.begin(i2c_sda, i2c_scl);

    if (!bmp280.begin(BMP280_ADDR, BMP280_CHIPID))
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
        values.pressure = (bmp280.readPressure()/100); // mBar
        values.altitude = bmp280.readAltitude(1013.25);
        //values.altitude = bmp280.readAltitude();
        #ifdef TRACE_INFO
        Serial.print("BMP280: Temperature: ");
        Serial.println(values.temperature);
        Serial.print("BMP280: Pressure   : ");
        Serial.println(values.pressure);
        Serial.print("BMP280: Altitude   : ");
        Serial.println(values.altitude);
        #endif
        #ifdef NODE_FEATURE_AMBIENT_LIGHT
            pinMode(ALS_PIN, INPUT);
            values.als = analogRead(ALS_PIN); //Read light levelM
            //float reading = analogRead(ALS_PIN); //Read light level
            //float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
            //square_ratio = pow(square_ratio, 2.0);
            //values.als = reading;
        #endif
        #ifdef TRACE_INFO
        Serial.print("temt6000: ");
        Serial.print(values.als);
        Serial.print("\n");
        #endif
        }
    return values;
    } //read_bmp280
#endif

#ifdef SENSOR_BME280
Values ReadSensors::read_bme280()
    {
    char bme280_status;
    float T,P,H,p0;
    Values values;

    //     #define BME280_ADDRESS                (0x77)

    Adafruit_BME280 bme280;
    Wire.begin(i2c_sda, i2c_scl);
    //Wire.setClock(100000);
    //uint8_t BME280_i2caddr = BMP280_ADDRESS;
    //uint8_t BME280_i2caddr = BMP280_ADDRESS_ALT;

    if(!bme280.begin(BME280_ADDRESS))
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
        #ifdef TRACE_INFO
        Serial.print("BME280: Temperature: ");
        Serial.println(values.temperature);
        Serial.print("BME280: Humidity: ");
        Serial.println(values.humidity);
        Serial.print("BME280: Pressure: ");
        Serial.println(values.pressure);
        Serial.print("BME280: Altitude: ");
        Serial.println(values.altitude);
        #endif

        #ifdef NODE_FEATURE_AMBIENT_LIGHT
            pinMode(ALS_PIN, INPUT);
            float reading = analogRead(ALS_PIN); //Read light level
            //float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
            //square_ratio = pow(square_ratio, 2.0);
            values.als = reading;
        #endif
        }
    return values;
} //read_bme280
#endif

#ifdef SENSOR_TEMT6000_ALONE
Values ReadSensors::read_temt6000()
    {
    Values values;
    pinMode(ALS_PIN, INPUT);
    float reading = analogRead(ALS_PIN); //Read light level
    float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
    square_ratio = pow(square_ratio, 2.0);
    values.als = reading;
    return values;
    } //read_temt6000
#endif
