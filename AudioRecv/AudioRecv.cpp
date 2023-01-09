#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "NRF24.h"

#define PA_LVL RF24_PA_LOW // RF24_PA_MAX is default.
#define DATA_RATE RF24_2MBPS // Define the rate that the module will transfer data

#define CE_PIN  7
#define CSN_PIN 8

// A single sample of audio. This variable also defines the size of an nRF24 packet
uint8_t payload[2];

int main()
{
    stdio_init_all(); // init necessary IO for the RP2040

    // /** RADIO SETUP FUNCTIONALITY **/
    NRF24 nrf(spi0, CSN_PIN, CE_PIN); // pin numbers connected to the radio's CE and CSN pins (respectively)

    nrf.config(); // Configure NRF24 module
    nrf.rx_mode(); // Set to transmit mode
    /** END RADIO FUNCTIONS **/

    /** PWM FUNCTIONS **/

    uint8_t PWM_pin = 27;

    // Set GPIO PWM_pin as PWM out
    gpio_set_function(PWM_pin, GPIO_FUNC_PWM);

    // Get the slice number running on GPIO PWM_pin
    uint slice_num = pwm_gpio_to_slice_num(PWM_pin);

    // Get a PWM configuration
    pwm_config config = pwm_get_default_config();

    // Set the PWM to wrap at 2^12, the same resolution as the ADC
    pwm_config_set_wrap(&config, 4095);

    // pwm_config_set_clkdiv(&config, 4.f);

    // Set the PWM slice running
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(PWM_pin, 2048);

    int PWM_level = 0;

    /** END PWM FUNCTIONS **/

    while(1){
        // Read radio packet into payload
        // if(nrf.message_available()){
        //     nrf.receive_message(payload);

        //     printf("READING %d BYTES %d %d\n", sizeof(payload), payload[0], payload[1]);
        //     // printf("RECEIVED\n");
        //     // Write payload into PWM
        //     // pwm_set_gpio_level(PWM_pin, payload);
        // }
        
        uint8_t res = nrf.read_reg(STATUS);

        // printf("%d\n", res);

        uint8_t msg_avl = nrf.message_available();

        uint8_t buf[2];

        if(msg_avl){
            nrf.receive_message(buf);

            printf("%d\n", buf[1]);
        }
    }

    return 0;
}
