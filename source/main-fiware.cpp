#include "select-demo.h"
#if DEMO == DEMO_FIWARE
#include "mbed.h"
#include "https_request.h"
#include "EthernetInterface.h"
#include <string>

 
const char SSL_CA_PEM[] = ""
/* 
"-----BEGIN CERTIFICATE REQUEST-----\n"
"MIICijCCAXICAQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUx"
"ITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZIhvcN"
"AQEBBQADggEPADCCAQoCggEBAM6xsXe5+1g6aljyfFyziPBMWCaOEVnNPnwIeN5m"
"5Dvyo0/jCLbC2M2cxAyigvWpHlrYeGDSKiLcYtPYq3T8sut2G734ulYDmgFZ5sci"
"oYmUpTo0a3/GFewCJs1B/j8MCe9rjgpEkc3qp0cB9apOUHnshAcsRjaACvP/IK0z"
"pDAxDa9R3OWAdjUmtIeV2Kwqr7FLXPoABbE7XCFLbsLsFmyioPPo3Hv/m+YktiME"
"YfNOza51nkmiUVtCKaet8tFye1ypGCDmLST1tBn3xhV0c1xm2GsZKNIkf18MKOtO"
"zj56lb6I/hXBHftQTjlni+dQ/hako6iNFZNpQ00VoZGUYzkCAwEAAaAAMA0GCSqG"
"SIb3DQEBCwUAA4IBAQC17T3B8X088ZAWideOAdsLPtRpgES+ycF6iyv/HS7EWmr6"
"n7FxhELtVyE0S6SojLRAbFXAXXwqYqQx2onUl0rw59WBsYsgRmoC8tQOpcL35cYu"
"KwoT3ri1OmBnd4QPtzIOwcF6Q6hTZ1p3Zak13S2MC5Ith8JHwBwlWJVHGAIZS7mk"
"2liZIgs34tEzXv09biw7bccANifjp3jy20enbMAerY0w7Q1eTjdsXbX3tuNUGwXm"
"guziNCgmnK7t2JH5SOCNdMxE4UvuwPqRcI1kJrhPxWWBBWyBxTw+e12J3qNsowhk"
"iHJStXKeruSdjKXb4OBZypcV7hFUqgETtLKpMZ5H"
"-----END CERTIFICATE REQUEST-----" */;


void get_response(HttpResponse* res) {
    printf("Status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\r\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\r\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\r\nBody (%d bytes):\r\n\r\n%s\r\n", res->get_body_length(), res->get_body_as_string().c_str());
}

// Network interface
EthernetInterface wire;
NetworkInterface* netwire = &wire;
// Send info
int main() {
    printf("HTTPS request token...\r\n");
    
    int connect_success_f = wire.connect();
    if(connect_success_f == 0) {
        printf("[Network] Connected to Network successfully\r\n");
    } else {
        printf("[Network] Connection to Network Failed %d!\r\n", connect_success_f);
    }

    // POST request to httpbin.org
    {
        printf("\n----- HTTPS POST request -----\r\n");
        HttpsRequest* post_req = new HttpsRequest(netwire, SSL_CA_PEM, HTTP_POST, "http://192.168.1.51:3000");
        post_req->set_header("Content-Type", "application/x-www-form-urlencoded");
        post_req->set_header("Authorization", "Basic M2RlYjhiZGMtMTUzNy00ZDgwLTg4MjMtOTk5YjNkZDcxNDYxOmFjN2VhYjFmLTA5MmYtNDYwMy05NDgwLTYwNjk2Nzk4N2Q4OA==");
 
        const char body[] = "grant_type=password&username=admin@test.com&password=1234&client_id=3deb8bdc-1537-4d80-8823-999b3dd71461&client_secret=ac7eab1f-092f-4603-9480-606967987d88";
 
        HttpResponse* post_res = post_req->send(body, strlen(body));
        if (!post_res) {
            printf("HttpRequest failed (error code %d)\r\n", post_req->get_error());
            return 1;
        }
 
        printf("\n----- HTTP POST response -----\r\n");
        get_response(post_res);
 
        delete post_req;
    }
    wait(osWaitForever);
 } 
 
#endif