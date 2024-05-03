#ifndef MAIN_H
#define	MAIN_H
#include <xc.h>
#include"clcd.h"
#include"ADC.h"
#include"ds1307.h"
#include"I2C.h"

#include"display.h"
#include"digital_keypad.h"
#include"EEprom.h"
#include"timer2.h"
#include<string.h>
#include "uart.h"
#define LOGIN_SUCCESS           0x01
#define RETURN_BACK             0x03
#define TASK_SUCCESS            0x11
#define TASK_FAIL               0x22

#define DASH_BOARD_FLAG         0x01
#define LOGIN_FLAG              0x02
# define LOGIN_MENU_FLAG        0x04
#define VIEW_LOG_FLAG           0x08
#define CLEAR_LOG_FLAG          0x10
#define DOWNLOAD_LOG_FLAG 		0x20
#define SET_TIME_FLAG	 		0x40
#define CHANGE_PASSWORD_FLAG 	0x80
#define RESET_NOTHING	        0x00
#define RESET_PASSWORD          0x01
#define RESET_LOGIN_MENU        0x05
#define  RESET_MEMORY           0x04
#define RESET_VIEW_LOG_POS      0x08
#define RESET_TIME				0x10


#endif	/* MAIN_H */

