#include"main.h"
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
char *gear[]= {"GN","GR","G1","G2","G3","G4"};// 6 gear we are using for the car black box 
            // 0    1    2    3    4     5
static void init_config(void) {
    init_clcd();//To initialize the clcd display
    init_i2c(100000);//To initialize the i2c communication protocol with baud rate 100khz 
    init_ds1307();//To initialize the RTC
    init_adc();//To initialize the adc
    init_digital_keypad();//To initialize the digital keypad
    init_timer2();
    init_uart(9600);//To initialize the timer2 
    //To Enable the global Interrupt pin
    GIE = 1;
    //To Enable the peripheral Interrupt pin
    PEIE = 1;
    /*write your initialization*/
}

void main(void) {
    char event[3] = "ON";
    char key, key1;
    unsigned char speed = 0;
    char control_flag = DASH_BOARD_FLAG;
    char gr = 0;
    char reset_flag;
    char  menu_pos =0;
    int wait_time = 0;
    init_config();
    log_car_event(event,speed);//To log the inital state of car event and the speed in the EEprom 
    
    //To store the password in the first 4 location of the Ext EEprom 24c02 
    ext_eeprom_24C02_byte_write(0x00,'1');
    ext_eeprom_24C02_byte_write(0x01, '0');
    ext_eeprom_24C02_byte_write(0x02, '1');
    ext_eeprom_24C02_byte_write(0x03, '0');
    ext_eeprom_24C02_byte_write(0x04,'\0');
    
    while (1) {
        /*read speed from pot1*/
        speed = read_adc() / 10;// 102
        if(speed > 99)
        {
            speed = 99;
        }
        /*read key*/
        key = read_digital_keypad();
        //key1 = read_digital_keypad(LEVEL);
        for(int i = 2000; i--; );
        /*for collison*/
        if(key == SW1)
        {
            strcpy(event,"C ");
            log_car_event(event,speed);
        }
        /*for increasing gear*/
        else if(key == SW2 && gr < 6)
        {
                strcpy(event,gear[gr]);
                log_car_event(event,speed);
                gr++;
        }
        /*for decreasing gear*/
       else if(key == SW3 && gr > 0)
       {
           gr--;
           strcpy(event,gear[gr]);
           log_car_event(event,speed);
       }
        /*to enter the password*/
        else if((key == SW4 || key == SW5) && (control_flag == DASH_BOARD_FLAG))
       {
           clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            __delay_us(500);
           clcd_print(" ENTER PASSWORD ",LINE1(0));
           clcd_putch(' ',LINE2(6));
           clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
           __delay_us(100);
           control_flag = LOGIN_FLAG;
           reset_flag = RESET_PASSWORD;
           TMR2ON = 1;//To turn on the timer 2
       }
        /*to enter into the menu options*/
        else if ( key == LPSW4 && control_flag == LOGIN_MENU_FLAG )
		{
            

			switch ( menu_pos )
			{
				case 0:                              //to view log
					CLEAR_DISP_SCREEN1;
					clcd_print("Logs",LINE1(0));
					control_flag = VIEW_LOG_FLAG;
					reset_flag = RESET_VIEW_LOG_POS ;
					break;
				case 1 :                              //to clear log
					CLEAR_DISP_SCREEN1;
					control_flag = CLEAR_LOG_FLAG ;
					reset_flag = RESET_MEMORY;
					break;
				case 2 :                              //download log
					log_car_event("DL",speed);
					download_log();
                    CLEAR_DISP_SCREEN1;
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
					break;
				case 3 :                               // set time
					log_car_event("CT",speed);
					CLEAR_DISP_SCREEN1;
					control_flag = SET_TIME_FLAG;
					reset_flag = RESET_TIME;
					break;
				case 4:                                  //change password
					log_car_event("CP",speed);
					CLEAR_DISP_SCREEN1;
					control_flag = CHANGE_PASSWORD_FLAG;
					reset_flag = RESET_PASSWORD;
					DISP_ON_AND_CURSOR_ON;
					TMR2ON = 1;
					break;

			}
        
		}
        else if ( key == LPSW5 && (control_flag == LOGIN_MENU_FLAG ||control_flag == VIEW_LOG_FLAG))
            {
                control_flag = DASH_BOARD_FLAG;
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            }
        else if ( key == LPSW4 && control_flag == VIEW_LOG_FLAG )
            {
                control_flag = LOGIN_MENU_FLAG;
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            }
 
        switch(control_flag)
        {
            case DASH_BOARD_FLAG : //dashboard on
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                display_dash_board(event,speed);
                break;
                
            case LOGIN_FLAG:     //login password
               switch ( login( key, reset_flag))
               {
                   case LOGIN_SUCCESS:
                       control_flag = LOGIN_MENU_FLAG;
                       reset_flag = RESET_LOGIN_MENU;
                       clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                       __delay_us(500);
                       clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        break;
                        
                   case RETURN_BACK:
                       control_flag = DASH_BOARD_FLAG;
                       TMR2ON = 0; 
                       clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                       __delay_us(100);
                       break;
      
                }
               break;
            case LOGIN_MENU_FLAG: //login menu
                
                menu_pos = login_menu(key,reset_flag);
                break;  
                
            case VIEW_LOG_FLAG :  //view logs
                
				view_log(key, reset_flag);
				break;
                
			case CLEAR_LOG_FLAG : 
                //clear logs
				if ( clear_log(reset_flag) == TASK_SUCCESS )
					wait_time = 50;
                
				if ( reset_flag == RESET_MEMORY )
					log_car_event("CL",speed);
                
				if (wait_time > 0 && --wait_time == 0 )
				{
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
                    CLEAR_DISP_SCREEN1;
					continue;
				}
                
				break;
			case SET_TIME_FLAG : //set time
                
				if ( change_time( key ,reset_flag ) == TASK_SUCCESS )
					wait_time = 100;
                
				if (wait_time > 0 && --wait_time == 0 )
				{
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
                    CLEAR_DISP_SCREEN1;
					continue;
				}
				break;
                
			case CHANGE_PASSWORD_FLAG : //change password
				switch ( change_password( key ,reset_flag ) )
				{
					case TASK_SUCCESS :
                        wait_time = 100;
                        //control_flag = DASH_BOARD_FLAG;
                        DISP_ON_AND_CURSOR_OFF1;
                        break;
                    
					case RETURN_BACK :
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        DISP_ON_AND_CURSOR_OFF1;
                        continue;
				}

				if (wait_time > 0 && --wait_time == 0 )
				{
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
                    CLEAR_DISP_SCREEN1;
					continue;
				}
				break;
        }
        reset_flag = RESET_NOTHING;
    }
    return;
}
