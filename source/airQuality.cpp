#include "select-demo.h"

#if DEMO == AIR_QUALITY

#include "airQuality.h"

//General
long samples = 0;

// EthernetInterface object
EthernetInterface eth;
NetworkInterface* netif = &eth;
int counter;
int ack = 0;

// K64F terminals
SDBlockDevice bd(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");
InterruptIn mypin(D3);  // shinyei conf 1: GND   3:  5v   4: D3
DigitalOut errorled(LED1);
DigitalOut myled(LED3);
Serial pc(USBTX, USBRX); // tx, rx


//Shinyei variables
Timer timer_Sh;
double pm =0;
int counter_pm;

//SPEC variables
int measure;
int counter_spec;
long concentration;
long temperature;
long humidity;



// Ticker to take measurements repeatedly
Ticker airTicker;

// Interrupt flags
bool flag_gas = true;


// Callback function called by the tikcer 
void flipFlags() {
    flag_gas = true;
    //printf("\r\nIn ticker callback\r\n");
}

// Callback functions for pwm read
void on_fall() {
    timer_Sh.start();
    myled = mypin;
}

void on_rise() {
    timer_Sh.stop();
    myled = mypin;
}
 
int main() {
    printf("[System]  Hello...\r\n");
    int error = 0;
    errorled = 1;
    myled = 1;
    printf("[System]  Mounting the filesystem on \"/fs\"...   ");
    error = fs.mount(&bd);
    return_error(error);
    printf("[System]  Creating csv labels...               ");
    FILE* fd = fopen("/fs/samples.csv", "w");
    errno_error(fd);
    fprintf(fd, ",o3,so2,co,no2,IAQ,error,pm25,pressure,wind_Hum,wind_Tem,Sent\r\n");
    printf("[System]  Running...\r\n");
    fclose(fd);    
 

    // Some valiables
    char gasConc[8] = "";           // To store the gas concentration
    bool network_ok = false;        // To store the network status


    SentiloServer sentiloServer = {"http://sistemic.udea.edu.co:9091", "udeaProvider", "f4b92b05becd3b0b4d894ed78ffd468126a2622011a5a5aaf3165119c24431b7"};

    // Connect to the network
    nsapi_error_t con_st = netif->connect();
    if(con_st == NSAPI_ERROR_OK) {
        printf("[Network] Connected to Network successfully\r\n");
        network_ok = true;
    } else {
        printf("[Network] Connection to Network Failed %d!\r\n", con_st);
    }

    // Set and start the ticker
    airTicker.attach(&flipFlags, MEASUREMENT_PERIOD); // the address of the function to be attached and the interval

	// Shinyei interrupts
	mypin.rise(&on_rise);
	mypin.fall(&on_fall);
    
    // spin in a main loop. flipper will interrupt it to call flip
    while(1) {
        //printf("\r\nIn main loop\r\n");
        if(flag_gas) {
            //printf("\r\nIn flag_gas\r\n");
            getGasConcentration(gasConc, &measure, &concentration, &temperature, &humidity, &counter_spec);
            getDust(&pm,&counter_pm);
            if(network_ok && counter >= 1){ // send to sentilo each 1 minutes...
                std::ostringstream ss1;
                std::ostringstream ss2;
                std::ostringstream ss3;
                std::ostringstream ss4;
                ss1 << concentration;
                ss2 << temperature;
                ss3 << humidity;
                ss4 << pm;
                std::string val_spec(ss1.str());
                std::string val_temp(ss2.str());
                std::string val_hum(ss3.str());
                std::string val_pm(ss4.str());
                long s_o3= -9999;
                long s_so2= -9999;
                long s_co= -9999;
                long s_no2= -9999;
                long s_IAQ= -9999;
                int conc_error = 0;
                switch(measure) {
                    case 0  : s_o3 = concentration;
                    break;
                    case 1  : s_so2 = concentration;
                    break;
                    case 2  : s_co = concentration;
                    break;
                    case 3  : s_no2 = concentration;
                    break;
                    case 4  : s_IAQ = concentration;
                    break;
                    default : conc_error = 1;}
                sendObservation(sentiloServer, "spec", val_spec.c_str(),&ack);
                sendObservation(sentiloServer, "temp", val_temp.c_str(),&ack);
                sendObservation(sentiloServer, "hum", val_hum.c_str(),&ack);
                sendObservation(sentiloServer, "pm", val_pm.c_str(),&ack);
                printf("[System]  Appending file...\r\n");
                // long %ld double %lf
                // fprintf(fd, ",o3,so2,co,no2,IAQ,error,pm25,pressure,wind_Hum,wind_Tem,Sent\r\n");
                fprintf(fd, "%ld,%ld,%ld,%ld,%ld,%ld,%d,%lf,,%ld,%ld,%d\r\n",samples,s_o3,s_so2,s_co,s_no2,s_IAQ,conc_error,pm,humidity,temperature,ack);
                printf("[System]  File closed.\r\n");
                fclose(fd);
                errorled = (ack == 4);
                counter = 0;
                counter_pm = 0;
                counter_spec = 0;
                pm = 0;
                concentration = 0;
                temperature = 0;
                humidity = 0;
            }
            else{
                counter++;
            }
            flag_gas = false;
        }
        wait(0.2);
    }
}

/**
* getDust calculate the concentration of Pm using shinyei sensor the PMW signal
* conected to a digital input obtaining the  low state time in 30 secs
*/
void getDust(double *pm, int *counter){
    timer_Sh.stop();
    int timing= timer_Sh.read_ms();
    double perc = (((double)timing)/30000)*100;
    double pm_sample = (1.1*pow(perc,3) + 3.8*pow(perc,2) + 520*perc + 0.62)*0.283;
    *pm =  (((*pm)*(*counter))+pm_sample)/((*counter)+1);
    *counter = *counter + 1;
    printf("[SHINYEI] low state: %d (%lf %%) = %lf  Avr = %lf pcs/L.\r\n", timing, perc, pm_sample, *pm);
    timer_Sh.reset();
}

/**
* getGasConcentration captures the input from the SPEC sensor connected
* to the serial interface. The gas concentration is stored as a c-string
* in the passed char*.
*
* @param[in] char* gasConc: the c-string(char*) to store the captured value
*/

void getGasConcentration(char *gasCon, int *measure, long *concentration, long *temperature, long *humidity, int *counter) {
    Serial specSensor(D1, D0);      // To communicate with the spec sensor
    //printf("\r\nIn fucking getGasConcentration!!!\r\n");
    char gasID[16];
    char temp[4];
    char hum[4];
    // Ask the SPEC sensor for data
    specSensor.putc(ASCII_CR);
    // Read the response data
    specSensor.scanf("%15s", gasID);
    specSensor.scanf("%7s",  gasCon);
    specSensor.scanf("%4s",  temp);
    specSensor.scanf("%4s",  hum);

    if (strcmp(gasID, ID_03) == 0){
        printf("[SPEC]    O3: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
        *measure = 0;
    }
    else if (strcmp(gasID, ID_SO2) == 0){
        printf("[SPEC]    SO2: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
        *measure = 1;
    } 
    else if (strcmp(gasID, ID_CO) == 0){
        printf("[SPEC]    CO: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
        *measure = 2;
    }
    else if (strcmp(gasID, ID_NO2) == 0){
        printf("[SPEC]    NO2: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
        *measure = 3;
    } 
    else if (strcmp(gasID, ID_IAQ) == 0){
        printf("[SPEC]    IAQ: %s temp: %s hum: %s\r\n", gasCon, temp, hum);
        *measure = 4;
    }
    else{
        printf("[SPEC] SPEC sensor is not configurated");  
        *measure = 5;
    }
    if (*measure <5){
        *concentration =  (((*concentration)*(*counter))+ my_long(gasCon))/((*counter)+1);
        *temperature =  (((*temperature)*(*counter))+ my_long(temp))/((*counter)+1);
        *humidity =  (((*humidity)*(*counter))+ my_long(hum))/((*counter)+1);
        *counter = *counter + 1;
    }


    // Remove the ',' at the end of the gasConc
    for(int i = 0; gasCon[i] != '\0'; i++) {
		//printf("char #%d: %c\r\n", i, gasCon[i]);
        if(gasCon[i] == ',') {
            gasCon[i] = '\0';
        }
	}
}

/**
* dump_response prints out the HttpResponse.
*
* @param[in] HttpResponse* res: the response to be printed
*/
void dump_response(HttpResponse* res) {
    printf("[Network] Status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());
}

/**
* sendObservation sends an observation to the sentilo plataform.
*
* @param[in] SentiloServer &sentiloServer: the struct with the server info
* @param[in] std::string sensorID: the sensor ID
* @param[in] std::string value: the value of the observation to be sent
*/
void sendObservation(SentiloServer &sentiloServer, std::string sensorID, std::string value, int *ack) {
    
    // Build the URL Request
    std::string reqURL (sentiloServer.address + "/data/" + sentiloServer.providerId + "/" + sensorID + "/" + value);
    
    // PUT request to publish an observabtion on Sentilo
    HttpRequest* put_req = new HttpRequest(netif, HTTP_PUT, reqURL.c_str());
    //put_req->set_header("Content-Type", "application/json");
    put_req->set_header("IDENTITY_KEY", sentiloServer.token.c_str());

    //const char body[] = "{\"hello\":\"world\"}";

    HttpResponse* put_res = put_req->send();//send(body, strlen(body));
    if (!put_res) {
        printf("HttpRequest failed (error code %d)\r\n", put_req->get_error());
        //return 1;
    }
    else{
        *ack = *ack + 1;
    }
    dump_response(put_res);
    delete put_req;
}

#endif
