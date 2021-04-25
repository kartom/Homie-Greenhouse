#include <Arduino.h>
#include <Esp.h>
#include <U8x8lib.h>

#include <U8g2lib.h>

/* Constructor */
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/*reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/* u8x8.begin() is required and will sent the setup/init sequence to the display */
 
void setup(void)
{
    //u8x8.setI2CAddress(0x3C);
    u8x8.begin();
    u8g2.begin();
    u8x8.setFont(u8x8_font_8x13B_1x2_f);
    u8x8.clear();
    u8x8.drawString(0,0,"Test!"); // U8g2 Build-In functions
    delay(1000);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
}

void loop(void)
{
    u8x8.clear();
    u8x8.setFont(u8x8_font_8x13B_1x2_f);
    uint64_t start;
    uint16_t diff;
    start = millis();
    for(int i=0;i<100;i++)
    {
        u8x8.drawString(0,0,u8x8_u16toa(i, 5)); // U8g2 Build-In functions
    }
    diff = millis()-start;
    u8x8.drawString(0,2,u8x8_u16toa(diff, 5)); // U8g2 Build-In functions
    delay(1000);

    u8g2.setFont(u8g2_font_8x13B_tf);	// choose a suitable font
    start = millis();
    for(int i=0;i<100;i++)
    {
        u8g2.clearBuffer();					// clear the internal memory
        u8g2.drawStr(0,10,u8x8_u16toa(i, 5));	// write something to the internal memory
        u8g2.sendBuffer();					// transfer internal memory to the display
        ESP.wdtFeed();
    }
    diff = millis()-start;
    u8g2.drawStr(0,30,u8x8_u16toa(diff, 5));	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
    delay(1000);  
}