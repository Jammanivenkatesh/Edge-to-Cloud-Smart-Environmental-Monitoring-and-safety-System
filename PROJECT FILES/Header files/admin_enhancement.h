#ifndef __ADMIN_ENHANCEMENT_H__
#define __ADMIN_ENHANCEMENT_H__

#include "types.h"

/*#define EEPROM_SLAVE_ADDR  0x50 
#define ADDR_SETPOINT      0x0000
#define ADDR_PWD_START     0x0010 */

extern u8 setpoint;
extern char master_password[5];

void Synchronize_EEPROM_Config(void);
void Execute_Admin_Enhancement(void);

#endif
