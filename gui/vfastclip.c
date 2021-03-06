/*--------------------------------------------------- V_FCclip
 *  ��������� �������� ��������� FC (Fast Clipping)
 *  �������-���������-����, � ������������ �����
 *
 * int  V_FCclip (float *x0, float *y0, float *x1, float *y1)
 *
 * �������� �������, �������� ���������� ��������� ���
 * ����� (x0,y0), (x1,y1), �� ���� ���������, ���������
 * ����������� ��������� Wxlef, Wybot, Wxrig, Wytop
 *
 * ����������:
 * -1 - ������ � ������� ����
 *  0 - ������� �� �����
 *  1 - ������� �����
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
   int  visible= 0;             /* ������� ������� */

   FC_xn= *x0;  FC_yn= *y0;
   FC_xk= *x1;  FC_yk= *y1;

/*
 * ���������� �������� Code - ���� �������
 * ���� 0-3 - ��� �������� �����, 4-7 - ��� ���������
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

/* ��������� ��� ������� �� 81-�� ������� */

   switch (Code) {

     /* �� ������ */

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

     /* ����� */

     case 0x10: Clip0_Left();   ++visible;
     case 0x11: break;                          /* �������� */
     case 0x12: Clip0_Left();   Clip1_Right();
                ++visible;  break;
     case 0x14: Clip0_Left();
                if (FC_yn < Wybot) break;       /* �������� */
                Clip1_Bottom();
                ++visible;
     case 0x15: break;                          /* �������� */
     case 0x16: Clip0_Left();
                if (FC_yn < Wybot) break;       /* �������� */
                Clip1_Bottom();
                if (FC_xk > Wxrig) Clip1_Right();
                ++visible;
                break;
     case 0x18: Clip0_Left();
                if (FC_yn > Wytop) break;       /* �������� */
                Clip1_Top();
                ++visible;
     case 0x19: break;                          /* �������� */
     case 0x1A: Clip0_Left();
                if (FC_yn > Wytop) break;       /* �������� */
                Clip1_Top();
                if (FC_xk > Wxrig) Clip1_Right();
                ++visible;
                break;

     /* ������ */

     case 0x20: Clip0_Right(); ++visible;  break;
     case 0x21: Clip0_Right(); Clip1_Left(); ++visible;
     case 0x22: break;                          /* �������� */
     case 0x24: Clip0_Right();
                if (FC_yn < Wybot) break;       /* �������� */
                Clip1_Bottom();
                ++visible;
                break;
     case 0x25: Clip0_Right();
                if (FC_yn < Wybot) break;       /* �������� */
                Clip1_Bottom();
                if (FC_xk < Wxlef) Clip1_Left();
                ++visible;
     case 0x26: break;                          /* �������� */
     case 0x28: Clip0_Right();
                if (FC_yn > Wytop) break;       /* �������� */
                Clip1_Top();
                ++visible;
                break;
     case 0x29: Clip0_Right();
                if (FC_yn > Wytop) break;       /* �������� */
                Clip1_Top();
                if (FC_xk < Wxlef) Clip1_Left();
                ++visible;
     case 0x2A: break;                          /* �������� */

     /* ����� */

     case 0x40: Clip0_Bottom(); ++visible;  break;
     case 0x41: Clip0_Bottom();
                if (FC_xn < Wxlef) break;       /* �������� */
                Clip1_Left() ;
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
                break;
     case 0x42: Clip0_Bottom();
                if (FC_xn > Wxrig) break;       /* �������� */
                Clip1_Right();
                ++visible;
     case 0x44:
     case 0x45:
     case 0x46: break;                          /* �������� */
     case 0x48: Clip0_Bottom();
                Clip1_Top();
                ++visible;
                break;
     case 0x49: Clip0_Bottom();
                if (FC_xn < Wxlef) break;       /* �������� */
                Clip1_Left() ;
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;
     case 0x4A: Clip0_Bottom();
                if (FC_xn > Wxrig) break;       /* �������� */
                Clip1_Right();
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;

     /* ����� ����� */

     case 0x50: Clip0_Left();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
     case 0x51: break;                          /* �������� */
     case 0x52: Clip1_Right();
                if (FC_yk < Wybot) break;       /* �������� */
                Clip0_Bottom();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
     case 0x54:
     case 0x55:
     case 0x56: break;                          /* �������� */
     case 0x58: Clip1_Top();
                if (FC_xk < Wxlef) break;       /* �������� */
                Clip0_Bottom();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
     case 0x59: break;                          /* �������� */
     case 0x5A: Clip0_Left();
                if (FC_yn > Wytop) break;       /* �������� */
                Clip1_Right();
                if (FC_yk < Wybot) break;       /* �������� */
                if (FC_yn < Wybot) Clip0_Bottom();
                if (FC_yk > Wytop) Clip1_Top();
                ++visible;
                break;

     /* �����-������ */

     case 0x60: Clip0_Right();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
                break;
     case 0x61: Clip1_Left() ;
                if (FC_yk < Wybot) break;       /* �������� */
                Clip0_Bottom();
                if (FC_xn > Wxrig) Clip0_Right();
                ++visible;
     case 0x62:
     case 0x64:
     case 0x65:
     case 0x66: break;                          /* �������� */
     case 0x68: Clip1_Top();
                if (FC_xk > Wxrig) break;       /* �������� */
                Clip0_Right();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
                break;
     case 0x69: Clip1_Left() ;
                if (FC_yk < Wybot) break;       /* �������� */
                Clip0_Right();
                if (FC_yn > Wytop) break;       /* �������� */
                if (FC_yk > Wytop) Clip1_Top();
                if (FC_yn < Wybot) Clip0_Bottom();
                ++visible;
     case 0x6A: break;                          /* �������� */

     /* ������ */

     case 0x80: Clip0_Top();
                ++visible;
                break;
     case 0x81: Clip0_Top();
                if (FC_xn < Wxlef) break;       /* �������� */
                Clip1_Left() ;
                ++visible;
                break;
     case 0x82: Clip0_Top();
                if (FC_xn > Wxrig) break;       /* �������� */
                Clip1_Right();
                ++visible;
                break;
     case 0x84: Clip0_Top();
                Clip1_Bottom();
                ++visible;
                break;
     case 0x85: Clip0_Top();
                if (FC_xn < Wxlef) break;       /* �������� */
                Clip1_Left() ;
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
                break;
     case 0x86: Clip0_Top();
                if (FC_xn > Wxrig) break;       /* �������� */
                Clip1_Right();
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
     case 0x88:
     case 0x89:
     case 0x8A: break;                          /* �������� */

     /* ������-����� */

     case 0x90: Clip0_Left();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0x91: break;                          /* �������� */
     case 0x92: Clip1_Right();
                if (FC_yk > Wytop) break;       /* �������� */
                Clip0_Top();
                if (FC_xn < Wxlef) Clip0_Left();
                ++visible;
                break;
     case 0x94: Clip1_Bottom();
                if (FC_xk < Wxlef) break;       /* �������� */
                Clip0_Left();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0x95: break;                          /* �������� */
     case 0x96: Clip0_Left();
                if (FC_yn < Wybot) break;       /* �������� */
                Clip1_Right();
                if (FC_yk > Wytop) break;       /* �������� */
                if (FC_yn > Wytop) Clip0_Top();
                if (FC_yk < Wybot) Clip1_Bottom();
                ++visible;
     case 0x98:
     case 0x99:
     case 0x9A: break;                          /* �������� */

     /* ������-������ */

     case 0xA0: Clip0_Right();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
                break;
     case 0xA1: Clip1_Left() ;
                if (FC_yk > Wytop) break;       /* �������� */
                Clip0_Top();
                if (FC_xn > Wxrig) Clip0_Right();
                ++visible;
     case 0xA2: break;                          /* �������� */
     case 0xA4: Clip1_Bottom();
                if (FC_xk > Wxrig) break;       /* �������� */
                Clip0_Right();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
                break;
     case 0xA5: Clip1_Left() ;
                if (FC_yk > Wytop) break;       /* �������� */
                Clip0_Right();
                if (FC_yn < Wybot) break;       /* �������� */
                if (FC_yk < Wybot) Clip1_Bottom();
                if (FC_yn > Wytop) Clip0_Top();
                ++visible;
     case 0xA6:                                 /* �������� */
     case 0xA8:
     case 0xA9:
     case 0xAA: break;

     /* ������ */

     default:   visible= -1;
                break;
   }  /* switch */

   if (visible > 0) {
      *x0= FC_xn;  *y0= FC_yn;
      *x1= FC_xk;  *y1= FC_yk;
   }
   return (visible);
}  /* V_FCclip */