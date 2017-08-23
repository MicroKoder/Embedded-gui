#ifndef GUIDEFS
#define GUIDEFS

#include <stdint.h>

#define COLOR_WHITE          0xFFFF
#define COLOR_BLACK          0x0000
#define COLOR_GREY           0xF7DE
#define COLOR_BLUE           0x001F
#define COLOR_BLUE2          0x051F
#define COLOR_RED            0xF800
#define COLOR_MAGENTA        0xF81F
#define COLOR_GREEN          0x07E0
#define COLOR_CYAN           0x07BE
#define COLOR_YELLOW				 0xFFE0
#define COLOR_SKY						 0x4CDC

#define MAX_IMAGES 100
/* restore state*/

typedef uint16_t GUI_COLOR;

typedef const unsigned short *IMAGE;

static const unsigned short cyantoblack[] = {  
	 0x0000, 0x0165, 0x0208, 0x02AA, 0x034D, 0x03EF, 0x0492, 0x0534, 0x05D7, 0x0679, 0x071C, 0x07BE
};


#endif
