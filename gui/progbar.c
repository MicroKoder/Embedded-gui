#include "progbar.h"

/********************************************************************************************************************
**************************************************** PROGRESS BAR ***************************************************
*********
*********
*********************************************************************************************************************/

extern PROGBAR_struct progbarStorage[MAX_PROGRESS_BARS];

PROGBAR_Handle PROGBAR_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h){
	uint8_t i=0;
	while ((i< MAX_PROGRESS_BARS) && (!progbarStorage[i].destroyed)){
		++i;
	}
	progbarStorage[i].left = xPos;
	progbarStorage[i].top = yPos;
	progbarStorage[i].width = w;
	progbarStorage[i].height = h;
	progbarStorage[i].invalid = true;
	progbarStorage[i].destroyed = false;
	
	return i;
}

void PROGBAR_SetValue(PROGBAR_Handle hnd, uint8_t percent){

	if (percent > 100){
	progbarStorage[hnd].value = 100;		
	} else
	{
		progbarStorage[hnd].value = percent;		
	}
	
	progbarStorage[hnd].invalid = true;
	return;
}

uint8_t PROGBAR_GetValue(PROGBAR_Handle hnd){
	return progbarStorage[hnd].value;
}
