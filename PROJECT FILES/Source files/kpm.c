#include <lpc21xx.h>
#include "defines.h"
#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "kpm_defines.h"

u8 kpmLUT[4][4]={	{'1','2','3','+'},
									{'4','5','6','-'},
									{'7','8','9','*'},
									{'C','0','=','/'}};  //4x4 keypad matrix
void Initkpm(){
		WRITENIBBLE(IODIR1,ROW0,0xf);    // Set Row pins (ROW0 to ROW3) as outputs
}

u8 colscan(void){
	if((READNIBBLE(IOPIN1,COL0))<15)  // Check if any of the 4 column bits are 0
		return 0;   // Key press detected
	else
		return 1;   // Idle state
}

u8 rowcheck(void){
	u8 rno;
	
	for(rno=0;rno<=3;rno++){
		WRITENIBBLE(IOPIN1,ROW0,~(1<<rno));    // Clear target row bit (Active Low)
		if(colscan()==0){
			break;         // Active row matched with active key
		}
	}
	WRITENIBBLE(IOPIN1,ROW0,0X00);    // Return all rows back to low for subsequent captures
	return rno;
}

u8 colcheck(void){
	u8 cno;
	for(cno=0;cno<=3;cno++){
	if((STATUSBIT(IOPIN1,(COL0+cno)))==0){   // Locate the specific grounded column pin
		break;
		}
	}
	return cno;
}

u8 keyscan(void){
	u8 rno,cno,keyno;
	
	while(colscan()){}   // Block execution until a key is physically depressed (Active-Low)
	
	 delay_ms(20); // Software debounce window to skip voltage ripples/contact bounce
	
	rno=rowcheck();  // rowcheck function to collect row now
	
	cno=colcheck();  // colcheck function to collect column now
	
	keyno=kpmLUT[rno][cno];
	
	 while(!colscan()); // Block execution until the user fully releases the button
	
	return keyno;
}

u32 ReadNum(void)
{
	u8 key;
	u32 sum=0;
	while(1)
	{
		CmdLCD(0xc0);
	key=keyscan();
	if((key>='0')&&(key<='9'))
	{
		
		CharLCD(key);
		sum=(sum*10)+(key-48);
		
	}
	else if(key=='O')  // 'O' as key to confirm carriage return
		break;
	}
	return sum;
}
