#include "mbed.h"
#include "http_request.h"
#include "EthernetInterface.h"
#include <string>


void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\r\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\r\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\r\nBody (%d bytes):\r\n\r\n%s\r\n", res->get_body_length(), res->get_body_as_string().c_str());
}

/**
* Send a sensor's observation to the Sentilo platform.
*
*
*
* @param[in]
* @param[in]
*/
int sendObservation(string serverAddress, string providerID, string sensorID, string value, string token) {
    
    // Build the URL Request
    string reqURL (serverAddress + "/data/" + providerID + "/" + sensorID + "/" + value);

    // EthernetInterface object
    EthernetInterface eth;

    NetworkInterface* netif = &eth;
    int connect_success = eth.connect();
    if(connect_success == 0) {
        printf("[Network] Connected to Network successfully\r\n");
    } else {
        printf("[Network] Connection to Network Failed %d!\r\n", connect_success);
    }


    // PUT request to publish an observabtion on Sentilo
    {
        HttpRequest* put_req = new HttpRequest(netif, HTTP_PUT, reqURL.c_str());
        //put_req->set_header("Content-Type", "application/json");
        put_req->set_header("IDENTITY_KEY", token.c_str());

        //const char body[] = "{\"hello\":\"world\"}";

        HttpResponse* put_res = put_req->send();//send(body, strlen(body));
        if (!put_res) {
            printf("HttpRequest failed (error code %d)\r\n", put_req->get_error());
            return 1;
        }

        printf("\r\n----- HTTP PUT response -----\r\n");
        dump_response(put_res);

        delete put_req;
    }
    return 0;

}
