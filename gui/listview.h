#ifndef _LISTVIEW_
#define _LISTVIEW_

#define MAX_ROWS 20
#define MAX_COLS 4
#define MAX_LENGTH 16


#define MAX_LISTVIEW 1

#include <stdbool.h>
#include "GUIdef.h"

typedef uint8_t LISTVIEW_Handle;
typedef char STRING[MAX_LENGTH];



typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t width;
	uint16_t height;
	//BUTTON_state state;
	bool invalid;				//widget should be redrawed
	bool enabled;				//enable/disable callback func
	
	uint8_t rowCount;
	uint8_t colCount;
	
	uint8_t rowHeight;
	uint8_t colWidth;
	
	
	int selectedRow;
	int selectedCol;
	STRING cell[MAX_ROWS][MAX_COLS];
	
	int offsetV;
	int maxOffsetV;	
	
	bool destroyed;
	
} LISTVIEW_struct;

LISTVIEW_struct * LISTVIEW_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h);
void LISTVIEW_AddCol(LISTVIEW_struct *p, char* header);
void LISTVIEW_AddRow(LISTVIEW_struct *p);


void LISTVIEW_Select(LISTVIEW_struct * pLst, uint16_t touchX, uint16_t touchY);
void LISTVIEW_Slide(LISTVIEW_struct *p, short dy);
void LISTVIEW_DeleteRow(LISTVIEW_struct *p);
void LISTVIEW_SetCell(LISTVIEW_struct *p, uint8_t row, uint8_t col, char* text);
void LISTVIEW_Destroy(LISTVIEW_struct *p);


#endif
