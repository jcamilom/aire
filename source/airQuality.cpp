#include "select-demo.h"

#if DEMO == AIR_QUALITY

#include "mbed.h"
#include "airQuality.h"

// Serial communication
Serial pc(USBTX, USBRX);        // To communicate with the PC
Serial specSensor(D1, D0);      // To communicate with the spec sensor

// Ticker to take measurements repeatedly
Ticker airTicker;

// Interrupt flags
bool flag_gas = true;

// Callback function called by the tikcer 
void flipFlags() {
    flag_gas = true;
}

// Functions declaration
void getGasConcentration(char*);
 
int main() {
    airTicker.attach(&flipFlags, MEASUREMENT_PERIOD); // the address of the function to be attached and the interval

    char gasConc[8] = "";
 
    // spin in a main loop. flipper will interrupt it to call flip
    while(1) {
        if(flag_gas) {
            getGasConcentration(gasConc);
            printf("The received CO: %s\r\n", gasConc);
            flag_gas = false;
        }
        wait(0.2);
    }
}

/**
* getGasConcentration captures the input from the SPEC sensor connected
* to the serial interface. The gas concentration is stored as a c-string
* in the passed char*
*
* @param[in] char* gasConc: the c-string(char*) to store the captured value
*/
void getGasConcentration(char *gasCon) {
    char gasID[16];
    char temp[4];
    char hum[4];
    // Ask the SPEC sensor for data
    specSensor.putc(CR);
    // Read the response data
    specSensor.scanf("%15s", gasID);
    specSensor.scanf("%7s",  gasCon);
    specSensor.scanf("%4s",  temp);
    specSensor.scanf("%4s",  hum);
    
    if (strcmp(gasID, ID_03) == 0){
        printf("O3: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
    } 
    else if (strcmp(gasID, ID_SO2) == 0){
        printf("SO2: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
    } 
    else if (strcmp(gasID, ID_CO) == 0){
        printf("CO: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
    }
    else if (strcmp(gasID, ID_NO2) == 0){
        printf("NO2: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
    } 
    else if (strcmp(gasID, ID_IAQ) == 0){
        printf("IAQ: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
    }
    else{
        printf("SPEC sensor is not configurated");  
    }

    // Remove the ',' at the end of the gasConc
    for(int i = 0; gasCon[i] != '\0'; i++) {
		//printf("char #%d: %c\r\n", i, gasCon[i]);
        if(gasCon[i] == ',') {
            gasCon[i] = '\0';
        }
	}
}

#endif
