//************************************************************************
// Bitmap.C
//************************************************************************
#include <avr/pgmspace.h>
#include "Bitmap.h"
#include "KS0108.h"
//##########################################################
void LoadBitmap(unsigned char *bitmap)
{
uint16_t i, j,by;
for(i=0; i<64; i+=8)

		for(j=0; j<128; j++)
		{
			by=pgm_read_byte(bitmap++);
			GLCD_GoTo(j, i);
			GLCD_WriteData(by);
		}
}
