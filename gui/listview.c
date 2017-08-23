#include "listview.h"

#include <cstring>

extern LISTVIEW_struct listviewStorage[MAX_LISTVIEW];

LISTVIEW_struct * LISTVIEW_Create(uint16_t xPos, uint16_t yPos, uint16_t w, uint16_t h){
uint8_t i=0;
	while ((i< MAX_LISTVIEW) && (!listviewStorage[i].destroyed)){
		++i;
	}
	if (i== MAX_LISTVIEW) return 0;	//ERROR value
	
	listviewStorage[i].left =xPos;
	listviewStorage[i].top = yPos;
	
	listviewStorage[i].colCount = 0;
	
	listviewStorage[i].colWidth = 100;
	listviewStorage[i].rowCount = 1;
	listviewStorage[i].rowHeight = 30;
	
	listviewStorage[i].destroyed = false;
	listviewStorage[i].invalid = true;
	
	listviewStorage[i].width = w;
	listviewStorage[i].height = h;
	
	listviewStorage[i].offsetV = 0;
	listviewStorage[i].maxOffsetV = 0;
	
	listviewStorage[i].selectedCol = -1;
	listviewStorage[i].selectedRow = -1;
	
	memset(listviewStorage[i].cell,'\0', MAX_COLS*MAX_ROWS*MAX_LENGTH);
	
	return &listviewStorage[i];
}

LISTVIEW_struct *LISTVIEW_GetPointer(LISTVIEW_Handle hnd){
	return &listviewStorage[hnd];
}

void  LISTVIEW_AddCol(LISTVIEW_struct *p, char* header){
	if (p->colCount < MAX_COLS){
		p->colCount++;
		memcpy(p->cell[0][p->colCount-1], header, strlen(header));
	}
	
	
}

void CalcMaxOffset(LISTVIEW_struct* plist){
	plist->maxOffsetV = (plist->rowCount*plist->rowHeight) - plist->height;
	if (plist->maxOffsetV < 0) plist->maxOffsetV = 0;
	
}

void LISTVIEW_AddRow(LISTVIEW_struct *p){
	//short i;
	if (p->rowCount == MAX_ROWS) return;
	//else
	p->rowCount++;
	
		
	CalcMaxOffset(p);
	p->invalid = true;
}

void LISTVIEW_DeleteRow(LISTVIEW_struct *p){
	if (p->rowCount > 1) 
		p->rowCount --;
	
	CalcMaxOffset(p);
	p->invalid = true;
	
	p->selectedCol = -1;
	p->selectedRow = -1;
}

void LISTVIEW_Slide(LISTVIEW_struct *p, short dy){
			p->offsetV -= dy;
			p->invalid = true;
	
	if (p->offsetV < 0)
		p->offsetV = 0;
	
	if (p->offsetV > p->maxOffsetV)
		p->offsetV = p->maxOffsetV;
	
	p->selectedCol = -1;
	p->selectedRow = -1;
}

void LISTVIEW_Select(LISTVIEW_struct * pLst, uint16_t touchX, uint16_t touchY){
		pLst->selectedCol = (touchX - pLst->left)/pLst->colWidth;
		pLst->selectedRow = (touchY - pLst->top + pLst->offsetV)/pLst->rowHeight;
	
	if (pLst->selectedCol > pLst->colCount-1) pLst->selectedCol = -1;
	if (pLst->selectedRow > pLst->rowCount-1) pLst->selectedRow = -1;
}

void LISTVIEW_SetCell(LISTVIEW_struct *p, uint8_t row, uint8_t col, char* text){
	memcpy(p->cell[row][col], text, strlen(text));
	p->invalid = true;
}

void LISTVIEW_Destroy(LISTVIEW_struct *p){
	p->destroyed = true;
}
