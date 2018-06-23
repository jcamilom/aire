#include "select-demo.h"
#if DEMO == DEMO_SPEC
#include "mbed.h"
// Creates an event bound to the specified event queue


char gas[16];
char dato[8];
char temp[4];
char hum[4];
int counter_button = 0;
InterruptIn bget(SW3);
InterruptIn bchange(SW2);
//K64F
Serial pc(USBTX, USBRX); // tx, rx
Serial spec(D1, D0);  // tx, rx

void on_fall() {
        spec.putc(13);
        spec.scanf("%15s", gas);
        spec.scanf("%7s",  dato);
        spec.scanf("%4s",  temp);
        spec.scanf("%4s",  hum);
        if (strcmp(gas, "092117010705,") == 0){
            printf("O3: %s temp: %s hum: %s\r\n", dato, temp, hum);
        } 
        else if (strcmp(gas, "081116050515,") == 0){
            printf("SO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        } 
        else if (strcmp(gas, "041117010433,") == 0){
            printf("CO: %s temp: %s hum: %s\r\n", dato, temp, hum);
        }
        else if (strcmp(gas, "091117010115,") == 0){
            printf("NO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        } 
        else if (strcmp(gas, "081216010737,") == 0){
            printf("IAQ: %s temp: %s hum: %s\r\n", dato, temp, hum);
        }
        else{
            printf("SPEC sensor is not configurated");  
        } 
}

int main() {
    bget.fall(&on_fall);
    while(1) {   
        if(pc.readable()) {
            spec.putc(pc.getc());
        }
    }
}

#endif