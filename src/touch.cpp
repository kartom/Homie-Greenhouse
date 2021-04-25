#include "touch.h"
#include "display.h"

bool has_mpr121 = false;
Adafruit_MPR121 cap = Adafruit_MPR121();
// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
float current=30.0;

unsigned long touch_millis = millis();



void touch_setup() 
{
    // Default address is 0x5A, if tied to 3.3V its 0x5B
    // If tied to SDA its 0x5C and if SCL then 0x5D
    if (!cap.begin(0x5A)) {
        display_println("MPR121 ERROR");
        delay(ERROR_DELAY);
    }
    else
    {
        has_mpr121 = true;
        cap.setThresholds(20, 10);
        cap.writeRegister(MPR121_AFE1_REG, MPR121_FFI_34 | (40 & MPR121_CDC_MASK)); // 34 means + 40ua
        cap.writeRegister(MPR121_AFE2_REG, MPR121_CDT_0_5us | MPR121_SFI_10 | MPR121_ESI_8ms); // 0.5uS encoding, Second filter 18samples, 1ms sample interval 

        cap.writeRegister(0x5E,0xBF); 

        cap.writeRegister(0x68,60); //60uA for pin #9
        /*
        reg=cap.readRegister8(0x59);
        Serial.print("ThrT: 0x");
        Serial.println(reg,HEX);
        reg=cap.readRegister8(0x5A);
        Serial.print("ThrR: 0x");
        Serial.println(reg,HEX);

        // prox sensing 
        cap.writeRegister(MPR121_PROX_MHDR, 0xFF);
        cap.writeRegister(MPR121_PROX_NHDAR, 0xFF);
        cap.writeRegister(MPR121_PROX_NCLR, 0x00);
        cap.writeRegister(MPR121_PROX_FDLR, 0x00);

        // prox sensing
        cap.writeRegister(MPR121_PROX_MHDF, 0x01);
        cap.writeRegister(MPR121_PROX_NHDAF, 0x01);
        cap.writeRegister(MPR121_PROX_NCLF, 0xFF);
        cap.writeRegister(MPR121_PROX_NDLF, 0xFF);
 
        // Section E - Set proximity sensing threshold and release
        cap.writeRegister(MPR121_PRO_T, 0x3F);   // sets the proximity sensor threshold
        cap.writeRegister(MPR121_PRO_R, 0x3C);   // sets the proximity sensor release

        // Section F - Set proximity sensor debounce
        cap.writeRegister(MPR121_PROX_DEB, 0x50);  // PROX debounce

        // Section G - Set Auto Config and Auto Reconfig for prox sensing
        cap.writeRegister(MPR121_ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   
        cap.writeRegister(MPR121_ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
        cap.writeRegister(MPR121_ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
        cap.writeRegister(MPR121_ATO_CFG0, 0x0B);

        cap.writeRegister(0x5E, 0x3C);  // Start listening to all electrodes and the proximity sensor*/

        display_println("MPR121 OK");
    }
}

uint16_t read_touch() 
{
    // Get the currently touched pads
    uint16_t currtouched = cap.touched();
    
    #ifdef DEBUG
        for (uint8_t i=0; i<12; i++) {
            // it if *is* touched and *wasnt* touched before, alert!
            if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
                Serial.print(i); Serial.println(" touched");
            }
            // if it *was* touched and now *isnt*, alert!
            if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
                Serial.print(i); Serial.println(" released");
            }
        }
        #ifdef  DEBUG_MPR121
            /* Extra debug info */
            if( millis() - touch_millis >= 200) {
                touch_millis = millis();
                //Serial.println(cap.baselineData(12)); 
                /*
                int max = 0;
                for (uint8_t i=0; i<12; i++) {
                    int base = cap.baselineData(i);
                    int filt = cap.filteredData(i);
                    Serial.print(base-filt); Serial.print("\t");
                    if( base > max ) max = base;
                    if( filt > max ) max = filt;

                }
                Serial.print(currtouched,HEX);
                Serial.print("\tMax:");
                Serial.print(max);
                Serial.print("\tProx:\t");
                Serial.print(cap.filteredData(12));
                Serial.print("\t");
                Serial.println();*/
                int i=9;
                int base = cap.baselineData(i);
                int filt = cap.filteredData(i);
                Serial.printf("Base:%d\t Sens:%d\t Diff:%d \tProx:%d\t %d\n",base, filt, base-filt, cap.readRegister8(0x2A), cap.baselineData(12));   
            }
        #endif
    #endif


    uint16_t touched = currtouched & ~lasttouched;
    lasttouched = currtouched;
    return touched;
}