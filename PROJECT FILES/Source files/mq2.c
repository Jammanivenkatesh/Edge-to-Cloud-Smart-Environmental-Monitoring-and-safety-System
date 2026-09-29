#include <LPC21xx.h>
#include "mq2.h"
#include "defines.h"

void Init_MQ2(void)
{
    // Configure P0.19 as Input for MQ-2
    CLRBIT(IODIR0, MQ2_PIN);
    
    // Configure P0.22 as Output for the Buzzer
    SETBIT(IODIR0, BUZZER_PIN);
    
    // Ensure buzzer is OFF initially (Assuming Active-High buzzer)
    CLRBIT(IOPIN0, BUZZER_PIN);
}

u8 Check_Smoke(void)
{
    // Read the logic state of P0.19
    // Returns 1 if smoke is present, 0 if clean (adjust depending on hardware module)
    return STATUSBIT(IOPIN0, MQ2_PIN);
}

void Alarm_Buzzer(u8 state)
{
    if(state)
    {
        SETBIT(IOSET0, BUZZER_PIN); // Turn buzzer ON
    }
    else
    {
        SETBIT(IOCLR0, BUZZER_PIN); // Turn buzzer OFF
    }
}
