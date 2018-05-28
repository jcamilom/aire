#include "select-demo.h"

#if DEMO == DEMO_CO2_CAL

// This file is intended to be use to calibrate the CO2 sensor SEN0159.
// It sends to Sentilo the voltage in AnalogIn when reset is pressed.
// Blue light mean stand by. Red HTTP failure. Green HTTP success.

#include "mbed.h"
#include "sentilo.h"
#include <string>
#include <sstream>

#define     BOARD_VCC                       (3.3)   // The Board's voltage power supply

// Initialize a pins to perform analog input and digital output fucntions
AnalogIn   ain(A0);
DigitalOut rLED(LED1, 1);
DigitalOut grLED(LED2, 1);
DigitalOut bLED(LED3, 0);

int main(void) {
    
    // print the percentage and 16 bit normalized values
    float voltage = ain.read() * BOARD_VCC;
    printf("voltage is: %f\r\n", voltage);
    //printf("percentage: %3.3f%%\n", ain.read() * 100.0f);
    //printf("normalized: 0x%04X \n", ain.read_u16());
    //wait(0.2f);

    std::ostringstream ss;
    ss << voltage;
    std::string val(ss.str());

    std::string ser ("http://theproviderserver:8081");
    string prov ("udeaProvider");
    string sen ("udeaTemp");
    //string val ("15.9");
    string tok ("29adcdf9b8f0401d99e0637b7eb7281159f2e776352752f0a2358bf325261cd0");

    int rt = sendObservation(ser, prov, sen, val, tok);
    if(rt == 1) {
        rLED = 0;
        bLED = 1; 
    } else {
        grLED = 0;
        bLED = 1;
    }
    printf("URL: %d\r\n", rt);

    wait(osWaitForever);
}

#endif
