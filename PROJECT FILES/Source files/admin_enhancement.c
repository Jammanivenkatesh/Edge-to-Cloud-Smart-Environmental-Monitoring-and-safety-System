#include <LPC21xx.h>
#include "admin_enhancement.h"
#include "kpm.h"            // Keypad Driver (keyscan())
#include "lcd.h"            // LCD Driver (CmdLCD(), CharLCD(), StrLCD())
#include "delay.h"          // Delay functions (delay_ms())
#include "i2c_eeprom.h"     // I2C EEPROM headers


// CONFIGURATION & GLOBAL SHARING DEFINITIONS

#define EEPROM_SLAVE_ADDR   0x50 


#define FORCE_EEPROM_RESET  0  // CRITICAL FORCE RESET FLAG: Set to 1 to wipe old memory junk. Change to 0 later

extern unsigned int eeprom_adddr; // Set to 0x0000 in main.c
extern unsigned int aadr;         // Set to 0x0010 in main.c
extern unsigned char setpoint;    // SetPoint variable

extern volatile u8 admin_mode_requested;


// 1. BOOT-TIME SYSTEM SYNCHRONIZATION

void Synchronize_EEPROM_Config(void)
{
    unsigned char i;
    unsigned char default_pwd[4] = {'1', '2', '3', '4'};
    unsigned char first_byte;
    unsigned char stored_thresh;
    
    // Read current state from the EEPROM chips
    first_byte = i2c_eeprom_read(EEPROM_SLAVE_ADDR, eeprom_adddr);
    stored_thresh = i2c_eeprom_read(EEPROM_SLAVE_ADDR, aadr);
    
    // Check if the memory is factory blank (0xFF) OR if we are forcing a system factory wipe
    if (first_byte == 0xFF || FORCE_EEPROM_RESET == 1)
    {
        CmdLCD(0x01);
        StrLCD("Clearing Memory..");
        
        // Sequentially write character bytes '1', '2', '3', '4' directly into memory
        for(i = 0; i < 4; i++)
        {
            i2c_eeprom_write(EEPROM_SLAVE_ADDR, eeprom_adddr + i, default_pwd[i]);
            delay_ms(20); // Hardware safety delay for EEPROM internal write cycle
        }
        
        // Write factory default temperature threshold (35 C)
        i2c_eeprom_write(EEPROM_SLAVE_ADDR, aadr, 35);
        setpoint = 35;
        delay_ms(20);
        
        CmdLCD(0x01);
        StrLCD("Reset Complete!");
        delay_ms(1500);
    }
    else
    {
        // If memory is already cleanly initialized, simply sync RAM with EEPROM contents
        setpoint = stored_thresh;
    }
}


// 2. ADMINISTRATIVE CONTROL PANEL UTILITY

void Execute_Admin_Enhancement(void)
{
    unsigned char input_pwd[4] = {0};
    unsigned char saved_pwd[4] = {0};
    unsigned char digit_count = 0;
    unsigned char key, selection, i;
    
    CmdLCD(0x01); 
    StrLCD("Enter Admin Pwd:");
    CmdLCD(0xC0); 
    
    // Keypad Input Collection loop
    while(1)
    {
        key = keyscan(); 
        
        // Cancel Key Logic ('C') to escape accidental menu activation
        if (key == 'C')
        {
            CmdLCD(0x01);
		  admin_mode_requested=0;
            StrLCD("Exiting Menu...");
            delay_ms(200);
		  CmdLCD(0x01);
            return;// Terminate function execution and step back to main.c loop
        }
        
        // Backspace operational mapping ('-')
        else if (key == '-')
        {
            if (digit_count > 0)
            {
                digit_count--;
                input_pwd[digit_count] = 0; // Wipe the array entry slot
                
                CmdLCD(0x10);  // Shift cursor backward
                CharLCD(' ');  // Blank out visual asterisk
                CmdLCD(0x10);  // Shift cursor back again
            }
        }
        // OK/Confirm operation mapping ('=')
        else if (key == '=')
        {
            if (digit_count == 4) break; // Break out to validation step only when full 4 digits are submitted
        }
        // Numeric digit assignment filters ('0' - '9')
        else if (key >= '0' && key <= '9')
        {
            if (digit_count < 4) 
            {
                input_pwd[digit_count] = key; // Keep the raw literal character digit straight from driver
                digit_count++;
                CharLCD('*'); 
            }
        }
    }
    
    //Read Saved Password String From EEPROM
    for(i = 0; i < 4; i++)
    {
        saved_pwd[i] = i2c_eeprom_read(EEPROM_SLAVE_ADDR, eeprom_adddr + i);
    }
    
    //String Comparision Match Routine
    for(i = 0; i < 4; i++)
    {
        if (input_pwd[i] != saved_pwd[i])
        {
            CmdLCD(0x01);
            StrLCD("Access Denied!");
            delay_ms(2000);
            return; // Terminate early and reject access if a single character mismatch occurs
        }
    }
    
    CmdLCD(0x01);
    StrLCD("Access Granted!");
    delay_ms(1500);
    
    //Main Admin Management Routine Hub
    while(1)
    {
        CmdLCD(0x01);
        StrLCD("1:Set Temp Thresh");
        CmdLCD(0xC0);
        StrLCD("2:Change Pwd 3:Ex");
        
        while(1)
        {
            selection = keyscan();
            if (selection == '1' || selection == '2' || selection == '3') break;
        }
        
        // Option 1: Adjust Core System Temperature Threshold Boundary
        if (selection == '1')
        {
            u32 new_thresh = 0;
            CmdLCD(0x01);
            StrLCD("New Thresh + '='");
            CmdLCD(0xC0);
            
            while(1)
            {
                key = keyscan();
                if (key == '=') break;
                else if (key >= '0' && key <= '9')
                {
                    new_thresh = (new_thresh * 10) + (key - '0');
                    CharLCD(key);
                }
            }
            
            if (new_thresh > 0 && new_thresh <= 99)
            {
                setpoint = (unsigned char)new_thresh;
                i2c_eeprom_write(EEPROM_SLAVE_ADDR, aadr, setpoint); 
                
                CmdLCD(0x01);
                StrLCD("Thresh Saved!");
                delay_ms(1500);
            }
        }
        // Option 2: Overwrite Stored Character Password
        else if (selection == '2')
        {
            unsigned char new_pwd_digits[4] = {0};
            unsigned char new_count = 0;
            
            CmdLCD(0x01);
            StrLCD("New Pwd + '='");
            CmdLCD(0xC0);
            
            while(1)
            {
                key = keyscan();
                if (key == '=')
                {
                    if (new_count == 4) break;
                }
                else if (key >= '0' && key <= '9')
                {
                    if (new_count < 4)
                    {
                        new_pwd_digits[new_count] = key;
                        new_count++;
                        CharLCD('*');
                    }
                }
            }
            
            // Push each array cell elements sequentially into separate 1-byte registers
            for(i = 0; i < 4; i++)
            {
                i2c_eeprom_write(EEPROM_SLAVE_ADDR, eeprom_adddr + i, new_pwd_digits[i]);
                delay_ms(20);
            }
            
            CmdLCD(0x01);
            StrLCD("Password Saved!");
            delay_ms(1500);
            
            //Break out of the menu loop completely back to main operation panel
            break; 
        }
        // Option 3: Exit Configuration Interface Dashboard Explicitly
        else if (selection == '3')
        {
            break;
        }
    }
    CmdLCD(0x01);
    admin_mode_requested=0;
}

