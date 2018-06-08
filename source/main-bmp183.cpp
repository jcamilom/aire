#include "select-demo.h"

#if DEMO == DEMO_BMP183

#include "mbed.h"
 
SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D2);
 
int main() {
    // Chip must be deselected
    cs = 1;

    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(5000000);
 
    // Select the device by seting chip select low
    cs = 0;
 
    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0xD0 | 0x80);
 
    // Send a dummy byte to receive the contents of the WHOAMI register
    uint8_t whoami = spi.write(0x00);
    printf("WHOAMI register = 0x%X\n", whoami);
 
    // Deselect the device
    cs = 1;
}

#endif
