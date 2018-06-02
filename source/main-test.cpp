#include "select-demo.h"
#include "mbed.h"
// Creates an event bound to the specified event queue

#if DEMO == DEMO_TEST

//counter handlers
class Counter {
public:
    Counter(PinName pin) : _interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
        _interrupt.rise(callback(this, &Counter::increment)); // attach increment function of this counter instance
    }

    void increment() {
        _count++;
    }

    int read() {
        return _count;
    }

    void reset_count(){
        _count = 0;
    }

private:
    InterruptIn _interrupt;
    volatile int _count;
};

Counter counter(SW2);

//events handlers
EventQueue queue;
void handler(int count);
Event<void(int)> event(&queue, handler);

void handler(int count) {
    printf("Count so far: %d\r\n", counter.read_ms());
    counter.reset_count();
    return;
}

void Thread_main(void) {
    event.post(1);
}



// main
int main() {
    Thread event_thread;
    // The event can be manually configured for special timing requirements
    // specified in milliseconds
    event.delay(100);       // Starting delay - 100 msec
    event.period(2000);      // Delay between each evet - 200msec
    
    event_thread.start(callback(Thread_main));
    
    // Posted events are dispatched in the context of the queue's
    // dispatch function
    queue.dispatch(-1);        // Dispatch time - 400msec
    // 400 msec - Only 2 set of events will be dispatched as period is 200 msec
    
    event_thread.join();
}


#endif