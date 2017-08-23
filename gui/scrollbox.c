#include "scrollbox.h"

extern SCROLLBOX_struct scrollbarStorage[MAX_SCROLLBARS];

SCROLLBOX_Handle CreateHScrollBox(uint16_t xPos, uint16_t yPos, uint16_t l){
	uint8_t i=0;
	while ((i< MAX_SCROLLBARS) && (!scrollbarStorage[i].destroyed)){
		++i;
	}
	if (i == MAX_SCROLLBARS) return UINT8_MAX;
	
	scrollbarStorage[i].left = xPos;
	scrollbarStorage[i].top = yPos;
	scrollbarStorage[i].width = l;
	scrollbarStorage[i].height = 20;
	
	//scrollbarStorage[i].value = 0;
	scrollbarStorage[i].sliderPos = 0;
	
	scrollbarStorage[i].orient = HOR;
	scrollbarStorage[i].invalid = true;
	scrollbarStorage[i].changed = true;
	scrollbarStorage[i].destroyed = false;
	
	scrollbarStorage[i].sliderMaxPos = l - 3*scrollbarStorage[i].height;
	return i;
}

bool SCROLLBOX_changed(SCROLLBOX_Handle hnd){
	return scrollbarStorage[hnd].changed;
}

float GetScrollValue(SCROLLBOX_Handle hnd){
	//return scrollbarStorage[hnd].value;
	return ((float)scrollbarStorage[hnd].sliderPos/(float)scrollbarStorage[hnd].sliderMaxPos*100.f);
}

void SetScrollValue(SCROLLBOX_Handle hnd, float newVal){
	if (newVal<0){
		scrollbarStorage[hnd].sliderPos=0;
		return;
	}
	if (newVal>100){
		scrollbarStorage[hnd].sliderPos=scrollbarStorage[hnd].sliderMaxPos;
		return;
	}
	
	scrollbarStorage[hnd].sliderPos = newVal/100.f * scrollbarStorage[hnd].sliderMaxPos;
}


void SCROLLBOX_processTouch(SCROLLBOX_Handle hnd,uint16_t rx, uint16_t ry){
//	uint16_t sliderPos;
	if (scrollbarStorage[hnd].orient == HOR){
		
		if (rx < scrollbarStorage[hnd].height){
			scrollbarStorage[hnd].sliderPos -= 10;
			
		}else		
		if (rx > scrollbarStorage[hnd].width - scrollbarStorage[hnd].height){
			scrollbarStorage[hnd].sliderPos += 10;
			
		}else		
			scrollbarStorage[hnd].sliderPos = rx - 30;
		
		
		if (scrollbarStorage[hnd].sliderPos < 0)
			scrollbarStorage[hnd].sliderPos = 0;
		
		if (scrollbarStorage[hnd].sliderPos > scrollbarStorage[hnd].sliderMaxPos){
			scrollbarStorage[hnd].sliderPos = scrollbarStorage[hnd].sliderMaxPos;
		}
	}
}
