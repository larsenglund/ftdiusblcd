#include "hd44780-charmap.h"


//      HD44780 Instruction Set             DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//      =======================             === === === === === === === ===
#define Clear_Display                0x01 // 0   0   0   0   0   0   0   1

#define Return_Home                  0x02 // 0   0   0   0   0   0   1   *

#define Set_Entry_Mode               0x04 // 0   0   0   0   0   1  I/D  S

#define Set_Display                  0x08 // 0   0   0   0   1   D   C   B

#define Set_Cursor_and_Display_Shift 0x10 // 0   0   0   1  S/C R/L  *   *

#define Set_Function                 0x20 // 0   0   1   DL  N   F   *   *

#define Set_CGRAM_Address            0x40 // 0   1   A   A   A   A   A   A

#define Set_DDRAM_Address            0x80 // 1   A   A   A   A   A   A   A

//      HD44780 Parameters 
//      ==================
//      N.B. explicit values for EVERY corresponding parameter
//      ==== MUST be passed each time any instruction is used

//      Set_Entry_Mode
//      ==============
#define Decrement_Address            0x00 // .   .   .   .   .   .   0   .
#define Increment_Address            0x02 // .   .   .   .   .   .   1   .

#define Shift_Display_Off            0x00 // .   .   .   .   .   .   .   0
#define Shift_Display_On             0x01 // .   .   .   .   .   .   .   1

//      Set_Display
//      ===========
#define Display_Off                  0x00 // .   .   .   .   .   0   .   .
#define Display_On                   0x04 // .   .   .   .   .   1   .   .

#define Cursor_Off                   0x00 // .   .   .   .   .   .   0   .
#define Cursor_On                    0x02 // .   .   .   .   .   .   1   .

#define Blink_Off                    0x00 // .   .   .   .   .   .   .   0
#define Blink_On                     0x01 // .   .   .   .   .   .   .   1

//      Set_Cursor_and_Display_Shift
//      ============================
#define Cursor                       0x00 // .   .   .   .   0   .   .   .
#define Display_and_Cursor           0x08 // .   .   .   .   1   .   .   .

#define Left                         0x00 // .   .   .   .   .   0   .   .
#define Right                        0x04 // .   .   .   .   .   1   .   .

//      Set_Function
//      ============
#define Data_Length_4                0x00 // .   .   .   0   .   .   .   .
#define Data_Length_8                0x10 // .   .   .   1   .   .   .   .

#define One_Display_Line             0x00 // .   .   .   .   0   .   .   .
#define Two_Display_Lines            0x08 // .   .   .   .   1   .   .   .

#define Font_5x7                     0x00 // .   .   .   .   .   0   .   .
#define Font_5x10                    0x04 // .   .   .   .   .   1   .   .

#define Line2_Offset 0x40


#define FTDI_HD44780_GPOI 0x80
#define FTDI_HD44780_E 0x40
#define FTDI_HD44780_RS 0x20
#define FTDI_HD44780_RW 0x10
#define FTDI_HD44780_DATABITS 0x0F


class ftdi_hd44780 {
private:
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	DWORD bytesWritten;
	unsigned char port;

	void USBWrite(unsigned char byte) {
		//Sleep(1);
		FT_Write(ftHandle, &byte, 1, &bytesWritten);
		//Sleep(1);
	};

	void pulse_e() {
		port |= FTDI_HD44780_E;
		USBWrite(port);
		port &= ~FTDI_HD44780_E;
		USBWrite(port);
	};

public:
	ftdi_hd44780(FT_HANDLE handle) {
		ftHandle = handle;
		port = 0x00;
		USBWrite(port);
	};

	void outnibble(unsigned char n, unsigned char rs) {
		port &= ~FTDI_HD44780_RW;
		//USBWrite(port);
		if (rs) {
			port |= FTDI_HD44780_RS;
		}
		else {
			port &= ~FTDI_HD44780_RS;
		}
		//USBWrite(port);
		port = (port & ~FTDI_HD44780_DATABITS) | (n & FTDI_HD44780_DATABITS);
		USBWrite(port);
		pulse_e();
	};

	void outbyte(unsigned char b, unsigned char rs) {
		outnibble(b >> 4, rs);
		outnibble(b & 0xf, rs);
	};

	void outCMD(unsigned char cmd) {
		outbyte(cmd, 0);
	};

	void outData(unsigned char data) {
		outbyte(data, 1);
	};

	void outChar(char ch) {
		outData(HD44780_charmap[(unsigned char) ch]);
	};

	void outString(char *str) {
		for (int i = 0; str[i]; i++) {
			outChar(str[i]);
		}
	};
};