#ifndef _CLIPLINE_
#define _CLIPLINE_
/*--------------------------------------------------- V_FCclip
 *  FC (Fast Clipping)
 *  Алгоритм отсечения Собкова-Поспишила-Янга
 *
 * int  V_FCclip (float *x0, float *y0, float *x1, float *y1)
 *
 * 
 * 
 * Wxlef, Wybot, Wxrig, Wytop
 *
 * 
 * -1 - Ошибка задания окна /error
 *  0 - отрезок не видим /visible
 *  1 - отрезок видим /unvisible
 */
void SetCliparea(float wleft, float wbot, float wright, float wtop);
int  V_FCclip (float* x0, float* y0, float* x1, float* y1);

#endif

