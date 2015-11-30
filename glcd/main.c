/*
 * Title:	Print ITS by use of characters on a 128x64 GLCD
 * Author:	Stefan Binna
 *
 * Special thanks to Rados?aw Kwiecie? for using part of his ks0108 library: http://en.radzio.dxp.pl/ks0108/
 * 
 */

#include <avr/io.h>
#include <util/delay.h>
#include "KS0108.h"
#include "font5x8.h"

int main()
{	
	GLCD_Initalize();
	
	while(1)
	{
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
		
		_delay_ms(1000);
	}
	
	return 0;
}