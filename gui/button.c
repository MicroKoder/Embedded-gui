
#include "button.h"
#define ERROR_VALUE UINT8_MAX

extern BUTTON_struct buttonStorage[MAX_BUTTONS_IN_STORAGE];
/**************************************************************************************************************
**************************************************** BUTTON ***************************************************
*********
*********
***************************************************************************************************************
*/
bool BUTTON_Pressed(BUTTON_Handle hnd){
	return (buttonStorage[hnd].pressed && !buttonStorage[hnd].destroyed);
}

BUTTON_Handle BUTTON_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text, void (*cbFunction)()){	
	uint8_t i=0;
	while ((i< MAX_BUTTONS_IN_STORAGE) && (!buttonStorage[i].destroyed)){
		++i;
	}
	if (i== MAX_BUTTONS_IN_STORAGE) return ERROR_VALUE;	//ERROR value
	
	buttonStorage[i].top = yPos;
	buttonStorage[i].left = xPos;
	
	buttonStorage[i].width = w;
	buttonStorage[i].height = h;
	
	buttonStorage[i].str = text;
	buttonStorage[i].cbFunc = cbFunction;
	
	buttonStorage[i].destroyed = false;
	buttonStorage[i].enabled = true;
	buttonStorage[i].invalid = true;		//button need redraw first
//	buttonStorage[i].state = B_IDLE;
	buttonStorage[i].style = B_STANDART;
	buttonStorage[i].animstate = 0;	
	buttonStorage[i].restoreCount = 0;
	//buttonStorage[i].colorPRESSED = COLOR_BLUE2;
	//buttonStorage[i].colorUP = COLOR_BLUE;
	buttonStorage[i].pressed = false;
	return i;
}

BUTTON_Handle BUTTON_CreateImg(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text, IMAGE  imgUp, IMAGE  imgDown, void (*cbFunction)()){	
	uint8_t i=0;
	while ((i< MAX_BUTTONS_IN_STORAGE) && (!buttonStorage[i].destroyed)){
		++i;
	}
	
		if (i== MAX_BUTTONS_IN_STORAGE) return ERROR_VALUE;	//ERROR value
	
	buttonStorage[i].top = yPos;
	buttonStorage[i].left = xPos;
	
	buttonStorage[i].width = w;
	buttonStorage[i].height = h;
	
	buttonStorage[i].str = text;
	buttonStorage[i].cbFunc = cbFunction;
	
	buttonStorage[i].destroyed = false;
	buttonStorage[i].enabled = true;
	buttonStorage[i].invalid = true;		//button need redraw first
//	buttonStorage[i].state = B_IDLE;
	buttonStorage[i].style = B_IMAGED;	
	
	buttonStorage[i].pressed = false;
	buttonStorage[i].picUP = imgUp;
	buttonStorage[i].picDOWN = imgDown;
	
	return i;
}

void BUTTON_SetColor(BUTTON_Handle hnd, GUI_COLOR newColorUP, GUI_COLOR newColorPRESSED){
	if (hnd == ERROR_VALUE) return;	//верить нельзя никому
	
	
	//buttonStorage[hnd].colorUP = newColorUP;
	//buttonStorage[hnd].colorPRESSED = newColorPRESSED;	
	buttonStorage[hnd].invalid = true;
	return;
		
}
void BUTTON_SetText(BUTTON_Handle hnd, char *text){
	if (hnd == ERROR_VALUE) return;
	
	buttonStorage[hnd].str = text;
	buttonStorage[hnd].invalid = true;
}
void BUTTON_Destroy(BUTTON_Handle hnd){
	if (hnd == ERROR_VALUE) return;
	
	buttonStorage[hnd].destroyed = true;
}
