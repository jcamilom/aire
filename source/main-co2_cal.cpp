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
    SentiloServer sServer = {"address"};

    // Create a provider
    Provider provider = {"providerName", "theToken"};
    
    // Create a sensor's array
    Sensor sensors[3] = {Sensor("name1"), Sensor("name2"), Sensor("name3")};
    for(int i = 0; i < 3; i++) {
		//sensors[i].setID("sensor" + std::to_string(i));
        sensors[i].setValue(val);
	}

    Sensor *pSensors;
    pSensors = sensors;

    // Create a component for the air measuring system
    Component airComponent ("componentName", sServer, provider, pSensors, 3);

    // Initilize network connection
    nsapi_error_t con_st = airComponent.initConnection();
    // Check for connection success
    if(con_st == NSAPI_ERROR_OK) {
        while(1) {
            // Reset blue light
            rLED = 1;
            grLED = 1;
            bLED = 0;

            // Send sensor's observations
            con_st = airComponent.sendSensorsObservations();    
            if(con_st == 1) {
                // Set red light
                rLED = 0;
                bLED = 1;
            } else {
                // Set green light
                grLED = 0;
                bLED = 1;
            }
            printf("SendObservations response: %d\r\n", con_st);

            wait(60.0);
        }
    }

    wait(osWaitForever);
}

#endif
