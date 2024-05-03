#include <xc.h>
#include"digital_keypad.h"
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
static int count=0,count1 = 0;;

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

unsigned char read_digital_keypad()
{
	static char once = 0 ;
	static int longpressed = 1 ;
	static unsigned char pre_key ;
	unsigned char key =  KEYPAD_PORT & INPUT_LINES ;
	if ( key != ALL_RELEASED && !once )
	{
		once = 1;
		longpressed = 0 ;
		pre_key = key;
	}
	else if( key == ALL_RELEASED && once )
	{
		once = 0;
		if ( longpressed < 30 )
		return pre_key ;
	}
	else if (once && longpressed <= 30 )
		longpressed++;
	else if ( once && longpressed == 31  && key == SW4)
	{ 
		longpressed ++;
		return  LPSW4;
	}
    else if ( once && longpressed == 31  && key == SW5)
	{ 
		longpressed++;
		return  LPSW5;
	}
	return ALL_RELEASED;
} 

