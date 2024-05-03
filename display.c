#include"main.h"

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
unsigned char clock_reg[3];
char time[7];
char log[11];//To log the event and speed in the external EEPROM(24C02)
int pos= -1;
extern int sec;
char overflow = 0;
int access = -1;
extern int return_time;
char *menu[] = {"view log","clear log","Download log ","set time ","change password"};
                // 0           1            2             3             4

//function to display the screen
void  get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR);//read the hour from address 0x02
    clock_reg[1] = read_ds1307(MIN_ADDR);//read the min from the address 0x01
    clock_reg[2] = read_ds1307(SEC_ADDR);//read the sec from the address 0x01
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';//convert the hour into ascii value charecter 
    
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}
    

void display_time(void)
{
    //Function to get the current time 
    get_time();
    //hours
    clcd_putch(time[0],LINE2(2));
    clcd_putch(time[1],LINE2(3));
    //minutes
    clcd_putch(':',LINE2(4));
    clcd_putch(time[2],LINE2(5));
    clcd_putch(time[3],LINE2(6));
    //seconds
    clcd_putch(':',LINE2(7));
    clcd_putch(time[4],LINE2(8));
    clcd_putch(time[5],LINE2(9));
    
}
void display_dash_board( char event[] , unsigned char speed)
{
    clcd_print("  TIME     E  SP",LINE1(0));
    //to display the event 
    clcd_print(event,LINE2(11));
    //TO display the speed in the 14th location of clcd 
    clcd_putch(speed / 10 + '0',LINE2(14));// to convert the speed value of into the single digit 
    clcd_putch(speed % 10 + '0',LINE2(15));//To convert the speed into integer
    
    //To display current time 
    display_time();
}

void log_event( )
{
    //storing events in external eeprom
	char add;

	if( ++pos == 10 )
	{
		pos = 0;
		overflow = 1;
	}

	add = (pos * 10) + 5;
	ext_eeprom_24C02_str_write( add , log );

	if (access < 9 )
		access++;
}

void log_car_event (char *event ,unsigned char speed)
{
    //extracting all evenets
	get_time();
	strncpy( log , time , 6 );
	strncpy( &log[6], event, 2 );
	log[8] = (speed / 10) + '0';
	log[9] = (speed % 10) + '0';
	log[10] = '\0';

	log_event();
}

void get_password(char *spassword)
{
    //getting prevousy stored password
    for(int i = 0 ; i < 5;i++)
    {         
        spassword[i]=ext_eeprom_24C02_read(i);
    }
}

char login(char key ,char reset_flag)
{
    char spassword[5];
    static char npassword[5];
    static char i = 0;
    static unsigned char attempt_rem;
    if(reset_flag == RESET_PASSWORD)
    {
        key = ALL_RELEASED;
        attempt_rem = 3;
        i = 0;
        sec = 0;
        npassword[0] = 0;
        npassword[1] = 0;
        npassword[2] = 0;
        npassword[3] = 0;
        npassword[4] = '\0';
         return_time = 5;
     
    }
    if(return_time == 0)
    {
        return RETURN_BACK;
    }
    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    // sw4 =1 , sw5 = 0
    if (key == SW4  && i < 4 )
    {
        return_time = 5;
        npassword[i] = '1';
        clcd_putch('*',LINE2(6 + i));
        i++;
    }
     
    else if(key == SW5  && i < 4 )
    {
        return_time = 5;
        npassword[i] = '0';
        clcd_putch('*',LINE2(6 + i));
        i++;
    }
    //chcecking correct password entered or not
    if(i == 4)
    {
        get_password(spassword);
        if(strcmp(npassword,spassword) == 0)// if password compare correct then return Login success   
        {        
             return LOGIN_SUCCESS;   
        } 
    else
    {     
       if(--attempt_rem == 0 )
            {
           //blocking for 60 secss
              clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(500);
                clcd_print("You are blocked",LINE1(0));
                clcd_print("wait for ..60sec",LINE2(0));
                sec = 60;
                while(sec)
                {
                    clcd_putch(sec / 10 + '0',LINE2(11)); 
                    clcd_putch(sec % 10 + '0',LINE2(12)); 
                }
                attempt_rem = 3;
                   
            }
            else
            {
           //showing failed attempts
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                      __delay_us(500);
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                     __delay_us(100);      
                clcd_print(" WRONG PASSWORD ",LINE1(0));
                clcd_putch(attempt_rem + '0',LINE2(0));
                clcd_print("attempt Remains",LINE2(2));
                 sec = 3;
                 while(sec); //blocking delay 
            }
         clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
             __delay_us(500);
        clcd_print(" ENTER PASSWORD ",LINE1(0));
        
           i = 0;
       return_time = 5;    
    }
    }
    return 0x10;//if login not success then return 0x10 ;
}

    
char login_menu(char key,char reset_flag)
{
    static int menu_pos = 0  ;
	static int select_pos = 1 ;
	if ( reset_flag == RESET_LOGIN_MENU )
	{
		menu_pos = 0;
		select_pos = 1;
	}
    /*logic for scrolling menu and star*/
	if ( key == SW4 && menu_pos < 4 ) //sw4 to forward
	{
		menu_pos++;
		if ( select_pos < 2 )
			select_pos++;
        
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
	}
    
	else if( key == SW5 && menu_pos > 0) //sw5 to move back
	{
		menu_pos--;
		if ( select_pos > 1 )
			select_pos--;
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
	}

	if ( select_pos == 1 )
	{
		clcd_putch('*',LINE1(0));
		clcd_print( menu[menu_pos],LINE1(2));
		clcd_print( menu[menu_pos+1],LINE2(2));
	}
	else
	{
		clcd_putch('*',LINE2(0));
		clcd_print( menu[menu_pos-1],LINE1(2));
		clcd_print( menu[menu_pos],LINE2(2));
	}

	return menu_pos;

}




void view_log(unsigned char key, char reset_shift)
{
	static int shift ;
	char rlog[11];
	rlog[10]= 0;
	int rpos ;
	unsigned char add;
	if ( access == -1 )
	{
		clcd_print ("No logs avaiable" , LINE2(0));
	}
	else
	{
		if ( reset_shift == RESET_VIEW_LOG_POS )
			shift = 0 ;
		rpos = (overflow)? (pos + 1) % 10 : 0;

		if ( key == SW4 && shift < access ) //sw4 to move forward
        {
			shift++;
            CLEAR_LINE2 ;
        }
		else if ( key == SW5 && shift > 0 )// sw5 to move backward
        {
			shift--;
            CLEAR_LINE2 ;
        }

		rpos = (rpos + shift) % 10;

		for ( int i = 0 ; i < 10 ; i++ )
		{
			add = rpos * 10 + 5 + i ;
			rlog[i] = ext_eeprom_24C02_read(add);
		}
		/*printing logs on clcd one by one*/
		clcd_putch ( shift + '0' , LINE2(0));
		clcd_putch ( rlog[0] , LINE2(2));
		clcd_putch ( rlog[1] , LINE2(3));
		clcd_putch ( ':' , LINE2(4));
		clcd_putch ( rlog[2] , LINE2(5));
		clcd_putch ( rlog[3] , LINE2(6));
		clcd_putch ( ':' , LINE2(7));
		clcd_putch ( rlog[4] , LINE2(8));
		clcd_putch ( rlog[5] , LINE2(9));
		clcd_putch ( rlog[6] , LINE2(11));
		clcd_putch ( rlog[7] , LINE2(12));
		clcd_putch ( rlog[8] , LINE2(14));
		clcd_putch ( rlog[9] , LINE2(15));
        for(unsigned long int i =0;i < 20000;i++);
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
	}
}


char clear_log (char reset_memory)
{
	clcd_print( "Logs cleared",LINE1(2));
	clcd_print( "successfully", LINE2(2));
    /*clearing all the logs*/
	if ( reset_memory == RESET_MEMORY )
	{
		pos = -1;
		access = -1 ;
		return TASK_SUCCESS;
	}
	return TASK_FAIL;
}

void download_log(void)
{
	int next = -1 ;
	char rlog[11];
	rlog[10] = 0;
	int rpos = 0 ;
	unsigned char add;
    //display if no logs available
	if ( access == -1 )
	{
		putsx("No logs avaiable");
	}
	else
	{
        //displaying all logs available
		putsx("Logs :");
		putchars('\n');
		putchars('\n');
		putchars('\r');

		while (  next < access )
		{
			next++;
			rpos = (overflow)? (pos + 1) % 10 : 0;
			rpos = (rpos + next) % 10;

			for ( int i = 0 ; i < 10 ; i++ )
			{
				add = rpos * 10 + 5 + i ;
				rlog[i] = ext_eeprom_24C02_read(add );
			}
            //printing all logs in cutecom
			putchars ( next + '0');
			putchars(' ');
			putchars ( rlog[0] );
			putchars ( rlog[1] );
			putchars (':');
			putchars ( rlog[2] );
			putchars ( rlog[3] );
			putchars (':');
			putchars ( rlog[4] );
			putchars ( rlog[5] );
			putchars (' ');
			putchars ( rlog[6] );
			putchars ( rlog[7] );
			putchars (' ');
			putchars ( rlog[8] );
			putchars ( rlog[9] );
			putchars ( '\n' );
			putchars ('\r');
		}
	}
}

char change_time( unsigned char key ,unsigned char reset_time )
{
    /*logic to change time*/
	static unsigned int new_time[3];
	static int blink_pos ;
	static unsigned char wait , blink ;
	char dummy;
	static char t_done = 0;
	if ( reset_time == RESET_TIME )
	{
		get_time();
		new_time[0] = (time[0] & 0x0F ) * 10 + (time[1] & 0x0F) ;
		new_time[1] = (time[2]  & 0x0F ) * 10 + (time[3] & 0x0F) ;
		new_time[2] = (time[4] & 0x0F ) * 10 + (time[5] & 0x0F) ;
		clcd_print("Time (HH:MM:SS)",LINE1(0));
		blink_pos = 2;
		wait = 0;
		blink = 0;
		key = ALL_RELEASED;
		t_done = 0;
	}

	if( t_done)
		return TASK_FAIL;

	switch ( key )
	{

		case SW4 :
			new_time[blink_pos]++;
			break;
		case SW5 :
			blink_pos = (blink_pos + 1) % 3;
			break;
		case SW6 :
            /*storing new time*/
			get_time();
			dummy = ((new_time[0] / 10 ) << 4 ) | new_time[0] % 10 ;
			clock_reg[0] = (clock_reg[0] & 0xc0) | dummy ;
			write_ds1307(HOUR_ADDR, clock_reg[0] );
			dummy = ((new_time[1] / 10 ) << 4 ) | new_time[1] % 10 ;
			clock_reg[1] = (clock_reg[1] & 0x80) | dummy;
			write_ds1307(MIN_ADDR, clock_reg[1] );
			dummy = ((new_time[2] / 10 ) << 4 ) | new_time[2] % 10 ;
			clock_reg[2] = (clock_reg[2] & 0x80) | dummy;
			write_ds1307(SEC_ADDR, clock_reg[2] );
			CLEAR_DISP_SCREEN1;
			clcd_print("Time changed",LINE1(2));
			clcd_print("Successfully",LINE2(2));
            for(unsigned long int i = 0;i < 20000;i++);
			t_done = 1;
			return  TASK_SUCCESS;
	}

	if( new_time[0]  > 23 )
		new_time[0] = 0;
	if ( new_time[1] > 59 )
		new_time[1] = 0;
	if ( new_time[2] > 59 )
		new_time[2] = 0;

	if ( wait ++ == 50 )
	{
		wait = 0;
		blink = !blink;
		clcd_putch( new_time[0] / 10 + 48, LINE2(0));
		clcd_putch( new_time[0] % 10 + 48 , LINE2(1));
		clcd_putch( ':', LINE2(2));
		clcd_putch( new_time[1] / 10 + 48, LINE2(3));
		clcd_putch( new_time[1] % 10 + 48, LINE2(4));
		clcd_putch( ':', LINE2(5));
		clcd_putch( new_time[2] / 10 + 48 , LINE2(6));
		clcd_putch( new_time[2] % 10 + 48, LINE2(7));
/*logic to blink at the current pos*/

		if ( blink)
		{
			switch( blink_pos )
			{
				case 0:
					clcd_print("  ",LINE2(0));
					break;
				case 1:
					clcd_print("  ",LINE2(3));
					break;
				case 2:
					clcd_print("  ",LINE2(6));
					break;
			}
		}
	}
	return TASK_FAIL;
}



char change_password(unsigned char key, char reset_pwd )
{
	static char r_pwd[9];
	static int n_p_pos = 0;
	static char p_chg = 0;

    /*logic to change password*/

	if ( reset_pwd == RESET_PASSWORD )
	{
		strncpy(r_pwd, "    ",4);
		n_p_pos = 0;
		p_chg = 0;
		return_time = 30;
        
	}
    
	if (!return_time)
		return RETURN_BACK;

	if(p_chg)
		return TASK_FAIL;

	if ( n_p_pos < 4 )
		clcd_print("Enter new pwd    ",LINE1(0));
    
	else if ( n_p_pos > 3 && n_p_pos < 8 )
		clcd_print("Re enter new pwd    ",LINE1(0));
    
	
    

	switch( key)
	{
		case SW5 :
			r_pwd[n_p_pos] = '0';
			clcd_putch('*',LINE2(n_p_pos%4));
			n_p_pos++;
            return_time = 30;
            if ( n_p_pos == 4 )
            CLEAR_LINE2;
			break;
            
		case SW4 :
			r_pwd[n_p_pos] = '1';
			clcd_putch('*',LINE2(n_p_pos%4));
			n_p_pos++;
            return_time = 30;
            if ( n_p_pos == 4 )
            CLEAR_LINE2;
			break;
	}

	if ( n_p_pos == 8)
	{
		if ( strncmp(r_pwd, &r_pwd[4], 4) == 0 )
		{
        /*sucessfully changing password*/
			r_pwd[8]=0;
			ext_eeprom_24C02_str_write( 0x00, &r_pwd[4]);
			n_p_pos++;
			CLEAR_DISP_SCREEN1;
			clcd_print("Password changed",LINE1(0));
			clcd_print("successfully ",LINE2(2));
			p_chg = 1;
			return TASK_SUCCESS;
		}
		else
		{
            /*displaying password change fail*/
			CLEAR_DISP_SCREEN1;
			clcd_print ( "Password  change" , LINE1(0));
			clcd_print("failed",LINE2(5));
			p_chg = 1;
			return TASK_SUCCESS;
		}
	}

	return TASK_FAIL;
}