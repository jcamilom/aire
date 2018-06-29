#include "select-demo.h"

#if DEMO == AIR_QUALITY

#include "mbed.h"
#include "http_request.h"
#include "EthernetInterface.h"
#include <string>

#define         ASCII_CR                    13
#define         MEASUREMENT_PERIOD          10.0
#define         ID_03                       "092117010705,"
#define         ID_SO2                      "081116050515,"
#define         ID_CO                       "041117010433,"
#define         ID_NO2                      "091117010115,"
#define         ID_IAQ                      "081216010737,"

// A struct for the SentiloServer object
typedef struct SentiloServerSt {
    std::string address;        // The Sentilo server address
    std::string providerId;     // Provider's id
    std::string token;          // Authorization token
} SentiloServer;

// Functions declaration
void getGasConcentration(char *);
void dump_response(HttpResponse *);
void sendObservation(SentiloServer &, std::string, std::string);

#endif
