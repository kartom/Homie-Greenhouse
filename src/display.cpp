#include "display.h"
#include "temperature.h"
#include "door.h"

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

unsigned long last_changed;
unsigned long last_flash;
String line[MAX_DISPLAY_HEIGHT];
int current_line=0;
int display_width=0;
int display_height=0;
bool display_ok = true;
int display_num = 0; 
unsigned long awake_millis = millis();
bool awake = true;

HomieSetting<long> displayInterval("displayInterval", "Display change interval in ms");
//HomieNode displayMessage("messages","Log messages","log");

#define limit_line(x) ((x)<0)?(x)+display_height:((x)>=display_height)?(x)-display_height:(x)

void display_wake(bool reset) {
    u8x8.setPowerSave(false);    
    awake_millis =millis();
    awake = true;
    if( reset ) display_num = 0;
    #ifdef DEBUG
        Serial.println("Display wake up!");
    #endif
}

bool display_is_awake(void) {
    //Returns true if display is on
    return awake;
}

void display_setup(void)
{
    Wire.beginTransmission(DISPLAY_ADDRESS);
    if(Wire.endTransmission()){
        Serial.println("DISPLAY: ERROR");
        display_ok = false;
    }

    if( display_ok ) {
        //u8x8.setI2CAddress(DISPLAY_ADDRESS);
        u8x8.initDisplay(); 
        delay(100);
        u8x8.clearDisplay(); 
        delay(100);
        u8x8.setPowerSave(0);
        u8x8.setFlipMode(1);
        display_width = u8x8.getCols();
        display_height = u8x8.getRows();
        #ifdef DEBUG
            if( display_height > MAX_DISPLAY_HEIGHT ) {
                Serial.printf("\n\nCRITICAL!! MAX_DISPLAY_HEIGHT too small, should be %d\n\n", display_height);
                display_ok = false;
            }
        #endif

        //Print initial message
        u8x8.setFont(FONT_2x3);
        u8x8.drawString(0,0,"WELCOME!"); 
        u8x8.setFont(FONT_1x2);
        u8x8.drawString(0,3,FIRMWARE_NAME); 
        u8x8.drawString(0,5,FIRMWARE_VERSION); 
        u8x8.setFont(FONT_1x1);
        String datestr=__TIMESTAMP__;
        u8x8.drawString(0,7,(datestr.substring(0,9)+datestr.substring(19,24)).c_str()); 

        //displayMessage.advertise("message").setName("Message").setDatatype("string");

        delay(3000);
        display_println("Display "+String(display_width)+"x"+String(display_height)+" OK");
    }
    
    displayInterval.setDefaultValue(DEFAULT_DISPLAY_CHANGE_INTERVAL).setValidator([] (long candidate) {
        return (candidate >= 500) && (candidate <= 10000);
    });  

    if( display_ok ) Serial.println("Display OK");

    last_changed = millis();
    last_flash = millis();
}

void display_print_messages()
{
    u8x8.setFlipMode(1);
    if( !display_ok || !awake ) return;
    u8x8.home();
    u8x8.setFont(FONT_1x1);
    for( int i=display_height-1; i>=0; i--) {
        u8x8.println(line[limit_line(current_line-i)].c_str());
    }
}

void display_println(String str)
{
    current_line=limit_line(current_line+1);
    line[current_line] = str;
    #ifdef DEBUG
        if(display_ok && (line[current_line].length()>(unsigned int)display_width)) {
                Serial.printf("Too wide line for display: %s (max %u chars)\n", str.c_str(), display_width);
        }
    #endif

    Serial.println(str.c_str());
    //displayMessage.setProperty("message").send(str);
    u8x8.clear();
    display_print_messages();
}

void display_center2(int col, int row, int width, String str)
{
    u8x8.setCursor(col+width-str.length(), row);
    u8x8.print(str.c_str());
}

void display_flash(String row1, String row2)
{
    if( !display_ok ) return;
    display_wake();
    #ifdef DEBUG
        unsigned int w2 = display_width/2;
        if( display_ok && (row1.length()>w2) ) {
                Serial.printf("Row 1 too wide line for display: %s (max %d chars)\n", row1.c_str(), w2);
        }
        if( display_ok && (row2.length()>w2) ) {
                Serial.printf("Row 2 too wide line for display: %s (max %d chars)\n", row2.c_str(), w2);
        }
    #endif
    Serial.println("Display flash: "+row1+ " - " + row2);
    if( millis() -last_flash < 500 ) {
        delay(10);
    }
    last_flash = millis();
    u8x8.clear();
    u8x8.setFont(FONT_2x4);
    display_center2(0, 0, 8, row1);
    u8x8.setFont(FONT_2x3);
    display_center2(0, 5, 8, row2);
    last_changed = millis();
}

void display_loop(void)
{
    unsigned long millis_now = millis();
    if( awake && (millis_now-awake_millis >= DISPLAY_WAKE_TIME) ) {
        awake=false;
        u8x8.setPowerSave(true);
        #ifdef DEBUG
            Serial.println("Display goes to sleep...");
        #endif

    }
    if( !display_ok || !awake ) return;
    if(millis_now- last_changed >= (unsigned long)displayInterval.get() ) {
        last_changed = millis_now;
        u8x8.clear();
        String str;
        switch ( display_num )
        {
        case 0:
            u8x8.setFont(FONT_4x8);
            str = String(get_min_temp(),1);
            u8x8.print(str.substring(0,4));
            u8x8.setFont(FONT_1x1);
            u8x8.setCursor(0,7);
            u8x8.printf(" %5.1f   %5.1f",get_temp(0), get_temp(1));
            break;
        case 1:
            u8x8.setFont(FONT_2x4);
            display_center2(8, 0, 4, String(door_get_opening(0)));
            display_center2(0, 0, 4, String(door_get_opening(1)));
            u8x8.setFont(FONT_2x3);
            display_center2(8, 4, 4, door_get_state(0));
            display_center2(0, 4, 4, door_get_state(1));
            u8x8.setFont(FONT_1x1);
            u8x8.setCursor(0,7);
            u8x8.print("<- ||      || ->");
            break;
        case 2:
            //display_print_messages();
            u8x8.setFont(FONT_2x3);
            display_center2(0, 0, 8, get_homie_info(HI_WIFI_STATUS));
            display_center2(0, 3, 8, get_homie_info(HI_MQTT_STATUS));
            u8x8.setFont(FONT_1x1);
            u8x8.setCursor(0,6);
            u8x8.println(get_homie_info(HI_HOMIE_MODE));
            u8x8.println(get_homie_info(HI_WIFI_IP));
            /*
            for(int i=0; i<HI_NOF; i++) {
                u8x8.setFont(FONT_1x1);
                u8x8.println(get_homie_info(i));
            }*/
            break;
        }

        display_num++;
        if( display_num>2 ) display_num=0;
        
    }
}