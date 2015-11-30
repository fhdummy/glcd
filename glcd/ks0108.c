//-------------------------------------------------------------------------------------------------
// Universal GLCD driver library
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "KS0108.h"
#include "font5x8.h"

unsigned char screen_x = 0, screen_y = 0;

//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
	_delay_us(1);
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : CTRL_PORT &= ~GLCD_CS1; break;
	case 1 : CTRL_PORT &= ~GLCD_CS2; break;
	case 2 : CTRL_PORT &= ~GLCD_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : CTRL_PORT |= GLCD_CS1; break;
	case 1 : CTRL_PORT |= GLCD_CS2; break;
	case 2 : CTRL_PORT |= GLCD_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
	unsigned char status;
	GLCD_DATA_DIR = 0x00;
	GLCD_DATA_PORT = 0x00;
	CTRL_PORT |= GLCD_RW;
	CTRL_PORT &= ~GLCD_RS;
	GLCD_EnableController(controller);
	CTRL_PORT |= GLCD_EN;
	GLCD_Delay();
	status = GLCD_DATA_PIN;
	CTRL_PORT &= ~GLCD_EN;
	GLCD_DisableController(controller);
	return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
	char t = GLCD_ReadStatus(controller);
	while(t&DISPLAY_STATUS_BUSY)
	{
		t = GLCD_ReadStatus(controller);
	}
	GLCD_DATA_DIR = 0xFF;
	CTRL_PORT &= ~(GLCD_RW | GLCD_RS);
	GLCD_EnableController(controller);
	GLCD_DATA_PORT = commandToWrite;
	CTRL_PORT |= GLCD_EN;
	GLCD_Delay();
	CTRL_PORT &= ~GLCD_EN;
	GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
	unsigned char data;
	while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
	GLCD_DATA_DIR = 0x00;
	CTRL_PORT |= (GLCD_RW | GLCD_RS);
	GLCD_EnableController(screen_x / 64);
	GLCD_Delay();
	CTRL_PORT |= GLCD_EN;
	GLCD_Delay();
	data = GLCD_DATA_PIN;
	CTRL_PORT &= ~GLCD_EN;
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
	GLCD_DATA_DIR = 0xFF;
	CTRL_PORT &= ~GLCD_RW;
	CTRL_PORT |= GLCD_RS;
	GLCD_DATA_PORT = dataToWrite;
	GLCD_EnableController(screen_x / 64);
	CTRL_PORT |= GLCD_EN;
	GLCD_Delay();
	CTRL_PORT &= ~GLCD_EN;
	GLCD_DisableController(screen_x / 64);
	screen_x++;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitalizePorts(void)
{
	CTRL_DIR |= (GLCD_CS1 | GLCD_CS2 | GLCD_CS3 | GLCD_RS | GLCD_RW | GLCD_EN);
	CTRL_PORT |= (GLCD_CS1 | GLCD_CS2 | GLCD_CS3);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
	return pgm_read_byte(ptr);
}

void GLCD_Initalize(void)
{
	unsigned char i;
	GLCD_InitalizePorts();
	for(i = 0; i < 3; i++)
		GLCD_WriteCommand((DISPLAY_ON_CMD | ON), i);
}

void GLCD_GoTo(unsigned char x, unsigned char y)
{
	unsigned char i;
	screen_x = x;
	screen_y = y;

	for(i = 0; i < GLCD_SCREEN_WIDTH/64; i++)
	{
		GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
		GLCD_WriteCommand(DISPLAY_SET_X | y,i);
		GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
	}
	GLCD_WriteCommand(DISPLAY_SET_Y | (x % 64), (x / 64));
	GLCD_WriteCommand(DISPLAY_SET_X | y, (x / 64));
}

void GLCD_ClearScreen(void)
{
	unsigned char i, j;
	for(j = 0; j < GLCD_SCREEN_HEIGHT/8; j++)
	{
		GLCD_GoTo(0,j);
		for(i = 0; i < GLCD_SCREEN_WIDTH; i++)
		GLCD_WriteData(0x00);
	}
}

void GLCD_WriteChar(char charToWrite)
{
	int i;
	charToWrite -= 32; 
	for(i = 0; i < 5; i++) 
		GLCD_WriteData(GLCD_ReadByteFromROMMemory((char *)((int)font5x8 + (5 * charToWrite) + i))); 
	GLCD_WriteData(0x00);
}

void GLCD_WriteString(char * stringToWrite)
{
	while(*stringToWrite)
		GLCD_WriteChar(*stringToWrite++);
}

void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
	unsigned char tmp;
	GLCD_GoTo(x, (y / 8));
	tmp = GLCD_ReadData();
	GLCD_GoTo(x, (y / 8));
	tmp = GLCD_ReadData();
	GLCD_GoTo(x, (y / 8));
	tmp |= (1 << (y % 8));
	GLCD_WriteData(tmp);
}


