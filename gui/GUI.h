#ifndef _IDSGUI_
#define _IDSGUI_

#include "GLCD.h"
#include "GUIdef.h"
#include "button.h"
#include "progbar.h"
#include "textbox.h"
#include "scrollbox.h"
#include "graph.h"
#include "listview.h"

#include "clipline.h"
//#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdbool.h>

//typedef uint8_t IMAGE_handle;

/* for GUI delay*/



void InitGUI(void);
/*
Fill structure in storage, returns handle
*/




/*

*/

//передает координаты тача в GUI
void GUITouch(uint16_t x, uint16_t y);
void GUIUntouch(uint16_t x, uint16_t y);
void GUISlide(int dx,int dy);
TEXTBOX_Handle GetFocusedTextBox(void);

void GUIRefresh(int dt);
void GUIClear(void);
void GUIClearScr(uint16_t color);
#endif
