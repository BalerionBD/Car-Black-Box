#include <xc.h>
#include "uart.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    SPEN = 1;
    
    /*high speed baud rate*/
    BRGH = 1;
    
    /*TRANSMISSION enabale bit*/
    TXEN = 1;
    
    /* Continuous Reception Enable Bit */
    CREN = 1;
    
    /* Baud Rate Setting Register */
    SPBRG = (FOSC / (16 * baud)) - 1;
}



void putchars(unsigned char data)
{
    /* Transmit the data to the Serial Port */
    TXREG = data;
    
    /* Wait till the transmission is complete */
    do
    {
        continue;
    } while (TXIF != 1);
    
    /* Clear the interrupt flag */
    TXIF = 0;
}

void putsx(const char *s)
{
    while (*s)
    {
        putchars(*s++);
    }
}


