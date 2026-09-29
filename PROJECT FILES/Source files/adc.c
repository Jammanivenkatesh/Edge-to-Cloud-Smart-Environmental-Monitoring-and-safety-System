#include <lpc21xx.h>
#include "types.h"
#include "adc_defines.h"
#include "delay.h"
#include "lcd.h"

void InitADC(void){
	PINSEL1|=(1<<24); //Selecting P0.28 as ADC pin
	ADCR=PDN_BIT|CLKDIV;   // Set operational clock divider and power on ADC(operational mode)
}


void ReadADC(u32 chno,f32* eAR,u32 *AdcDval){
		ADCR&=~(255<<0);  // Clear previous channel selections (Bits 0-7)
	  ADCR|=chno|STA_CONV;  // Select current channel and start conversion
		
		delay_us(3);   // Small hardware channel switching settling delay
	
		while(((ADDR>>DONE_BIT)&1)==0);  // Polling: wait until conversion finishes
	
		ADCR&=~(STA_CONV);  // Clear start flag to prevent continuous conversions
	
		*AdcDval=((ADDR>>RESULT)&1023);  // Extract the 10-bit converted value (Bits 6-15)
	  *eAR=((3.3/1023)*(*AdcDval));    // Translate digital value to actual analog voltage
}

void DisplayADC(float *eAR){
		float temp;
		//temp=0.25*100;
		temp= (*eAR * 100.0);   // Scale voltage step representation to engineering units (e.g. Temperature)
		CmdLCD(0x80+5);
		U32LCD(temp);
		CharLCD(223);    //ASCII degree symbol (°)
	 CharLCD('C');
}
