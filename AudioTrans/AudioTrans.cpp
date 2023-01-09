#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "NRF24.h"

#define PA_LVL RF24_PA_LOW // RF24_PA_MAX is default.
#define DATA_RATE RF24_2MBPS // Define the rate that the module will transfer data

#define CE_PIN  7
#define CSN_PIN 8

// Buffer for 
uint8_t* payload;

// NUMBER OF CLOCK CYCLES BETWEEN ADC SAMPLES (Based on nc = fc/fs = 48MHz/44.1kHz. Change this for higher sample frequency)
#define ADC_SAMP_DIV 1088 // should be 1088

// Flag for ADC interrupt
static bool irq_flag = 0;

// Handler for ADC interrupts
// static void adc_irq(void){
//     irq_flag = 1;
//     return;
// }

// uint8_t check_stat = 0;

int main()
{
    stdio_init_all(); // init necessary IO for the RP2040


    NRF24 nrf(spi0, CSN_PIN, CE_PIN); // pin numbers connected to the radio's CE and CSN pins (respectively)

    nrf.config(); // Configure NRF24 module
    nrf.tx_mode(); // Set to transmit mode

    // check_stat = nrf.read_reg(REG_CONFIG);

    /** ADC FUNCTIONALITY **/
    adc_init();
 
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
 
    // Setup the ADC FIFO to receive samples
    adc_fifo_setup(true, true, 4, false, false);

    // // Set up the ADC sample frequency
    adc_set_clkdiv(ADC_SAMP_DIV);

    // // Set ADC to run
    adc_run(true);
    /** END ADC **/

    while(1){

        if(adc_fifo_get_level() >= 4){
            // Read four samples from ADC fifo into payload
            for(uint8_t i = 0; i < 8; i+=2){
                payload[i] = adc_fifo_get() & 0x00ff;
                payload[i+1] = adc_fifo_get() & 0xff00;
            }

            // Transmit the sample
            nrf.send_message(payload, 4);
        }
    }

    return 0;
}
