/*
 * mcp23s17.c
 *
 *  Created on: Nov 25, 2011
 *      Author: Doug
 *
 * Copyright (C) 2011-2012 Doug Brown
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "KS0108.h"
#include "font5x8.h"
#include "graphic.h"

#define KS0108_DATA_PORT	PORTD
#define KS0108_DATA_DIR		DDRD
#define KS0108_DATA_PIN		PIND

#define KS0108_CTRL_PORT	PORTB
#define KS0108_CTRL_DIR		DDRB

#define KS0108_RS			(1 << 2)
#define KS0108_RW			(1 << 1)
#define KS0108_EN			(1 << 0)

#define KS0108_CS1			(1 << 6)
#define KS0108_CS2			(1 << 5)
#define KS0108_CS3			(1 << 5)

static bool MCP23S17_Inited = false;

// Pin definitions on PORTB
#define SPI_CS                  (1 << 4)
#define SPI_SCK                 (1 << 7)
#define SPI_MOSI                (1 << 5)
#define SPI_MISO                (1 << 6)
#define MCP23S17_RESET  (1 << 0)

#define ASSERT_CS()             PORTB &= ~SPI_CS
#define DEASSERT_CS()   PORTB |= SPI_CS;

// A few register defines
#define MCP23S17_CONTROL_WRITE(address) (0x40 | (address << 1))
#define MCP23S17_CONTROL_READ(address)  (0x40 | (address << 1) | 1)

#define MCP23S17_IODIRA                 0x00
#define MCP23S17_IODIRB                 0x01
#define MCP23S17_IPOLA                  0x02
#define MCP23S17_IPOLB                  0x03
#define MCP23S17_GPINTENA               0x04
#define MCP23S17_GPINTENB               0x05
#define MCP23S17_DEFVALA                0x06
#define MCP23S17_DEFVALB                0x07
#define MCP23S17_INTCONA                0x08
#define MCP23S17_INTCONB                0x09
#define MCP23S17_IOCON                  0x0A
#define MCP23S17_IOCON_AGAIN    0x0B
#define MCP23S17_GPPUA                  0x0C
#define MCP23S17_GPPUB                  0x0D
#define MCP23S17_INTFA                  0x0E
#define MCP23S17_INTFB                  0x0F
#define MCP23S17_INTCAPA                0x10
#define MCP23S17_INTCAPB                0x11
#define MCP23S17_GPIOA                  0x12
#define MCP23S17_GPIOB                  0x13
#define MCP23S17_OLATA                  0x14
#define MCP23S17_OLATB                  0x15

// Private functions
void MCP23S17_WriteBothRegs(uint8_t addrA, uint16_t value);
uint16_t MCP23S17_ReadBothRegs(uint8_t addrA);
uint8_t MCP23S17_ByteRW(uint8_t b);

void MCP23S17_Init()
{
        // If it has already been initialized, no need to do it again.
        if (MCP23S17_Inited)
        {
                return;
        }

        // Initialize the SPI pins
        // Set MOSI, SCLK, and CS as outputs, MISO as input
        // (Also, set the MCP23S17 reset line as an output)
        DDRB |= SPI_CS | SPI_SCK | SPI_MOSI | MCP23S17_RESET;
        DDRB &= ~SPI_MISO;

        // Initialize the SPI peripheral
        // We can run it at 8 MHz (divider of 2 from 16 MHz system clock -- maximum speed of MCP23S17 10 MHz)
#if ((F_CPU / 2) > 10000000)
#error This code assumes that the CPU clock divided by 2 is less than or equal to the MCP23S17's maximum speed of 10 MHz, and in this case, it's not.
#endif

        SPCR =  (0 << SPIE) | // No SPI interrupts
                        (1 << SPE) |  // Enable SPI
                        (0 << DORD) | // MSB first
                        (1 << MSTR) | // Master mode
                        (0 << CPOL) | // SPI mode 0,0
                        (0 << CPHA) |
                        (0 << SPR0);  // SCK frequency = F_CPU/2 (because of SPI2X being set below

        SPSR = (1 << SPI2X);  // Double the SPI clock rate -- allows /2 instead of /4

        // Leave CS deasserted
        DEASSERT_CS();

        // Pull the MCP23S17 out of reset (it's pulled down to GND on the board with a 100k pulldown
        // so it won't activate during AVR ISP programming)
        PORTB |= MCP23S17_RESET;

        _delay_ms(50);

        // All done!
        MCP23S17_Inited = true;
}

void MCP23S17_SetDDR(uint16_t ddr)
{
        // The MCP23S17's DDR is backwards from the AVR's.
        // I like dealing with it so it behaves like the AVR's,
        // so I invert any DDR values in this driver.
        // In other words, when you set or get the DDR through
        // this driver, the 1s and 0s are backwards from what
        // the MCP23S17's datasheet says, but they are
        // consistent with the AVR. I value the consistency more.
        MCP23S17_WriteBothRegs(MCP23S17_IODIRA, ~ddr);
}

void MCP23S17_SetPins(uint16_t data)
{
        MCP23S17_WriteBothRegs(MCP23S17_GPIOA, data);
}

uint16_t MCP23S17_ReadPins(void)
{
        return MCP23S17_ReadBothRegs(MCP23S17_GPIOA);
}

void MCP23S17_SetPullups(uint16_t pullups)
{
        MCP23S17_WriteBothRegs(MCP23S17_GPPUA, pullups);
}

// Determines the output values of output pins without reading any input pins
uint16_t MCP23S17_GetOutputs(void)
{
        return MCP23S17_ReadBothRegs(MCP23S17_OLATA);
}

uint16_t MCP23S17_GetDDR(void)
{
        // As I mentioned above, DDR bits are inverted from
        // what the MCP23S17's datasheet says, but
        // consistent with what the AVR's datasheet says
        return ~MCP23S17_ReadBothRegs(MCP23S17_IODIRA);
}

uint16_t MCP23S17_GetPullups(void)
{
        return MCP23S17_ReadBothRegs(MCP23S17_GPPUA);
}

uint8_t MCP23S17_ByteRW(uint8_t b)
{
        SPDR = b;
        while ((SPSR & (1 << SPIF)) == 0);
        return SPDR;
}

void MCP23S17_WriteBothRegs(uint8_t addrA, uint16_t value)
{
        // addrA should contain the address of the "A" register.
        // the chip should also be in "same bank" mode.

        ASSERT_CS();

        // Start off the communication by telling the MCP23S17 that we are writing to a register
        MCP23S17_ByteRW(MCP23S17_CONTROL_WRITE(0));

        // Tell it the first register we're writing to (the "A" register)
        MCP23S17_ByteRW(addrA);

        // Write the first byte of the register
        MCP23S17_ByteRW((uint8_t)((value >> 8) & 0xFF));

        // It should auto-increment to the "B" register, now write that
        MCP23S17_ByteRW((uint8_t)(value & 0xFF));

        DEASSERT_CS();
}

uint16_t MCP23S17_ReadBothRegs(uint8_t addrA)
{
        uint16_t returnVal;

        ASSERT_CS();

        // Start off the communication by telling the MCP23S17 that we are reading from a register
        MCP23S17_ByteRW(MCP23S17_CONTROL_READ(0));

        // Tell it which register we're reading from (the "A" register)
        MCP23S17_ByteRW(addrA);

        // Read the first byte of the register
        returnVal = (((uint16_t)MCP23S17_ByteRW(0)) << 8);

        // It should auto-increment to the "B" register, now read that
        returnVal |= MCP23S17_ByteRW(0);

        DEASSERT_CS();

        return returnVal;
}

int main()
{
	/*DDRB = 0xFF;
	DDRD = 0xFF;
	
	KS0108_CTRL_PORT |= KS0108_EN;
	_delay_ms(20);
	KS0108_CTRL_PORT &= ~KS0108_RS;
	KS0108_CTRL_PORT &= ~KS0108_CS1;
	KS0108_CTRL_PORT &= ~KS0108_CS2;
	
	KS0108_DATA_PORT = 0b00111110;
	
	KS0108_CTRL_PORT &= ~KS0108_EN;*/
	
	
	GLCD_Initalize();
	GLCD_ClearScreen();

	GLCD_GoTo(0,0);
	GLCD_WriteString("+-------------------+");
	GLCD_GoTo(0,1);
	GLCD_WriteString("|     Universal     |");
	GLCD_GoTo(0,2);
	GLCD_WriteString("|   KS0108 library  |");
	GLCD_GoTo(0,3);
	GLCD_WriteString("|                   |");
	GLCD_GoTo(0,4);
	GLCD_WriteString("|  en.radzio.dxp.pl |");
	GLCD_GoTo(0,5);
	GLCD_WriteString("|  STM32 Cortex-M3  |");
	GLCD_GoTo(0,6);
	GLCD_WriteString("|  microcontrollers |");
	GLCD_GoTo(0,7);
	GLCD_WriteString("+-------------------+");
	
	while(1)
	{
		
	}
	
	return 0;
}