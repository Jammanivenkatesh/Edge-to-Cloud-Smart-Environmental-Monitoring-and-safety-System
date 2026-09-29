/* main.c - Split Modular Edge-to-Cloud System Application Entry */
#include <LPC21xx.h>
#include <stdio.h>
#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "adc.h"
#include "adc_defines.h"
#include "rtc.h"
#include "mq2.h"
#include "esp01.h"
#include "i2c.h"
#include "uart0.h"
#include "kpm.h"
#include "interrupts.h"
#include "admin_enhancement.h"

// Tracking variables
f32 eAR;
u32 AdcDval;
u8 last_smoke_status = 0;

//Global EEPROM configurations & threshold variable
u32 eeprom_adddr = 0x0000; // Secure password start location
u32 aadr         = 0x0010; // Temperature setpoint memory location
u8 setpoint    = 35;     // Live RAM threshold variable (Default 35°C)

void Init_System(void)
{
    RTC_Init();                     // Initialize internal real-time parameters
    InitLCD();                      // Configure display pins and lines
    Initkpm();                      // Configure Port 1 input rows/columns
    InitADC();                      // Initialize LM35 Channel AD0.1
    Init_MQ2();                     // Setup P0.19 and Alert Buzzer
    InitUART0();                    // Start instructor serial background interrupts
    init_i2c();                     // Setup I2C Speed registers
    
    Configure_Interrupts();         // Link the vector controller tables
    
    CmdLCD(0x01);
    StrLCD("Loading Config..");
    Synchronize_EEPROM_Config();    // Match parameters against AT24C256
    
    esp01_connectAP();              // Associate module with local network access point
    CmdLCD(0x01);
}


int main(void)
{
    u8 current_smoke = 0;
   // char upload_buffer[12];
    
    Init_System();
    
    while(1)
    {
        // Process asynchronous EINT3 button intercepts
        if (admin_mode_requested)
        {
            Execute_Admin_Enhancement();
        }
        					  
        //Fetch and convert ambient thermal values via the ADC
        ReadADC(CHNO2, &eAR, &AdcDval);
        CmdLCD(0x80);
        StrLCD("TEMP:");
        DisplayADC(&eAR);
        CmdLCD(0x80+10);
        // Render target safety limitations pulled from your non-volatile configuration
        StrLCD(" SP:");
        U32LCD(setpoint);
        

	   CmdLCD(0xC0);

	   if(current_smoke==1){
	   		StrLCD("Smoke:Detected");
	   }
	   else{
	   	StrLCD("Smoke:NtDetected");
	   }
        //Evaluate safety parameters against active threshold configurations
        if ((u32)(eAR * 100.0) >= (u32)(setpoint))
        {
            Alarm_Buzzer(1); // Enable warning sounders locally
        
	   //	  esp01_sendToThingspeak(((u32)(eAR * 100.0)),2);
	   		
	   }
        
        // Track environmental hazards using MQ-2 drivers
        current_smoke = Check_Smoke();
        if (current_smoke == 1)
        {
            Alarm_Buzzer(1);
            if (last_smoke_status == 0)
            {
                CmdLCD(0x01);
                StrLCD("FIRE ALARM ALERT");
                esp01_sendToThingspeak(2,2); // Send immediate emergency cloud update
                last_smoke_status = 1;
                CmdLCD(0x01);
            }
        }
        else
        {
            // Clear alert states if environment returns to normal parameters
            if ((u32)(eAR * 100.0) < (u32)(setpoint))
            {
                Alarm_Buzzer(0);
            }
            
            if (last_smoke_status == 1)
            {
                CmdLCD(0x01);
                StrLCD("ENVIRON SAFE");
                delay_ms(2000);
                esp01_sendToThingspeak(1,2);   // Send clear signal to cloud
                last_smoke_status = 0;
                CmdLCD(0x01);
            }
        }
        
        // Check if background CIIR interrupt scheduled regular telemetry uploads
        if (upload_timer_flag == 1)
        {
            u32 processed_temp = (u32)(eAR * 100.0);
            
            CmdLCD(0x01);
            StrLCD("Pushing to Cloud");
            esp01_sendToThingspeak(processed_temp,1); // Upload data to ThingSpeak channel
            
            upload_timer_flag = 0; // Clear scheduling flag
            CmdLCD(0x01);
        }
        
        delay_ms(100); // System loop stabilization delay
    }
}
