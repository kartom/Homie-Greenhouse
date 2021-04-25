//Libraries
#include <Arduino.h>
#include <Wire.h>
#include <Homie.h>
#include <U8g2lib.h>
//Project files
#include "settings.h"
#include "light.h"
#include "temperature.h"
#include "control.h"
#include "door.h"
#include "touch.h"
#include "display.h"
#include "i2c_scanner.h"

/* IMPORTANT:
    In case of "error 'size_t' does not name a type":
      add "#include <Arduino.h>" to the file with the error
*/
 
unsigned long connectedMillis = millis();
unsigned long measuredMillis = millis();
unsigned long keyboardMillis = millis();

HomieSetting<double> sampleInterval("sampleInterval", "Sample interval for temperatures and control");

String homie_info[HI_NOF];

String get_homie_info(int type) {
    return homie_info[type];
}

void onHomieEvent(const HomieEvent& event) {
    String str;
    switch(event.type) {
        case HomieEventType::STANDALONE_MODE:
            display_flash("HOMIE","STANDALONE");
            homie_info[HI_HOMIE_MODE] = "STANDALONE";
            break;
        case HomieEventType::CONFIGURATION_MODE:
            display_flash("HOMIE","CONFIG");
            homie_info[HI_HOMIE_MODE] = "CONFIG";
            break;
        case HomieEventType::NORMAL_MODE:
            display_flash("HOMIE","NORMAL");
            homie_info[HI_HOMIE_MODE] = "NORMAL";
            break;
        break;
        case HomieEventType::OTA_STARTED:
            display_flash("HOMIE","OTA");
            break;
        case HomieEventType::OTA_PROGRESS:
            // You can use event.sizeDone and event.sizeTotal
            display_flash("OTA",String(event.sizeDone/event.sizeTotal,0)+"%");
            break;
        case HomieEventType::OTA_FAILED:
            display_flash("OTA","FAILED");
            break;
        case HomieEventType::OTA_SUCCESSFUL:
            display_flash("OTA","SUCCESS");
        break;
        case HomieEventType::ABOUT_TO_RESET:
            display_flash("HOME","RESET!");
        break;
        case HomieEventType::WIFI_CONNECTED:
            // You can use event.ip, event.gateway, event.mask
            display_flash("WIFI","OK");
            homie_info[HI_WIFI_STATUS]="WIFI OK";
            homie_info[HI_WIFI_IP]=event.ip.toString();
            homie_info[HI_WIFI_GATEWAY]=event.gateway.toString();
            homie_info[HI_WIFI_MASK]=event.mask.toString();
        break;
        case HomieEventType::WIFI_DISCONNECTED:
            if( event.wifiReason<200 )
                display_flash("WIFI","ERR: "+String((int)event.wifiReason));
            else
                display_flash("WIFI","ERR: "+String((int)event.wifiReason-110));
            homie_info[HI_WIFI_STATUS]="WIFI "+String((int)event.wifiReason);
            /*  You can use event.wifiReason (souce: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino)
                WIFI_DISCONNECT_REASON_UNSPECIFIED              = 1,
                WIFI_DISCONNECT_REASON_AUTH_EXPIRE              = 2,
                WIFI_DISCONNECT_REASON_AUTH_LEAVE               = 3,
                WIFI_DISCONNECT_REASON_ASSOC_EXPIRE             = 4,
                WIFI_DISCONNECT_REASON_ASSOC_TOOMANY            = 5,
                WIFI_DISCONNECT_REASON_NOT_AUTHED               = 6,
                WIFI_DISCONNECT_REASON_NOT_ASSOCED              = 7,
                WIFI_DISCONNECT_REASON_ASSOC_LEAVE              = 8,
                WIFI_DISCONNECT_REASON_ASSOC_NOT_AUTHED         = 9,
                WIFI_DISCONNECT_REASON_DISASSOC_PWRCAP_BAD      = 10,  
                WIFI_DISCONNECT_REASON_DISASSOC_SUPCHAN_BAD     = 11,  
                WIFI_DISCONNECT_REASON_IE_INVALID               = 13,  
                WIFI_DISCONNECT_REASON_MIC_FAILURE              = 14,  
                WIFI_DISCONNECT_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,  
                WIFI_DISCONNECT_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,  
                WIFI_DISCONNECT_REASON_IE_IN_4WAY_DIFFERS       = 17,  
                WIFI_DISCONNECT_REASON_GROUP_CIPHER_INVALID     = 18,  
                WIFI_DISCONNECT_REASON_PAIRWISE_CIPHER_INVALID  = 19, 
                WIFI_DISCONNECT_REASON_AKMP_INVALID             = 20, 
                WIFI_DISCONNECT_REASON_UNSUPP_RSN_IE_VERSION    = 21,  
                WIFI_DISCONNECT_REASON_INVALID_RSN_IE_CAP       = 22,  
                WIFI_DISCONNECT_REASON_802_1X_AUTH_FAILED       = 23,
                WIFI_DISCONNECT_REASON_CIPHER_SUITE_REJECTED    = 24,
                WIFI_DISCONNECT_REASON_BEACON_TIMEOUT           = 90,
                WIFI_DISCONNECT_REASON_NO_AP_FOUND              = 91,
                WIFI_DISCONNECT_REASON_AUTH_FAIL                = 92,
                WIFI_DISCONNECT_REASON_ASSOC_FAIL               = 93,
                WIFI_DISCONNECT_REASON_HANDSHAKE_TIMEOUT        = 94,       
            */
            break;
        case HomieEventType::MQTT_READY:
            display_flash("MQTT","READY");
            homie_info[HI_MQTT_STATUS]="MQTT OK";
            break;
        case HomieEventType::MQTT_DISCONNECTED:
            display_flash("MQTT","ERR: "+String((int)event.mqttReason));
            homie_info[HI_MQTT_STATUS]="MQTT E"+String((int)event.mqttReason);

            /*  You can use event.mqttReason (source: https://github.com/marvinroger/async-mqtt-client/blob/master/src/AsyncMqttClient/DisconnectReasons.hpp)
                0 TCP_DISCONNECTED
                1 MQTT_UNACCEPTABLE_PROTOCOL_VERSION
                2 MQTT_IDENTIFIER_REJECTED
                3 MQTT_SERVER_UNAVAILABLE
                4 MQTT_MALFORMED_CREDENTIALS
                5 MQTT_NOT_AUTHORIZED
                6 ESP8266_NOT_ENOUGH_SPACE
                7 TLS_BAD_FINGERPRINT
            */
        break;
        case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
        // Do whatever you want when an MQTT packet with QoS > 0 is acknowledged by the broker
        // You can use event.packetId
        break;
        case HomieEventType::READY_TO_SLEEP:
        // After you've called `prepareToSleep()`, the event is triggered when MQTT is disconnected
        break;
        case HomieEventType::SENDING_STATISTICS:
        // Do whatever you want when statistics are sent in normal mode
        break;
  }
}

void setup() {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        Wire.begin();
        Serial.begin(SERIAL_SPEED);
        display_setup();  // Setup display first so we have somethin to communicate on

        #ifdef DEBUG 
            i2c_scanner();
        #endif

        light_setup();
        touch_setup();
        door_setup();
        temperature_setup();
        control_setup();

        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        
        sampleInterval.setDefaultValue(DEFAULT_SAMPLE_INTERVAL).setValidator([] (double candidate) {
            return (candidate >= 5) && (candidate <= 60);
        });

        #ifndef NO_HOMIE
            Serial.println("Setting up Homie");
            for( int i=0; i<HI_NOF; i++) homie_info[i] = "";
            // Initialize Homie ////////////////////////////////
            Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
            // Homie.setResetTrigger(BUTTON1_PIN, LOW, 30000);
            Homie.disableResetTrigger(); //To prevent reset, but still happens!
            Homie.onEvent(onHomieEvent); 
            Homie.setup();
            Homie.setIdle(false);  //To prevent reset, since it has happened!
            Serial.println("Homie setup complete");
        #else
            Serial.println("Skipping Homie setup");
        #endif
}

void loop() {
    
    #ifndef NO_HOMIE
        /*if( NO_WIFI_TIMEOUT > 300 ) {
            if ( Homie.isConnected() ) {
                // This counter will stop when disconnected
                connectedMillis = millis();
            } else {
                // If disconnected for over 5 minutes
                if ( millis() - connectedMillis >= NO_WIFI_TIMEOUT*1000 ) {
                    Serial.println("Restarting in ten seconds");
                    delay(10000);
                    // The risk of being in OTA whilst disconnected from MQTT are slim.
                    ESP.restart();
                };
            }
        }*/
        Homie.loop();
    #endif
    
    //Handle keyboard

    //if( millis() - keyboardMillis>= 500) {
    //    keyboardMillis = millis();    
    uint16_t keys = read_touch();
    if( keys & KEY_WAKE )                                display_wake(true);
    if( (keys & KEY_OPEN_DOOR0) && display_is_awake() )  door_set_man_opening(0,100);
    if( (keys & KEY_OPEN_DOOR1) && display_is_awake() )  door_set_man_opening(1,100);
    if( (keys & KEY_CLOSE_DOOR0) && display_is_awake() ) door_set_man_opening(0,0);
    if( (keys & KEY_CLOSE_DOOR1) && display_is_awake() ) door_set_man_opening(1,0); 
    if( keys & KEY_AUTO_DOOR0 )                          door_set_auto(0,true);
    if( keys & KEY_AUTO_DOOR1 )                          door_set_auto(1,true);
    if( keys & KEY_INC_LIGHT )                           light_increase();
    if( keys & KEY_DEC_LIGHT )                           light_decrease();
    if( keys & KEY_ONOFF_LIGHT )                         light_onoff();

    if( millis() - measuredMillis >= sampleInterval.get()*1000) {
        measuredMillis = millis();    
        double temp = read_temp();
        float opening = door_get_opening_sp();
        float gain = 1.0;
        opening = control_loop(sampleInterval.get(), temp, opening, gain);
        door_set_auto_opening(opening);
    }

    door_loop();
    display_loop();
    ESP.wdtFeed();
}
