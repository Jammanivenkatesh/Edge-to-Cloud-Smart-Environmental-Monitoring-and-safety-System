#include <lpc214x.h> //lpc21xx.h

//include LCD header files
#include "lcd.h"
#include "lcd_defines.h"
#include "rtc.h"

// System clock and peripheral clock Macros
#define FOSC 12000000
#define CCLK (5*FOSC)    
#define PCLK (CCLK/4)

// RTC Prescaler Calculation Macros
// RTC requires 32.768 kHz clock for 1-second increment.
// PREINT and PREFRAC registers divide PCLK to generate 32.768 kHz.

// PREINT = int (PCLK / 32768) - 1;
// PREFRAC = PCLK -((PREINT + 1) * 32768);
// Note: This information collected from LPC2129 Manual

#define PREINT_VAL (int) ((PCLK / 32768) - 1)
#define PREFRAC_VAL (PCLK -((PREINT_VAL + 1) * 32768))

//RTC Control Register (CCR) Bit Definitions
// Bit 0 ? Clock Enable --> 1 = Enable RTC counters  0 = Disable RTC counters
#define RTC_ENABLE (1<<0)

// Bit 1 ? Clock Reset --> 1 = Reset RTC counters    0 = Normal operation
#define RTC_RESET (1<<1)
 

//only for LPC2148
// Bit 4 ? Clock Source Select 
// 1 = Use external 32.768 kHz oscillator
// 0 = Use internal PCLK as RTC clock source
#define RTC_CLKSRC (1<<4)


s32 hour,min,sec,date,month,year,day;

// Array to hold names of days of the week
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};



// Comment out the one you are NOT using. 
// For LPC2124/2129, keep TARGET_LPC2129 active.
	#define TARGET_LPC2129 
//#define TARGET_LPC2148

void RTC_Init(void) 
{
    CCR = RTC_RESET; // Stop and Reset RTC
    
    #ifdef TARGET_LPC2148
        // LPC2148 Hardware usually uses an external 32kHz crystal
        // Note: In Proteus, if the clock doesn't move, use CCR = RTC_ENABLE; instead
        CCR = RTC_ENABLE | RTC_CLKSRC; 
    #else
        // LPC2124 / LPC2129 uses the Prescaler to turn PCLK into 32kHz
        PREINT = PREINT_VAL;
        PREFRAC = PREFRAC_VAL;
        CCR = RTC_ENABLE; // Bit 4 is 0: uses internal PCLK
				ILR=0x03;
    #endif
}


/*
Get the current RTC time
hour Pointer to store the current hour
minute Pointer to store the current minute
second Pointer to store the current second
*/
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
	*hour = HOUR;
	*minute = MIN;
	*second = SEC;
}

/*
Display the RTC time on LCD
hour value (0-23)
minute value (0-59)
second value (0-59) seperated by ':'
*/
void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
	CmdLCD(GOTO_LINE1_POS0);
	CharLCD(hour/10+48);
	CharLCD(hour%10+48);
	CharLCD(':');
	CharLCD(minute/10+48);
	CharLCD(minute%10+48);
	CharLCD(':');
	CharLCD(second/10+48);		
	CharLCD(second%10+48);	
}

/*
Get the current RTC date
day Pointer to store the current date (1-31)
month Pointer to store the current month (1-12)
year Pointer to store the current year (four digits)
*/
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
	*date = DOM;
	*month = MONTH;
	*year = YEAR;
}

/*
Display the RTC date on LCD
Day of month (1-31)
Month (1-12)
Year (four digits) and seperated by '/'
*/
void DisplayRTCDate(u32 date, u32 month, u32 year)
{
	CmdLCD(GOTO_LINE2_POS0);
	CharLCD(date/10+48);
	CharLCD(date%10+48);
	CharLCD('/');
	CharLCD(month/10+48);
	CharLCD(month%10+48);
	CharLCD('/');
	U32LCD(year);
}

/*
Set the RTC time
Hour to set (0-23)
Minute to set (0-59)
Second to set (0-59)
*/
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
	HOUR = hour;
	MIN = minute;
	SEC = second;
}

/*
Set the RTC date
day of month to set (1-31)
month to set (1-12)
year to set (four digits)
*/
void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
	DOM = date;
	MONTH = month;
	YEAR = year;
}

/*
Get the current day of the week
dow Pointer to store Day of Week (0=Sunday, ..., 6=Saturday)
*/
void GetRTCDay(s32 *dow)
{
	*dow = DOW; 
}

/*
Display the current day of the week on LCD
dow (Day of Week) (0=Sunday, ..., 6=Saturday)
*/
void DisplayRTCDay(u32 dow)
{
	CmdLCD(GOTO_LINE1_POS0+10);
	StrLCD(week[dow]);
  
}

/*
Set the day of the week in RTC
Day of Week to set (0=Sunday, ..., 6=Saturday)
*/
void SetRTCDay(u32 dow)
{
	DOW = dow;
}
