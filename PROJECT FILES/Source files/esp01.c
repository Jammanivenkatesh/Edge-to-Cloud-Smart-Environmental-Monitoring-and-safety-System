/* esp01.c */
#include <string.h>
#include "uart0.h"
#include "delay.h"
#include "lcd.h"

extern char buff[200];
extern unsigned char i;

//Initializes the ESP01 module and connects it to the local Access Point (Wi-Fi).

void esp01_connectAP(void)
{
	
	  //Step 1: Send basic AT attention command to verify ESP01 presence
    CmdLCD(0x01); // Clears LCD
    CmdLCD(0x80);
    StrLCD("AT");
    delay_ms(1000);
    UART0_Str("AT\r\n");  // Transmit AT attention command via UART
    i = 0; memset(buff, '\0', 200);
    while(i < 4);
    delay_ms(500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "OK"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);		
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
    
		//Step 2: Disable command echoing (ATE0) to save processing buffer space
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("ATE0");
    delay_ms(1000);
    UART0_Str("ATE0\r\n");  // Send command to turn off character echo
    i = 0; memset(buff, '\0', 200);
    while(i < 4);
    delay_ms(500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "OK"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);		
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
		
    //Step 3: Configure ESP01 for single-connection mode (CIPMUX=0)
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("AT+CIPMUX");
    delay_ms(1000);
    UART0_Str("AT+CIPMUX=0\r\n");  // Set connection mode to single
    i = 0; memset(buff, '\0', 200);
    while(i < 4);
    delay_ms(500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "OK"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);		
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
		
		//Step 4: Disconnect from any currently active Access Point
    
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("AT+CWQAP");
    delay_ms(1000);
    UART0_Str("AT+CWQAP\r\n");  // Command to quit existing AP connection
    i = 0; memset(buff, '\0', 200);
    while(i < 4);
    delay_ms(1500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "OK"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);		
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
		
		//Step 5: Connect to the specific target Wi-Fi AP using credentials
    
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("AT+CWJAP");
    delay_ms(1000);
    // Wi-Fi network credentials
		// Pass SSID and password via AT+CWJAP command
    UART0_Str("AT+CWJAP=\"***\",\"starwars123\"\r\n");
    i = 0; memset(buff, '\0', 200);
    while(i < 4);
    delay_ms(2500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "WIFI CONNECTED"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);		
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
}

/*
 -> Establishes a TCP connection to ThingSpeak and publishes sensor field telemetry.
 -> val: The sensor metric data payload to transmit.
 -> n:   The field assignment index (1 for Field 1/Temperature, other values for Field 2/Smoke).
 */

void esp01_sendToThingspeak(char val,int n)
{
	
		//Step 1: Open a TCP client socket connection to ThingSpeak server
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("AT+CIPSTART");
    delay_ms(1000);
	
		// Initiate TCP stream connection on HTTP Port 80
    UART0_Str("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
    i = 0; memset(buff, '\0', 200);
    while(i < 5);
    delay_ms(2500);
    buff[i] = '\0';
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD(buff);
    delay_ms(2000);
    if(strstr(buff, "CONNECT") || strstr(buff, "ALREADY CONNECTED"))
    {
        CmdLCD(0xC0);
        StrLCD("OK");
        delay_ms(1000);
        
				//Step 2: Inform ESP01 of the exact data payload length to expect
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("AT+CIPSEND");
        delay_ms(1000);
        UART0_Str("AT+CIPSEND=48\r\n");  // Pre-allocate fixed HTTP stream byte count (48 bytes)
        i = 0; memset(buff, '\0', 200);
        delay_ms(500);
        
        // ThingSpeak write API key endpoint connection
				//Step 3: Format and transmit HTTP GET Request parameters
		if(n==1){
			// Target Field 1 for Temperature data
		   UART0_Str("GET /update?api_key=PCTTSYXBNTX0DQA3&field1=");
		}
		else{
				// Target Field 2 for Smoke/Gas monitoring data
     	   UART0_Str("GET /update?api_key=PCTTSYXBNTX0DQA3&field2=");
        }
		UART0_Int(val);
		if(val<10){
			UART0_Str("\r");  // Padding character adjusting byte alignment if number is single digit
		}
        UART0_Str("\r\n");
		
				// Comprehensive delay block (10 seconds total) allowing cloud processing and response feedback 
        delay_ms(5000);
        delay_ms(5000);
        buff[i] = '\0';
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD(buff);
        delay_ms(2000);
		
				// Verify receipt of HTTP acknowledgment packet from server side
        if(strstr(buff, "SEND OK"))
        {
            CmdLCD(0x01);
            StrLCD("DATA UPDATED");
            delay_ms(1000);			
        }
        else
        {
            CmdLCD(0x01);
            StrLCD("DATA NOT UPDATED");
            delay_ms(1000);	
        }
    }
    else
    {
        CmdLCD(0xC0);
        StrLCD("ERROR");
        delay_ms(1000);		
        return;
    }
}
