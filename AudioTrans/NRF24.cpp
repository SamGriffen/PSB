#include "NRF24.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

NRF24::NRF24(spi_inst_t *port, uint16_t csn, uint16_t ce)
{
    this->port = port;
    this->csn = csn;
    this->ce = ce;

    spi_init(this->port, SPI_BAUD ); // Initialise communication with NRF24 at 1MHz
    gpio_set_function(16, GPIO_FUNC_SPI);
    gpio_set_function(18, GPIO_FUNC_SPI);
    gpio_set_function(19, GPIO_FUNC_SPI);

    gpio_init(csn);
    gpio_init(ce);

    gpio_set_dir(csn, 1);
    gpio_set_dir(ce, 1);

    ce_low();   // Set enable to low to enable NRF device
    csn_high(); // Set CSN high until an SPI transaction is started

}

NRF24::~NRF24(){

}

// Read a register from the NRF24 device
uint8_t NRF24::read_reg(uint8_t reg){
    uint8_t result = 0; // Memory for storing register result
    reg = (0b00011111 & reg);   // Read instruction is 000A AAAA (Where AAAAA is the address)

    csn_low(); // Bring CSN pin low to begin SPI transaction
    spi_write_blocking(port, &reg, 1);
    spi_read_blocking(port, 0xff, &result, 1);
    csn_high();

    sleep_us(100); // Ensure that the CSN transition occurs

    return result;
}

// Write [data] to register [reg] on NRF24 device
void NRF24::write_reg( uint8_t reg, uint8_t data){
    write_reg(reg, &data, 1);
}


void NRF24::write_reg( uint8_t reg, uint8_t *data, uint8_t size){
    reg = 0b00100000 | ( 0b00011111 & reg);  // Write instruction to the specified register

    csn_low(); // Bring CSN pin low to begin SPI transaction
    spi_write_blocking(port, &reg, 1); // Tell the NRF24 what register to write to
    spi_write_blocking(port, (uint8_t*)data, size); // Write the data to the register
    csn_high();

    sleep_us(10); // Ensure that the CSN transition occurs
}

void NRF24::config(){
    csn_high();
    ce_low();

    sleep_ms(11);

    write_reg(REG_CONFIG, 0b00001010);  // Configure the chip to have CRC enabled, and put it in power up mode
    sleep_us(1500);

    write_reg(REG_ENAA, 0b00000000); // Turn off auto acknowledge on all ports

    write_reg(RX_ADDR_P0, (uint8_t*)"rvetx",5); // Set the receive address
    write_reg(TX_ADDR, (uint8_t*)"rvetx",5); // Set transmit address

    write_reg(RX_PW_P0, 0b00001000); // Set RX payloads to have 32 bits
    
    write_reg(RF_SETUP, 0b00000111); // Setup RF settings. Set for 1Mbps,0dBm and LNA gain
}

void NRF24::tx_mode(){
    uint8_t reg = read_reg(REG_CONFIG); // Read the current status of the config register

    reg &= 0b11111110;  // Set the PRIM_RX bit low

    write_reg(REG_CONFIG, reg);

    ce_low(); // Write CE low to finish placing into transmit mode. Will stay in TX mode until packet transmission is finished

    sleep_us(130);
}

void NRF24::rx_mode(){
    uint8_t reg = read_reg(REG_CONFIG);

    reg |= 0b00000001; // Write the PRIM_RX bit high

    write_reg(REG_CONFIG, reg);

    ce_high(); // Pull CE high for receive mode

    sleep_us(130);
}

uint8_t NRF24::message_available(){
    uint8_t reg = read_reg(FIFO_STATUS);

    return !((0b00000001 & reg) == 1);  // Return true if the RX FIFO has items in it
}

void NRF24::set_channel(uint8_t ch){
    write_reg(RF_CH, ch); // Set the RF_CH register to the channel to operate on
}

uint8_t NRF24::send_message(uint8_t* msg, uint8_t size){
    uint8_t reg = 0b10100000; // W_TX_PAYLOAD command
    uint8_t written = sizeof(msg);

    // Perform SPI transmission
    csn_low();
    spi_write_blocking(port, &reg, 1); // Write the W_TX_PAYLOAD command to SPI
    spi_write_blocking(port, msg, size);
    csn_high();

    
    ce_high(); // Pulse CE pin to start transmission
    sleep_us(10);
    ce_low(); // Return CE pin to low state. Places module back in TX mode

    return written;
}

void NRF24::receive_message(uint8_t* msg, uint8_t size){
    uint8_t reg = 0b01100001; // R_RX_PAYLOAD command - Reads RX payload. Payload to be deleted from RX fifo when complete

    csn_low();
    spi_write_blocking(port, &reg, 1);
    spi_read_blocking(port, 0xff, msg, size); // TODO: Expects to read from SPI into an array of uint8_t - For 16 bit packet make a way to read both and then construct them into a 16 bit integer.
    csn_high();
}