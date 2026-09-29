#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd_defines.h"

void WriteLCD(u8 data){
	SCLRBIT(IOCLR0,RW); // Clear R/W pin: Set LCD to Write Mode
	WRITEBYTE(IOPIN0,LCD_DATA,data);  // Output the 8-bit data/command onto data lines
	SSETBIT(IOSET0,EN); // Assert Enable pin HIGH to initiate latching
	delay_us(1);        // Minimum data setup time execution delay
	SCLRBIT(IOCLR0,EN); // Pull Enable pin LOW to complete the latch cycle
	delay_ms(2);        // Wait for the LCD internal controller to process the byte
	
}

void CmdLCD(u8 cmd){
	SCLRBIT(IOCLR0,RS);   // Clear RS pin: Set instruction register mode
	WriteLCD(cmd);        // Transmit the command byte
}

void CharLCD(u8 ch){
	SSETBIT(IOSET0,RS);  // Set RS pin: Set data register mode
	WriteLCD(ch);        // Transmit the character byte
}
void InitLCD(void){
	WRITEBYTE(IODIR0,LCD_DATA,0XFF);  // Configure LCD data pins as Digital Outputs
	SETBIT(IODIR0,RS);  // Configure Register Select (RS) pin as Output
	SETBIT(IODIR0,RW);  // Configure Read/Write (RW) pin as Output
	SETBIT(IODIR0,EN);  // Configure Enable (EN) pin as Output
	
	// Hardware Reset Sequence for HD44780 Controller
	delay_ms(15);
	CmdLCD(0x30);
	delay_ms(5);
	CmdLCD(0x30);
	delay_us(100);
	CmdLCD(0x30);
	
	CmdLCD(0x38);  // Function Set: 8-bit mode, 2-line display, 5x7 font style
	CmdLCD(0x0c);  // Display Control: Display ON, Cursor OFF, Blink OFF
	CmdLCD(0x01);  // Clear Display: Clear screen memory and reset cursor home
	CmdLCD(0x06);  // command : SHIFT_CUR_RIGHT
	
}

void StrLCD(char* str){
	while(*str){
		CharLCD(*str++);  // Print current character and advance pointer
	}
}

void U32LCD(s32 num){  //for integer (both positive and negative integers)
	s32 i=0;
	s8 buf[100];
	if(num<0){
		CharLCD('-');
		num=-num;
	}
	if(num==0){
		CharLCD('0');
		return;
	}
	while(num){
		buf[i++]=(num%10)+'0';
		num/=10;
	}
	buf[i]='\0';
	for(--i;i>=0;i--){
		CharLCD(buf[i]);
	}
}

void FloatLCD(f32 fnum,s32 dp){  //1st argument -> float, 2nd argument-> no of decimal points to consider
	s32 num;
	if(fnum<0.0){
		CharLCD('-');
		fnum=-fnum;
	}
	num=fnum;
	U32LCD(num);
	CharLCD('.');
	while(dp){
		fnum=fnum-num;
		fnum*=10;
		num=fnum;
		CharLCD(num+'0');
		dp--;
	}
	
}

void BinLCD(u32 n,u8 nbd){ //to print Binary, 1st argument->unsigned integer and 2nd argument-> no of bits to print
	s32 i=0;
	for(i=(nbd-1);i>=0;i--){
		CharLCD(((n>>i)&1)+'0');
	}
}

void Buildcgram(u8* cg,u8 bytes){
	s32 i=0;
	CmdLCD(0x40);   // Point LCD Address Counter (AC) to start of CGRAM address space
	for(i=0;i<bytes;i++){
		CharLCD(cg[i]);  // Write pixel configuration row byte by row
	}
	CmdLCD(0x80);  // Safely return Address Counter (AC) back to standard DDRAM (Screen)
}

