#include "textbox.h"

#include <cstring>

extern TEXTBOX_struct textboxStorage[MAX_TEXTBOXES];



TEXTBOX_Handle CreateTextbox(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h, char *text){
	uint8_t i=0;
	while ((i< MAX_TEXTBOXES) && (!textboxStorage[i].destroyed)){
		++i;
	}
	
	textboxStorage[i].top = yPos;
	textboxStorage[i].left=  xPos;
	
	textboxStorage[i].width = w;
	textboxStorage[i].height = h;
	
	textboxStorage[i].invalid = true;				//widget should be redrawed
	textboxStorage[i].enabled = true;
	
	textboxStorage[i].color = COLOR_BLACK;
	textboxStorage[i].colorFOCUSED = COLOR_SKY;
	
	//memcpy(textboxStorage[i].text, text, strlen(text));
	//textboxStorage[i].text = text;
	memset(textboxStorage[i].text, '\0',MAX_LENGTH);
	TEXTBOX_SetText(i,text);
	
	
	textboxStorage[i].align = ALIGN_LEFT;
	textboxStorage[i].focused = false;
	textboxStorage[i].pressed = false;
	textboxStorage[i].destroyed = false;
	
	return i;
}

void TEXTBOX_SetColor(TEXTBOX_Handle hnd, GUI_COLOR newColor, GUI_COLOR newColorFOCUSED){
	textboxStorage[hnd].color = newColor;
	textboxStorage[hnd].colorFOCUSED = newColorFOCUSED;
	textboxStorage[hnd].invalid = true;
}
	
void TEXTBOX_SetText(TEXTBOX_Handle hnd, char *text){
	memset(textboxStorage[hnd].text, '\0',MAX_LENGTH);
	memcpy(textboxStorage[hnd].text, text, strlen(text));
	
	textboxStorage[hnd].invalid = true;
}

bool TEXTBOX_Pressed(TEXTBOX_Handle hnd){
		return (textboxStorage[hnd].pressed && (!textboxStorage[hnd].destroyed));
}
void DestroyTEXTBOX(TEXTBOX_Handle hnd){
	textboxStorage[hnd].destroyed = true;
}
