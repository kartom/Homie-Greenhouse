#define FIRMWARE_NAME "Greenhouse"
#define FIRMWARE_VERSION "0.2.0"
/* Revision history:
   0.2.0 Reworked revision with multiple source files, two doors and Homie v3.0.0?
   0.1.0 First version
*/

/* DEBUG SETTINGS, SHOULD BE OUTCOMMENTED IN RELEASE */
// #define NO_HOMIE
// #define DEBUG
// #define DEBUG_MPR121
// #define DEBUG_DOORS

/* WIFI CONNECTION */
#define NO_WIFI_TIMEOUT 0  //timeout in seconds, no restart if <300

/* PIN DEFIINITIONS */
#define OPEN_DOOR0  D5 
#define CLOSE_DOOR0  D6
#define LIGHT_PIN D3
#define EXTRA_PIN D8 
#define OPEN_DOOR1 D0
#define CLOSE_DOOR1 D7

/* DISPLAY */
#define DEFAULT_DISPLAY_CHANGE_INTERVAL 2000
#define ERROR_DELAY 2000

/* CONTROL */
#define DEFAULT_SAMPLE_INTERVAL 10
#define DEFAULT_CONTROL_P 2.0
#define DEFAULT_CONTROL_I 720.0
#define DEFAULT_CONTROL_SP 25.0

/* DOORS */
#define DEFAULT_DOOR_HYST 10.0
#define DEFAULT_DOOR_OPENING_TIME0 70.0
#define DEFAULT_DOOR_OPENING_TIME1 70.0
#define DEFAULT_DOOR_SIZE0 300.0
#define DEFAULT_DOOR_SIZE1 300.0

/* KEY MAPPING */
#define KEY_WAKE _BV(9)
#define KEY_OPEN_DOOR0 _BV(7)
#define KEY_AUTO_DOOR0 _BV(8)
#define KEY_CLOSE_DOOR0 _BV(6)

#define KEY_INC_LIGHT _BV(5)
#define KEY_ONOFF_LIGHT _BV(4)
#define KEY_DEC_LIGHT _BV(3)

#define KEY_OPEN_DOOR1 _BV(2)
#define KEY_AUTO_DOOR1 _BV(1)
#define KEY_CLOSE_DOOR1 _BV(0)

/* FONTS */
#define FONT_1x1 u8x8_font_torussansbold8_r
#define FONT_1x2 u8x8_font_8x13B_1x2_f
#define FONT_2x3 u8x8_font_courB18_2x3_f
#define FONT_4x8 u8x8_font_inb46_4x8_n
#define FONT_2x4 u8x8_font_inb21_2x4_r

/* HOMIE INFO */
#define HI_HOMIE_MODE   0
#define HI_WIFI_STATUS  1
#define HI_WIFI_IP      2
#define HI_WIFI_GATEWAY 3
#define HI_WIFI_MASK    4
#define HI_MQTT_STATUS  5
#define HI_NOF          6 //Number of infotypes

String get_homie_info(int type);

/* MISC */
#define MILLISECOND 0.001

