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

int main()
{	
	GLCD_Initalize();
	GLCD_ClearScreen();
	
	GLCD_GoTo(0,0);
	GLCD_WriteString(" #   #######  ######");
	GLCD_GoTo(0,1);
	GLCD_WriteString(" #      #     #     ");
	GLCD_GoTo(0,2);
	GLCD_WriteString(" #      #     #     ");
	GLCD_GoTo(0,3);
	GLCD_WriteString(" #      #     #_____");
	GLCD_GoTo(0,4);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,5);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,6);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,7);
	GLCD_WriteString(" #      #     ######");
	
	while(1)
	{
		
	}
	
	return 0;
}