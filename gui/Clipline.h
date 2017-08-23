#ifndef _CLIPLINE_
#define _CLIPLINE_
/*--------------------------------------------------- V_FCclip
 *  FC (Fast Clipping)
 *  �������� ��������� �������-���������-����
 *
 * int  V_FCclip (float *x0, float *y0, float *x1, float *y1)
 *
 * 
 * 
 * Wxlef, Wybot, Wxrig, Wytop
 *
 * 
 * -1 - ������ ������� ���� /error
 *  0 - ������� �� ����� /visible
 *  1 - ������� ����� /unvisible
 */
void SetCliparea(float wleft, float wbot, float wright, float wtop);
int  V_FCclip (float* x0, float* y0, float* x1, float* y1);

#endif

