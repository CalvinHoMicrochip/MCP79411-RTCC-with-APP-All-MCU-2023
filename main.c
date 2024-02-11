 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
? [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "OLED128x64.h"
#include "MCP79411.h"

volatile unsigned char      I2C_Wbuffer[16];   
volatile unsigned char      I2C_Rbuffer[16] ;
unsigned char                 ASCII_Buffer[20];

volatile time_t getTime, setTime;
struct tm *tm_t;

volatile bool bRTCOverflow = 0 ;
uint8_t     StateMachine = 0 ;
uint16_t    Counter = 0 ;

struct tm working_timeptr ;


void    LED_Shift(void) ;

void    RTC_OVF_Callback(void)
{
    bRTCOverflow = 1 ;
}

/*
    Main application
*/

int main(void)
{
    SYSTEM_Initialize();
    // Important workaround for I2C Pin MAX setting
    // Set  PC2/PC3 as Host/Client pin of TWI0 
    // PORTMUX.TWIROUTEA = 0b00000000;             
    // Important workaround for I2C Pin MAX setting 
    RTC_SetOVFIsrCallback(RTC_OVF_Callback);
    MCP79411_Initialize();
 
//        working_timeptr.tm_sec = 55;
//        working_timeptr.tm_min = 0 ;
//        working_timeptr.tm_hour = 0 ;
//        working_timeptr.tm_mon = 0 ;
//        working_timeptr.tm_mday = 0 ;
//        working_timeptr.tm_year = 0;
//        working_timeptr.tm_yday = 0;
//        
//        setTime =  mktime(&working_timeptr) ;
        // setTime = time(&working_timeptr) ;

        OLED_Init();                                                    
        OLED_CLS();
        OLED_Put8x16Str(0,0," APP-Nono-BASE");
        OLED_Put8x16Str(0,2,"MCU:AVR128DA48");
        

//            memset(tm_t, 0, sizeof (tm_t));        
//            tm_t->tm_year = 2024-1900 ;
//            tm_t->tm_mon = 1 ;                  //  Febryary ( month 0:11)
//            tm_t->tm_mday = 10 ;               // mday (1~31)
//            tm_t->tm_hour = 18 ;
//            tm_t->tm_min = 34;
//            tm_t->tm_sec = 00
//            setTime =  mktime(tm_t) ;
//            MCP79411_SetTime(setTime);
        
    while(1)
    {
        if (bRTCOverflow ==1)
        {
            LED_Shift();
            bRTCOverflow = 0;
//            printf("Second Count : %10.0f\n\r", (float)setTime);
//                            sprintf (ASCII_Buffer,"S: %10.0f", (float)setTime) ;
//                                OLED_Put8x16Str(0,6,(uint8_t*)ASCII_Buffer) ;      
            
            getTime = MCP79411_GetTime();
            tm_t = localtime(&getTime);
            printf("%04d-%02d-%02d %02d:%02d:%02d\n\r", tm_t->tm_year+1900, tm_t->tm_mon+1, tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);   
            sprintf (ASCII_Buffer,"D = %04d-%02d-%02d", tm_t->tm_year+1900, tm_t->tm_mon+1, tm_t->tm_mday) ;
            OLED_Put8x16Str(0,4,(uint8_t*)ASCII_Buffer) ; 
            sprintf (ASCII_Buffer,"T = %02d:%02d:%02d", tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec) ;
            OLED_Put8x16Str(0,6,(uint8_t*)ASCII_Buffer) ; 
//            setTime =  mktime(tm_t) ;
//            printf ("getTime = %12.1f & setTime = %12.1f \n\r", (float)getTime , (float)setTime ) ;
        }
    }    
}

void    LED_Shift(void)
{
 switch(StateMachine)
        {
            case 0:
                LED1_SetHigh();
                LED2_SetLow();
                LED3_SetLow();
                LED4_SetLow();
                StateMachine ++ ;
                break ;
            case 1:
                LED1_SetLow();
                LED2_SetHigh();
                LED3_SetLow();
                LED4_SetLow();    
                StateMachine ++ ;
                break ;
            case 2 :
                LED1_SetLow();
                LED2_SetLow();
                LED3_SetHigh();
                LED4_SetLow();      
                StateMachine ++ ;
                break ;
            case 3:
                LED1_SetLow();
                LED2_SetLow();
                LED3_SetLow();
                LED4_SetHigh();    
                StateMachine ++ ;
                break ;
            case 4:
                LED1_SetLow();
                LED2_SetLow();
                LED3_SetHigh();
                LED4_SetLow();       
                StateMachine ++ ;
                break ;                
            case 5:
                LED1_SetLow();
                LED2_SetHigh();
                LED3_SetLow();
                LED4_SetLow();      
                StateMachine = 0 ;
                break ;                                               
            default :
                break ;
                                           
        }    
}
