
/*******************************************************************************

    Description:    read sensors and return Values struct
                    Supported sensors: DHT11 & DTH22,
                    BMB180 & BMP280, BME280, TEMT6000

*******************************************************************************/

/* ------------ Version history ------------------------------------------------
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

------------------------------------------------------------------------------*/
#include <read_sensors.h>

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

    SFE_BMP180 bmp180;
    Wire.begin(i2c_sda, i2c_scl);
    // Initialize the sensor (it is important to get calibration values stored on the device).
    if (bmp180.begin() == 0)
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BMP180 init FAIL!!");
        values.temperature = ERROR_VALUE;
        values.pressure = ERROR_VALUE;
        values.altitude = ERROR_VALUE;
        values.fail_count ++;
        return values;
        }
    bmp180_status = bmp180.startTemperature();
    if(bmp180_status == 0)
        {
        Serial.println("BMP180: startTemperature FAILED!!!");
        values.temperature = ERROR_VALUE;
        values.pressure = ERROR_VALUE;
        values.altitude = ERROR_VALUE;
        values.fail_count ++;
        return values;
        }
    else
        {
        // Wait for the measurement to complete:
        delay(bmp180_status);
        // Retrieve the completed temperature measurement:
        // Note that the measurement is stored in the variable T.
        // Function returns 1 if successful, 0 if failure.
        bmp180_status = bmp180.getTemperature(T);
        if(bmp180_status == 0)
            {
            Serial.println("BMP180: getTemperature FAILED!!!");
            values.temperature = ERROR_VALUE;
            values.pressure = ERROR_VALUE;
            values.altitude = ERROR_VALUE;
            values.fail_count ++;
            return values;
            }
        else
            {
            values.temperature = T;
            #if defined __DEBUG__
            Serial.println("BMP180: getTemperature OK: ");
            Serial.print(values.temperature);
            Serial.print("\n");
            #endif
            }
        }
        bmp180_status = bmp180.startPressure(3);
        if(bmp180_status == 0)
            {
            Serial.println("BMP180: startPressure FAILED!!!");
            values.pressure = ERROR_VALUE;
            values.fail_count ++;
            return values;
            }
        else
            {
            // Wait for the measurement to complete:
            delay(bmp180_status);
            // Retrieve the completed pressure measurement:
            // Note that the measurement is stored in the variable P.
            // Note also that the function requires the previous temperaturmeasurement (T).
            // If temperature is stable, you can do one temperature measurement for number of pressure measurements.
            // Function returns 1 if successful, 0 if failure.
            bmp180_status = bmp180.getPressure(P,T);
            if(bmp180_status == 0)
                {
                Serial.println("BMP180: getPressure FAILED!!!");
                values.pressure = ERROR_VALUE;
                values.fail_count ++;
                return values;
                }
            else
                {
                values.pressure = P;
                #if defined __DEBUG__
                Serial.println("BMP180: getPressure OK: ");
                Serial.print(values.pressure);
                Serial.print("\n");
                #endif
                p0 = bmp180.sealevel(P,ALTITUDE); // we're at ALTIDUDE meters
                values.altitude = bmp180.altitude(P,p0); // Calculated altitude
                #if defined __DEBUG__
                Serial.println("BMP180: Altitude: ");
                Serial.print(values.altitude);
                Serial.print("\n");
                #endif
                }
            }
        return values;
    } //read_bmp180
#endif

#if defined SENSOR_TEMT6000
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


/*
#if defined SENSOR_BMP280
Values ReadSensors::read_bmp280()
    {
    Values values;
    //values.fail_count = 0;

    Adafruit_BMP280 bmp280;

    Wire.begin(i2c_sda, i2c_scl);

    if (!bmp280.begin())
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BMP280 init FAIL!!");
        values.temperature = ERROR_VALUE;
        values.pressure = ERROR_VALUE;//if (values.fail_count > MAX_RETRYCOUNT)
    //{
    // reboot
    //}
        values.altitude = ERROR_VALUE;
        delay (5000);
        }
    else
        {
        values.temperature = bmp280.readTemperature();
        //values.humidity = bmp280.readHumidity();
        values.pressure = bmp280.readPressure();
        //values.altitude = bmp280.readAltitude(1013.25);
        values.altitude = bmp280.readAltitude();
        }
    return values;
    } //read_bmp280
#endif
*/

/*
#if defined SENSOR_BME280
Values ReadSensors::read_bme280()
    {
    char bme280_status;
    double T,P,H,p0;
    Values values;
    //values.fail_count = 0;

    Adafruit_BME280 bme280;

    Wire.begin(i2c_sda, i2c_scl);
    //Wire.setClock(100000);
    //uint8_t BME280_i2caddr = BMP280_ADDRESS;
    uint8_t BME280_i2caddr = BMP280_ADDRESS_ALT;

    bme280_status = bme280.begin(BME280_i2caddr);
    if(bme280_status == 0)
    //if (!bme280.begin(BMP280_ADDRESS_ALT,BMP280_CHIPID))
    //if (bme280.begin() == 0)
        {// continue anyway but show valeus as "-999.99" (ERROR_VALUE)
        Serial.println("BME280 init FAIL!!");
        values.temperature  = ERROR_VALUE;
        values.pressure     = ERROR_VALUE;
        values.altitude     = ERROR_VALUE;
        values.humidity     = ERROR_VALUE;
        }
    else
        {
        values.temperature = bme280.readTemperature();
        Serial.println("BME280: Temperature: ");
        Serial.print(values.temperature);
        Serial.print("\n");

        values.humidity = bme280.readFloatHumidity();
        Serial.println("BME280: Humidity: ");
        Serial.print(values.humidity);
        Serial.print("\n");

        values.pressure = bme280.readFloatPrssure();
        Serial.println("BME280: Pressure: ");
        Serial.print(values.pressure);
        Serial.print("\n");

        //Serial.print(mySensor.readFloatAltitudeFeet(), 1);
        values.altitude = bme280.readFloatAltitudeMeters()();
        Serial.println("BME280: Altitude: ");
        Serial.print(values.altitude);
        Serial.print("\n");

        }
    return values;
    } //read_bme280
#endif
*/
