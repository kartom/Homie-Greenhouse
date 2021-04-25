#include "control.h"

#define limit(x,x_min,x_max) ((x)<(x_min)?(x_min):((x)>(x_max)?(x_max):(x)))

unsigned long last_execution;
float last_e = 999.9;

HomieSetting<double> controlP("P", "P action fopr the temperature controller");
HomieSetting<double> controlI("I", "I action fopr the temperature controller");
HomieSetting<double> controlSP("tempSP", "Setpoint for the temperature controller");

HomieNode controlNode("control", "Yemperature controller", "");


void control_setup()
{
   controlP.setDefaultValue(DEFAULT_CONTROL_P).setValidator([] (double candidate) {
        return (candidate >= 0.0) && (candidate <= 100.0);
    });
    controlI.setDefaultValue(DEFAULT_CONTROL_I).setValidator([] (double candidate) {
        return (candidate >= 0.0) && (candidate <= 10000.0);
    }); 
    controlSP.setDefaultValue(DEFAULT_CONTROL_SP).setValidator([] (double candidate) {
        return (candidate >= 10.0) && (candidate <= 40.0);
    });
    
    last_execution = millis();
    
    controlNode.advertise("e").setName("Control error")
                              .setUnit("°C")
                              .setDatatype("float");
    controlNode.advertise("P").setName("P-part")
                              .setUnit("%")
                              .setDatatype("float");
    controlNode.advertise("I").setName("I-part")
                              .setUnit("%")
                              .setDatatype("float");
    controlNode.advertise("out").setName("Output")
                              .setUnit("%")
                              .setDatatype("float");
}

float control_loop(double sample_interval, double temperature, float out, float gain)
{
    unsigned long this_execution = millis();
    float h = (this_execution-last_execution)*MILLISECOND;
    float e = temperature-controlSP.get();
    //initialize last_e
    if( last_e > 100.0) last_e = e;
    double P = controlP.get();
    double I = controlI.get();
    float delta_out = (e*h/I+(e-last_e))*P*gain;
    
    #ifdef DEBUG
        Serial.printf("CTRL:  PV=%2.2f  e=%5.2f  P=%5.2f  I=%5.2f  OUT=%6.2f\n", 
                        temperature, e, (e-last_e)*P*gain, e*h/I*P*gain, out+delta_out);
    #endif
    controlNode.setProperty("e").send(String(e));
    controlNode.setProperty("P").send(String((e-last_e)*P*gain));
    controlNode.setProperty("I").send(String(e*h/I*P*gain));
    controlNode.setProperty("out").send(String(out+delta_out));

    last_execution = this_execution;
    last_e = e;
    return limit(out+delta_out, 0.0, 100.0);
}

    /*     //Regression
    //int measure_count = 0;
    //double sum_y=0.0, sum_xy=0.0,sum_x=0.0, sum_xx=0.0;
    //double temperature;
    //double gradient;
    // float delta_out = 0;
    // int out = 0;

    double x = measure_count*float(sample_interval)/3600.0; //Time in hours
    double y = temperature; //TODO: Get the temperature as input!
    sum_x+=x;
    sum_y+=y;
    sum_xx+= x*x;
    sum_xy+= x*y;        
    measure_count++;
    if( measure_count>= controlInterval.get() ) {
        double nr=(measure_count*sum_xy)-(sum_x*sum_y);
        double dr=(measure_count*sum_xx)-(sum_x*sum_x);
        gradient=nr/dr; //Temperature gradient in °C/h
        temperature = sum_y/measure_count+gradient*(x/2.0); //temp=avg+grad*half the averaged time
        sum_y=0.0;
        sum_xy=0.0;
        sum_x=0.0;
        sum_xx=0.0;
        measure_count=0.0;

        //Control algorithm
        double temperature_sp = controlSP.get();
        double P = controlP.get();
        double I = controlI.get();
        float h = float(controlInterval.get()*sample_interval)/3600.0;
        float delta_out = ((temperature-temperature_sp)*I+gradient)*P*h*gain;
        out = limit(out+delta_out, 0.0, 100.0);
        // TODO: Tillåt viss windup för regleringen?

        //TODO: Fixa MQTT kommunikationen
        // if( Homie.isConnected() ) 
        // {
        //    tempNode.setProperty("gradient").send(String(gradient));
        //    tempNode.setProperty("average").send(String(temperature));
        //    tempNode.setProperty("out").send(String(out));
        //    tempNode.setProperty("delta_out").send(String(delta_out));                
        // }
        Serial.printf("==========   Rate of change: %.2f °C/h\n", gradient);
        Serial.printf("             Temp now      : %.2f °C\n", temperature);
        Serial.printf("             Opening delta  : %.2f %%\n", delta_out);
        Serial.printf("             Opening sp    : %.2f %%\n", out);
    } */  
 