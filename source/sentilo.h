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

void printErrorMessage(int err) {
    switch(err) {
        case NSAPI_ERROR_NO_CONNECTION:
            printf("Error: %d, %s.\r\n", err, "not connected to a network");
            break;
        case NSAPI_ERROR_DNS_FAILURE:
            printf("Error: %d, %s.\r\n\r\n", err, "DNS failed to complete successfully");
            break;
        default:
            printf("Error: %d.\r\n\r\n", err);
            break;
    }
}

// A struct for the SenstiloServer object
typedef struct SentiloServerSt {
    std::string address;        // The Sentilo server address
} SentiloServer;

// A struct for the Provider object
typedef struct ProviderSt {
    std::string id;             // Provider's id
    std::string token;          // Authorization token
} Provider;

// This class is intended to get actual values from the sensors and return then to the Component class
class Sensor {
        std::string id;
        std::string lastValue;
        bool lastValueOk;
    public:
        Sensor(std::string idArg): id(idArg) {
            lastValueOk = false; // TODO: inline???
        }

        std::string getValue() {
            return lastValue;
        }

        void setValue(std::string lastValueArg) {
            lastValue = lastValueArg;
            lastValueOk = true;
        }

        bool lastValueOK() {
            return lastValueOk;
        }

        void setLastValueErr() {
            lastValueOk = false;
        }

        std::string getID() { return id;}
};

class Component {
    public:
        std::string id;                 // Component's id
        SentiloServer sentiloServer;    // The sentiloServer member
        Provider provider;              // The provider member
        Sensor *pSensors;               // Pointer to the first element of the array of sensors
                                        // for this component.
        int nSensors;                   // Number of sensors pointed by pSensors
        NetworkInterface* netif;        // Pointer to the the network interface

        Component(std::string idArg, SentiloServer &sentiloServerArg, Provider &providerArg, Sensor *pSensorsArg, int nSensorsArg) {
            id = idArg;
            sentiloServer = sentiloServerArg;
            provider = providerArg;
            pSensors = pSensorsArg;
            nSensors = nSensorsArg;

            // EthernetInterface object
            EthernetInterface eth;

            netif = &eth;
        }
        
        nsapi_error_t initConnection(void);
        int sendSensorObservation(int idx);
        int sendSensorsObservations(void);

    private:
        nsapi_connection_status_t checkConnection(void);

};

/**
* Initialize the network connection.
*
*
*
* @param[in]
* @param[in]
*/
nsapi_error_t Component::initConnection(void) {
    nsapi_error_t con_st = netif->connect();
    if(con_st == NSAPI_ERROR_OK) {
        printf("[Network] Connected to Network successfully\r\n");
    } else {
        printf("[Network] Connection to Network Failed!\r\n");
        printErrorMessage(con_st);
    }
    return con_st;
}

/**
* Check the network connection.
*
*
*
* @param[in]
* @param[in]
*/
nsapi_connection_status_t Component::checkConnection(void) {
    nsapi_connection_status_t con_st = netif->get_connection_status();
    if(con_st == 0) {
        printf("[Network] Connection status: up\r\n");
    } else {
        printf("[Network] Connection status: no connection.\r\n");
        printErrorMessage(con_st);
    }
    return con_st;
}

/**
* Send a sensor's observation to the Sentilo platform.
*
*
*
* @param[in]
* @param[in]
*/
int Component::sendSensorObservation(int idx) {
    // Build the URL Request
    std::string reqURL (sentiloServer.address + "/data/" + provider.id + "/" + (pSensors + idx)->getID() + "/" + (pSensors + idx)->getValue());

    // PUT request to publish an observabtion on Sentilo
    {
        HttpRequest* put_req = new HttpRequest(netif, HTTP_PUT, reqURL.c_str());
        //put_req->set_header("Content-Type", "application/json");
        put_req->set_header("IDENTITY_KEY", provider.token.c_str());

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

/**
* Send all sensor's observations to the Sentilo platform.
*
*
*
* @param[in]
* @param[in]
*/
int Component::sendSensorsObservations(void) {
    // TODO: fix behavior of get_connection_status
    //int con_st = checkConnection();

    // If connection ok start sending observations.
    // Perhaps macros need to be defined.

    int resp_success = 0;

    for(int i = 0; i < nSensors; i++) {
		if((pSensors + i)->lastValueOK()) {
            // If status ok, send data
            resp_success = sendSensorObservation(i);
            if(resp_success == 1) return 1;
        } else {
            // TODO: send error value
        }
	}
    return 0;
}
