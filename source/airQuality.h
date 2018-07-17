#include "select-demo.h"

#if DEMO == FIWARE
// #if DEMO == AIR_QUALITY

#include "mbed.h"
#include "http_request.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "EthernetInterface.h"
#include <string>
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <errno.h>

#define         ENTER                       13
#define         ASCII_CR                    13
#define         MEASUREMENT_PERIOD          30.0
#define         ID_03                       "092117010705,"
#define         ID_SO2                      "081116050515,"
#define         ID_CO                       "041117010433,"
#define         ID_NO2                      "091117010115,"
#define         ID_IAQ                      "081216010737,"
// fiware
#define         IDM_ADDR                    "http://192.168.1.54:3000/oauth2/token"
#define         PEP_UPDT                    "http://192.168.1.54:8585/v2/entities/freedom_01/attrs?options=keyValues"
#define         BASE64                      "Basic ZDFkOWJlNWYtNGQwMy00NTMzLWExZjgtZjA2Y2EwY2I2YTBiOmQyNWQ4YmRhLWVkNzgtNDIwMy1iYWY2LTczODM1YTdlYjUwNQ=="
// #define         BODY                        "grant_type=password&username=kl27z_01@test.com&password=freedom_01&client_id=d1d9be5f-4d03-4533-a1f8-f06ca0cb6a0b&client_secret=d25d8bda-ed78-4203-baf6-73835a7eb505"
#define         BODY                        "grant_type=password&username=joansriozz@gmail.com&password=Blink182&client_id=d1d9be5f-4d03-4533-a1f8-f06ca0cb6a0b&client_secret=d25d8bda-ed78-4203-baf6-73835a7eb505"

// A struct for the SentiloServer object
typedef struct SentiloServerSt {
    std::string address;        // The Sentilo server address
    std::string providerId;     // Provider's id
    std::string token;          // Authorization token
} SentiloServer;

// A struct for the SentiloServer object

// Functions declaration
void getGasConcentration(char *, int *measure, long *concentration, long *temperature, long *humidity, int *counter);
void getDust(double *pm, int *counter);
void dump_response(HttpResponse *);
void dump_token(HttpResponse *, std::string);
void dump_attrs(HttpResponse *);
void sendObservation(SentiloServer &, std::string, std::string, int *ack);
void sendUpdate(std::string, char *);
void getToken(std::string);

// Storage error functions

void return_error(int ret_val){
  if (ret_val)
    printf("Failure. %d\r\n", ret_val);
  else
    printf("done.\r\n");
}

void errno_error(void* ret_val){
  if (ret_val == NULL)
    printf(" Failure. %d \r\n", errno);
  else
    printf(" done.\r\n");
}

// long conversor

long my_long(char *str){
  long result=0;
  long puiss=1;
  while (('-' == (*str)) || ((*str) == '+'))
    {
      if (*str == '-')
        puiss = puiss * -1;
      str++;
    }
  while ((*str >= '0') && (*str <= '9'))
    {
      result = (result * 10) + ((*str) - '0');
      str++;
    }
  return (result * puiss);}

#endif
