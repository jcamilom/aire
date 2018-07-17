#include "select-demo.h"

#if DEMO == FIWARE
// #if DEMO == AIR_QUALITY

#include "airQuality.h"

//General
long samples = 0;

// EthernetInterface object
EthernetInterface eth;
NetworkInterface* netif = &eth;
bool network_ok = false;        // To store the network status
int counter;
int ack = 0;

// K64F terminals
SDBlockDevice bd(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");
InterruptIn mypin(D3);  // shinyei conf 1: GND   3:  5v   4: D3
DigitalOut errorled(LED1);
DigitalOut myled(LED3);
Serial pc(USBTX, USBRX); // tx, rx


int main() {
    printf("[Network] Generating token...\r\n");
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
    //get token 
    while(1) {
        std::string atributo = "{\"data\":{\"contaminants\":{\"co\": 8,\"no\": 7,\"nox\": 6,\"no2\": 7,\"o3\": 6,\"pm10\": 8,\"pm25\": 8,\"so2\": 6, \"iaq\": 7},\"wheater\": {\"wind_dir\": 5,\"wind_hum\": 7,\"wind_spe\": 9,\"wind_tem\": 26,\"pressure\": 6,\"precipitation\": 8}}}";
        getToken(atributo, &ack);
        wait(3600);
        errorled = 1;
        myled = 1;    
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
