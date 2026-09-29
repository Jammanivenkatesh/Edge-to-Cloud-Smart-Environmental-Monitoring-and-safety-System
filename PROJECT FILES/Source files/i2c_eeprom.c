/* i2c_eeprom.c */
#include <LPC21xx.h>
#include "types.h"
#include "i2c.h"
#include "delay.h"

void i2c_eeprom_write(u8 slaveAddr, u16 wBuffAddr, u8 dat)
{
    i2c_start();	
    i2c_write(slaveAddr << 1);             // SlaveAddr + Write
    i2c_write((wBuffAddr >> 8) & 0xFF);    // High Byte Address
    i2c_write((wBuffAddr & 0xFF));         // Low Byte Address
    i2c_write(dat);    
    i2c_stop();
    delay_ms(10);
}			 

u8 i2c_eeprom_read(u8 slaveAddr, u16 rBuffAddr)
{
    u8 dat;
    
    i2c_start();	
    i2c_write(slaveAddr << 1);             // SlaveAddr + Write
    i2c_write((rBuffAddr >> 8) & 0xFF);    // High Byte Address
    i2c_write((rBuffAddr & 0xFF));         // Low Byte Address
    i2c_restart();	
    i2c_write((slaveAddr << 1) | 1);       // SlaveAddr + Read
    dat = i2c_nack();	
    i2c_stop();
    return dat;
}


void i2c_eeprom_page_write(u8 slaveAddr, u16 wBuffStartAddr, u8 *p, u8 nBytes)
{
    u8 i;
    i2c_start();	
    i2c_write(slaveAddr << 1);                 // SlaveAddr + Write
    i2c_write((wBuffStartAddr >> 8) & 0xFF);   // High Byte Address
    i2c_write((wBuffStartAddr & 0xFF));        // Low Byte Address
    for(i = 0; i < nBytes; i++)
    {
        i2c_write(p[i]);             
    }
    i2c_stop();
    delay_ms(10);
}			 


void i2c_eeprom_seq_read(u8 slaveAddr, u16 rBuffStartAddr, u8 *p, u8 nBytes)
{
    u8 i;
    i2c_start();	
    i2c_write(slaveAddr << 1);                 // SlaveAddr + Write
    i2c_write((rBuffStartAddr >> 8) & 0xFF);   // High Byte Address
    i2c_write((rBuffStartAddr & 0xFF));        // Low Byte Address
    i2c_restart();	
    i2c_write((slaveAddr << 1) | 1);           // SlaveAddr + Read
    for(i = 0; i < nBytes - 1; i++)
    {
        p[i] = i2c_masterack();	
    }
    p[i] = i2c_nack();
    i2c_stop();
}
