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

    // Create a Sentilo server
    SentiloServer sServer = {"sistemic.udea.edu.co:9091"};    

    // Create a provider
    Provider provider = {"udeaProvider", "f4b92b05becd3b0b4d894ed78ffd468126a2622011a5a5aaf3165119c24431b7"};
    
    // Create a sensor's array
    Sensor sensors[3] = {Sensor("spec"),Sensor("temp"),Sensor("hum")};
    sensors[0].setValue("0.0");
    sensors[1].setValue("5.0");
    sensors[2].setValue("10.0");
    /* for(int i = 0; i < 1; i++) {
	//	sensors[i].setID("sensor" + std::to_string(i));
        sensors[i].setValue(std::to_string(i));
	} */

    Sensor *pSensors;
    pSensors = sensors;

    // Create a component for the air measuring system
    Component airComponent ("componentName", sServer, provider, pSensors, 3);

    int rt = airComponent.sendSensorsObservations();    
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
