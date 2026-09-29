#include "types.h"

void delay_s(u32 tdly){
	tdly*=12000000; //1-second software delay
	while(tdly--);
}

void delay_ms(u32 tdly){
	tdly*=12000;  //1-milli second software delay
	while(tdly--);
}

void delay_us(u32 tdly){
	tdly*=12;  //1-micro second software delay
	while(tdly--);
}
