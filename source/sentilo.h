#include "mbed.h"
#include "http_request.h"
#include "EthernetInterface.h"
#include <string>

class SentiloServer {
        std::string address;         // The Sentilo server address
    public:
        SentiloServer(std::string add);
        std::string getAddress() { return address;}
};

// Member functions definitions including constructor
SentiloServer::SentiloServer(std::string add) {
    address = add;
}

class Provider {
        std::string id;             // Provider's id
        std::string token;          // Authorization token
        SentiloServer *pServer;     // The Sentilo server
    public:
        Provider(std::string idArg, std::string tokenArg, SentiloServer *pServerArg);
        std::string getServerAddress() { return pServer->getAddress();}
};

Provider::Provider(std::string idArg, std::string tokenArg, SentiloServer *pServerArg) {
    id = idArg;
    token = tokenArg;
    pServer = pServerArg;
}

// This class is intended to get actual values from the sensors and return then to the Component class
class Sensor {
        std::string id;
    public:        
        std::string lastValue;
        bool lastValueOk;

        std::string getValue() {
            return "1.0";
        }

        void setID(std::string idArg) {
            id = idArg;
        }
};  

class Component {
    public:
        std::string id;             // Component's id
        Provider *pProvider;        // Pointer to the related provider
        Sensor *pSensors;           // Pointer to the first element of the array of sensors
                                    // for this component.

        Component(std::string idArg, Provider *pProviderArg, Sensor *pSensorsArg) {
            id = idArg;
            pProvider = pProviderArg;
            pSensors = pSensorsArg;
        }

        int sendSensorObservation(int idx) {
            return 0;
        }
        
};

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

    // Creates a Sentilo server
    SentiloServer server("address.com");

    // Creates a provider
    Provider provider("provname", "token", &server);

    // Create a sensor's array
    Sensor sensors[2];
    for(int i = 0; i < 2; i++) {
		sensors[i].setID("sensor" + std::to_string(i));
	}

    Sensor *pSensors;
    pSensors = sensors;

    // Create a component
    Component component("udeaComponent", &provider, pSensors);
    
    // Test the link between the provider created and the server
    std::cout << "Getting the server address from the provider :" << provider.getServerAddress() << std::endl;
    // "(*(provider.pServer)).getAddress()"" is the same as "(provider.pServer)->getAddress()"
    
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
