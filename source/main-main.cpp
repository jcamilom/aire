#include "select-demo.h"
#if DEMO == DEMO_MAIN
#include "functions.h"
#include "sentilo.h"
#include "mbed.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string.h>

#define ENTER    13


// K64F terminals
SDBlockDevice bd(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");
InterruptIn mypin(D3);  // shinyei conf 1: GND   3:  5v   4: D3
DigitalOut errorled(LED1);
DigitalOut myled(LED3);
Serial pc(USBTX, USBRX); // tx, rx
Serial spec(D1, D0);  // tx, rx   //spec conf  1: 3.3  3: gnd  6: D0 7: D1


Timer t;
int var;
int timing;
int sample = 0;
int period = 30000;
int step=0;
int conc_error = 0;
double perc = 0;
double pm_sample = 0;
double pm=0;
char gas[16];
char dato[8];
char temp[4];
char hum[4];
long s_o3=0;
long s_so2=0;
long s_co=0;
long s_no2=0;
long s_IAQ=0;
long concentracion=0;
long temperatura=0;
long humedad=0;
int n_dust=0;
int n_spec=0;

std::ostringstream ss1;
std::ostringstream ss2;
std::ostringstream ss3;
std::ostringstream ss4;
SentiloServer sServer = {"sistemic.udea.edu.co:9091"};
Provider provider = {"udeaProvider", "f4b92b05becd3b0b4d894ed78ffd468126a2622011a5a5aaf3165119c24431b7"};
Sensor sensors[4] = {Sensor("spec"), Sensor("temp"), Sensor("hum"), Sensor("pm")};
Sensor *pSensors = sensors;
// Create a component for the air measuring system
Component airComponent ("udeaComponent", sServer, provider, pSensors, 4);

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

void handler(int count) {
    t.stop();
    timing= t.read_ms();
    perc = (((double)timing)/period)*100;
    pm_sample = 1.1*pow(perc,3) + 3.8*pow(perc,2) + 520*perc + 0.62;
    printf("The low state time was %d ms\r\n",timing);
    printf("The low state percent was %lf \r\n", perc);
    printf("pm_sample: %lf pcs/0.01cf \r\n", pm_sample);
    t.reset();
    pm =  ((pm*n_dust)+pm_sample)/(n_dust+1);
    n_dust +=1;
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
        var = 0;
    } 
    else if (strcmp(gas, "081116050515,") == 0){
        printf("SO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
        var = 1;
    } 
    else if (strcmp(gas, "041117010433,") == 0){
        printf("CO: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
        var = 2;
    }
    else if (strcmp(gas, "091117010115,") == 0){
        printf("NO2: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
        var = 3;
    } 
    else if (strcmp(gas, "081216010737,") == 0){
        printf("IAQ: %s temp: %s hum: %s\r\n", dato, temp, hum);
        concentracion = ((concentracion*n_spec)+my_getnbr(dato))/(n_spec+1);
        temperatura = ((temperatura*n_spec)+my_getnbr(temp))/(n_spec+1);
        humedad = ((humedad*n_spec)+my_getnbr(hum))/(n_spec+1);
        n_spec +=1;
        var = 4;
    }
    else{
        printf("SPEC sensor is not configurated\r\n");  
        var = 5;
    }
        printf("step: %d...\r\n",step);
    if(step>=4){
        printf("Opening and Appending to the file.\r\n");  
        FILE* fd = fopen("/fs/samples.csv", "a");
        errno_error(fd);    
        ss1 << concentracion;
        ss2 << temperatura;
        ss3 << humedad;
        ss4 << pm;
        std::string val1(ss1.str());
        std::string val2(ss2.str());
        std::string val3(ss3.str());
        std::string val4(ss4.str());        
        sensors[0].setValue(val1);
        sensors[1].setValue(val2);
        sensors[2].setValue(val3);
        sensors[3].setValue(val4);
        s_o3= -999;
        s_so2= -999;
        s_co= -999;
        s_no2= -999;
        s_IAQ= -999;
        conc_error = 0;
        switch(var) {
            case 0  : s_o3 = concentracion;
            break;
            case 1  : s_so2 = concentracion;
            break;
            case 2  : s_co = concentracion;
            break;
            case 3  : s_no2 = concentracion;
            break;
            case 4  : s_IAQ = concentracion;
            break;
            default : conc_error = 1;}
        printf("Sending data...\r\n");
        int rt = airComponent.sendSensorsObservations();
        printf("Appending file...\r\n");
        fprintf(fd, "%d,%ld,%ld,%ld,%ld,%ld,%d,%lf,,%ld,%ld,%d\r\n",sample,s_o3,s_so2,s_co,s_no2,s_IAQ,conc_error,pm,humedad,temperatura,!(rt));
        printf("Closing file.\r\n");
        fclose(fd);
        errorled = !(rt);
        step = 0;
        concentracion = 0;
        temperatura = 0;
        humedad = 0;
        n_dust = 0;
        n_spec = 0;
        pm = 0;
    }
    else{
        step +=1;
    }
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
    printf("Hello...\r\n");
    int error = 0;
    errorled = 1;
    myled = 1;
    printf("Mounting the filesystem on \"/fs\".\r\n");
    error = fs.mount(&bd);
    return_error(error);
    printf("Creating csv labels...\r\n");
    FILE* fd = fopen("/fs/samples.csv", "w");
    errno_error(fd);
    fprintf(fd, ",o3,so2,co,no2,IAQ,error,pm25,pressure,wind_Hum,wind_Tem,Sent\r\n");
    fclose(fd);

	// interrupts
	mypin.rise(&on_rise);
	mypin.fall(&on_fall);
 
	// threads
    Thread event_thread;
    event.delay(100);       // Starting delay
    event.period(period);      // Delay between each evet
    
    event_thread.start(callback(Thread_main));
    // infinite loop of the events
    queue.dispatch(-1);
    // run threads
    event_thread.join();
}
#endif