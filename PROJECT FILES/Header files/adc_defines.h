//adc_defines.h
#ifndef _ADC_DEFINES_H_
#define _ADC_DEFINES_H_

//System Clock Configuration & Calculations
#define FOSC 12000000   // External crystal Oscillator Frequency (12 MHz)
#define CCLK (FOSC*5)  // Core Clock (60 MHz) adjusted via Phase-Locked Loop (PLL)
#define PCLK (CCLK/4)  // Peripheral Clock (15 MHz) derived from APB divider
#define ADCLK 3000000  // Targeted ADC Operational Clock Speed
#define CLK_DIV ((PCLK/ADCLK)-1)   // Scaler value computed for the AD0CR clock divisor field

//Channel Selection Macros
#define CHNO1 0X01  //Channel 1
#define CHNO2 0X02  //Channel 2
#define CHNO3 0X04  //Channel 3
#define CHNO4 0X08  //Channel 4

//ADCR(ADC Control Register Configuration)
#define PDN_BIT (1<<21)  // 1 -> operatinal mode 0 ->powerdown mode
#define STA_CONV (1<<24) // start conversation -> 1 , stop conversation -> 0
#define CLKDIV (CLK_DIV<<8)   //clock divisor value

//ADDR(ADC Data Register Configuration)
#define RESULT 6   //10-bit result
#define DONE_BIT 31   //Done Bit (1 -> conversion complete)


#endif
