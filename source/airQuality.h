#include "select-demo.h"

#if DEMO == AIR_QUALITY

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


#define         ASCII_CR                    13
#define         MEASUREMENT_PERIOD          30.0
#define         ID_03                       "092117010705,"
#define         ID_SO2                      "081116050515,"
#define         ID_CO                       "041117010433,"
#define         ID_NO2                      "091117010115,"
#define         ID_IAQ                      "081216010737,"
#define         ENTER    13

// A struct for the SentiloServer object
typedef struct SentiloServerSt {
    std::string address;        // The Sentilo server address
    std::string providerId;     // Provider's id
    std::string token;          // Authorization token
} SentiloServer;

// Functions declaration
void getGasConcentration(char *, int *measure, long *concentration, long *temperature, long *humidity, int *counter);
void getDust(double *pm, int *counter);
void dump_response(HttpResponse *);
void sendObservation(SentiloServer &, std::string, std::string, int *ack);

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
