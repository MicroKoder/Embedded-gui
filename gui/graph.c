#include "graph.h"
extern GRAPH_struct graphStorage[MAX_GRAPH];

GRAPH_Handle CreateGraph(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, char * xName, char * yName){
	
	uint8_t i,k;
	i=0;
	while ((i< MAX_GRAPH) && (!graphStorage[i].destroyed)){
		++i;
	}
	
  
	
	graphStorage[i].left = xPos;
	graphStorage[i].top = yPos;
	
	graphStorage[i].width = width;
	graphStorage[i].height = height;
	
	graphStorage[i].xName = xName;
	graphStorage[i].yName = yName;
	
	graphStorage[i].destroyed = false;
  graphStorage[i].invalid = true;
	graphStorage[i].OffsetX = 0;
	graphStorage[i].OffsetY = 0;
	
	graphStorage[i].ScaleX = 1;
	graphStorage[i].ScaleY = 1;
	graphStorage[i].showGrid = true;
	
	graphStorage[i].showCheckPoint = false;
	for (k=0; k<MAX_CHARTS; k++){
		GRAPH_SetChart(i,k,0,0,0);
	}

	return i;
}

void GRAPH_ToggleGrid(GRAPH_Handle hnd){
	graphStorage[hnd].showGrid = !graphStorage[hnd].showGrid;
	graphStorage[hnd].invalid = true;
}

void GRAPH_CalcSpacing(GRAPH_Handle hnd){
	graphStorage[hnd].VscrSpacing = graphStorage[hnd].VLineSpacing * graphStorage[hnd].ScaleY;
	graphStorage[hnd].HscrSpacing = graphStorage[hnd].HLineSpacing * graphStorage[hnd].ScaleX;
}

void GRAPH_SetHSpacing(GRAPH_Handle hnd, float hSpacing){
	graphStorage[hnd].HLineSpacing = hSpacing;
	GRAPH_CalcSpacing(hnd);
}

void GRAPH_SetVSpacing(GRAPH_Handle hnd, float vSpacing){
	graphStorage[hnd].VLineSpacing = vSpacing;
	GRAPH_CalcSpacing(hnd);
}


void GRAPH_SetChart(GRAPH_Handle hnd,uint8_t chartID, FPOINT *pts, uint16_t numPts, GUI_COLOR color){
	graphStorage[hnd].chart[chartID].p = pts;
	graphStorage[hnd].chart[chartID].n = numPts;
	graphStorage[hnd].chart[chartID].color = color;
	
	graphStorage[hnd].VLineSpacing = 20;
	graphStorage[hnd].HLineSpacing = 20;
	GRAPH_CalcSpacing(hnd);
	graphStorage[hnd].invalid = true;
}

void GRAPH_SetScale(GRAPH_Handle hnd, float scX, float scY){
		graphStorage[hnd].ScaleX = scX;
		graphStorage[hnd].ScaleY = scY;
	graphStorage[hnd].invalid = true;
	
	GRAPH_CalcSpacing(hnd);
	graphStorage[hnd].showCheckPoint = false;
}
void GRAPH_SetScaleX(GRAPH_Handle hnd, float scX){
		graphStorage[hnd].ScaleX = scX;		
	graphStorage[hnd].invalid = true;
	
	GRAPH_CalcSpacing(hnd);
	graphStorage[hnd].showCheckPoint = false;
}

void GRAPH_SetOffset(GRAPH_Handle hnd, float ofX, float ofY){
	graphStorage[hnd].OffsetX = ofX;
	graphStorage[hnd].OffsetY = ofY;
	graphStorage[hnd].invalid = true;
	graphStorage[hnd].showCheckPoint = false;
}

void GRAPH_Translate(GRAPH_Handle hnd, float tx, float ty){
	graphStorage[hnd].OffsetX += tx;
	graphStorage[hnd].OffsetY += ty;
	graphStorage[hnd].invalid = true;
	graphStorage[hnd].showCheckPoint = false;
}

FPOINT GRAPH_GetScale(GRAPH_Handle hnd){
	FPOINT result;
	result.x = graphStorage[hnd].ScaleX;
	result.y = graphStorage[hnd].ScaleY;
	return result;
}

FPOINT GRAPH_GetOffset(GRAPH_Handle hnd){
	FPOINT result;
	result.x = graphStorage[hnd].OffsetX;
	result.y = graphStorage[hnd].OffsetY;
	return result;
}

void GRAPH_Zoom(GRAPH_Handle hnd, float f){
	graphStorage[hnd].ScaleX += f;
	graphStorage[hnd].ScaleY += f;
	
	if (graphStorage[hnd].ScaleX < 0.1f) graphStorage[hnd].ScaleX = 0.1f;
	if (graphStorage[hnd].ScaleY < 0.1f) graphStorage[hnd].ScaleY = 0.1f;
		
	GRAPH_CalcSpacing(hnd);
	
	graphStorage[hnd].invalid = true;
	graphStorage[hnd].showCheckPoint = false;
}

void GRAPH_ZoomX(GRAPH_Handle hnd, float f){
	graphStorage[hnd].ScaleX += f;
		
	if (graphStorage[hnd].ScaleX < 0.1f) graphStorage[hnd].ScaleX = 0.1f;
			
	GRAPH_CalcSpacing(hnd);
	
	graphStorage[hnd].invalid = true;
	graphStorage[hnd].showCheckPoint = false;
}

void GRAPH_ZoomY(GRAPH_Handle hnd, float f){
	graphStorage[hnd].ScaleY += f;
		
	if (graphStorage[hnd].ScaleY < 0.1f) graphStorage[hnd].ScaleX = 0.1f;
			
	GRAPH_CalcSpacing(hnd);
	
	graphStorage[hnd].invalid = true;
	graphStorage[hnd].showCheckPoint = false;
}

void GRAPH_GetPoint(GRAPH_Handle hnd, uint16_t px, uint16_t py){
	// преобразуем координаты нажатия в координаты графика
	uint8_t chart=0;
	uint16_t n,i;
	FPOINT *pts;
	FPOINT result;
	
	for (chart= 0; chart < 3; chart++){ 
	if (!graphStorage[hnd].chart[chart].n){
		result.x = 0;
		result.y = 0;
		graphStorage[hnd].chart[chart].checkPoint = result;
	//	return;
		break;
	} /*CHART is empty*/
	
	result.x = ((float)px - graphStorage[hnd].OffsetX) / graphStorage[hnd].ScaleX;
	
	n = graphStorage[hnd].chart[chart].n;			/*two lines for keep me in mind*/
	pts = graphStorage[hnd].chart[chart].p;	
	
	if (result.x < graphStorage[hnd].chart[chart].p[0].x || result.x > graphStorage[hnd].chart[chart].p[n-1].x){
		result.x = 0;
		result.y = 0;
		graphStorage[hnd].chart[chart].checkPoint = result;
		//return;
		break;
	}	/*out from bounds*/
	
	i=1; /**/ 
	
	while ( result.x > pts[i].x){
		i++;
	}	
	/*
		interpolate point between p[i-1] and p[i]
	*/
	result.y = (pts[i].y - pts[i-1].y)/(pts[i].x - pts[i-1].x)*(result.x - pts[i-1].x) + pts[i-1].y;
	graphStorage[hnd].chart[chart].checkPoint = result;
}// for all charts
	

	
	graphStorage[hnd].showCheckPoint = true;
	
	graphStorage[hnd].checkPointdpX = px;
	graphStorage[hnd].checkPointdpY = py;
	graphStorage[hnd].invalid = true;
	return;
}

void GRAPH_DeleteChart(GRAPH_Handle hnd, uint8_t chartID){
	graphStorage[hnd].chart[chartID].n = 0;
}

