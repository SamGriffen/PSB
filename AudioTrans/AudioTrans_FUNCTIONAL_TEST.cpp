#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "NRF24.h"

#define PA_LVL RF24_PA_LOW // RF24_PA_MAX is default.
#define DATA_RATE RF24_2MBPS // Define the rate that the module will transfer data

#define CE_PIN  7
#define CSN_PIN 8

// A single sample of audio. This variable also defines the size of an nRF24 packet
uint16_t payload = 0;

int main()
{
    stdio_init_all(); // init necessary IO for the RP2040
    
    NRF24 nrf(spi0, CSN_PIN, CE_PIN); // pin numbers connected to the radio's CE and CSN pins (respectively)

    nrf.config(); // Configure NRF24 module
    nrf.tx_mode(); // Set to transmit mode

    /** ADC FUNCTIONALITY **/
    adc_init();
 
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
 
    // Set ADC to run
    adc_run(true);
    /** END ADC **/

    printf("Initialising Radio\n");

    /** RADIO SETUP FUNCTIONALITY **/
    // if (!radio.begin(&spi)) {
    //     printf("Radio hardware is not responding!\n");
    // }
    
    // // Set the PA Level low to try preventing power supply related problems
    // // because these examples are likely run with nodes in close proximity to
    // // each other.
    // radio.setPALevel(PA_LVL); 

    // // save on transmission time by setting the radio to only transmit the
    // // number of bytes we need to transmit a float
    // radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // // Disable auto acknowledgement on this device. Ensure this is also disabled
    // // on receiving devices
    // radio.setAutoAck(0, false);

    // // Open a writing pipe on the radio
    // radio.openWritingPipe(addresses[0]);

    // radio.stopListening();
    // /** END RADIO FUNCTIONS **/

    uint8_t data[2];
    
    data[0] = 1;
    data[1] = 4;
    
    while(1){
        // Sample ADC into payload
        // payload = adc_hw->result;

        printf("%d\n", sizeof(data));

        uint8_t written;

        written = nrf.send_message(data, 2);


        printf("SIZE: %d || WRITTEN: %d\n", sizeof(data), written);

        // data++;
        // if(data >= 255){
        //     data = 0;
        // };

        // sleep_us(100);
    }

    return 0;
}
