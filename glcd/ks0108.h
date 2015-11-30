

/* Port D is our data port, respective DB0 from LCD to PORTD0 from µC */
#define KS0108_DATA_PORT	PORTD
#define KS0108_DATA_DIR		DDRD
#define KS0108_DATA_PIN		PIND

/* Port B is our control port, D/I from LCD is connected to KS0108_RS */
#define CTRL_PORT	PORTB
#define CTRL_DIR	DDRB

#define KS0108_RS			(1 << 2)	//PB2
#define KS0108_RW			(1 << 1)	//PB1
#define KS0108_EN			(1 << 0)	//PB0

#define KS0108_CS1			(1 << 6)	//PB6
#define KS0108_CS2			(1 << 5)	//PB5
#define KS0108_CS3			(1 << 7)	//PB7

#define KS0108_SCREEN_WIDTH		128
#define KS0108_SCREEN_HEIGHT	64


#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
  #define ON	0x01
  #define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

extern unsigned char screen_x;
extern unsigned char screen_y;

void GLCD_Delay(void);
void GLCD_Initalize(void);
void GLCD_WriteData(unsigned char);
void GLCD_WriteCommand(unsigned char, unsigned char);
void GLCD_ClearScreen(void);
void GLCD_GoTo(unsigned char, unsigned char);
void GLCD_WriteString(char *);
unsigned char GLCD_ReadByteFromROMMemory(char *);
unsigned char GLCD_ReadData(void);
void GLCD_Bitmap(char *, unsigned char, unsigned char, unsigned char, unsigned char);



