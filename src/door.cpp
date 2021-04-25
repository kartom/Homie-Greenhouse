#include "door.h"
#include "display.h"

enum States
{
    ST_IDLE,
    ST_CLOSING,
    ST_OPENING,
    ST_SHUTTING,
};

const int n_doors=2;
States state[n_doors]= {ST_IDLE,ST_IDLE};
float opening_sp[n_doors] = {0.0 , 0.0};
float opening_pv[n_doors] = {100.0, 100.0};
bool opening_auto[n_doors] =  {true, true};
// float opening_time[n_doors];
unsigned long state_start[n_doors];
unsigned long last_step;
uint8_t open_pin[n_doors] = {OPEN_DOOR0, OPEN_DOOR1};
uint8_t close_pin[n_doors] = {CLOSE_DOOR0, CLOSE_DOOR1};

HomieSetting<double> openingHyst("doors_openingHyst", "Hysterezis in %");
HomieSetting<double> *openingTime[n_doors]= { new HomieSetting<double>("door0_openingTime", "Time for full stroke of door opener in seconds"), 
                                              new HomieSetting<double>("door1_openingTime", "Time for full stroke of door opener in seconds") };
HomieSetting<double> *openingSize[n_doors]= { new HomieSetting<double>("door0_openingSize","Size of door opening in mm"),
                                              new HomieSetting<double>("door1_openingSize","Size of door opening in mm")};


HomieNode* doorNode[n_doors] = {new HomieNode("door0", "Door status", ""), new HomieNode("door1", "Door status","") };

String door_get_state(uint8_t num) {
    if( !opening_auto[num] ) return "MAN";
    switch( state[num] ) {
        case ST_IDLE:     return "   ";
        case ST_CLOSING:  return "CLS";
        case ST_OPENING:  return "OPN";
        case ST_SHUTTING: return "SHT";
    }
    return "unknown?";
}

/* HOMIE HANDLERS */
bool homie_set_opening(uint8_t num, const HomieRange& range, const String& value) 
{
    opening_sp[num] = value.toInt();
    #ifdef DEBUG
        Serial.printf("Setting opening %d sp to: %f from MQTT\n", num, opening_sp[num]);
    #endif
    doorNode[num]->setProperty("sp").send(String(opening_sp[num]));
    return true;
}
bool homie_set_opening0(const HomieRange& range, const String& value) { return homie_set_opening(0, range, value); };
bool homie_set_opening1(const HomieRange& range, const String& value) { return homie_set_opening(1, range, value); };

bool homie_set_auto(uint8_t num, const HomieRange& range, const String& value) 
{
    if (value != "true" && value != "false" && value != "1" && value != "0") return false;
    opening_auto[num] = (value == "true" || value =="1");
    #ifdef DEBUG
        Serial.printf("Setting auto %d to: %d from MQTT\n", num, opening_auto[num]);
    #endif
    doorNode[num]->setProperty("auto").send(opening_auto[num]?"1":"0");
    return true;
}
bool homie_set_auto0(const HomieRange& range, const String& value) { return homie_set_auto(0, range, value); };
bool homie_set_auto1(const HomieRange& range, const String& value) { return homie_set_auto(1, range, value); };

/* --- API ---------- API ---------- API ---------- API ---------- API ---------- API ---------- API ---------- API ------- */

void door_set_man_opening(uint8_t num, float opening) {
    opening_sp[num] = opening;    
    opening_auto[num] = false;
    #ifdef DEBUG
        Serial.printf("Setting opening to: %.1f from program, current value: %.1f\n", opening_sp[num], opening_pv[num]);
    #endif
    doorNode[num]->setProperty("sp").send(String(opening_sp[num]));
    doorNode[num]->setProperty("auto").send(opening_auto[num]?"1":"0");
    display_flash((num)?"<- ||   ":"   || ->", "MAN "+String(opening,0)+"%");
}

void door_set_auto_opening(float opening) {
    for(int i=0; i<n_doors; i++) {
        if( opening_auto[i] ) {
            opening_sp[i]=opening;
            doorNode[i]->setProperty("sp").send(String(opening_sp[i]));
        }
    }
}

void door_set_auto(uint8_t num, bool auto_value) {
    opening_auto[num] = auto_value;
    #ifdef DEBUG
        Serial.printf("Setting door %d auto to: %d from program\n", num, opening_auto[num]);
    #endif
    doorNode[num]->setProperty("auto").send(opening_auto[num]?"1":"0");
    display_flash((num)?"<- ||   ":"   || ->", (auto_value)?"AUTO":"MAN "+String(opening_sp[num])+"%");
}

float door_get_opening_sp() {
    float size_auto=0;
    float sp_sum=0.0;
    for(int i=0; i<n_doors; i++) {
        if( opening_auto[i]) {
            size_auto += (float)openingSize[i]->get();
            sp_sum += opening_sp[i]*(float)openingSize[i]->get();
        }
    }
    return sp_sum/size_auto;
}

int door_get_opening(uint8_t num) {
    int opening = round(opening_sp[num]);
    if( opening < openingHyst.get() ) opening = 0.0;
    // Serial.printf("Get opening: %.1f %.1f %i\n",opening_sp[num], opening_pv[num], opening);
    return opening;
}

float door_get_gain() {
    unsigned int size[n_doors]; // = {openingSize0.get(), openingSize0.get()};
    unsigned int size_auto =0;
    unsigned int size_tot = 0;
    for(int i=0; i<n_doors; i++) {
        if( opening_auto[i]) size_auto += size[i];
        size_tot += size[i];
    }
    return size_auto/size_tot;
}

/* --- SETUP ---------- SETUP ---------- SETUP ---------- SETUP ---------- SETUP ---------- SETUP ------- */

void door_setup() 
{
    //Prepare the door control to outputs and not moving
    for(int i=0; i<n_doors; i++)
    {
        pinMode(open_pin[i], OUTPUT);
        pinMode(close_pin[i], OUTPUT);
        digitalWrite(open_pin[i],LOW);
        digitalWrite(close_pin[i],LOW); 
    }

    typedef std::function<bool(const HomieRange& range, const String& value)> PropertyInputHandler;
    PropertyInputHandler opening_handlers[2] = { homie_set_opening0, homie_set_opening1 };
    PropertyInputHandler auto_handlers[2] = { homie_set_auto0, homie_set_auto1 };

    for( int i=0; i<n_doors; i++)
    {
        doorNode[i]->advertise("sp").setName("Opening setpoint")
                                    .setUnit("%")
                                    .setDatatype("integer")
                                    .settable(opening_handlers[i]);
        doorNode[i]->advertise("pv").setName("Actual opening")
                                    .setUnit("%")
                                    .setDatatype("integer");
        doorNode[i]->advertise("state").setName("State of the door")
                                    .setUnit("")
                                    .setDatatype("string");
        doorNode[i]->advertise("auto").setName("Auto/manual for the door controller")
                                    .setUnit("")
                                    .setDatatype("")
                                    .settable(auto_handlers[i]);
        openingTime[i]->setDefaultValue(DEFAULT_DOOR_OPENING_TIME0).setValidator([] (double candidate) {
            return (candidate >= 20) && (candidate <= 300);
        });
        openingSize[i]->setDefaultValue(DEFAULT_DOOR_SIZE0).setValidator([] (double candidate) {
            return (candidate >= 100) && (candidate <= 1000);
        });
    }
    openingHyst.setDefaultValue(DEFAULT_DOOR_HYST).setValidator([] (double candidate) {
        return (candidate >= 0) && (candidate <= 100);
    });    
}

/* --- LOOP ---------- LOOP ---------- LOOP ---------- LOOP ---------- LOOP ---------- LOOP -------  */

void set_state(int i, States new_state) 
{
    state[i] = new_state;
    switch( new_state ) {
        case ST_IDLE:
            digitalWrite(open_pin[i],LOW);                
            digitalWrite(close_pin[i],LOW);                
            Serial.println("Door "+String(i)+" idle");
            doorNode[i]->setProperty("state").send("Idle");
            break;
        case ST_OPENING:
            digitalWrite(open_pin[i],HIGH);
            Serial.println("Opening door "+String(i));
            doorNode[i]->setProperty("state").send("Opening");
            break;        
        case ST_CLOSING:
            Serial.println("Closing door "+String(i));
            digitalWrite(close_pin[i],HIGH);                
            doorNode[i]->setProperty("state").send("Closing");
            break;
        case ST_SHUTTING:
            Serial.println("Shutting door "+String(i));
            state_start[i] = millis();
            doorNode[i]->setProperty("state").send("Shutting");
            break;
    }
}

void door_loop() 
{
    if( millis() - last_step >= DOOR_STEPTIME) {
        last_step = millis();

        for(int i=0; i<n_doors ; i++)
        {
            int j=n_doors-1-i; //Opposite door

            //Calculate chagne in opening in % from step time in ms
            float delta_opening = DOOR_STEPTIME /((float)openingTime[i]->get()*10.0);
            if( state[i] == ST_OPENING ) {
                opening_pv[i] += delta_opening;
                if(opening_pv[i]>100.1) opening_pv[i] = 100.1;
            }
            if( (state[i] == ST_CLOSING) || (state[i] == ST_SHUTTING))  {
                opening_pv[i] -= delta_opening;
                if(opening_pv[i]<-0.1) opening_pv[i] = -0.1;
            }
            #ifdef DEBUG_DOORS
                Serial.printf("DOOR %d:  SP=%5.2f  PV=%5.2f  dPV=%5.2f  %s  %s\n", 
                                i, opening_sp[i], opening_pv[i], delta_opening, opening_auto[i]?"AUTO":"MAN" , door_get_state(i).c_str());
            #endif

            //Communicate on MQTT
            doorNode[i]->setProperty("pv").send(String(opening_pv[i]));

            //Check for state change
            switch( state[i] ) 
            {
                case ST_IDLE:
                    if( (opening_sp[i] > opening_pv[i]+openingHyst.get())  //Hysteres passed
                         || ((opening_sp[i] > 99.9) && (opening_pv[i]<100.0)) //Open door fully
                         && state[j]==ST_IDLE ) 
                    {
                        set_state(i, ST_OPENING );
                    } 
                    else if( (opening_sp[i] < opening_pv[i]-openingHyst.get()) //Close door fully
                             || ((opening_sp[i] < openingHyst.get()) && (opening_pv[i]>0.0))
                             && state[j]==ST_IDLE ) 
                    {
                        set_state(i, ST_CLOSING);
                    }
                    break;
                case ST_OPENING:
                    if( opening_pv[i] >= opening_sp[i] ) {
                            set_state(i,ST_IDLE);
                    }
                    break;
                case ST_CLOSING:
                    //Check if we are ready with closing
                    if( opening_pv[i] <= opening_sp[i] ) 
                    {
                        // If the setpoint is less than the hysteresis, then shut the door
                        if( opening_sp[i] < openingHyst.get() ) 
                        {
                            set_state(i,ST_SHUTTING);
                        } 
                        // If the setpoint is above the hysteresis, then idle
                        else 
                        {
                            set_state(i, ST_IDLE);
                        }
                    }
                    break;
                case ST_SHUTTING:
                    // Check if the door needs to be open, then abort shutting and begin opening
                    if( opening_sp[i] > opening_pv[i]+openingHyst.get() ) {
                        set_state(i, ST_OPENING);
                    } 
                    // Check if we have been shutting the door for 20% of stroke time, then idle
                    else if( (millis()-state_start[i]) > (openingTime[i]->get()*200) ) {
                        set_state(i, ST_IDLE);
                    }
                    break;
            }
        }
    }
}

