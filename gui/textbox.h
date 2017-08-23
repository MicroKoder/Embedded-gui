#ifndef _TEXTBOX_
#define _TEXTBOX_

#define MAX_TEXTBOXES 30
#define MAX_LENGTH 16

#include "GUIDef.h"
#include <stdbool.h>

typedef char STRING[MAX_LENGTH];
typedef uint8_t TEXTBOX_Handle;
typedef enum{
	ALIGN_LEFT,
	ALIGN_RIGHN,
	ALIGN_CENTER
} TEXTBOX_ALIGN;



typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t width;
	uint16_t height;
	//BUTTON_state state;
	bool invalid;				//widget should be redrawed
	
	bool focused;
	bool enabled;
	bool pressed;
	
	GUI_COLOR color;
	GUI_COLOR colorFOCUSED;
	STRING text;
	TEXTBOX_ALIGN align;
	
	bool destroyed;			//free	
	
} TEXTBOX_struct;




TEXTBOX_Handle CreateTextbox(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text);

//задает цвета для обычного состояния и фокусированного состояния
void TEXTBOX_SetColor(TEXTBOX_Handle hnd, GUI_COLOR newColor, GUI_COLOR newColorFOCUSED);
void TEXTBOX_SetText(TEXTBOX_Handle hnd, char *text);

//возвращает true если элемент имеет фокус
bool TEXTBOX_Pressed(TEXTBOX_Handle hnd);
void DestroyTEXTBOX(TEXTBOX_Handle hnd);

#endif
