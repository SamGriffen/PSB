// NRF24 code - Based on datasheet from: https://www.sparkfun.com/datasheets/Components/nRF24L01_prelim_prod_spec_1_2.pdf
#ifndef __NRF24_H_
#define __NRF24_H_
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define REG_CONFIG 0x00
#define REG_ENAA 0x01

#define RF_CH 0x05

#define RF_SETUP 0x06

#define STATUS 0x07

#define RX_ADDR_P0 0x0A
#define TX_ADDR 0x10
#define RX_PW_P0 0x11

#define FIFO_STATUS 0x17

#define SPI_BAUD 921600

class NRF24
{
    private:    // Vars
        spi_inst_t *port;
        uint16_t csn;
        uint16_t ce;
    public:     // Vars

    private:    // Methods
        void csn_low(){gpio_put(csn, 0);}
        void csn_high(){gpio_put(csn, 1);}
        void ce_low(){gpio_put(ce, 0);}
        void ce_high(){gpio_put(ce, 1);}

    public:     // Methods
        uint8_t read_reg(uint8_t reg);  // Read a register from the NRF24 device

        void write_reg( uint8_t reg, uint8_t data); // Write [data] to register [reg] on NRF24 device
        void write_reg( uint8_t reg, uint8_t *data, uint8_t size); // Write [data] to register [reg] on NRF24 device

        void config();  // Configure NRF24 devices

        void tx_mode(); // Method to put device in transmit mode

        void rx_mode(); // Method to put device in rx mode 

        uint8_t send_message(uint8_t* msg, uint8_t size); // Send message pointed to by msg

        void receive_message(uint8_t* msg, uint8_t size); // Receive message. Store received message in memory pointed to by msg. Message should be size bytes long (Likely 4)

        uint8_t message_available(); // Check if a new message is available

        void set_channel(uint8_t ch); // Set the channel being transmitted/received

        NRF24(spi_inst_t *port, uint16_t csn, uint16_t ce); // Constructor
        ~NRF24();
};

#endif