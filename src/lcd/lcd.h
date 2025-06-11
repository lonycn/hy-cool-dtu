#ifndef __LCD_H
#define __LCD_H	 

#define  u8     unsigned char
#define  u16    uint16_t
void ClearRs(void);
void SetRs(void);
void ClearRW(void);
void SetR(void);
void ClearCE(void);
void SetCE(void);
void SetBL(void);
void ClrBL(void);
void  LCDInital(void);
void CodeLcdData(unsigned char dat);
void delay_50us(unsigned char Delay);
void waitMS(unsigned char tt);
void delay1ms(int16_t x)	;			 //µ•Œª£®1ms£©—” ±
void LCD_init(void);
unsigned char LCD_cmd(unsigned char cmd);
unsigned char LCD_char(char str);
unsigned char setxy(char x,char y);
//unsigned char  LCD_string(unsigned char x,unsigned char y,unsigned char *s);
unsigned char  wait_until_ready(void);
void LCD4_Clear(void);
void LCD4_On(void);
void LCD4_Off(void);
void LCD4_Home(void);
void LCD4_Home2(void);
void LCD4_PutC(unsigned char dat);
void LCD4_PutC_Const(const unsigned char dat);
void LCD4_PutS(unsigned char *dat);
void DisplayTitle(unsigned char DisplayMode);
void FloatConvert2ASCII(int16_t floatVal,unsigned char *p,unsigned char type);
void   DisplayTemp(unsigned char *VarList,unsigned char DisplayMode);

		 				    
#endif
