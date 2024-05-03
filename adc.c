#include <xc.h>
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#include"ADC.h"
void init_adc(void)
{
    /* Selecting Right Justification */
    ADFM = 1;
    
    /* Starting the ADC Module */
    ADON = 1;
}

unsigned short read_adc(void)
{
    unsigned short adc_reg_val;
    
    /* Start the ADC conversion */
    GO = 1;//set the go/done bit so that adc will start conversion 
    /* Wait for the convertion to complete */
    while (nDONE);
    
    adc_reg_val = (ADRESH << 8) | ADRESL;//to store the value into the register after conversion 
    // since adc is 10-bit register in pic16f877a and formal select bit is right justified 
    //so that ADRESL will store 8-bit and adresh will store only 2-bit 
    
    return adc_reg_val;
}

