#include <xc.h>
#include"timer2.h"
int sec;
int return_time;
void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1) //Enable the timer Interrupt Flag 
    {
        if (++count == 1250)
        {
            count = 0;
     
          if(sec > 0)
          {
              sec--;
          }
         if(return_time > 0)
         {
             return_time--;
         }
        }
        
        TMR2IF = 0;// clear the timer interrupt flag 
    }
}