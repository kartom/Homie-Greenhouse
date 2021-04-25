#include "light.h"
#include "display.h"

int light_level = 10;
bool light_on = false;

HomieNode lightNode("light","light","Light level");

void set_light_on() {
    light_on = true;
    analogWrite(LIGHT_PIN, (1<<light_level)-1); 
    lightNode.setProperty("level").send(String(light_level));
    lightNode.setProperty("on").send("1");
    #ifdef DEBUG
        Serial.printf("Light set to %s %d\n",light_on?"ON":"OFF", light_level);
    #endif
    display_flash(">o<",String(light_level));
}

void set_light_off() {
    light_on = false;
    analogWrite(LIGHT_PIN, 0); 
    lightNode.setProperty("level").send(String(light_level));
    lightNode.setProperty("on").send("0");
    #ifdef DEBUG
        Serial.printf("Light set to %s %d\n",light_on?"ON":"OFF", light_level);
    #endif
    display_flash(">o<","OFF");
}

bool lightLevelHandler(const HomieRange& range, const String& value) {
    #ifdef DEBUG
        Serial.println("MQTT Light level: "+value);
    #endif
    int level = value.toInt();
    if( level >=1 && level <=10 ) {
        light_level = level;
        set_light_on();
    }
    return true;
}

bool lightOnOffHandler(const HomieRange& range, const String& value) {
    #ifdef DEBUG
        Serial.println("MQTT Light on/off: "+value);
    #endif
    if (value != "true" && value != "false" && value != "1" && value != "0") return false;
    if (value == "true" || value =="1") 
        set_light_on();
    else
        set_light_off();
    return true;
}

void light_increase() {
    light_level += 2;
    if( light_level > 10) light_level = 10;
    set_light_on();
}

void light_decrease() {
    light_level -= 2;
    if( light_level < 2) light_level = 2;    
    set_light_on();
}

void light_onoff() {
    if( light_on) 
        set_light_off();
    else
        set_light_on();
}

void light_setup() 
{
    pinMode(LIGHT_PIN, OUTPUT);
    // Turn off lights
    analogWrite(LIGHT_PIN, 0); 

    //Register Homie node
    lightNode.advertise("level").setName("Light level")
                                .setDatatype("integer")
                                .setUnit("[1-10]")
                                .settable(lightLevelHandler);  
    lightNode.advertise("on")   .setName("Light on/off")
                                .setDatatype("boolean")
                                .setUnit("0/1")
                                .settable(lightOnOffHandler);
    
}

