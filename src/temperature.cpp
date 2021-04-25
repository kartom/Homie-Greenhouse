#include "temperature.h"
#include "display.h"
#include <Esp.h>

int sensor_count = 0;
DeviceAddress device_address[MAX_TEMPSENSORS];
double last_min = VALID_MAX; 
double last_value[MAX_TEMPSENSORS];

OneWire ds2482;
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&ds2482);

HomieNode tempNode("temperature", "temperature","Air temperature");

double get_min_temp(){
    return last_min;
}

double get_temp(uint8_t num) 
{
    return last_value[num];
}

void temperature_setup() {
    bool has_error = false;

    sensors.setWaitForConversion(false);

    // Initialize 1-wire ////////////////////////////////
    if( !ds2482.checkPresence() ) {
        display_println("DS2482: ERROR");
        delay(ERROR_DELAY);
        return;
    }
    
    ds2482.deviceReset();

    uint8_t config = ds2482.readConfig() | DS2482_CONFIG_APU;
    ds2482.writeConfig(config);
    if( ds2482.readConfig() != config ) {
        display_println("DS2482: CFG ERR");
        has_error = true;
    }

    delay(500);
    sensors.begin();

    if ( !has_error ) {
        display_println("DS2482: OK");
    }

    if( !ds2482.wireReset() ) {
        display_println("SENSOR ERROR");
        has_error=true;
    }
    
    sensor_count = sensors.getDeviceCount();
    if(sensor_count > MAX_TEMPSENSORS) sensor_count = MAX_TEMPSENSORS;
    display_println("Sensors: "+String(sensor_count));
    if(sensor_count ==0 ) has_error = true;
    sensors.requestTemperatures();
    delay(1000); //Give time for conversion
    for(int i=0; i<sensor_count; i++) {
        if( sensors.getAddress(device_address[i],i) )
        {
            double temp = sensors.getTempC((uint8_t*) device_address[i]);
            last_value[i] = temp;
            String str = "  T"+String(i)+"="+String(temp,1);
            if( (temp > VALID_MAX)  || ( temp< VALID_MIN) ) {
                has_error = true;
                str += " ERR";
            } 
            else if (temp < last_min) {
                last_min = temp;
            }
            display_println(str);
            //Print address
            str="  Address:";
            for (uint8_t j=0; j<8; j++) str += String(device_address[i][j],HEX)+" ";
            Serial.println(str);
        }
    }
    sensors.requestTemperatures();

    if( has_error ) delay(ERROR_DELAY);

    tempNode.advertise("value").setName("Temperature in greenhouse")
                              .setUnit("°C")
                              .setDatatype("float");
    for( int i=0; i<sensor_count; i++) {
        tempNode.advertise(String(i).c_str()).setName(("Temperature sensor "+String(i)).c_str())
                                                      .setUnit("°C")
                                                      .setDatatype("float");
    }
}

double read_temp() {
    float min_temp = VALID_MAX; //Start at maximum valid sensor value
    for (int i=0; i<sensor_count; i++)
    {
        float temp = sensors.getTempC((uint8_t*) device_address[i]);
        tempNode.setProperty(String(i)).send(String(temp));
        if( temp < min_temp ) min_temp = temp;
        last_value[i] = temp;
    }
    //Filter out error in temp readings.
    if( min_temp > VALID_MAX || min_temp < VALID_MIN ) {
        min_temp = last_min;
    }
    last_min = min_temp;
    tempNode.setProperty("value").send(String(min_temp));
    if( sensor_count > 0 ) sensors.requestTemperatures();
    return last_min;
}
