#include <avr/io.h>
#include "twi.h"
#include "uart.h"

void twi_init(void) {
    // set prescaler=1
    TWSR = 0x00;
    // set bit rate register for 100kHz
    TWBR = 72;
    // enable TWI
    TWCR = (1 << TWEN);
}

void twi_start(void) {
    // send start
    TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);
    //wait for TWINT flag
    while (!(TWCR & (1 << TWINT)));
}

void twi_stop(void) {
    //send stop
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void twi_write(uint8_t data) {
    //load data
    TWDR = data;
    //clear TWINT to start transmition
    TWCR = (1 << TWINT) | (1 << TWEN);
    //wait 
    while (!(TWCR & (1 << TWINT)));
}

uint8_t twi_read_ack(void) {
    //TWEA -> sends ACK after recieving
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while(!(TWCR & (1 << TWINT)));
    return TWDR;
}
uint8_t twi_read_nack(void) {
    //no TWEA -> no acc which tells the slave transmition finished
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    
    return TWDR;
}