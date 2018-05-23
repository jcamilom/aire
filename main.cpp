#include "mbed.h"
#include "http_request.h"
#include "EthernetInterface.h"

EthernetInterface eth;

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\r\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\r\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\r\nBody (%d bytes):\r\n\r\n%s\r\n", res->get_body_length(), res->get_body_as_string().c_str());
}
 
int main() {
    NetworkInterface* netif = &eth;
    int connect_success = eth.connect();
    if(connect_success == 0) {
        printf("[EasyConnect] Connected to Network successfully\r\n");
    } else {
        printf("[EasyConnect] Connection to Network Failed %d!\r\n", connect_success);
    }


    // Do a GET request to httpbin.org
    {
        // By default the body is automatically parsed and stored in a buffer, this is memory heavy.
        // To receive chunked response, pass in a callback as last parameter to the constructor.
        HttpRequest* get_req = new HttpRequest(netif, HTTP_GET, "http://httpbin.org/status/418");

        HttpResponse* get_res = get_req->send();
        if (!get_res) {
            printf("HttpRequest failed (error code %d)\r\n", get_req->get_error());
            return 1;
        }

        printf("\r\n----- HTTP GET response -----\r\n");
        dump_response(get_res);

        delete get_req;
    }

    // POST request to httpbin.org
    {
        HttpRequest* post_req = new HttpRequest(netif, HTTP_POST, "http://httpbin.org/post");
        post_req->set_header("Content-Type", "application/json");

        const char body[] = "{\"hello\":\"world\"}";

        HttpResponse* post_res = post_req->send(body, strlen(body));
        if (!post_res) {
            printf("HttpRequest failed (error code %d)\r\n", post_req->get_error());
            return 1;
        }

        printf("\r\n----- HTTP POST response -----\r\n");
        dump_response(post_res);

        delete post_req;
    }

    wait(osWaitForever);
}
