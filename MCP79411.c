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

#include <stdint.h>
#include <string.h>
#include "mcc_generated_files/system/system.h"
#include "MCP79411.h"


extern volatile unsigned char  I2C_Wbuffer[16];   
extern volatile unsigned char  I2C_Rbuffer[16] ;

#define EEADDR          0x64
#define RTCC_ADDR   0x6F //0x57
#define STATUS          0x01 // STATUS Register of EEPROM


/*********************** RTCC Register Addresses ***************************/

#define RTCC_SECONDS                0x00
#define RTCC_MINUTES                0x01
#define RTCC_HOUR                     0x02
#define RTCC_DAY                       0x03
#define RTCC_DATE                     0x04
#define RTCC_MONTH                  0x05
#define RTCC_YEAR                     0x06

#define CONTROL_REG                 0x07
#define CALIBRATION                 0x08

#define ALARM0_SECONDS              0x0A
#define ALARM0_MINUTES              0x0B
#define ALARM0_HOUR                 0x0C
#define ALARM0_DAY                  0x0D
#define ALARM0_DATE                 0x0E
#define ALARM0_MONTH                0x0F

#define ALARM1_SECONDS              0x11
#define ALARM1_MINUTES              0x12
#define ALARM1_HOUR                 0x13
#define ALARM1_DAY                  0x14
#define ALARM1_DATE                 0x15
#define ALARM1_MONTH                0x16

#define PWR_DOWN_MINUTES            0x18
#define PWR_DOWN_HOUR               0x19
#define PWR_DOWN_DATE               0x1A
#define PWR_DOWN_MONTH              0x1B

#define PWR_UP_MINUTES              0x1C
#define PWR_UP_HOUR                 0x1D
#define PWR_UP_DATE                 0x1E
#define PWR_UP_MONTH                0x1F

#define MAC_ADDR_48                 0x02
#define MAC_ADDR_64                 0x00


/******************************************************************************/

#define  SQWE                       0x40            //  Square Wave Enable BIT
#define  ALM_NO                     0x00            //  no alarm activated
#define  MFP_01H                    0x00            //  MFP = SQWAV(01 HERZ)
#define  OSCON                      0x20            //  state of the oscillator(running or not)
#define  VBATEN                     0x08            //  enable battery for back-up
#define  VBAT_CLR                   0xEF            //  Mask to clear VBAT flag BIT
#define  EXTOSC                     0x08            //  enable external 32khz signal
#define  ST_SET                     0x80            //  start oscillator

#define EEPROM_LOCK 0x0C
#define EEPROM_UNLOCK 0x00

#define EEPR_PAGE_SIZE               8

/******************************************************************************/

static void rtcc_write(uint8_t addr, uint8_t data);
static uint8_t rtcc_read(uint8_t addr);
static void MCP79411_EEPRWriteLatchEnable(void);
static void MCP79411_EEPRWriteLatchDisable(void);
static uint8_t MCP79411_EEPRReadStatusRegister(void);

/******************************************************************************/

DateTime_t dateTime;

static uint8_t rtcc_read(uint8_t addr) {
    // return i2c_read1ByteRegister(RTCC_ADDR, addr);
    I2C_Wbuffer[0] = addr ;
    I2C0_Host.WriteRead(RTCC_ADDR,I2C_Wbuffer,1,I2C_Rbuffer,1);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
    return I2C_Rbuffer[0];
}

static void rtcc_write(uint8_t addr, uint8_t data) {
    // i2c_write1ByteRegister(RTCC_ADDR, addr, data);
    I2C_Wbuffer[0] = addr ;
    I2C_Wbuffer[1] = data ;
    I2C0_Host.Write(RTCC_ADDR,I2C_Wbuffer,2);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
    DELAY_milliseconds(5);
}

void MCP79411_Initialize(void) {
    uint8_t reg = 0;

    dateTime.day = (rtcc_read(RTCC_DAY) & 0x07);
    rtcc_write(RTCC_DAY, (dateTime.day | 0x08));
    dateTime.sec = rtcc_read(RTCC_SECONDS);

    //Configure Control Register - SQWE=1, ALM0 = 00 {No Alarms Activated},
    //                             RS2, RS1, RS0 = 000 {1 HZ}
    rtcc_write(CONTROL_REG, ALM_NO + SQWE + MFP_01H);

    // Start the external crystal and check OSCON to know when it is running
    rtcc_write(RTCC_SECONDS, dateTime.sec | ST_SET);
    while (!reg) {
        reg = rtcc_read(RTCC_DAY);
        reg &= OSCON;
    }

    // Configure external battery enable BIT and clear the VBAT flag
    rtcc_write(RTCC_DAY, dateTime.day | (VBATEN & VBAT_CLR));

}

void MCP79411_EnableAlarms(bool alarm0, bool alarm1){
    uint8_t reg = rtcc_read(CONTROL_REG);
    reg = (reg & 0xCF) | (alarm0 << 4) | (alarm1 << 5);
    rtcc_write(CONTROL_REG, reg);
}

static void MCP79411_SetComponent(uint8_t location, uint8_t mask, uint8_t time){
    uint8_t inMemory = rtcc_read(location) & mask;
    rtcc_write(location, inMemory | (time % 10) | ((time / 10) << 4)); 
}

void MCP79411_SetTime(time_t t) {

    struct tm *tm_t;
    memset(tm_t, 0, sizeof (tm_t));

    tm_t = localtime(&t);
    MCP79411_SetComponent(RTCC_YEAR, 0x00, tm_t->tm_year % 100); // RTC only has two digits for year
    MCP79411_SetComponent(RTCC_MONTH, 0xD0, tm_t->tm_mon + 1); // time.h gives January as zero, clock expects 1
    MCP79411_SetComponent(RTCC_DATE, 0x00, tm_t->tm_mday);
    MCP79411_SetComponent(RTCC_MINUTES, 0x00, tm_t->tm_min);
    MCP79411_SetComponent(RTCC_SECONDS, 0x80, tm_t->tm_sec);
    MCP79411_SetComponent(RTCC_HOUR, 0x00, tm_t->tm_hour);
}

static uint8_t MCP79411_GetComponent(uint8_t location, uint8_t mask){
    uint8_t working = rtcc_read(location) & mask;
    return (working & 0x0F) + (((working & (mask & 0xF0)) >> 4) * 10);
}

time_t MCP79411_GetTime(void) {
    struct tm tm_t;
    memset(&tm_t, 0, sizeof (tm_t));
    
    tm_t.tm_year = MCP79411_GetComponent(RTCC_YEAR, 0xFF) + 100; // Result only has two digits, this assumes 20xx
    tm_t.tm_mon = MCP79411_GetComponent(RTCC_MONTH, 0x1F) - 1; // time.h expects January as zero, clock gives 1
    tm_t.tm_mday = MCP79411_GetComponent(RTCC_DATE, 0x3F);
    tm_t.tm_hour = MCP79411_GetComponent(RTCC_HOUR, 0x3F);
    tm_t.tm_min = MCP79411_GetComponent(RTCC_MINUTES, 0x7F);
    tm_t.tm_sec = MCP79411_GetComponent(RTCC_SECONDS, 0x7F);

    return mktime(&tm_t);
}

void MCP79411_SetAlarm0(struct tm tm_t, bool almpol, uint8_t mask){
    rtcc_write(ALARM0_SECONDS, tm_t.tm_sec);
    rtcc_write(ALARM0_MINUTES, tm_t.tm_min);
    rtcc_write(ALARM0_HOUR, tm_t.tm_hour);
    rtcc_write(ALARM0_DATE, tm_t.tm_mday);
    rtcc_write(ALARM0_MONTH, tm_t.tm_mon + 1); // time.h gives January as zero, clock expects 1
    rtcc_write(ALARM0_DAY, tm_t.tm_wday | almpol << 7 | mask << 4);
}

void MCP79411_ClearAlarm0(void){
    uint8_t reg = rtcc_read(ALARM0_DAY);
    rtcc_write(ALARM0_DAY, reg);
}

void MCP79411_SetAlarm1(struct tm tm_t, bool almpol, uint8_t mask){
    rtcc_write(ALARM1_SECONDS, tm_t.tm_sec);
    rtcc_write(ALARM1_MINUTES, tm_t.tm_min);
    rtcc_write(ALARM1_HOUR, tm_t.tm_hour);
    rtcc_write(ALARM1_DATE, tm_t.tm_mday);
    rtcc_write(ALARM1_MONTH, tm_t.tm_mon + 1); // time.h gives January as zero, clock expects 1
    rtcc_write(ALARM1_DAY, tm_t.tm_wday | almpol << 7 | mask << 4);
}

void MCP79411_ClearAlarm1(void){
    uint8_t reg = rtcc_read(ALARM1_DAY);
    rtcc_write(ALARM1_DAY, reg);
}

/******************************************************************************/

static void MCP79411_EEPRWriteLatchEnable(void) {
    // i2c_write1ByteRegister(EEADDR, STATUS, EEPROM_UNLOCK);
    I2C_Wbuffer[0] = STATUS ;
    I2C_Wbuffer[1] = EEPROM_UNLOCK ;
    I2C0_Host.Write(EEADDR,I2C_Wbuffer,2);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
}

static void MCP79411_EEPRWriteLatchDisable(void) {
    // i2c_write1ByteRegister(EEADDR, STATUS, EEPROM_LOCK);
    I2C_Wbuffer[0] = STATUS ;
    I2C_Wbuffer[1] = EEPROM_LOCK ;
    I2C0_Host.Write(EEADDR,I2C_Wbuffer,2);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
}

static uint8_t MCP79411_EEPRReadStatusRegister(void) {
    // i2c_read1ByteRegister(EEADDR, STATUS);
    I2C_Wbuffer[0] = STATUS ;
    I2C0_Host.WriteRead(RTCC_ADDR,I2C_Wbuffer,1,I2C_Rbuffer,1);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
    return I2C_Rbuffer[0];
}

uint8_t MCP79411_ReadByteEEPROM(uint8_t addr) {
    uint8_t stat_reg;

    MCP79411_EEPRWriteLatchDisable(); //Disable write latch

    do {
        stat_reg = MCP79411_EEPRReadStatusRegister(); //Read Status Register
    } while ((stat_reg & 0x03) != 0x00);

    // return i2c_read1ByteRegister(EEADDR, addr);
    I2C_Wbuffer[0] = addr ;
    I2C0_Host.WriteRead(RTCC_ADDR,I2C_Wbuffer,1,I2C_Rbuffer,1);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
    return I2C_Rbuffer[0];
}

void MCP79411_WriteByteEEPROM(uint8_t addr, uint8_t data) {
    uint8_t stat_reg;

    MCP79411_EEPRWriteLatchEnable(); //Enable write latch

    do {
        stat_reg = MCP79411_EEPRReadStatusRegister(); //Read Status Register
    } while ((stat_reg & 0x03) != 0x02);

    // i2c_write1ByteRegister(EEADDR, addr, data);
    I2C_Wbuffer[0] = addr ;
    I2C_Wbuffer[1] = data ;
    I2C0_Host.Write(EEADDR,I2C_Wbuffer,2);      // Read One Byte from Address : BME280_ID_REG
    while (I2C0_Host.IsBusy()) ;
    
    MCP79411_EEPRWriteLatchDisable(); //Disable write latch
}
