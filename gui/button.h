#ifndef _BUTTON_
#define _BUTTON_

//#include "stm32f4xx.h"

#define MAX_BUTTONS_IN_STORAGE 20
#define BUTTON_RESTORE_TIME 200	
#define MAX_COLOR_ANIM 12

#include <stdlib.h>
#include <stdbool.h>
#include "GUIdef.h"

typedef uint8_t BUTTON_Handle;

typedef enum{
	B_IDLE,
	B_PRESSED
	
} BUTTON_state;

typedef enum{
	B_STANDART,
	B_IMAGED,
	B_SKINNED
} BUTTON_style;




typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t width;
	uint16_t height;
	//BUTTON_state state;
	bool invalid;				//widget should be redrawed
	bool enabled;				//enable/disable callback func
	
	//GUI_COLOR colorUP;
	//GUI_COLOR colorPRESSED;
	char * str;
	
	
	bool destroyed;			//free
	bool pressed;
	
	IMAGE picUP;
  IMAGE picDOWN;
	BUTTON_style style;
	unsigned short animstate;	//0 - for pressed, 1..max_color_anim - unpressed
	
	uint8_t restoreCount;			//animation time, ms
	
	void (* cbFunc)();
} BUTTON_struct;


//добавить простую кнопку
BUTTON_Handle BUTTON_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text, void (*cbFunction)());
//добавить кнопку с заданными картинками для состояний
BUTTON_Handle BUTTON_CreateImg(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text, IMAGE  imgUp, IMAGE  imgDown, void (*cbFunction)());
void BUTTON_SetColor(BUTTON_Handle hnd, GUI_COLOR newColorUP, GUI_COLOR newColorPRESSED);
void BUTTON_SetText(BUTTON_Handle hnd, char *text);

//возвращает true  если кнопка нажата
bool BUTTON_Pressed(BUTTON_Handle hnd);

void BUTTON_Destroy(BUTTON_Handle hnd);

#endif
