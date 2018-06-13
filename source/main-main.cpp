#include "select-demo.h"
#include "sentilo.h"
#include "mbed.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string.h>

#if DEMO == DEMO_MAIN
#define ENTER    13


InterruptIn mypin(D3);  // shinyei conf 1: GND   3:  5v   4: D3
DigitalOut errorled(LED1);
DigitalOut myled(LED3);
Serial pc(USBTX, USBRX); // tx, rx
Serial spec(D1, D0);  // tx, rx   //spec conf  1: 3.3  3: gnd  6: D0 7: D1


Timer t;
int a;
int sample = 30000;
int step=0;
double perc = 0;
double concentration = 0;
double pm=0;
char gas[16];
char dato[8];
char temp[4];
char hum[4];
long concentracion=0;
long temperatura=0;
long humedad=0;
int n_dust=0;
int n_spec=0;

std::ostringstream ss1;
std::ostringstream ss2;
std::ostringstream ss3;
std::ostringstream ss4;


long my_getnbr(char *str){
  long result=0;
  long puiss=1;
  while (('-' == (*str)) || ((*str) == '+'))
    {
      if (*str == '-')
        puiss = puiss * -1;
      str++;
    }
  while ((*str >= '0') && (*str <= '9'))
    {
      result = (result * 10) + ((*str) - '0');
      str++;
    }
  return (result * puiss);}

//events handlers
EventQueue queue;
void handler(int count);
Event<void(int)> event(&queue, handler);

void handler_dust() {
    t.stop();
    a = t.read_ms();
    perc = (((double)a)/sample)*100;
    concentration = 1.1*pow(perc,3) + 3.8*pow(perc,2) + 520*perc + 0.62;
    printf("The low state time was %d ms\r\n",a);
    printf("The low state percent was %lf \r\n", perc);
    printf("concentration: %lf pcs/0.01cf \r\n", concentration);
    t.reset();
    pm =  ((pm*n_dust)+concentration)/(n_dust+1);
    n_dust +=1;
    return;}
void handler_spec() {
    printf("sending enter\r\n");
    spec.putc(ENTER); //macro
    spec.scanf("%15s", gas);
    spec.scanf("%7s",  dato);
    spec.scanf("%4s",  temp);
    spec.scanf("%4s",  hum);
    if (strcmp(gas, "092117010705,") == 0){
        printf("O3: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
    } 
    else if (strcmp(gas, "081116050515,") == 0){
        printf("SO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
    } 
    else if (strcmp(gas, "041117010433,") == 0){
        printf("CO: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
    }
    else if (strcmp(gas, "091117010115,") == 0){
        printf("NO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
    } 
    else if (strcmp(gas, "081216010737,") == 0){
        printf("IAQ: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
    }
    else{
        printf("SPEC sensor is not configurated");  
    }
    return;}
void handler_send() {
    printf("step: %d...\r\n",step);
    if(step>=120){
        ss1 << concentracion;
        ss2 << temperatura;
        ss3 << humedad;
        ss4 << pm;
        std::string val1(ss1.str());
        std::string val2(ss2.str());
        std::string val3(ss3.str());
        std::string val4(ss4.str());
        SentiloServer sServer = {"sistemic.udea.edu.co:9091"};
        Provider provider = {"udeaProvider", "f4b92b05becd3b0b4d894ed78ffd468126a2622011a5a5aaf3165119c24431b7"};
        Sensor sensors[4] = {Sensor("spec"), Sensor("temp"), Sensor("hum"), Sensor("pm")};
        sensors[0].setValue(val1);
        sensors[1].setValue(val2);
        sensors[2].setValue(val3);
        sensors[3].setValue(val4);
        Sensor *pSensors;
        pSensors = sensors;
        // Create a component for the air measuring system
        Component airComponent ("udeaComponent", sServer, provider, pSensors, 4);
        int rt = airComponent.sendSensorsObservations();
        if(rt == 0) {
           step = 0;
           errorled = 1;
           concentracion = 0;
           temperatura = 0;
           humedad = 0;
           n_dust = 0;
           n_spec = 0;
           pm = 0;
        } else {
            printf("Network error,we will try again in 1 minute...\r\n");
            step -= 2;
            errorled = 0;
        }
    }
    step +=1;
}
void handler(int count) {
    handler_dust();
    handler_spec();
    handler_send();
}

void on_fall() {
    t.start();
    myled = mypin;
}

void on_rise() {
    t.stop();
    myled = mypin;
}

void Thread_main(void) {
   event.post(1);
}
 
int main() {
    errorled = 1;
    myled = 1;
	// interrupts
	mypin.rise(&on_rise);
	mypin.fall(&on_fall);
 
	// threads
    Thread event_thread;
    event.delay(100);       // Starting delay
    event.period(sample);      // Delay between each evet
    
    event_thread.start(callback(Thread_main));
    // infinite loop of the events
    queue.dispatch(-1);
    // run threads
    event_thread.join();
}

#endif