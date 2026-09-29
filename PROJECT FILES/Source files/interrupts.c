#include <LPC21xx.h>
#include "interrupts.h"
#include "uart0.h"


extern void UART0_isr(void) __irq; 

volatile u8 admin_mode_requested = 0;
volatile u8 upload_timer_flag = 0;
volatile u32 minute_counter = 0;

// Configured for EINT3 on P0.20
void eint3_isr(void) __irq
{
    admin_mode_requested = 1; // Set flag to process cleanly in while(1) context
    EXTINT = 1 << 3;          // Clear External Interrupt 3 flag register (Bit 3)
    VICVectAddr = 0;          // Acknowledge Interrupt Vector Controller execution
}

void rtc_ciir_isr(void) __irq
{
    if ((ILR >> 0) & 1) // Check if the Counter Increment Interrupt flag is high
    {
        minute_counter++;
        if (minute_counter >= 3)
        {
            upload_timer_flag = 1; // Mark 3-minute telemetry upload boundary
            minute_counter = 0;
        }
    }
    ILR = 0x03;      // Clear internal RTC Interrupt Flags
    VICVectAddr = 0; // Acknowledge Interrupt Vector Controller execution
}

void Configure_Interrupts(void)
{
    // 1. Configure P0.20 as EINT3 (Bits 8-9 of PINSEL1 must be set to 11)
    PINSEL1 = (PINSEL1 & ~0x00000300) | 0x00000300; 
    
    EXTMODE |= (1 << 3);                            // Set EINT3 to Edge-sensitive activation mode
    EXTPOLAR &= ~(1 << 3);                          // Set EINT3 to trigger on a Falling Edge (button press)
    
    // 2. Configure Vectored Interrupt Controller Slots
    VICIntSelect = 0x00000000;                      // Map channels as standard IRQs
    
    // Slot 0: Background Serial UART0 Buffer Engine
    VICVectAddr0 = (unsigned int)UART0_isr;
    VICVectCntl0 = (1 << 5) | 6;                    // Channel 6 = UART0
    
    // Slot 1: EINT3 Security Admin Button Configuration (Channel 17 = EINT3)
    VICVectAddr1 = (unsigned int)eint3_isr;
    VICVectCntl1 = (1 << 5) | 17;                   // Channel 17 = EINT3
    
    // Slot 2: Background RTC 3-Minute Minute Timer Engine
    VICVectAddr2 = (unsigned int)rtc_ciir_isr;
    VICVectCntl2 = (1 << 5) | 13;                   // Channel 13 = RTC
    
    // Enable Hardware Vectors globally inside the Controller (Channels 6, 17, and 13)
    VICIntEnable = (1 << 6) | (1 << 17) | (1 << 13);
    
    // Configure internal clock tracking registers to trigger every minute
    CIIR = (1 << 1); 
    ILR = 0x03;      // Flush baseline tracking bits at bootup
}
