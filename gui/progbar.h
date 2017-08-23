#ifndef _PROGBAR_
#define _PROGBAR_

#define MAX_PROGRESS_BARS 10

#include "GUIdef.h"
#include <stdbool.h>
typedef uint8_t PROGBAR_Handle;
typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t width;
	uint16_t height;
	
	uint8_t value;
	
	bool invalid;
	bool destroyed;
	
 uint8_t str[5];	//4 символа на цифры
	
} PROGBAR_struct;


PROGBAR_Handle PROGBAR_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h);

void PROGBAR_SetValue(PROGBAR_Handle hnd, uint8_t percent);
uint8_t PROGBAR_GetValue(PROGBAR_Handle hnd);


#endif
