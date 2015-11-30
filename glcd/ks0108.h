#ifndef	KS0108_H
#define KS0108_H

/* Port D is our data port, respective DB0 from LCD to PORTD0 from µC */
#define GLCD_DATA_PORT	PORTD
#define GLCD_DATA_DIR		DDRD
#define GLCD_DATA_PIN		PIND

/* Port B is our control port, D/I from LCD is connected to GLCD_RS */
#define CTRL_PORT	PORTB
#define CTRL_DIR	DDRB

#define GLCD_RS		(1 << 2)	//PB2
#define GLCD_RW		(1 << 1)	//PB1
#define GLCD_EN		(1 << 0)	//PB0
#define GLCD_CS1	(1 << 6)	//PB6
#define GLCD_CS2	(1 << 5)	//PB5

/* Define screen properties */
#define GLCD_SCREEN_WIDTH	128
#define GLCD_SCREEN_HEIGHT	64

/* Define specific commands */
#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
  #define ON	0x01
  #define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

/* Define the speed of the ticker in µs */
#define TICKER_SPEED 800

extern unsigned char screen_x;
extern unsigned char screen_y;

void GLCD_Delay(void);
void GLCD_EnableController(unsigned char);
void GLCD_DisableController(unsigned char);
unsigned char GLCD_ReadStatus(unsigned char);
void GLCD_WriteCommand(unsigned char, unsigned char);
unsigned char GLCD_ReadData(void);
void GLCD_WriteData(unsigned char);
void GLCD_InitalizePorts(void);
unsigned char GLCD_ReadByteFromROMMemory(char *);
void GLCD_Initalize(void);
void GLCD_GoTo(unsigned char, unsigned char);
void GLCD_ClearScreen(void);
void GLCD_WriteChar(char);
void GLCD_WriteString(char *);
void GLCD_SetPixel(unsigned char, unsigned char, unsigned char);

#endif


