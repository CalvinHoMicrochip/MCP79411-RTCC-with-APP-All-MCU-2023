 /*
     (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

#ifndef         RTC6_H
#define	RTC6_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int sec, min, hr;
    int year, month, date, day;
} DateTime_t;

void MCP79411_Initialize(void);

void MCP79411_EnableAlarms(bool alarm0, bool alarm1);
void MCP79411_SetAlarm0(struct tm tm_t, bool almpol, uint8_t mask);
void MCP79411_SetAlarm1(struct tm tm_t, bool almpol, uint8_t mask);

void MCP79411_ClearAlarm0(void);
void MCP79411_ClearAlarm1(void);

void MCP79411_SetTime(time_t);
time_t MCP79411_GetTime(void);

uint8_t MCP79411_ReadByteEEPROM(uint8_t addr);
void MCP79411_WriteByteEEPROM(uint8_t addr, uint8_t data);

#endif
