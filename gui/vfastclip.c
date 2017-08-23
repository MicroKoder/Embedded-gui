/*--------------------------------------------------- V_FCclip
 *  Реализует алгоритм отсечения FC (Fast Clipping)
 *  Собкова-Поспишила-Янга, с кодированием линий
 *
 * int  V_FCclip (float *x0, float *y0, float *x1, float *y1)
 *
 * Отсекает отрезок, заданный значениями координат его
 * точек (x0,y0), (x1,y1), по окну отсечения, заданному
 * глобальными скалярами Wxlef, Wybot, Wxrig, Wytop
 *
 * Возвращает:
 * -1 - ошибка в задании окна
 *  0 - отрезок не видим
 *  1 - отрезок видим
 */

static float FC_xn, FC_yn, FC_xk, FC_yk;

static void Clip0_Top(void)
{FC_xn= FC_xn + (FC_xk-FC_xn)*(Wytop-FC_yn)/(FC_yk-FC_yn);
 FC_yn= Wytop; }

static void Clip0_Bottom(void)
{FC_xn= FC_xn + (FC_xk-FC_xn)*(Wybot-FC_yn)/(FC_yk-FC_yn);
 FC_yn= Wybot; }

static void Clip0_Right(void)
{FC_yn= FC_yn + (FC_yk-FC_yn)*(Wxrig-FC_xn)/(FC_xk-FC_xn);
 FC_xn= Wxrig; }

static void Clip0_Left(void)
{FC_yn= FC_yn + (FC_yk-FC_yn)*(Wxlef-FC_xn)/(FC_xk-FC_xn);
 FC_xn= Wxlef; }

static void Clip1_Top(void)
{FC_xk= FC_xk + (FC_xn-FC_xk)*(Wytop-FC_yk)/(FC_yn-FC_yk);
 FC_yk= Wytop; }

static void Clip1_Bottom(void)
{FC_xk= FC_xk + (FC_xn-FC_xk)*(Wybot-FC_yk)/(FC_yn-FC_yk);
 FC_yk= Wybot; }

static void Clip1_Right(void)
{FC_yk= FC_yk + (FC_yn-FC_yk)*(Wxrig-FC_xk)/(FC_xn-FC_xk);
 FC_xk= Wxrig; }

static void Clip1_Left(void)
{FC_yk= FC_yk + (FC_yn-FC_yk)*(Wxlef-FC_xk)/(FC_xn-FC_xk);
 FC_xk= Wxlef; }

int  V_FCclip (x0, y0, x1, y1)
float *x0, *y0, *x1, *y1;
{  int  Code= 0;
   int  visible= 0;             /* Отрезок невидим */

   FC_xn= *x0;  FC_yn= *y0;
   FC_xk= *x1;  FC_yk= *y1;

/*
 * Вычисление значения Code - кода отрезка
 * Биты 0-3 - для конечной точки, 4-7 - для начальной
 *
 */
   if (FC_yk > Wytop) Code+= 8; else
   if (FC_yk < Wybot) Code+= 4;

   if (FC_xk > Wxrig) Code+= 2; else
   if (FC_xk < Wxlef) Code+= 1;

   if (FC_yn > Wytop) Code+= 128; else
   if (FC_yn < Wybot) Code+= 64;

   if (FC_xn > Wxrig) Code+= 32; else
   if (FC_xn < Wxlef) Code+= 16;

/* Отсечение для каждого из 81-го случаев */

   switch (Code) {

     /* Из центра */

     case 0x00: ++visible;  break;
     case 0x01: Clip1_Left() ;   ++visible;  break;
     case 0x02: Clip1_Right();  ++visible;  break;
     case 0x04: Clip1_Bottom(); ++visible;  break;
     case 0x05: Clip1_Left() ;
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;  break;
     case 0x06: Clip1_Right();
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;  break;
     case 0x08: Clip1_Top();    ++visible;  break;
     case 0x09: Clip1_Left() ;
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;  break;
     case 0x0A: Clip1_Right();
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;  break;

     /* Слева */

     case 0x10: Clip0_Left();   ++visible;
     case 0x11: break;                          /* Отброшен */
     case 0x12: Clip0_Left();   Clip1_Right();
                ++visible;  break;
     case 0x14: Clip0_Left();
                if (FC_yn < Wybot) break;       /* Отброшен */
                Clip1_Bottom();
                ++visible;
     case 0x15: break;                          /* Отброшен */
     case 0x16: Clip0_Left();
                if (FC_yn < Wybot) break;       /* Отброшен */
                Clip1_Bottom();
                if (FC_xk > Wxrig) Clip1_Right();
                ++visible;
                break;
     case 0x18: Clip0_Left();
                if (FC_yn > Wytop) break;       /* Отброшен */
                Clip1_Top();
                ++visible;
     case 0x19: break;                          /* Отброшен */
     case 0x1A: Clip0_Left();
                if (FC_yn > Wytop) break;       /* Отброшен */
                Clip1_Top();
                if (FC_xk > Wxrig) Clip1_Right();
                ++visible;
                break;

     /* Справа */

     case 0x20: Clip0_Right(); ++visible;  break;
     case 0x21: Clip0_Right(); Clip1_Left(); ++visible;
     case 0x22: break;                          /* Отброшен */
     case 0x24: Clip0_Right();
                if (FC_yn < Wybot) break;       /* Отброшен */
                Clip1_Bottom();
                ++visible;
                break;
     case 0x25: Clip0_Right();
                if (FC_yn < Wybot) break;       /* Отброшен */
                Clip1_Bottom();
                if (FC_xk < Wxlef) Clip1_Left();
                ++visible;
     case 0x26: break;                          /* Отброшен */
     case 0x28: Clip0_Right();
                if (FC_yn > Wytop) break;       /* Отброшен */
                Clip1_Top();
                ++visible;
                break;
     case 0x29: Clip0_Right();
                if (FC_yn > Wytop) break;       /* Отброшен */
                Clip1_Top();
                if (FC_xk < Wxlef) Clip1_Left();
                ++visible;
     case 0x2A: break;                          /* Отброшен */

     /* Снизу */

     case 0x40: Clip0_Bottom(); ++visible;  break;
     case 0x41: Clip0_Bottom();
                if (FC_xn < Wxlef) break;       /* Отброшен */
                Clip1_Left() ;
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
                break;
     case 0x42: Clip0_Bottom();
                if (FC_xn > Wxrig) break;       /* Отброшен */
                Clip1_Right();
                ++visible;
     case 0x44:
     case 0x45:
     case 0x46: break;                          /* Отброшен */
     case 0x48: Clip0_Bottom();
                Clip1_Top();
                ++visible;
                break;
     case 0x49: Clip0_Bottom();
                if (FC_xn < Wxlef) break;       /* Отброшен */
                Clip1_Left() ;
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;
     case 0x4A: Clip0_Bottom();
                if (FC_xn > Wxrig) break;       /* Отброшен */
                Clip1_Right();
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;

     /* Снизу слева */

     case 0x50: Clip0_Left();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
     case 0x51: break;                          /* Отброшен */
     case 0x52: Clip1_Right();
                if (FC_yk < Wybot) break;       /* Отброшен */
                Clip0_Bottom();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
     case 0x54:
     case 0x55:
     case 0x56: break;                          /* Отброшен */
     case 0x58: Clip1_Top();
                if (FC_xk < Wxlef) break;       /* Отброшен */
                Clip0_Bottom();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
     case 0x59: break;                          /* Отброшен */
     case 0x5A: Clip0_Left();
                if (FC_yn > Wytop) break;       /* Отброшен */
                Clip1_Right();
                if (FC_yk < Wybot) break;       /* Отброшен */
                if (FC_yn < Wybot) Clip0_Bottom();
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;

     /* Снизу-справа */

     case 0x60: Clip0_Right();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
                break;
     case 0x61: Clip1_Left() ;
                if (FC_yk < Wybot) break;       /* Отброшен */
                Clip0_Bottom();
                if (FC_xn > Wxrig) Clip0_Right();
                ++visible;
     case 0x62:
     case 0x64:
     case 0x65:
     case 0x66: break;                          /* Отброшен */
     case 0x68: Clip1_Top();
                if (FC_xk > Wxrig) break;       /* Отброшен */
                Clip0_Right();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
                break;
     case 0x69: Clip1_Left() ;
                if (FC_yk < Wybot) break;       /* Отброшен */
                Clip0_Right();
                if (FC_yn > Wytop) break;       /* Отброшен */
                if (FC_yk > Wytop) Clip1_Top();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
     case 0x6A: break;                          /* Отброшен */

     /* Сверху */

     case 0x80: Clip0_Top();
                ++visible;
                break;
     case 0x81: Clip0_Top();
                if (FC_xn < Wxlef) break;       /* Отброшен */
                Clip1_Left() ;
                ++visible;
                break;
     case 0x82: Clip0_Top();
                if (FC_xn > Wxrig) break;       /* Отброшен */
                Clip1_Right();
                ++visible;
                break;
     case 0x84: Clip0_Top();
                Clip1_Bottom();
                ++visible;
                break;
     case 0x85: Clip0_Top();
                if (FC_xn < Wxlef) break;       /* Отброшен */
                Clip1_Left() ;
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
                break;
     case 0x86: Clip0_Top();
                if (FC_xn > Wxrig) break;       /* Отброшен */
                Clip1_Right();
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
     case 0x88:
     case 0x89:
     case 0x8A: break;                          /* Отброшен */

     /* Сверху-слева */

     case 0x90: Clip0_Left();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0x91: break;                          /* Отброшен */
     case 0x92: Clip1_Right();
                if (FC_yk > Wytop) break;       /* Отброшен */
                Clip0_Top();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
                break;
     case 0x94: Clip1_Bottom();
                if (FC_xk < Wxlef) break;       /* Отброшен */
                Clip0_Left();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0x95: break;                          /* Отброшен */
     case 0x96: Clip0_Left();
                if (FC_yn < Wybot) break;       /* Отброшен */
                Clip1_Right();
                if (FC_yk > Wytop) break;       /* Отброшен */
                if (FC_yn > Wytop) Clip0_Top();
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
     case 0x98:
     case 0x99:
     case 0x9A: break;                          /* Отброшен */

     /* Сверху-справа */

     case 0xA0: Clip0_Right();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
                break;
     case 0xA1: Clip1_Left() ;
                if (FC_yk > Wytop) break;       /* Отброшен */
                Clip0_Top();
                if (FC_xn > Wxrig) Clip0_Right();
                ++visible;
     case 0xA2: break;                          /* Отброшен */
     case 0xA4: Clip1_Bottom();
                if (FC_xk > Wxrig) break;       /* Отброшен */
                Clip0_Right();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
                break;
     case 0xA5: Clip1_Left() ;
                if (FC_yk > Wytop) break;       /* Отброшен */
                Clip0_Right();
                if (FC_yn < Wybot) break;       /* Отброшен */
                if (FC_yk < Wybot) Clip1_Bottom();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0xA6:                                 /* Отброшен */
     case 0xA8:
     case 0xA9:
     case 0xAA: break;

     /* Ошибка */

     default:   visible= -1;
                break;
   }  /* switch */

   if (visible > 0) {
      *x0= FC_xn;  *y0= FC_yn;
      *x1= FC_xk;  *y1= FC_yk;
   }
   return (visible);
}  /* V_FCclip */