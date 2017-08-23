#ifndef _GRAPH_
#define _GRAPH_

#define MAX_GRAPH 10
#define MAX_CHARTS 5

#include "GUIdef.h"
#include <stdbool.h>

typedef uint8_t GRAPH_Handle;

typedef struct{
	float x;
	float y;
} FPOINT;

typedef struct{
	FPOINT *p;
	uint16_t n;
	GUI_COLOR color;
	FPOINT checkPoint;
} CHART_struct;

typedef struct{
	uint16_t top;
	uint16_t left;
	
	uint16_t height;
	uint16_t width;
	
	
	//масштаб, количество единиц в пикселе
	float ScaleX,ScaleY;
	
	//смещение графика в экранных координатах/ graph offset in screen pts
	float OffsetX,OffsetY;
	
	
	bool showGrid;
	float VLineSpacing, HLineSpacing;	//шаг вспомогательных линий в координатах графика
																		//шаг вспомогательных линий в экранных координатах
	
	uint16_t VscrSpacing, HscrSpacing;	
	
	bool invalid;
	bool destroyed;
		
	char *xName;
	char *yName;
	
	CHART_struct chart[MAX_CHARTS];	
	
  FPOINT checkPoint;
	bool showCheckPoint;
	uint16_t checkPointdpX, checkPointdpY;	//text postion on graph
	
	
} GRAPH_struct;

GRAPH_Handle CreateGraph(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, char * xName, char * yName); 

void GRAPH_ToggleGrid(GRAPH_Handle hnd);
void GRAPH_SetChart(GRAPH_Handle hnd,uint8_t chartID, FPOINT *pts, uint16_t numPts, GUI_COLOR color);

void GRAPH_SetScale(GRAPH_Handle hnd, float scX, float scY);
void GRAPH_SetScaleX(GRAPH_Handle hnd, float scX);
void GRAPH_SetOffset(GRAPH_Handle hnd, float ofX, float ofY);

void GRAPH_Zoom(GRAPH_Handle hnd, float f);
void GRAPH_ZoomX(GRAPH_Handle hnd, float f);
void GRAPH_ZoomY(GRAPH_Handle hnd, float f);
	
void GRAPH_Translate(GRAPH_Handle hnd, float tx, float ty);


FPOINT GRAPH_GetScale(GRAPH_Handle hnd);
FPOINT GRAPH_GetOffset(GRAPH_Handle hnd);

/* Convert from screen point and return interpolated value */
void GRAPH_GetPoint(GRAPH_Handle hnd, uint16_t px, uint16_t py);

void GRAPH_DeleteChart(GRAPH_Handle hnd, uint8_t chartID);
#endif
