#include "i2c_scanner.h"

void i2c_scanner()
{
    byte error, address;
    int nDevices;
    bool has_error=false;
    
    
    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of the Write.endTransmisstion 
        // to see if a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            display_println("I2C dev:"+String(address, HEX)+" OK");
            nDevices++;
        }
        else if (error==4)
        {
            display_println("I2C dev:"+String(address, HEX)+" ERR");
            has_error = true;
        }    
        delay(10);
    }
    if (nDevices == 0) {
        display_println("No I2C devices!");
        has_error = true;
    }
    if(has_error) delay(ERROR_DELAY);
}