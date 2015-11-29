
/*
 * main.c
 *
 * Created: 29.11.2015 15:45:01
 *  Author: Stefan
 */ 

/*
 * Copyright:      Fabian Maximilian Thiele  mailto:me@apetech.de
 * Author:         Fabian Maximilian Thiele
 * Remarks:        this Copyright must be included
 * known Problems: none
 * Version:        1.1
 * Description:    KS0108 Library Demo Program 
 * 
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ks0108.h"
#include "arial_bold_14.h"
#include "corsiva_12.h"

void testGlcd();
void SPI_Write(unsigned char addr,unsigned char data);
unsigned char SPI_Read(unsigned char addr);
void initSPI();
void initMCP23S17();

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_CS   PB1

// MCP23S17 SPI Slave Device
#define SPI_SLAVE_ID    0x40
#define SPI_SLAVE_ADDR  0x00      // A2=0,A1=0,A0=0
#define SPI_SLAVE_WRITE 0x00
#define SPI_SLAVE_READ  0x01

// MCP23S17 Registers Definition for BANK=0 (default)
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCONA 0x0A
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPIOA  0x12
#define GPIOB  0x13

int main(void) 
{
	initSPI();
	initMCP23S17();
	
	while(1)
	{
		
	}
}

void initSPI()
{
	//Set MOSI (PB5), SCK (PB7) and SS (PB1) as output, others as input
	SPI_DDR = (1<<PB5) | (1<<PB7) | (1<<PB1);
	
	//CS pin is not active yet
	SPI_PORT |= (1<<SPI_CS);
	
	// Enable SPI, Master, set clock rate to fck/16
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void initMCP23S17()
{
	SPI_Write(IOCONA,0x28);   // I/O Control Register: BANK=0, SEQOP=1, HAEN=1 (Enable Addressing)
	SPI_Write(IODIRA,0x00);   // GPIOA As Output
	SPI_Write(IODIRB,0x00);   // GPIOB As Output
	SPI_Write(GPIOA,0x00);    // Reset Output on GPIOA
	SPI_Write(GPIOB,0x00);    // Reset Output on GPIOB
}

void SPI_Write(unsigned char addr,unsigned char data)
{
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	// Start MCP23S17 OpCode transmission
	SPDR = SPI_SLAVE_ID | ((SPI_SLAVE_ADDR << 1) & 0x0E)| SPI_SLAVE_WRITE;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Start MCP23S17 Register Address transmission
	SPDR = addr;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));

	// Start Data transmission
	SPDR = data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
}
unsigned char SPI_Read(unsigned char addr)
{
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	// Start MCP23S17 OpCode transmission
	SPDR = SPI_SLAVE_ID | ((SPI_SLAVE_ADDR << 1) & 0x0E)| SPI_SLAVE_READ;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	#if MCP23S17_EMULATION
	_delay_us(1);
	#endif
	// Start MCP23S17 Address transmission
	SPDR = addr;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));

	#if MCP23S17_EMULATION
	_delay_us(1);
	#endif
	// Send Dummy transmission for reading the data
	SPDR = 0x00;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));

	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
	return(SPDR);
}

void testGlcd()
{
	// Wait a little while the display starts up
	for(volatile uint16_t i=0; i<15000; i++);
		
	// Initialize the LCD
	ks0108Init(0);
		
	// Select a font
	ks0108SelectFont(Arial_Bold_14, ks0108ReadFontData, BLACK);
	// Set a position
	ks0108GotoXY(15,10);
	// Print some text
	ks0108Puts_P(PSTR("KS0108-Treiber"));
	// a nice little round rect
	ks0108DrawRoundRect(5, 5, 117, 20, 8, BLACK);

	// Once again :)
	// Select a font
	ks0108SelectFont(Corsiva_12, ks0108ReadFontData, BLACK);
	// Set a position
	ks0108GotoXY(5,30);
	// Print some text
	ks0108Puts_P(PSTR("http://www.apetech.de\nmailto:me@apetech.de"));
}

