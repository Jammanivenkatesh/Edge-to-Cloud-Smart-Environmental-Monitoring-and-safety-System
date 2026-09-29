
/* uart0.h */
#ifndef _UART0_H_
#define _UART0_H_

#define UART_INT_ENABLE 1

// Protect definitions with ifndef guards
#ifndef FOSC
#define FOSC      12000000   //Hz
#endif

#ifndef CCLK
#define CCLK  	  (5*FOSC)
#endif

#ifndef PCLK
#define PCLK  	  (CCLK/4)
#endif

#define BAUD  	  9600
#define DIVISOR   (PCLK/(16 * BAUD))

void InitUART0(void); /* Initialize Serial Interface       */ 
void UART0_Tx(char ch);  
char UART0_Rx(void); 
void UART0_Str(char *);
void UART0_Int(unsigned int);
void UART0_Float(float);


#endif
