#ifndef __MQ2_H__
#define __MQ2_H__

#include "types.h"

#define MQ2_PIN    19// P0.19 as input
#define BUZZER_PIN 22 // P0.22 as output

void Init_MQ2(void);
u8 Check_Smoke(void);
void Alarm_Buzzer(u8 state);

#endif
