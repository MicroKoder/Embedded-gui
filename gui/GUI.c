#include "GUI.h"
//#include "TouchPanel.h"

volatile int systemTimeMS=0;

#include <cstring>
#include <stdio.h>

 BUTTON_struct buttonStorage[MAX_BUTTONS_IN_STORAGE];
 PROGBAR_struct progbarStorage[MAX_PROGRESS_BARS];
 TEXTBOX_struct textboxStorage[MAX_TEXTBOXES];
 SCROLLBOX_struct scrollbarStorage[MAX_SCROLLBARS];
 GRAPH_struct graphStorage[MAX_GRAPH];
 LISTVIEW_struct listviewStorage[MAX_LISTVIEW];
//IMAGE imgStorage[MAX_IMAGES]

static TEXTBOX_Handle hFocusedTextBox;

static uint16_t GUI_textColor = COLOR_WHITE;

//static uint16_t lastTime;

GRAPH_Handle pickGraph = UINT8_MAX;
FPOINT graphPoint;
//////////////////////////////////////////////////////////////////////////////////////////
////
////														CUSTOM DRAWING
////
///////////////////////////////////////////////////////////////////////////////////////////

/////////////****************************************  PROGBAR ******************************************

void DrawProgbar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t value){
//	uint16_t i=0;
	char str[5];
	uint16_t f = (uint16_t)((float)value/100.f * (float)(w-1));
	/* clearing rect*/
	/*for (i=0; i<h; ++i){
		LCD_DrawLine(x,y+i,x+w,y+i,Black);		
	}*/
	LCD_FillRect(x,y,x+w,y+h,COLOR_BLACK);
	
	LCD_DrawLine(x,y, x+w, y, 			COLOR_GREY);
	LCD_DrawLine(x,y+h, x+w, y+h, 	COLOR_GREY);
	LCD_DrawLine(x,y, x, y+h, 			COLOR_GREY);
	LCD_DrawLine(x+w,y, x+w, y+h, 	COLOR_GREY);
	
	if (value>0){
	/*	for (i=1; i<(h-1); ++i){
			LCD_DrawLine(x+2,y+i,x+f,y+i, COLOR_CYAN);		
		}*/
		LCD_FillRect(x+1,y+1,x+f, y+h, COLOR_CYAN);
	}
	
	sprintf(str,"%d%%",value);
	LCD_TransparentText(x+w/2,y+h/2,str, COLOR_GREY);
	//LCD_DrawLine
}
/////////////****************************************  BUTTON ******************************************
void DrawButton(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, char * str, uint16_t color)
{
	//int k;
	
	

	LCD_FillRect(x0,y0, x0+w, y0+h,color);
	
/*	for(k=y0+1; k<y0+h; k++){
		LCD_DrawLine( x0+1, k, x0+w-1, k, color );
	}*/
	
	/*
	LCD_DrawLine( x0, y0, x0+w, y0, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0, x0+w, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0+h, x0, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0, y0+h, x0, y0, COLOR_WHITE );
	*/
	LCD_Rect(x0, y0, x0 + w, y0 + h, COLOR_WHITE);
	
	
	
	if (str)
	LCD_TransparentText(x0+w/2 - strlen(str)*4, y0+h/2-8, str,GUI_textColor);
}  


/////////////****************************************  IMAGEDBUTTON ******************************************
void DrawImagedButton(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, char * str,  const unsigned short * data){
	DrawImage(x0,y0,w,h,(data));
	if (str)
		LCD_TransparentText(x0+w/2- strlen(str)*4, y0+h/2-8,str,GUI_textColor);
}

/////////////****************************************  TEXTBOX ******************************************
void DrawTextbox(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, char * str, uint16_t bkColor)
{
	int k;
	LCD_DrawLine( x0, y0, x0+w, y0, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0, x0+w, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0+h, x0, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0, y0+h, x0, y0, COLOR_WHITE );
	
	
	for(k=y0+1; k<y0+h; k++){
		LCD_DrawLine( x0+1, k, x0+w-1, k, bkColor );
	}
	if (str)
	LCD_TransparentText(x0+w/2 - strlen(str)*4, y0+h/2-8, str,GUI_textColor);	//8-text height, need refactor
}

//*********************************************************** H scroll bar *****************************
#define SCROLLBAR_H 20
void DrawHScrollBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t sliderPos){
	uint16_t h = SCROLLBAR_H;
	int k;
	/*clear*/
	/*for(k=y0+1; k<y0+h; k++){
		LCD_DrawLine( x0+1, k, x0+w-1, k, COLOR_BLUE2 );
	}*/
	LCD_FillRect(x0,y0,x0+w,y0+h,COLOR_BLACK);
	
	LCD_DrawLine( x0, y0, x0+w, y0, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0, x0+w, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0+w, y0+h, x0, y0+h, COLOR_WHITE );
	LCD_DrawLine( x0, y0+h, x0, y0, COLOR_WHITE );
	
	/*buttons*/
	LCD_DrawLine(x0 + h, y0,x0 + h,y0+h, COLOR_CYAN);
	LCD_DrawLine(x0 + w - h, y0,x0 + w - h,y0+h, COLOR_CYAN);
	
	LCD_DrawLine (x0, y0 + h/2, x0 + h, y0, COLOR_CYAN);
	LCD_DrawLine (x0, y0 + h/2, x0 + h, y0 + h, COLOR_CYAN);
	
	LCD_DrawLine (x0 + w - h, y0, x0 + w, y0 + h/2, COLOR_CYAN);
	LCD_DrawLine (x0 + w - h, y0 + h, x0 + w, y0 + h/2, COLOR_CYAN);
	
	
	//p = (int)(((float)w - 3.f*(float)h)/100.f*(float)value + h);
	
	for(k=y0+1; k<y0+h; k++){
		LCD_DrawLine( x0+h+sliderPos, k, x0+h+sliderPos+h, k, COLOR_CYAN );
	}
}

//*********************************************************** GRAPH ************************************************

void DrawGraph(GRAPH_Handle hnd){
	char text[28];
	uint16_t k,i;
	uint16_t xPos = graphStorage[hnd].left;
	uint16_t yPos = graphStorage[hnd].top;
	
	uint16_t w = graphStorage[hnd].width;
	uint16_t h = graphStorage[hnd].height;
	
	float x0, y0;
	float x1, y1;
	
	
	
	//for(k=yPos; k<yPos+h; k++)	
	//	LCD_DrawLine( xPos, k, xPos + w, k, COLOR_BLACK);		
	LCD_FillRect(xPos,yPos,xPos+w,yPos+h,COLOR_BLACK);
	//LCD_Clear(COLOR_BLACK);
	
		
	//c** clear area **
	LCD_SetClip(xPos, yPos, xPos + w,  yPos+h);
	
	SetCliparea(0,0, w, h);//настраиваем отсечение в координатах виджета
	
	
	for (k=0; k<MAX_CHARTS; k++){	//дл€ каждой кривой
		if (graphStorage[hnd].chart[k].n < 2) continue;
		
		x0 = graphStorage[hnd].chart[k].p[0].x;
		y0 = graphStorage[hnd].chart[k].p[0].y;
		
		for (i=1; i< graphStorage[hnd].chart[k].n; i++){
			x1 = graphStorage[hnd].chart[k].p[i].x;
			y1 = graphStorage[hnd].chart[k].p[i].y;
			
			/*масштабируем*/
			x0 *= graphStorage[hnd].ScaleX;
			y0 *= graphStorage[hnd].ScaleY;
			
			x1 *= graphStorage[hnd].ScaleX;
			y1 *= graphStorage[hnd].ScaleY;			
			
			/*смещаем*/
			x0 += graphStorage[hnd].OffsetX;
			y0 += graphStorage[hnd].OffsetY;
			
			x1 += graphStorage[hnd].OffsetX;
			y1 += graphStorage[hnd].OffsetY;
			
			/*смотрим что получилось*/
			
			if (V_FCclip(&x0, &y0, &x1, &y1)==1){			//if line visible
			LCD_DrawLine(	xPos + 			(uint16_t)x0,
										yPos + h - 	(uint16_t)y0, 
										xPos + 			(uint16_t)x1, 
										yPos + h - 	(uint16_t)y1,
										graphStorage[hnd].chart[k].color);
			}
			
			x0 = graphStorage[hnd].chart[k].p[i].x;
			y0 = graphStorage[hnd].chart[k].p[i].y;			
		}		
	}//for chart
	
	/*¬спомогательные линии*/
	/*¬ертикальные линии*/
	if (graphStorage[hnd].showGrid){
			x0 = graphStorage[hnd].OffsetX + xPos;	//преобразуем в координаты графика
	
			while(x0<(xPos+w)) {
			if (x0>xPos)
				LCD_DrawDottedVLine(x0,yPos,yPos+h-1,12, 0x0165);
		
				x0 += graphStorage[hnd].HscrSpacing;		
			}
	
			/*√оризонтальные линии*/
		y0 = -graphStorage[hnd].OffsetY + yPos + h;	//преобразуем в координаты графика
	
			while(y0>yPos) {
				if (y0<(yPos + h))
				LCD_DrawDottedHLine(xPos,y0,xPos+w-1,12, 0x0165);
		
				y0 -= graphStorage[hnd].VscrSpacing;		
			}
}// if SHOW GRID
	/*названи€ осей*/
	LCD_TransparentText(xPos + w - strlen(graphStorage[hnd].xName)*8, yPos + h - 18, graphStorage[hnd].xName, COLOR_WHITE);
	LCD_TransparentText(xPos + 5, yPos + 5, graphStorage[hnd].yName, COLOR_WHITE);
	
	
	
	/*–исуем определ€емую точку*/
	//это просто баный пиздец а не код
	if (graphStorage[hnd].showCheckPoint){
		
	//sprintf(text, "x=%.2f; y=%.2f", graphStorage[hnd].checkPoint.x, graphStorage[hnd].checkPoint.y);		
	//GUI_TransparentText(graphStorage[hnd].checkPointdpX,graphStorage[hnd].checkPointdpY, text,COLOR_GREEN);
		/*LCD_DrawDottedVLine(xPos + graphStorage[hnd].checkPointdpX,yPos,yPos+h,10,COLOR_RED);
		sprintf(text, "x=%.2f", graphStorage[hnd].checkPoint.x);		
		GUI_TransparentText(xPos + graphStorage[hnd].checkPointdpX, yPos + graphStorage[hnd].checkPointdpY, text,COLOR_GREEN);
		sprintf(text, "y=%.2f", graphStorage[hnd].checkPoint.y);		
		GUI_TransparentText(xPos + graphStorage[hnd].checkPointdpX, yPos + graphStorage[hnd].checkPointdpY + 18, text,COLOR_GREEN);
		*/
		
		LCD_DrawDottedVLine(xPos + graphStorage[hnd].checkPointdpX,yPos,yPos+h,10,COLOR_GREY);
		
		if (graphStorage[hnd].chart[0].n){
		sprintf(text, "(%.0f; %.2f)", graphStorage[hnd].chart[0].checkPoint.x, graphStorage[hnd].chart[0].checkPoint.y);		
		LCD_TransparentText(xPos + graphStorage[hnd].checkPointdpX, yPos + graphStorage[hnd].checkPointdpY, text,graphStorage[hnd].chart[0].color);
		}
		
		if (graphStorage[hnd].chart[1].n){
		sprintf(text, "(%.0f; %.2f)", graphStorage[hnd].chart[1].checkPoint.x, graphStorage[hnd].chart[1].checkPoint.y);		
		LCD_TransparentText(xPos + graphStorage[hnd].checkPointdpX, yPos + graphStorage[hnd].checkPointdpY + 18, text,graphStorage[hnd].chart[1].color);
		}
		if (graphStorage[hnd].chart[2].n){
		sprintf(text, "(%.0f; %.2f)", graphStorage[hnd].chart[2].checkPoint.x, graphStorage[hnd].chart[2].checkPoint.y);		
		LCD_TransparentText(xPos + graphStorage[hnd].checkPointdpX, yPos + graphStorage[hnd].checkPointdpY + 36, text,graphStorage[hnd].chart[2].color);
		}
		
	}
	
	LCD_Rect(xPos,yPos, xPos + w - 1, yPos + h - 1, COLOR_GREY);
	
	
	LCD_SetClip(0,0,239,319);	//восстановить область отсечени€
	
	
}
//*********************************************************** POINT GRAPH ************************************************
void drawPoint(FPOINT fpt, uint16_t displayX, uint16_t displayY){
/*	uint8_t text[18];
	sprintf(text, "x=%.2f; y=%.2f", fpt.x, fpt.y);
	GUI_TransparentText(displayX,displayY, text,COLOR_GREEN);*/
}

void drawListView(LISTVIEW_struct *lst){
	uint8_t i=0;
	uint8_t j=0;
	
	
	
	LCD_FillRect(lst->left, lst->top,lst->left+lst->width, lst->top+lst->height, 0x200F);
	LCD_Rect(lst->left, lst->top, lst->left+lst->width, lst->top+lst->height, 0x04BF);
	LCD_SetClip(lst->left, lst->top, lst->left+lst->width, lst->top+lst->height);
	
	
	//draw grid
	for (i=0; i < lst->rowCount; i++)
	LCD_DrawHLine(lst->left, 
								lst->left + lst->width, 
								(lst->top - lst->offsetV + (i+1)*lst->rowHeight), 0x04BF);
	
	for (j=0; j < lst->colCount; j++)
	LCD_DrawVLine(lst->top, lst->top + lst->height, lst->left + (j+1)*lst->colWidth,0x04BF);
	
	
	for (i=0; i < lst->rowCount; i++)
		for (j=0; j < lst->colCount; j++){
			if (strlen(lst->cell[i][j]))
				LCD_Text(lst->left+lst->colWidth*(j)+10,lst->top + lst->rowHeight/2 - 8 - lst->offsetV + (i)*lst->rowHeight, lst->cell[i][j], 0x04BF, 0x200F);			
		}		
	
		if ((lst->selectedCol>-1)&&(lst->selectedRow>-1))
		LCD_Rect(	lst->left + lst->selectedCol * lst->colWidth,
						lst->top + lst->selectedRow * lst->rowHeight - lst->offsetV,
						lst->left + (lst->selectedCol+1) * lst->colWidth,
						lst->top + (lst->selectedRow+1) * lst->rowHeight - lst->offsetV, COLOR_RED);
	
	LCD_SetClip(0,0,MAX_X,MAX_Y);
	
}
/*******************************************************************************************
																		INIT GUI
********************************************************************************************/
void InitGUI(){
	uint8_t i;
	systemTimeMS = 0;
//	lastTime=0;
	for (i=0; i< MAX_BUTTONS_IN_STORAGE; i++){
		buttonStorage[i].destroyed = true;
	}
	
	
	for (i=0; i< MAX_PROGRESS_BARS; i++){
			progbarStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_TEXTBOXES; i++){
			textboxStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_SCROLLBARS; i++){
			scrollbarStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_GRAPH; i++){
			graphStorage[i].destroyed = true;
		graphStorage[i].invalid = false;
	}
	
	for (i=0; i< MAX_LISTVIEW; i++){
		listviewStorage[i].destroyed = true;
		listviewStorage[i].invalid = false;
	}
		
	hFocusedTextBox = UINT8_MAX;
}

void UnfocusTextboxes(void){
uint8_t i;
	for (i=0; i< MAX_TEXTBOXES; i++){
		if (textboxStorage[i].focused && (!textboxStorage[i].destroyed)) 
			textboxStorage[i].focused = false;
			textboxStorage[i].invalid = true;
	}
	hFocusedTextBox = UINT8_MAX;
}
/**/

/*******************************************************************************************
																		GUI TOUCH
********************************************************************************************/

void GUITouch(uint16_t x, uint16_t y){
	uint8_t i;
	

	
	
	//********************       BUTTONS            *************************
	for (i=0; i< MAX_BUTTONS_IN_STORAGE; i++){
		if (buttonStorage[i].destroyed || !buttonStorage[i].enabled) continue;
		
		if ((buttonStorage[i].top < y)&&((buttonStorage[i].top+buttonStorage[i].height) > y)&&
			(buttonStorage[i].left < x)&&((buttonStorage[i].left+buttonStorage[i].width) > x)){
				/*If callback function defined do it*/
				if (buttonStorage[i].cbFunc)
					(*buttonStorage[i].cbFunc)();
				buttonStorage[i].pressed = true;
		//		buttonStorage[i].state = B_PRESSED;
				/*button should be redrawed*/
				buttonStorage[i].invalid = true;	
				buttonStorage[i].restoreCount = BUTTON_RESTORE_TIME;		
				
				
				return;	//вр€дли за один раз было нажато больше одной кнопки
			}//touch on button
	}//for
	
	//**********************  SCROLLBARS  **************************************
	for (i=0; i< MAX_SCROLLBARS; i++){
			if (scrollbarStorage[i].destroyed) continue;
		
	if 	((scrollbarStorage[i].top < y)&&((scrollbarStorage[i].top+scrollbarStorage[i].height) > y)&&
			(scrollbarStorage[i].left < x)&&((scrollbarStorage[i].left+scrollbarStorage[i].width) > x)){
				SCROLLBOX_processTouch(i,x - scrollbarStorage[i].left,y - scrollbarStorage[i].top);
				scrollbarStorage[i].invalid = true;
				scrollbarStorage[i].changed = true;
				
				
				return;
			}//if
				
				
		 
	}//for scrollbars
	//*********************    TOUCH GRAPH  ***************************************
	for (i=0; i< MAX_GRAPH; i++){
			if (graphStorage[i].destroyed) continue;
		
	if 	((graphStorage[i].top < y)&&((graphStorage[i].top+graphStorage[i].height) > y)&&
			(graphStorage[i].left < x)&&((graphStorage[i].left+graphStorage[i].width) > x)){
				
				GRAPH_GetPoint(i, x - graphStorage[i].left, y - graphStorage[i].top);
				pickGraph = i;
		
//				drawPoint(graphPoint,x,y);
				
				return;
			}//if
				
				
		 
	}//for graph
	//*************************************   TOUCH TEXTBOX ****************************
	for (i=0; i< MAX_TEXTBOXES; i++){
		if (textboxStorage[i].destroyed) continue;
		if 	((textboxStorage[i].top < y)&&
			  ((textboxStorage[i].top+textboxStorage[i].height) > y)&&
			  (textboxStorage[i].left < x)&&
		    ((textboxStorage[i].left+textboxStorage[i].width) > x)&&
		     (textboxStorage[i].enabled)){
				UnfocusTextboxes();
				
				textboxStorage[i].focused = true;
				textboxStorage[i].pressed = true;
				textboxStorage[i].invalid = true;
				
				hFocusedTextBox = i;
				return;
			}//if
		
	}//touch textbox
	
	if (!listviewStorage[0].destroyed)
		if ((listviewStorage[0].top < y)&&(listviewStorage[0].top+ listviewStorage[0].height > y)&&(listviewStorage[0].left < x)&&(listviewStorage[0].left + listviewStorage[0].width)){
			LISTVIEW_Select(&listviewStorage[0], x, y);
			listviewStorage[0].invalid = true;
		UnfocusTextboxes();
	}//listview select
}

/*                                      UNTOUCH                                                      */
void GUIUntouch(uint16_t x, uint16_t y){
	pickGraph = UINT8_MAX;
}

/*                                      SLIDE                                                      */
void GUISlide(int dx,int dy){
		if (pickGraph != UINT8_MAX){
			GRAPH_Translate(pickGraph,dx,-dy);
		}
		
		if (abs(dy)>0){
			if (!listviewStorage[0].destroyed)
				LISTVIEW_Slide(&listviewStorage[0],dy);
		}
}

/*																			Redraw widgets on their positions                                    */
/* —осто€ни€ pressed, changed перевод€тс€ в false*/
void GUIRefresh(int dt){
	uint8_t i;
	
	for (i=0; i< MAX_BUTTONS_IN_STORAGE; i++){
		
		if (buttonStorage[i].destroyed) continue;
		
		buttonStorage[i].pressed = false; //in any case
		
		if (buttonStorage[i].restoreCount > dt ){
			buttonStorage[i].restoreCount -= dt;
			buttonStorage[i].animstate = (uint8_t)(((float)buttonStorage[i].restoreCount/(float)BUTTON_RESTORE_TIME)*((float)MAX_COLOR_ANIM));
			//buttonStorage[i].animstate--;
			buttonStorage[i].invalid = true;
		} else if (buttonStorage[i].restoreCount > 0 ){
			buttonStorage[i].restoreCount =0;
			buttonStorage[i].animstate = 0;
			buttonStorage[i].invalid = true;		/* Button state changed, redraw*/
		}
		
		
		if (buttonStorage[i].invalid && !buttonStorage[i].destroyed){
			
			if (buttonStorage[i].style == B_STANDART){
				/*draw pressed button*/
				if (buttonStorage[i].restoreCount ==0)
					buttonStorage[i].invalid = false;
				//	DrawButton(buttonStorage[i].left, buttonStorage[i].top, buttonStorage[i].width, buttonStorage[i].height, buttonStorage[i].str, buttonStorage[i].colorPRESSED);
				//else /*draw idle standart button*/
				//	DrawButton(buttonStorage[i].left, buttonStorage[i].top, buttonStorage[i].width, buttonStorage[i].height, buttonStorage[i].str, buttonStorage[i].colorUP);
				DrawButton(buttonStorage[i].left, buttonStorage[i].top, buttonStorage[i].width, buttonStorage[i].height, buttonStorage[i].str, cyantoblack[buttonStorage[i].animstate]);
			}					
			/*draw image button*/
			else if (buttonStorage[i].style == B_IMAGED){
				buttonStorage[i].invalid = false;
				if (buttonStorage[i].restoreCount >0)
					/* PRESSED*/
					DrawImagedButton(buttonStorage[i].left, buttonStorage[i].top, buttonStorage[i].width, buttonStorage[i].height, buttonStorage[i].str, buttonStorage[i].picDOWN);
				else /*IDLE*/
					DrawImagedButton(buttonStorage[i].left, buttonStorage[i].top, buttonStorage[i].width, buttonStorage[i].height, buttonStorage[i].str, buttonStorage[i].picUP);
			}
			
		}//if invalid
	}//for
	
	for (i=0; i< MAX_PROGRESS_BARS; i++){
		if (progbarStorage[i].invalid && !progbarStorage[i].destroyed){
			progbarStorage[i].invalid = false;
			DrawProgbar(progbarStorage[i].left,
			progbarStorage[i].top,
			progbarStorage[i].width,
			progbarStorage[i].height,
			progbarStorage[i].value);
		}
	}
	
	
	for (i=0; i< MAX_TEXTBOXES; i++){
		textboxStorage[i].pressed = false;
		if (textboxStorage[i].invalid && !textboxStorage[i].destroyed){
			
			DrawTextbox(textboxStorage[i].left,
									textboxStorage[i].top,
									textboxStorage[i].width,
									textboxStorage[i].height,
									textboxStorage[i].text,
									textboxStorage[i].focused ? textboxStorage[i].colorFOCUSED : textboxStorage[i].color);
			textboxStorage[i].invalid = false;
		}
	}//for
	
	
		for (i=0; i< MAX_SCROLLBARS; i++){
			scrollbarStorage[i].changed = false;
			if (scrollbarStorage[i].invalid && !scrollbarStorage[i].destroyed){
				DrawHScrollBar(scrollbarStorage[i].left, scrollbarStorage[i].top, scrollbarStorage[i].width, scrollbarStorage[i].sliderPos);
				scrollbarStorage[i].invalid = false;
			}
	}//for
		
		for (i=0; i< MAX_GRAPH; i++){
			if (graphStorage[i].invalid && !graphStorage[i].destroyed){
				DrawGraph(i);			
				graphStorage[i].invalid = false;
			}
			
	}//for graph
	
	for (i=0; i< MAX_LISTVIEW; i++){
		if (!listviewStorage[i].destroyed && listviewStorage[i].invalid){
			drawListView(&listviewStorage[i]);
			listviewStorage[i].invalid = false;
		}
			
	}
}

void GUIClearScr(uint16_t color){
		uint8_t i;
	LCD_Clear(color);
	for (i=0; i< MAX_BUTTONS_IN_STORAGE; i++){
		if (!buttonStorage[i].destroyed){
			buttonStorage[i].invalid = true;
		}
	}
	
	for (i=0; i< MAX_PROGRESS_BARS; i++)
		progbarStorage[i].invalid = true;
		
	for (i=0; i< MAX_TEXTBOXES; i++)
		textboxStorage[i].invalid = true;
	
		for (i=0; i< MAX_GRAPH; i++)
				graphStorage[i].invalid = true;
	
		for (i=0; i< MAX_SCROLLBARS; i++){
			scrollbarStorage[i].invalid = true;
		}
		for (i=0; i< MAX_LISTVIEW; i++){
			listviewStorage[i].invalid = true;
		}
}
//*destroy all widgets
void GUIClear(){
	/*destroy buttons*/
	uint8_t i;
	for (i=0; i< MAX_BUTTONS_IN_STORAGE; i++){
		buttonStorage[i].destroyed = true;
	}
	for (i=0; i< MAX_PROGRESS_BARS; i++){
		progbarStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_TEXTBOXES; i++)
		textboxStorage[i].destroyed = true;
	
	for (i=0; i< MAX_GRAPH; i++){
		graphStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_LISTVIEW; i++){
			listviewStorage[i].destroyed = true;
	}
	
	for (i=0; i< MAX_SCROLLBARS; i++)
			scrollbarStorage[i].destroyed = true;
	
	LCD_Clear(Black);
}

TEXTBOX_Handle GetFocusedTextBox(void){
	return hFocusedTextBox;
}


