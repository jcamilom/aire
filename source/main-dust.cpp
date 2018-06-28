#include "select-demo.h"
#if DEMO == DEMO_DUST

#include "mbed.h"
#include <math.h>

InterruptIn mypin(D3);
DigitalOut myled(LED1);

Timer t;
int a;
int sample = 30000;
double perc = 0;
double #include "select-demo.h"

#if DEMO == DEMO_DUST

#include "mbed.h"
#include <math.h>

InterruptIn mypin(D3);
DigitalOut myled(LED1);

Timer t;
int a;
int sample = 30000;
double perc = 0;
double concentration = 0;


//events handlers
EventQueue queue;
void handler(int count);
Event<void(int)> event(&queue, handler);

void handler(int count) {
	t.stop();
	a = t.read_ms();
	perc = (((double)a)/sample)*100;
	concentration = 1.1*pow(perc,3) + 3.8*pow(perc,2) + 520*perc + 0.62;
	printf("The low state time was %d ms\r\n",a);
	printf("The low state percent was %lf \r\n", perc);
	printf("concentration: %lf pcs/0.01cf \r\n", concentration);
	t.reset();
    return;
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
concentration = 0;


//events handlers
EventQueue queue;
void handler(int count);
Event<void(int)> event(&queue, handler);

void handler(int count) {
	t.stop();
	a = t.read_ms();
	perc = (((double)a)/sample)*100;
	concentration = 1.1*pow(perc,3) + 3.8*pow(perc,2) + 520*perc + 0.62;
	printf("The low state time was %d ms\r\n",a);
	printf("The low state percent was %lf \r\n", perc);
	printf("concentration: %lf pcs/0.01cf \r\n", concentration);
	t.reset();
    return;
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
