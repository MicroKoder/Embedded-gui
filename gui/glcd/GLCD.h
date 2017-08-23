/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.h
** Descriptions:		Has been tested SSD1289、ILI9320、R61505U、SSD1298、ST7781、SPFD5408B、ILI9325、ILI9328、
**						HX8346A、HX8347A
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2012-2-28
** Version:				2.1
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

#ifndef __GLCD_H 
#define __GLCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
#define DISP_ORIENTATION  0  /* angle 0 90 */ 

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240   

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  240
#define  MAX_Y  320   

#endif

/* LCD Registers */
#define R34            0x22

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

/******************************************************************************
* Function Name  : RGB565CONVERT
* Description    : 24位转换16位
* Input          : - red: R
*                  - green: G 
*				   - blue: B
* Output         : None
* Return         : RGB 颜色值
* Attention		 : None
*******************************************************************************/
#define RGB565CONVERT(red, green, blue)\
(uint16_t)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))

/* Private function prototypes -----------------------------------------------*/
void LCD_Init(void);
void LCD_Clear(uint16_t Color);
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);

//***
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color );
void LCD_FillRect(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1, uint16_t Color);
void LCD_Rect(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1, uint16_t Color);


void LCD_SetClip(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_PutChar( uint16_t Xpos, uint16_t Ypos, char ASCI, uint16_t charColor, uint16_t bkColor );
void LCD_PutTransparentChar( uint16_t Xpos, uint16_t Ypos, char ASCI, uint16_t charColor);
void LCD_Text(uint16_t Xpos, uint16_t Ypos, char *str,uint16_t Color, uint16_t bkColor);
void LCD_TransparentText(uint16_t Xpos, uint16_t Ypos, char *str,uint16_t Color);
//void PutChinese(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor);
//void GUI_Chinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor);
//void LCD_DrawButton(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint8_t * str);

//********************
void LCD_DrawDottedHLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t dash, uint16_t color);
void LCD_DrawDottedVLine(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t dash, uint16_t color);

void LCD_DrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void LCD_DrawVLine(uint16_t y0, uint16_t y1, uint16_t x, uint16_t color);

//void LCD_DrawImage(uint16_t x, uint16_t y,uint16_t size_w, uint16_t size_h, const unsigned short *data);
void DrawImage(uint16_t x, uint16_t y,uint16_t size_w, uint16_t size_h, const unsigned short *data);

void LCD_DrawTime(uint16_t x, uint16_t y, int timeMS);


#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
