#include "select-demo.h"

#if DEMO == DEMO_DFROBOT
 
#include "mbed.h"

/************************Hardware Related Macros************************************/
#define         BOARD_VCC                   (3.3)   // The Board's voltage power supply
#define         SEN_PIN                     (A0)    // Define the analog input channel to use
#define         BOOL_PIN                    (LED1)
#define         DC_GAIN                     (8.5)   // Define the DC gain of amplifier

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL        (50)    // Define the time interval(in milliseconds) between each samples in normal operation
#define         READ_SAMPLE_TIMES           (5)     //define how many samples you are going to take in normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE          (0.324) // Define the output of the sensor in volts when the concentration of CO2 is 400PPM (324mV from graph) TODO: Calibrate this
#define         REACTION_VOLTGAE            (0.020) // Define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2 -> (0.324 - 0.304) = 0.020 (from graph)

/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTGAE / (2.602 - 3))};   
// Define a equation for the curve using two points.
// Data format:{x, y, slope}; point1: (log400, 0.324), point2: (log1000, 0.304) 
// This is a semilogaritmic graph, where the x-axis is logaritmic and y-axis is lineal.
// The slope here is denoted by:
// m = (y2 - y1) / (log(x2) - log(x1))
// We use the REACTION_VOLTGAE, which is the drop of the sensor when move from clean air(400ppm)
// to a concentration of 1000ppm. With this two points, the slope is:
// m = REACTION_VOLTGAE / (log(400) - log(1000)) = -0.05

// Initialize a pins to perform analog input and digital output functions
AnalogIn        ain(SEN_PIN);
DigitalOut      dout(BOOL_PIN);

// Functions declaration
float SENRead(void);
int getConcentration(float, float *);

int main(void) {
    float voltage;
    int conc;
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
        printf("\r\nSensor's value: %fV\r\n", voltage);

        // Get the ppm concentration
        conc = getConcentration(voltage, CO2Curve);
        printf("CO2: ");
        if (conc == -1) {
            printf( "<400" );
        } else {
            printf("%d", conc);
        }

        // print the percentage and 16 bit normalized values
        //printf("percentage: %3.3f%%\r\n", ain.read()*100.0f);
        //printf("normalized: 0x%04X \r\n", ain.read_u16());
        wait(2.0f);
    }

}

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

/*****************************  getConcentration **********************************
Input:      volts - SEN0159 output measured in volts
            pcurve  - pointer to the curve of the target gas
Output:     ppm of the target gas
Remarks:    By using the slope and a point of the line. The x(logarithmic value of ppm) 
            of the line could be derived if y(MG-811 output) is provided. As it is a logarithmic
            coordinate, power of 10 is used to convert the result to non-logarithmic value.
************************************************************************************/ 
int getConcentration(float volts, float *pcurve) {
    //printf("volts / DC_GAIN : %f, ZERO_POINT_VOLTAGE: %f", volts / DC_GAIN, ZERO_POINT_VOLTAGE);
    if ((volts / DC_GAIN) >= ZERO_POINT_VOLTAGE) {
        return -1;
    } else { 
        return pow(10, ((volts / DC_GAIN) - pcurve[1]) / pcurve[2] + pcurve[0]);
    }
}

#endif
