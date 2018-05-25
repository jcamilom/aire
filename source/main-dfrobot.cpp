#include "select-demo.h"

#if DEMO == DEMO_DFROBOT
 
#include "mbed.h"

/************************Hardware Related Macros************************************/
#define         BOARD_VCC                   (5.0)   // The Board's voltage power supply
//#define         MG_PIN                      (A0)     //define which analog input channel you are going to use
//#define         BOOL_PIN                    (2)
//#define         DC_GAIN                     (8.5)   //define the DC gain of amplifier

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL        (50)    //define the time interval(in milliseconds) between each samples in normal operation
#define         READ_SAMPLE_TIMES           (5)     //define how many samples you are going to take in normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
//#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
//#define         REACTION_VOLTGAE             (0.030) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2


// Initialize a pins to perform analog input and digital output functions
AnalogIn   ain(A0);
DigitalOut dout(LED1);

/*****************************  SENRead *********************************************
Input:   none
Output:  output voltge of SEN0159
Remarks: This function reads the output voltage of SEN0159
************************************************************************************/ 
float SENRead(void) {
    int i;
    float v = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++) {
        v += ain.read();
        wait_ms(READ_SAMPLE_INTERVAL);
    }
    v = (v / READ_SAMPLE_TIMES) * BOARD_VCC;
    return v;
}

int main(void) {
    float voltage;
    while(true) {
        // reads the sensor's output and if greater than 0.3 * BOARD_VCC
        // set the digital pin to a logic 1 otherwise a logic 0
        voltage = SENRead();
        if(voltage > 0.3f * BOARD_VCC) {
            dout = 1;
        } else {
            dout = 0;
        }

        // print the output value
        printf("Sensor's value: %fV\r\n", voltage);
        // print the percentage and 16 bit normalized values
        //printf("percentage: %3.3f%%\r\n", ain.read()*100.0f);
        //printf("normalized: 0x%04X \r\n", ain.read_u16());
        wait(2.0f);
    }
}

#endif
