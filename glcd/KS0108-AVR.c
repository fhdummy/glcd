//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// Atmel AVR MCU low-level driver.
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>

/* Port D is our data port, respective DB0 from LCD to PORTD0 from µC */

#define KS0108_DATA_PORT	PORTD
#define KS0108_DATA_DIR		DDRD
#define KS0108_DATA_PIN		PIND

/* Port B is our control port, D/I from LCD is connected to KS0108_RS */

#define CTRL_PORT	PORTB
#define CTRL_DIR		DDRB

#define KS0108_RS			(1 << 2)	//PB2
#define KS0108_RW			(1 << 1)	//PB1
#define KS0108_EN			(1 << 0)	//PB0

#define KS0108_CS1			(1 << 6)	//PB6
#define KS0108_CS2			(1 << 5)	//PB5
#define KS0108_CS3			(1 << 7)	//PB7

extern unsigned char screen_x;
extern unsigned char screen_y;

#define DISPLAY_STATUS_BUSY	0x80

//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{

	asm("nop");
	//_delay_ms(1);
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : CTRL_PORT &= ~KS0108_CS1; break;
	case 1 : CTRL_PORT &= ~KS0108_CS2; break;
	case 2 : CTRL_PORT &= ~KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : CTRL_PORT |= KS0108_CS1; break;
	case 1 : CTRL_PORT |= KS0108_CS2; break;
	case 2 : CTRL_PORT |= KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;
KS0108_DATA_DIR = 0x00;
KS0108_DATA_PORT = 0x00;
CTRL_PORT |= KS0108_RW;
CTRL_PORT &= ~KS0108_RS;
GLCD_EnableController(controller);
CTRL_PORT |= KS0108_EN;
GLCD_Delay();
status = KS0108_DATA_PIN;
CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(controller);
return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
	char t = GLCD_ReadStatus(controller);
while(t&DISPLAY_STATUS_BUSY){
	t = GLCD_ReadStatus(controller);
}
KS0108_DATA_DIR = 0xFF;
CTRL_PORT &= ~(KS0108_RW | KS0108_RS);
GLCD_EnableController(controller);
KS0108_DATA_PORT = commandToWrite;
CTRL_PORT |= KS0108_EN;
GLCD_Delay();
CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char data;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0x00;
CTRL_PORT |= (KS0108_RW | KS0108_RS);
GLCD_EnableController(screen_x / 64);
//GLCD_Delay();
CTRL_PORT |= KS0108_EN;
GLCD_Delay();
data = KS0108_DATA_PIN;
CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
return data;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
CTRL_PORT &= ~KS0108_RW;
CTRL_PORT |= KS0108_RS;
KS0108_DATA_PORT = dataToWrite;
GLCD_EnableController(screen_x / 64);
CTRL_PORT |= KS0108_EN;
GLCD_Delay();
CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitalizePorts(void)
{
CTRL_DIR |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
CTRL_PORT |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
return pgm_read_byte(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
