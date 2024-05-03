#include <xc.h>
#include"EEprom.h"
#include"I2C.h"
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
//function to read into external EEPROM 
unsigned char ext_eeprom_24C02_read(unsigned char addr)
{
    //declaration of local variable
    unsigned char data;
    
    //sending start bit in I2C bus
    i2c_start();
    
    //sending the Slave address byte with write bit LSB 
    i2c_write(SLAVE_WRITE_EEPROM); 
    //writing the address to the I2C line to read the data from that address
    i2c_write(addr);
    //restarting the I2C bus
    i2c_rep_start();
    //sending the Slave address byte with read bit LSB
    i2c_write(SLAVE_READ_EEPROM);
    
    //reading the data from SSPBUFF
    data = i2c_read(0);
    
    //sending Stop bit to make ready the I2C bus for further transmissions
    i2c_stop();
    
    return data;
}

//function to write from external EEPROM
void ext_eeprom_24C02_byte_write(unsigned char addr,  char data)
{
    //sending start bit in I2C bus
    i2c_start();
    
    //sending the Slave address byte with write bit LSB
    i2c_write(SLAVE_WRITE_EEPROM);
    
    //writing the address to the slave to write the data in that address
    i2c_write(addr);
    
    //writing the data to the slave
    i2c_write(data);
    
    //sending Stop bit to make ready the I2C bus for further transmissions
    i2c_stop();
}
void ext_eeprom_24C02_str_write(unsigned char addr , char *data)
{
    while(*data != 0)
    {
        ext_eeprom_24C02_byte_write(addr,*data);
        data++;
        addr++;
    }
 }

