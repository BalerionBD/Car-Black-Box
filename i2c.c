#include <xc.h>
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#include "I2C.h"

void init_i2c(unsigned long baud)
{
    /* Set I2C Master Mode */
    SSPM3 = 1;
    
    /* Set the Required Baudrate */
    SSPADD  = (FOSC / (4 * baud)) - 1;
    
    /* Enable SSP */
    SSPEN = 1;
}

static void i2c_wait_for_idle(void)
{
    /* Wait till no activity on the bus */
    while (R_nW || (SSPCON2 & 0x1F));
}

void i2c_start(void)
{
    i2c_wait_for_idle();
    SEN = 1;//initiate the start condition on sda and scl pin .
}

void i2c_rep_start(void)
{
    i2c_wait_for_idle();
    RSEN = 1;//repeat the start enable condition by initiate this bit 
}

void i2c_stop(void)
{
    i2c_wait_for_idle();
    PEN = 1;//initiate the stop condition enable bit 
}

unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;
    
    i2c_wait_for_idle();
    RCEN = 1;
    
    i2c_wait_for_idle();
    data = SSPBUF;
    
    if (ack == 1)
    {
        ACKDT = 1;//not had been acknowledge
    }
    else
    {
        ACKDT = 0;//ack in master mode
    }
    
    ACKEN = 1;//initiate the ack sequence on SDA and SCL pin and transmitt ACKDT bit.
    
    return data;
}

int i2c_write(unsigned char data)
{
    i2c_wait_for_idle();
    SSPBUF = data;
    
    return !ACKSTAT;
}
