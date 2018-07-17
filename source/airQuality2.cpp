#include "select-demo.h"

#if DEMO == AIR_QUALITY2

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
    //
    printf("[Network] Cheking Connection...\r\n");
    // Connect to the network
    nsapi_error_t con_st = netif->connect();
    if(con_st == NSAPI_ERROR_OK) {
        printf("[Network] Connected to Network successfully\r\n");
        network_ok = true;
    } else {
        printf("[Network] Connection to Network Failed %d!\r\n", con_st);
    }
    errorled = 1;
    myled = 1;    

    // Set and start the ticker
    airTicker.attach(&flipFlags, MEASUREMENT_PERIOD); // the address of the function to be attached and the interval

    // Shinyei interrupts
    mypin.rise(&on_rise);
    mypin.fall(&on_fall);
    // spin in a main loop. flipper will interrupt it to call flip
    //get token 
    while(1) {
        if(flag_gas) {
            //printf("\r\nIn flag_gas\r\n");
            getGasConcentration(gasConc, &measure, &concentration, &temperature, &humidity, &counter_spec);
            getDust(&pm,&counter_pm);
            if(network_ok && counter >= 1){ // send to sentilo each 1 minutes...
                
                std::ostringstream ss_pm25;
                std::ostringstream ss_pm10;
                std::ostringstream ss_o3;
                std::ostringstream ss_so2;
                std::ostringstream ss_co;
                std::ostringstream ss_no2;
                std::ostringstream ss_IAQ;

                std::ostringstream ss_temp;
                std::ostringstream ss_hum;
                std::ostringstream ss_dir;
                std::ostringstream ss_spe;
                std::ostringstream ss_press;
                std::ostringstream ss_precip;

                long s_o3= -9999;
                long s_so2= -9999;
                long s_co= -9999;
                long s_no2= -9999;
                long s_IAQ= -9999;
                long s_pm10= -9999;

                // ambientales 
                long s_dir= -9999;
                long s_spe= -9999;
                long s_press= -9999;
                long s_precip= -9999;

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
                // ambientales
                ss_temp << temperature;
                ss_hum << humidity;
                ss_dir << s_dir;
                ss_spe << s_spe;
                ss_press << s_press;
                ss_precip << s_precip;

                // concentraciones 
                ss_pm25 << pm;
                ss_pm10 << s_pm10;
                ss_o3 << s_o3;
                ss_so2 << s_so2;
                ss_co  << s_co;
                ss_no2 << s_no2;
                ss_IAQ << s_IAQ;

                
                // ambientales
                std::string val_temp(ss_temp.str());
                std::string val_hum(ss_hum.str());
                std::string val_dir(ss_dir.str());
                std::string val_spe(ss_spe.str());
                std::string val_press(ss_press.str());
                std::string val_precip(ss_precip.str());

                // concentraciones 
                std::string val_pm(ss_pm25.str());
                std::string val_pm10(ss_pm10.str());
                std::string val_o3(ss_o3.str());
                std::string val_so2(ss_so2.str());
                std::string val_co(ss_co.str());
                std::string val_no2(ss_no2.str());
                std::string val_iaq(ss_IAQ.str());


                std::string atributo = "{\"data\":{\"contaminants\":{\"co\":" + val_co +",\"no\": " + val_no2 +",\"nox\": " + val_no2 +",\"no2\": " + val_no2 +",\"o3\": " + val_o3 +",\"pm10\": " + val_pm10 +",\"pm25\": " + val_pm +",\"so2\": " + val_so2 +",\"iaq\": " + val_iaq +"},\"wheater\": {\"wind_dir\": " + val_dir +",\"wind_hum\": " + val_hum +",\"wind_spe\": " + val_spe +",\"wind_tem\": " + val_temp +",\"pressure\": " + val_press +",\"precipitation\": " + val_precip +"}}}";

                getToken(atributo, &ack);
                printf("[System]  Appending file...\r\n");
                // long %ld double %lf
                // fprintf(fd, ",o3,so2,co,no2,IAQ,error,pm25,pressure,wind_Hum,wind_Tem,Sent\r\n");
                fprintf(fd, "%ld,%ld,%ld,%ld,%ld,%ld,%d,%lf,-9999,%ld,%ld,%d\r\n",samples,s_o3,s_so2,s_co,s_no2,s_IAQ,conc_error,pm,humidity,temperature,ack);
                printf("[System]  File closed.\r\n");
                fclose(fd);
                errorled = (ack == 2);
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



 
void dump_token(HttpResponse* res, std::string atributos, int *counter) {
    printf("[Network] token status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());
    char token[42];  // where we will put a copy of the input
    strncpy(token, res->get_body_as_string().c_str() + 17, 40);
    token[40]='\0';
    printf("[Network] the token is: %s \r\n", token);
    sendUpdate(atributos, token, counter);
}

void dump_attrs(HttpResponse* res){
    printf("[Network] update status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());
}

void getToken(std::string atributos, int *counter) {
    HttpRequest* tok_req = new HttpRequest(netif, HTTP_POST, IDM_ADDR);
    tok_req->set_header("Content-Type", "application/x-www-form-urlencoded");
    tok_req->set_header("Authorization", BASE64);
    HttpResponse* tok_res = tok_req->send(BODY, strlen(BODY));
    if (!tok_res) {
        printf("[Network] Error token...\r\n");
        printf("[Network] HttpRequest failed (error code %d)\r\n", tok_req->get_error());
        errorled = 0;
    }
    else{
        myled = 0;
        *counter = *counter + 1;
    }
    dump_token(tok_res, atributos,counter);
    delete tok_req;
}

void sendUpdate(std::string atributos, char* token, int *counter) {
    HttpRequest* upd_req = new HttpRequest(netif, HTTP_PATCH, PEP_UPDT);
    upd_req->set_header("Content-Type", "application/json");
    upd_req->set_header("Accept", "application/json");
    upd_req->set_header("Fiware-ServicePath", "/Nodes");
    upd_req->set_header("X-Auth-Token", token);
    HttpResponse* upd_res = upd_req->send(atributos.c_str(), strlen(atributos.c_str()));
    if (!upd_res) {
        printf("[Network] Error attrs...\r\n");
        printf("[Network] HttpRequest failed (error code %d)\r\n", upd_req->get_error());
        errorled = 0;
    }
    else{
        myled = 0;
        *counter = *counter + 1;
    }
    dump_attrs(upd_res);
    delete upd_req;
}

#endif