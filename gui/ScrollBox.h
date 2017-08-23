#ifndef _SCROLLBOX_
#define _SCROLLBOX_

#define MAX_SCROLLBARS 10

#include "GUIdef.h"
#include <stdbool.h>

typedef uint8_t SCROLLBOX_Handle;

typedef enum{
	VER,
	HOR
} SCROLLBAR_orient;

typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t height;
	uint16_t width;
	
	//float value;
	int sliderPos;
	uint16_t sliderMaxPos;
	
	SCROLLBAR_orient orient;
	bool invalid;
	bool destroyed;
	bool changed;
	
} SCROLLBOX_struct;

//создает элемент управления прокруткой с кнопками и бегунком
//xPos, yPos - положение, l - длина в пикселях
SCROLLBOX_Handle CreateHScrollBox(uint16_t xPos, uint16_t yPos, uint16_t l);

//true если было изменено значение
bool SCROLLBOX_changed(SCROLLBOX_Handle hnd);
//void Slide(PROGBAR_Handle hnd, uint8_t percent);

//возвращает положение в процентах
float GetScrollValue(SCROLLBOX_Handle hnd);
//задает положение в процентах
void SetScrollValue(SCROLLBOX_Handle hnd, float newVal);


//обработка тача, 
void SCROLLBOX_processTouch(SCROLLBOX_Handle hnd, uint16_t rx, uint16_t ry);
#endif
