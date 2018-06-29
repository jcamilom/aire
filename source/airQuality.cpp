#include "select-demo.h"

#if DEMO == AIR_QUALITY

#include "airQuality.h"

// Serial communication
Serial pc(USBTX, USBRX);        // To communicate with the PC

// EthernetInterface object
EthernetInterface eth;
NetworkInterface* netif = &eth;

// Ticker to take measurements repeatedly
Ticker airTicker;

// Interrupt flags
bool flag_gas = true;

// Callback function called by the tikcer 
void flipFlags() {
    flag_gas = true;
    //printf("\r\nIn ticker callback\r\n");
}
 
int main() {

    //printf("\r\nIn main\r\n");

    // Some valiables
    char gasConc[8] = "";           // To store the gas concentration
    bool network_ok = false;        // To store the network status

    SentiloServer sentiloServer = {"address", "providerId", "theToken"};

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
    
    // spin in a main loop. flipper will interrupt it to call flip
    while(1) {
        //printf("\r\nIn main loop\r\n");
        if(flag_gas) {
            //printf("\r\nIn flag_gas\r\n");
            getGasConcentration(gasConc);
            //printf("The received CO: %s\r\n", gasConc);
            if(network_ok){
                sendObservation(sentiloServer, "udeaTemp", gasConc);
            }
            flag_gas = false;
        }
        wait(0.2);
    }
}

/**
* getGasConcentration captures the input from the SPEC sensor connected
* to the serial interface. The gas concentration is stored as a c-string
* in the passed char*.
*
* @param[in] char* gasConc: the c-string(char*) to store the captured value
*/
void getGasConcentration(char *gasCon) {
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

/**
* dump_response prints out the HttpResponse.
*
* @param[in] HttpResponse* res: the response to be printed
*/
void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\r\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\r\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\r\nBody (%d bytes):\r\n\r\n%s\r\n", res->get_body_length(), res->get_body_as_string().c_str());
}

/**
* sendObservation sends an observation to the sentilo plataform.
*
* @param[in] SentiloServer &sentiloServer: the struct with the server info
* @param[in] std::string sensorID: the sensor ID
* @param[in] std::string value: the value of the observation to be sent
*/
void sendObservation(SentiloServer &sentiloServer, std::string sensorID, std::string value) {
    
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

    printf("\r\n----- HTTP PUT response -----\r\n");
    dump_response(put_res);

    delete put_req;
}

#endif
