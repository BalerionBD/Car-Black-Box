#include <xc.h>
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#include"ds1307.h"
#include"I2C.h"
void init_ds1307(void)
{
    unsigned char dummy;
    
    dummy = read_ds1307(SEC_ADDR);
    dummy = dummy & 0x7F;//01111111 // in dummy variable loading ch bit to 0 and all other bit to be 1 ;
    write_ds1307(SEC_ADDR, dummy);//write into sec add to initialize the ch bit to 0 so that oscillator start working
}

unsigned char read_ds1307(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void write_ds1307(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

