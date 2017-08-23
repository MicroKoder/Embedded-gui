//*******************************************************
//************		FOR STM32F407		***********************
//*******************************************************
// Last modify by ZayrullinAZ 19.11.2015
/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 

#include "AsciiLib.h"
#include <stdio.h>
/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8346A	10 /* 0x0046 */  
#define  HX8347D    11 /* 0x0047 */
#define  HX8347A    12 /* 0x0047 */	
#define  LGDP4535   13 /* 0x4535 */  
#define  SSD2119    14 /* 3.5 LCD 0x9919 */

/* Îïðåäåëèòü àäðåñ èñïîëüçóÿ ðåæèì øèíû */

#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */

uint16_t ClipTop = 0;
uint16_t ClipBottom = MAX_Y;
uint16_t ClipLeft = 0;
uint16_t ClipRight = MAX_X;

/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_CtrlLinesConfig(void)
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;

	// -- GPIO Configuration ------------------------------------------------------
	// LCD DB15-DB0       PD10..PD8, PE15..PE7, PD1, PD0, PD15, PD14
	// LCD nCS  (7)       NCE2 (PD7)
	// LCD nRD  (10)      NOE  (PD4)
	// LCD nWR  (9)       NWE  (PD5)
	// LCD RS   (8)       CLE  (PD11)
	// SRAM Data lines,  NOE and NWE configuration
	                            //15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	GPIOD->MODER |= 0xA0AA8A0A; //10 10 00 00 10 10 10 10 10 00 10 10 00 00 10 10
	GPIOD->OTYPER |= 0;
	GPIOD->PUPDR |= 0;
	                             //  7     6    5    4    3    2    1    0
	GPIOD->AFR[0] |= 0xC0CC00CC; // 1100 0000 1100 1100 0000 0000 1100 1100
	GPIOD->AFR[1] |= 0xCC00CCCC; // 1100 1100 0000 0000 1100 1100 1100 1100
	GPIOD->OSPEEDR |= 0xA0AA8A0A; // 50MHz

                                //15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    GPIOE->MODER |= 0xAAAA8000; //10 10 10 10 10 10 10 10 10 00 00 00 00 00 00 00
    GPIOE->OTYPER |= 0;
    GPIOE->PUPDR |= 0;
                                 //  7     6    5    4    3    2    1    0
    GPIOE->AFR[0] |= 0xC0000000; // 1100 0000 0000 0000 0000 0000 0000 0000
    GPIOE->AFR[1] |= 0xCCCCCCCC; // 1100 1100 1100 1100 1100 1100 1100 1100
    GPIOE->OSPEEDR |= 0xAAAA8000; // 50MHz

    SysTick_Config ( 168000); 
}

/*******************************************************************************
* Function Name  : LCD_FSMCConfig
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_FSMCConfig(void)
{

	RCC->AHB3ENR = RCC_AHB3ENR_FSMCEN;

	// -- FSMC Configuration SRAM Bank 3----------------------------------------
	// FSMC_Bank1_NORSRAM4 configuration

    FSMC_Bank1->BTCR [ 0 ] =
                (uint32_t)(0x00001010);

    // Bank1 NOR/SRAM timing register configuration
    FSMC_Bank1->BTCR [ 0 + 1 ] =(uint32_t)( 10 | (1 << 4) | (15 << 8) );
	//FSMC_AddressSetupTime = 10;
	//FSMC_AddressHoldTime = 1;
	//FSMC_DataSetupTime = 15;
   FSMC_Bank1E->BWTR [ 0 ] = 0x0FFFFFFF;

    // Enable FSMC Bank1_SRAM Bank
   FSMC_Bank1->BTCR [ 0 ] |= 0x00000001;
}

/*******************************************************************************
* Function Name  : LCD_Configuration
* Description    : Configure the LCD Control pins and FSMC Parallel interface
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
 /* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint8_t LCD_Reg,uint16_t LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
	
  LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD_RAM);
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteRAM_Prepare(void)
{
  LCD_REG = R34;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteRAM(uint16_t RGB_Code)					 
{
  /* Write 16-bit GRAM Reg */
  LCD_RAM = RGB_Code;
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadRAM(void)
{
  volatile uint16_t dummy; 
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = R34; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  dummy = LCD_RAM; 
  
  return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_SetCursor( uint16_t Xpos, uint16_t Ypos )
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif

	
	 
  switch( LCD_Code )
  {
     default:		 // 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 

     case SSD1298: 	 // 0x8999 
     case SSD1289:   // 0x8989 
	 
	        LCD_WriteReg(0x004e, Xpos );      
          LCD_WriteReg(0x004f, Ypos );          
	      break;  

     case HX8346A: 	 // 0x0046 
     case HX8347A: 	 // 0x0047 
     case HX8347D: 	 // 0x0047 
	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
	      LCD_WriteReg(0x03, Xpos );  

	      LCD_WriteReg(0x06, Ypos>>8 );                           
	      LCD_WriteReg(0x07, Ypos );    
	
	      break;     
     case SSD2119:	 // 3.5 LCD 0x9919 
	      break; 
  }//switch 
	
}


/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 

/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Init(void)
{
	uint16_t DeviceCode;
	
	LCD_Configuration();
	delay_ms(100);
	DeviceCode = LCD_ReadReg(0x0000);		/* ¶ÁÈ¡ÆÁID	*/	
	/* ²»Í¬ÆÁÇý¶¯IC ³õÊ¼»¯²»Í¬ */
	if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
		LCD_WriteReg(0x000c,0x0001);	/* display setting */        
		LCD_WriteReg(0x000d,0x0000); 			        
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		/* display on sequence */    
		LCD_WriteReg(0x0007,0x0133);
		
		LCD_WriteReg(0x0020,0x0000);  /* ÐÐÊ×Ö·0 */                                                          
		LCD_WriteReg(0x0021,0x0000);  /* ÁÐÊ×Ö·0 */     
		
		delay_ms(1000);
	}
	else if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )
	{
	    LCD_Code = ILI9320;
	    LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	/* Driver Output Contral */
		LCD_WriteReg(0x02,0x0700);	/* LCD Driver Waveform Contral */
		LCD_WriteReg(0x03,0x1018);	/* Entry Mode Set */
		
		LCD_WriteReg(0x04,0x0000);	/* Scalling Contral */
	    LCD_WriteReg(0x08,0x0202);	/* Display Contral */
		LCD_WriteReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
		LCD_WriteReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
	    LCD_WriteReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
		LCD_WriteReg(0x0d,0x0000);	/* Frame Maker Position */
		LCD_WriteReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */
		
	    delay_ms(100);  /* delay 100 ms */		
		LCD_WriteReg(0x07,0x0101);	/* Display Contral */
	    delay_ms(100);  /* delay 100 ms */		
	
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
		LCD_WriteReg(0x11,0x0007);								/* Power Control 2 */
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				/* Power Control 3.(0x0138)	*/
		LCD_WriteReg(0x13,0x0b00);								/* Power Control 4 */
		LCD_WriteReg(0x29,0x0000);								/* Power Control 7 */
		
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));
			
		LCD_WriteReg(0x50,0);       /* Set X Start */
		LCD_WriteReg(0x51,239);	    /* Set X End */
		LCD_WriteReg(0x52,0);	    /* Set Y Start */
		LCD_WriteReg(0x53,319);	    /* Set Y End */
		
		LCD_WriteReg(0x60,0x2700);	/* Driver Output Control */
		LCD_WriteReg(0x61,0x0001);	/* Driver Output Control */
		LCD_WriteReg(0x6a,0x0000);	/* Vertical Srcoll Control */
		
		LCD_WriteReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
		LCD_WriteReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
		LCD_WriteReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
		LCD_WriteReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
		LCD_WriteReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
		LCD_WriteReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */
		
	    LCD_WriteReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
		LCD_WriteReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
		LCD_WriteReg(0x93,0x0001);	/* Panel Interface Contral 3. */
	    LCD_WriteReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
		LCD_WriteReg(0x97,(0<<8));	
		LCD_WriteReg(0x98,0x0000);	/* Frame Cycle Contral */
	
	    LCD_WriteReg(0x07,0x0173);
	}
	else if( DeviceCode == 0x9331 )
	{
	    LCD_Code = ILI9331;
		LCD_WriteReg(0x00E7, 0x1014);
		LCD_WriteReg(0x0001, 0x0100);   /* set SS and SM bit */
		LCD_WriteReg(0x0002, 0x0200);   /* set 1 line inversion */
		LCD_WriteReg(0x0003, 0x1030);   /* set GRAM write direction and BGR=1 */
		LCD_WriteReg(0x0008, 0x0202);   /* set the back porch and front porch */
	    LCD_WriteReg(0x0009, 0x0000);   /* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(0x000A, 0x0000);   /* FMARK function */
		LCD_WriteReg(0x000C, 0x0000);   /* RGB interface setting */
		LCD_WriteReg(0x000D, 0x0000);   /* Frame marker Position */
		LCD_WriteReg(0x000F, 0x0000);   /* RGB interface polarity */
		/* Power On sequence */
		LCD_WriteReg(0x0010, 0x0000);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
		LCD_WriteReg(0x0011, 0x0007);   /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x0012, 0x0000);   /* VREG1OUT voltage	*/
		LCD_WriteReg(0x0013, 0x0000);   /* VDV[4:0] for VCOM amplitude */
	    delay_ms(200);  /* delay 200 ms */		
		LCD_WriteReg(0x0010, 0x1690);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
		LCD_WriteReg(0x0011, 0x0227);   /* DC1[2:0], DC0[2:0], VC[2:0] */
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012, 0x000C);   /* Internal reference voltage= Vci	*/
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013, 0x0800);   /* Set VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x0029, 0x0011);   /* Set VCM[5:0] for VCOMH */
		LCD_WriteReg(0x002B, 0x000B);   /* Set Frame Rate */
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020, 0x0000);   /* GRAM horizontal Address */
		LCD_WriteReg(0x0021, 0x0000);   /* GRAM Vertical Address */
		/* Adjust the Gamma Curve */
		LCD_WriteReg(0x0030, 0x0000);
		LCD_WriteReg(0x0031, 0x0106);
		LCD_WriteReg(0x0032, 0x0000);
		LCD_WriteReg(0x0035, 0x0204);
		LCD_WriteReg(0x0036, 0x160A);
		LCD_WriteReg(0x0037, 0x0707);
		LCD_WriteReg(0x0038, 0x0106);
		LCD_WriteReg(0x0039, 0x0707);
		LCD_WriteReg(0x003C, 0x0402);
		LCD_WriteReg(0x003D, 0x0C0F);
		/* Set GRAM area */
		LCD_WriteReg(0x0050, 0x0000);   /* Horizontal GRAM Start Address */
		LCD_WriteReg(0x0051, 0x00EF);   /* Horizontal GRAM End Address */
		LCD_WriteReg(0x0052, 0x0000);   /* Vertical GRAM Start Address */
		LCD_WriteReg(0x0053, 0x013F);   /* Vertical GRAM Start Address */
		LCD_WriteReg(0x0060, 0x2700);   /* Gate Scan Line */
		LCD_WriteReg(0x0061, 0x0001);   /*  NDL,VLE, REV */
		LCD_WriteReg(0x006A, 0x0000);   /* set scrolling line */
		/* Partial Display Control */
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		/* Panel Control */
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);
		LCD_WriteReg(0x0007,0x0021);		
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0007,0x0061);
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0007,0x0133);    /* 262K color and display ON */
	}
	else if( DeviceCode == 0x9919 )
	{
	    LCD_Code = SSD2119;
	    /* POWER ON &RESET DISPLAY OFF */
		LCD_WriteReg(0x28,0x0006);
		LCD_WriteReg(0x00,0x0001);		
		LCD_WriteReg(0x10,0x0000);		
		LCD_WriteReg(0x01,0x72ef);
		LCD_WriteReg(0x02,0x0600);
		LCD_WriteReg(0x03,0x6a38);	
		LCD_WriteReg(0x11,0x6874);
		LCD_WriteReg(0x0f,0x0000);    /* RAM WRITE DATA MASK */
		LCD_WriteReg(0x0b,0x5308);    /* RAM WRITE DATA MASK */
		LCD_WriteReg(0x0c,0x0003);
		LCD_WriteReg(0x0d,0x000a);
		LCD_WriteReg(0x0e,0x2e00);  
		LCD_WriteReg(0x1e,0x00be);
		LCD_WriteReg(0x25,0x8000);
		LCD_WriteReg(0x26,0x7800);
		LCD_WriteReg(0x27,0x0078);
		LCD_WriteReg(0x4e,0x0000);
		LCD_WriteReg(0x4f,0x0000);
		LCD_WriteReg(0x12,0x08d9);
		/* Adjust the Gamma Curve */
		LCD_WriteReg(0x30,0x0000);
		LCD_WriteReg(0x31,0x0104);	 
		LCD_WriteReg(0x32,0x0100);	
	    LCD_WriteReg(0x33,0x0305);	
	    LCD_WriteReg(0x34,0x0505);	 
		LCD_WriteReg(0x35,0x0305);	
	    LCD_WriteReg(0x36,0x0707);	
	    LCD_WriteReg(0x37,0x0300);	
		LCD_WriteReg(0x3a,0x1200);	
		LCD_WriteReg(0x3b,0x0800);		 
	    LCD_WriteReg(0x07,0x0033);
	}
	else if( DeviceCode == 0x1505 || DeviceCode == 0x0505 )
	{
		LCD_Code = R61505U;
		/* initializing funciton */   
		LCD_WriteReg(0xe5,0x8000);  /* Set the internal vcore voltage */   
		LCD_WriteReg(0x00,0x0001);  /* start OSC */   
		LCD_WriteReg(0x2b,0x0010);  /* Set the frame rate as 80 when the internal resistor is used for oscillator circuit */   
		LCD_WriteReg(0x01,0x0100);  /* s720  to  s1 ; G1 to G320 */   
		LCD_WriteReg(0x02,0x0700);  /* set the line inversion */   
		LCD_WriteReg(0x03,0x1018);  /* 65536 colors */    
		LCD_WriteReg(0x04,0x0000);   
		LCD_WriteReg(0x08,0x0202);  /* specify the line number of front and back porch periods respectively */   
		LCD_WriteReg(0x09,0x0000);   
		LCD_WriteReg(0x0a,0x0000);   
		LCD_WriteReg(0x0c,0x0000);  /* select  internal system clock */  
		LCD_WriteReg(0x0d,0x0000);   
		LCD_WriteReg(0x0f,0x0000);    
		LCD_WriteReg(0x50,0x0000);  /* set windows adress */   
		LCD_WriteReg(0x51,0x00ef);   
		LCD_WriteReg(0x52,0x0000);   
		LCD_WriteReg(0x53,0x013f);   
		LCD_WriteReg(0x60,0x2700);   
		LCD_WriteReg(0x61,0x0001);   
		LCD_WriteReg(0x6a,0x0000);   
		LCD_WriteReg(0x80,0x0000);   
		LCD_WriteReg(0x81,0x0000);   
		LCD_WriteReg(0x82,0x0000);   
		LCD_WriteReg(0x83,0x0000);   
		LCD_WriteReg(0x84,0x0000);   
		LCD_WriteReg(0x85,0x0000);   
		LCD_WriteReg(0x90,0x0010);   
		LCD_WriteReg(0x92,0x0000);   
		LCD_WriteReg(0x93,0x0003);   
		LCD_WriteReg(0x95,0x0110);   
		LCD_WriteReg(0x97,0x0000);   
		LCD_WriteReg(0x98,0x0000);    
		/* power setting function */   
		LCD_WriteReg(0x10,0x0000);   
		LCD_WriteReg(0x11,0x0000);   
		LCD_WriteReg(0x12,0x0000);   
		LCD_WriteReg(0x13,0x0000);   
		delay_ms(100);   
		LCD_WriteReg(0x10,0x17b0);   
		LCD_WriteReg(0x11,0x0004);   
		delay_ms(50);   
		LCD_WriteReg(0x12,0x013e);   
		delay_ms(50);   
		LCD_WriteReg(0x13,0x1f00);   
		LCD_WriteReg(0x29,0x000f);   
		delay_ms(50);   
		LCD_WriteReg(0x20,0x0000);   
		LCD_WriteReg(0x21,0x0000);   
		
		/* initializing function */  	
		LCD_WriteReg(0x30,0x0204);   
		LCD_WriteReg(0x31,0x0001);   
		LCD_WriteReg(0x32,0x0000);   
		LCD_WriteReg(0x35,0x0206);   
		LCD_WriteReg(0x36,0x0600);   
		LCD_WriteReg(0x37,0x0500);   
		LCD_WriteReg(0x38,0x0505);   
		LCD_WriteReg(0x39,0x0407);   
		LCD_WriteReg(0x3c,0x0500);   
		LCD_WriteReg(0x3d,0x0503);   
		
		/* display on */  
		LCD_WriteReg(0x07,0x0173);	
	}	
	else if( DeviceCode == 0x8989 )
	{
	    LCD_Code = SSD1289;
	    LCD_WriteReg(0x0000,0x0001);    delay_ms(50);   /* ´ò¿ª¾§Õñ */
	    LCD_WriteReg(0x0003,0xA8A4);    delay_ms(50);   
	    LCD_WriteReg(0x000C,0x0000);    delay_ms(50);   
	    LCD_WriteReg(0x000D,0x080C);    delay_ms(50);   
	    LCD_WriteReg(0x000E,0x2B00);    delay_ms(50);   
	    LCD_WriteReg(0x001E,0x00B0);    delay_ms(50);   
	    LCD_WriteReg(0x0001,0x2B3F);    delay_ms(50);   /* Çý¶¯Êä³ö¿ØÖÆ320*240 0x2B3F */
	    LCD_WriteReg(0x0002,0x0600);    delay_ms(50);
	    LCD_WriteReg(0x0010,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0011,0x6070);    delay_ms(50);   /* ¶¨ÒåÊý¾Ý¸ñÊ½ 16Î»É« ºáÆÁ 0x6070 */
	    LCD_WriteReg(0x0005,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0006,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0016,0xEF1C);    delay_ms(50);
	    LCD_WriteReg(0x0017,0x0003);    delay_ms(50);
	    LCD_WriteReg(0x0007,0x0133);    delay_ms(50);         
	    LCD_WriteReg(0x000B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x000F,0x0000);    delay_ms(50);   /* É¨Ãè¿ªÊ¼µØÖ· */
	    LCD_WriteReg(0x0041,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0042,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0048,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0049,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x004A,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x004B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0044,0xEF00);    delay_ms(50);
	    LCD_WriteReg(0x0045,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0046,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x0030,0x0707);    delay_ms(50);
	    LCD_WriteReg(0x0031,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0032,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0033,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x0034,0x0507);    delay_ms(50);
	    LCD_WriteReg(0x0035,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0036,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0037,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x003A,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x003B,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x0023,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0024,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0025,0x8000);    delay_ms(50);
	    LCD_WriteReg(0x004f,0);        /* ÐÐÊ×Ö·0 */
	    LCD_WriteReg(0x004e,0);        /* ÁÐÊ×Ö·0 */
	}
	else if( DeviceCode == 0x8999 )
	{
		LCD_Code = SSD1298;		
		LCD_WriteReg(0x0028,0x0006);
		LCD_WriteReg(0x0000,0x0001);
		LCD_WriteReg(0x0003,0xaea4);    /* power control 1---line frequency and VHG,VGL voltage */
		LCD_WriteReg(0x000c,0x0004);    /* power control 2---VCIX2 output voltage */
		LCD_WriteReg(0x000d,0x000c);    /* power control 3---Vlcd63 voltage */
		LCD_WriteReg(0x000e,0x2800);    /* power control 4---VCOMA voltage VCOML=VCOMH*0.9475-VCOMA */
		LCD_WriteReg(0x001e,0x00b5);    /* POWER CONTROL 5---VCOMH voltage */   
		LCD_WriteReg(0x0001,0x3b3f);     
		LCD_WriteReg(0x0002,0x0600);
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x6830);
		//LCD_WriteReg(0x0011,0x6818);
		LCD_WriteReg(0x0005,0x0000);
		LCD_WriteReg(0x0006,0x0000);
		LCD_WriteReg(0x0016,0xef1c);  
		LCD_WriteReg(0x0007,0x0033);    /* Display control 1 */
		/* when GON=1 and DTE=0,all gate outputs become VGL */
		/* when GON=1 and DTE=0,all gate outputs become VGH */
		/* non-selected gate wires become VGL */     
		LCD_WriteReg(0x000b,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		LCD_WriteReg(0x0041,0x0000);
		LCD_WriteReg(0x0042,0x0000);
		LCD_WriteReg(0x0048,0x0000);
		LCD_WriteReg(0x0049,0x013f);
		LCD_WriteReg(0x004a,0x0000);
		LCD_WriteReg(0x004b,0x0000); 
		LCD_WriteReg(0x0044,0xef00);	/* Horizontal RAM start and end address */
		LCD_WriteReg(0x0045,0x0000);	/* Vretical RAM start address */
		LCD_WriteReg(0x0046,0x013f);	/* Vretical RAM end address */ 
		LCD_WriteReg(0x004e,0x0000);	/* set GDDRAM x address counter */
		LCD_WriteReg(0x004f,0x0000);    /* set GDDRAM y address counter */   
		/* y control */
		LCD_WriteReg(0x0030,0x0707);
		LCD_WriteReg(0x0031,0x0202);
		LCD_WriteReg(0x0032,0x0204);
		LCD_WriteReg(0x0033,0x0502);
		LCD_WriteReg(0x0034,0x0507);
		LCD_WriteReg(0x0035,0x0204);
		LCD_WriteReg(0x0036,0x0204);
		LCD_WriteReg(0x0037,0x0502);
		LCD_WriteReg(0x003a,0x0302);
		LCD_WriteReg(0x003b,0x0302); 
		LCD_WriteReg(0x0023,0x0000);
		LCD_WriteReg(0x0024,0x0000);
		LCD_WriteReg(0x0025,0x8000);
		LCD_WriteReg(0x0026,0x7000);
		LCD_WriteReg(0x0020,0xb0eb);
		LCD_WriteReg(0x0027,0x007c);
	}
	else if( DeviceCode == 0x5408 )
	{
		LCD_Code = SPFD5408B;
		
		LCD_WriteReg(0x0001,0x0100);	  /* Driver Output Contral Register */ 
		LCD_WriteReg(0x0002,0x0700);      /* LCD Driving Waveform Contral */
		LCD_WriteReg(0x0003,0x1030);	  /* Entry ModeÉèÖÃ */
		
		LCD_WriteReg(0x0004,0x0000);	  /* Scalling Control register */
		LCD_WriteReg(0x0008,0x0207);	  /* Display Control 2 */
		LCD_WriteReg(0x0009,0x0000);	  /* Display Control 3 */
		LCD_WriteReg(0x000A,0x0000);	  /* Frame Cycle Control */
		LCD_WriteReg(0x000C,0x0000);	  /* External Display Interface Control 1 */
		LCD_WriteReg(0x000D,0x0000);      /* Frame Maker Position */
		LCD_WriteReg(0x000F,0x0000);	  /* External Display Interface Control 2 */
		delay_ms(50);
		LCD_WriteReg(0x0007,0x0101);	  /* Display Control */
		delay_ms(50);
		LCD_WriteReg(0x0010,0x16B0);      /* Power Control 1 */
		LCD_WriteReg(0x0011,0x0001);      /* Power Control 2 */
		LCD_WriteReg(0x0017,0x0001);      /* Power Control 3 */
		LCD_WriteReg(0x0012,0x0138);      /* Power Control 4 */
		LCD_WriteReg(0x0013,0x0800);      /* Power Control 5 */
		LCD_WriteReg(0x0029,0x0009);	  /* NVM read data 2 */
		LCD_WriteReg(0x002a,0x0009);	  /* NVM read data 3 */
		LCD_WriteReg(0x00a4,0x0000);  
		LCD_WriteReg(0x0050,0x0000);	  /* ÉèÖÃ²Ù×÷´°¿ÚµÄXÖá¿ªÊ¼ÁÐ */
		LCD_WriteReg(0x0051,0x00EF);	  /* ÉèÖÃ²Ù×÷´°¿ÚµÄXÖá½áÊøÁÐ */
		LCD_WriteReg(0x0052,0x0000);	  /* ÉèÖÃ²Ù×÷´°¿ÚµÄYÖá¿ªÊ¼ÐÐ */
		LCD_WriteReg(0x0053,0x013F);	  /* ÉèÖÃ²Ù×÷´°¿ÚµÄYÖá½áÊøÐÐ */
		   
		LCD_WriteReg(0x0060,0x2700);	  /* Driver Output Control */
										  /* ÉèÖÃÆÁÄ»µÄµãÊýÒÔ¼°É¨ÃèµÄÆðÊ¼ÐÐ */
		LCD_WriteReg(0x0061,0x0003);	  /* Driver Output Control */
		LCD_WriteReg(0x006A,0x0000);	  /* Vertical Scroll Control */
		
		LCD_WriteReg(0x0080,0x0000);	  /* Display Position ¨C Partial Display 1 */
		LCD_WriteReg(0x0081,0x0000);	  /* RAM Address Start ¨C Partial Display 1 */
		LCD_WriteReg(0x0082,0x0000);	  /* RAM address End - Partial Display 1 */
		LCD_WriteReg(0x0083,0x0000);	  /* Display Position ¨C Partial Display 2 */
		LCD_WriteReg(0x0084,0x0000);	  /* RAM Address Start ¨C Partial Display 2 */
		LCD_WriteReg(0x0085,0x0000);	  /* RAM address End ¨C Partail Display2 */
		LCD_WriteReg(0x0090,0x0013);	  /* Frame Cycle Control */
		LCD_WriteReg(0x0092,0x0000); 	  /* Panel Interface Control 2 */
		LCD_WriteReg(0x0093,0x0003);	  /* Panel Interface control 3 */
		LCD_WriteReg(0x0095,0x0110);	  /* Frame Cycle Control */
		LCD_WriteReg(0x0007,0x0173);
	}
	else if( DeviceCode == 0x4531 )
	{	
		LCD_Code = LGDP4531;
		/* Setup display */
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x10,0x0628);
		LCD_WriteReg(0x12,0x0006);
		LCD_WriteReg(0x13,0x0A32);
		LCD_WriteReg(0x11,0x0040);
		LCD_WriteReg(0x15,0x0050);
		LCD_WriteReg(0x12,0x0016);
		delay_ms(50);
		LCD_WriteReg(0x10,0x5660);
		delay_ms(50);
		LCD_WriteReg(0x13,0x2A4E);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0300);	
		LCD_WriteReg(0x03,0x1030);		
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x30,0x0000);
		LCD_WriteReg(0x31,0x0402);
		LCD_WriteReg(0x32,0x0106);
		LCD_WriteReg(0x33,0x0700);
		LCD_WriteReg(0x34,0x0104);
		LCD_WriteReg(0x35,0x0301);
		LCD_WriteReg(0x36,0x0707);
		LCD_WriteReg(0x37,0x0305);
		LCD_WriteReg(0x38,0x0208);
		LCD_WriteReg(0x39,0x0F0B);
		delay_ms(50);
		LCD_WriteReg(0x41,0x0002);
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x90,0x0119);
		LCD_WriteReg(0x92,0x010A);
		LCD_WriteReg(0x93,0x0004);
		LCD_WriteReg(0xA0,0x0100);
		delay_ms(50);
		LCD_WriteReg(0x07,0x0133);
		delay_ms(50);
		LCD_WriteReg(0xA0,0x0000);
	}
	else if( DeviceCode == 0x4535 )
	{	
		LCD_Code = LGDP4535;	
		LCD_WriteReg(0x15, 0x0030); 	 /* Set the internal vcore voltage */                                              
		LCD_WriteReg(0x9A, 0x0010); 	 /* Start internal OSC */
		LCD_WriteReg(0x11, 0x0020);	     /* set SS and SM bit */
		LCD_WriteReg(0x10, 0x3428);	     /* set 1 line inversion */
		LCD_WriteReg(0x12, 0x0002);	     /* set GRAM write direction and BGR=1 */ 
		LCD_WriteReg(0x13, 0x1038);	     /* Resize register */
		delay_ms(40); 
		LCD_WriteReg(0x12, 0x0012);	     /* set the back porch and front porch */
		delay_ms(40); 
		LCD_WriteReg(0x10, 0x3420);	     /* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(0x13, 0x3045);	     /* FMARK function */
		delay_ms(70); 
		LCD_WriteReg(0x30, 0x0000);      /* RGB interface setting */
		LCD_WriteReg(0x31, 0x0402);	     /* Frame marker Position */
		LCD_WriteReg(0x32, 0x0307);      /* RGB interface polarity */
		LCD_WriteReg(0x33, 0x0304);      /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x34, 0x0004);      /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x35, 0x0401);      /* VREG1OUT voltage */
		LCD_WriteReg(0x36, 0x0707);      /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x37, 0x0305);      /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x38, 0x0610);      /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x39, 0x0610);      /* VREG1OUT voltage */
		LCD_WriteReg(0x01, 0x0100);      /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x02, 0x0300);      /* VCM[4:0] for VCOMH */
		LCD_WriteReg(0x03, 0x1030);      /* GRAM horizontal Address */
		LCD_WriteReg(0x08, 0x0808);      /* GRAM Vertical Address */
		LCD_WriteReg(0x0A, 0x0008);		
		LCD_WriteReg(0x60, 0x2700);	     /* Gate Scan Line */
		LCD_WriteReg(0x61, 0x0001);	     /* NDL,VLE, REV */
		LCD_WriteReg(0x90, 0x013E);
		LCD_WriteReg(0x92, 0x0100);
		LCD_WriteReg(0x93, 0x0100);
		LCD_WriteReg(0xA0, 0x3000);
		LCD_WriteReg(0xA3, 0x0010);
		LCD_WriteReg(0x07, 0x0001);
		LCD_WriteReg(0x07, 0x0021);
		LCD_WriteReg(0x07, 0x0023);
		LCD_WriteReg(0x07, 0x0033);
		LCD_WriteReg(0x07, 0x0133);
	} 		 		
	else if( DeviceCode == 0x0047 )
	{
		LCD_Code = HX8347D;
		/* Start Initial Sequence */
		LCD_WriteReg(0xEA,0x00);                          
		LCD_WriteReg(0xEB,0x20);                                                     
		LCD_WriteReg(0xEC,0x0C);                                                   
		LCD_WriteReg(0xED,0xC4);                                                    
		LCD_WriteReg(0xE8,0x40);                                                     
		LCD_WriteReg(0xE9,0x38);                                                    
		LCD_WriteReg(0xF1,0x01);                                                    
		LCD_WriteReg(0xF2,0x10);                                                    
		LCD_WriteReg(0x27,0xA3);                                                    
		/* GAMMA SETTING */
		LCD_WriteReg(0x40,0x01);                           
		LCD_WriteReg(0x41,0x00);                                                   
		LCD_WriteReg(0x42,0x00);                                                   
		LCD_WriteReg(0x43,0x10);                                                    
		LCD_WriteReg(0x44,0x0E);                                                   
		LCD_WriteReg(0x45,0x24);                                                  
		LCD_WriteReg(0x46,0x04);                                                  
		LCD_WriteReg(0x47,0x50);                                                   
		LCD_WriteReg(0x48,0x02);                                                    
		LCD_WriteReg(0x49,0x13);                                                  
		LCD_WriteReg(0x4A,0x19);                                                  
		LCD_WriteReg(0x4B,0x19);                                                 
		LCD_WriteReg(0x4C,0x16);                                                 
		LCD_WriteReg(0x50,0x1B);                                                   
		LCD_WriteReg(0x51,0x31);                                                    
		LCD_WriteReg(0x52,0x2F);                                                     
		LCD_WriteReg(0x53,0x3F);                                                    
		LCD_WriteReg(0x54,0x3F);                                                     
		LCD_WriteReg(0x55,0x3E);                                                     
		LCD_WriteReg(0x56,0x2F);                                                   
		LCD_WriteReg(0x57,0x7B);                                                     
		LCD_WriteReg(0x58,0x09);                                                  
		LCD_WriteReg(0x59,0x06);                                                 
		LCD_WriteReg(0x5A,0x06);                                                   
		LCD_WriteReg(0x5B,0x0C);                                                   
		LCD_WriteReg(0x5C,0x1D);                                                   
		LCD_WriteReg(0x5D,0xCC);                                                   
		/* Power Voltage Setting */
		LCD_WriteReg(0x1B,0x18);                                                    
		LCD_WriteReg(0x1A,0x01);                                                    
		LCD_WriteReg(0x24,0x15);                                                    
		LCD_WriteReg(0x25,0x50);                                                    
		LCD_WriteReg(0x23,0x8B);                                                   
		LCD_WriteReg(0x18,0x36);                           
		LCD_WriteReg(0x19,0x01);                                                    
		LCD_WriteReg(0x01,0x00);                                                   
		LCD_WriteReg(0x1F,0x88);                                                    
		delay_ms(50);
		LCD_WriteReg(0x1F,0x80);                                                  
		delay_ms(50);
		LCD_WriteReg(0x1F,0x90);                                                   
		delay_ms(50);
		LCD_WriteReg(0x1F,0xD0);                                                   
		delay_ms(50);
		LCD_WriteReg(0x17,0x05);                                                    
		LCD_WriteReg(0x36,0x00);                                                    
		LCD_WriteReg(0x28,0x38);                                                 
		delay_ms(50);
		LCD_WriteReg(0x28,0x3C);                                                
	}
	else if( DeviceCode == 0x7783 )
	{
		LCD_Code = ST7781;
		/* Start Initial Sequence */
		LCD_WriteReg(0x00FF,0x0001);
		LCD_WriteReg(0x00F3,0x0008);
		LCD_WriteReg(0x0001,0x0100);
		LCD_WriteReg(0x0002,0x0700);
		LCD_WriteReg(0x0003,0x1030);  
		LCD_WriteReg(0x0008,0x0302);
		LCD_WriteReg(0x0008,0x0207);
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000A,0x0000);
		LCD_WriteReg(0x0010,0x0000);  
		LCD_WriteReg(0x0011,0x0005);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0010,0x12B0);
		delay_ms(50);
		LCD_WriteReg(0x0011,0x0007);
		delay_ms(50);
		LCD_WriteReg(0x0012,0x008B);
		delay_ms(50);	
		LCD_WriteReg(0x0013,0x1700);
		delay_ms(50);	
		LCD_WriteReg(0x0029,0x0022);		
		LCD_WriteReg(0x0030,0x0000);
		LCD_WriteReg(0x0031,0x0707);
		LCD_WriteReg(0x0032,0x0505);
		LCD_WriteReg(0x0035,0x0107);
		LCD_WriteReg(0x0036,0x0008);
		LCD_WriteReg(0x0037,0x0000);
		LCD_WriteReg(0x0038,0x0202);
		LCD_WriteReg(0x0039,0x0106);
		LCD_WriteReg(0x003C,0x0202);
		LCD_WriteReg(0x003D,0x0408);
		delay_ms(50);				
		LCD_WriteReg(0x0050,0x0000);		
		LCD_WriteReg(0x0051,0x00EF);		
		LCD_WriteReg(0x0052,0x0000);		
		LCD_WriteReg(0x0053,0x013F);		
		LCD_WriteReg(0x0060,0xA700);		
		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x0090,0x0033);				
		LCD_WriteReg(0x002B,0x000B);		
		LCD_WriteReg(0x0007,0x0133);
	}
	else	/* special ID */
	{
		DeviceCode = LCD_ReadReg(0x67);

		if( DeviceCode == 0x0046 )
		{
	        LCD_Code = HX8346A;

			/* Gamma for CMO 3.2 */
			LCD_WriteReg(0x46,0x94);
			LCD_WriteReg(0x47,0x41);
			LCD_WriteReg(0x48,0x00);
			LCD_WriteReg(0x49,0x33);
			LCD_WriteReg(0x4a,0x23);
			LCD_WriteReg(0x4b,0x45);
			LCD_WriteReg(0x4c,0x44);
			LCD_WriteReg(0x4d,0x77);
			LCD_WriteReg(0x4e,0x12);
			LCD_WriteReg(0x4f,0xcc);
			LCD_WriteReg(0x50,0x46);
			LCD_WriteReg(0x51,0x82);
			/* 240x320 window setting */
			LCD_WriteReg(0x02,0x00);
			LCD_WriteReg(0x03,0x00);
			LCD_WriteReg(0x04,0x01);
			LCD_WriteReg(0x05,0x3f);
			LCD_WriteReg(0x06,0x00);
			LCD_WriteReg(0x07,0x00);
			LCD_WriteReg(0x08,0x00); 
			LCD_WriteReg(0x09,0xef); 
			
			/* Display Setting */
			LCD_WriteReg(0x01,0x06);						
			LCD_WriteReg(0x16,0xC8);	/* MY(1) MX(1) MV(0) */

			LCD_WriteReg(0x23,0x95);
			LCD_WriteReg(0x24,0x95);
			LCD_WriteReg(0x25,0xff);
			
			LCD_WriteReg(0x27,0x02);
			LCD_WriteReg(0x28,0x02);
			LCD_WriteReg(0x29,0x02);
			LCD_WriteReg(0x2a,0x02);
			LCD_WriteReg(0x2c,0x02);
			LCD_WriteReg(0x2d,0x02);						
			
			LCD_WriteReg(0x3a,0x01);
			LCD_WriteReg(0x3b,0x01);
			LCD_WriteReg(0x3c,0xf0);
			LCD_WriteReg(0x3d,0x00);
			delay_ms(2);
			LCD_WriteReg(0x35,0x38);
			LCD_WriteReg(0x36,0x78);
			
			LCD_WriteReg(0x3e,0x38);
			
			LCD_WriteReg(0x40,0x0f);
			LCD_WriteReg(0x41,0xf0);
			/* Power Supply Setting */
			LCD_WriteReg(0x19,0x49);
			LCD_WriteReg(0x93,0x0f);
			delay_ms(1);
			LCD_WriteReg(0x20,0x30);
			LCD_WriteReg(0x1d,0x07);
			LCD_WriteReg(0x1e,0x00);
			LCD_WriteReg(0x1f,0x07);
			/* VCOM Setting for CMO 3.2¡± Panel */
			LCD_WriteReg(0x44,0x4d);
			LCD_WriteReg(0x45,0x13);
			delay_ms(1);
			LCD_WriteReg(0x1c,0x04);
			delay_ms(2);
			LCD_WriteReg(0x43,0x80);
			delay_ms(5);
			LCD_WriteReg(0x1b,0x08);
			delay_ms(4);
			LCD_WriteReg(0x1b,0x10);		  
			delay_ms(4);		
			/* Display ON Setting */
			LCD_WriteReg(0x90,0x7f);
			LCD_WriteReg(0x26,0x04);
			delay_ms(4);
			LCD_WriteReg(0x26,0x24);
			LCD_WriteReg(0x26,0x2c);
			delay_ms(4);
			LCD_WriteReg(0x26,0x3c);	
			/* Set internal VDDD voltage */
			LCD_WriteReg(0x57,0x02);
			LCD_WriteReg(0x55,0x00);
			LCD_WriteReg(0x57,0x00);
		}
		if( DeviceCode == 0x0047 )
		{
	        LCD_Code = HX8347A;
	        LCD_WriteReg(0x0042,0x0008);   
	        /* Gamma setting */  
		    LCD_WriteReg(0x0046,0x00B4); 
		    LCD_WriteReg(0x0047,0x0043); 
		    LCD_WriteReg(0x0048,0x0013); 
		    LCD_WriteReg(0x0049,0x0047); 
		    LCD_WriteReg(0x004A,0x0014); 
		    LCD_WriteReg(0x004B,0x0036); 
		    LCD_WriteReg(0x004C,0x0003); 
		    LCD_WriteReg(0x004D,0x0046); 
		    LCD_WriteReg(0x004E,0x0005);  
		    LCD_WriteReg(0x004F,0x0010);  
		    LCD_WriteReg(0x0050,0x0008);  
		    LCD_WriteReg(0x0051,0x000a);  
		    /* Window Setting */
		    LCD_WriteReg(0x0002,0x0000); 
		    LCD_WriteReg(0x0003,0x0000); 
		    LCD_WriteReg(0x0004,0x0000); 
		    LCD_WriteReg(0x0005,0x00EF); 
		    LCD_WriteReg(0x0006,0x0000); 
		    LCD_WriteReg(0x0007,0x0000); 
		    LCD_WriteReg(0x0008,0x0001); 
		    LCD_WriteReg(0x0009,0x003F); 
		    delay_ms(10); 
		    LCD_WriteReg(0x0001,0x0006); 
		    LCD_WriteReg(0x0016,0x00C8);   
		    LCD_WriteReg(0x0023,0x0095); 
		    LCD_WriteReg(0x0024,0x0095); 
		    LCD_WriteReg(0x0025,0x00FF); 
		    LCD_WriteReg(0x0027,0x0002); 
		    LCD_WriteReg(0x0028,0x0002); 
		    LCD_WriteReg(0x0029,0x0002); 
		    LCD_WriteReg(0x002A,0x0002); 
		    LCD_WriteReg(0x002C,0x0002); 
		    LCD_WriteReg(0x002D,0x0002); 
		    LCD_WriteReg(0x003A,0x0001);  
		    LCD_WriteReg(0x003B,0x0001);  
		    LCD_WriteReg(0x003C,0x00F0);    
		    LCD_WriteReg(0x003D,0x0000); 
		    delay_ms(20); 
		    LCD_WriteReg(0x0035,0x0038); 
		    LCD_WriteReg(0x0036,0x0078); 
		    LCD_WriteReg(0x003E,0x0038); 
		    LCD_WriteReg(0x0040,0x000F); 
		    LCD_WriteReg(0x0041,0x00F0);  
		    LCD_WriteReg(0x0038,0x0000); 
		    /* Power Setting */ 
		    LCD_WriteReg(0x0019,0x0049);  
		    LCD_WriteReg(0x0093,0x000A); 
		    delay_ms(10); 
		    LCD_WriteReg(0x0020,0x0020);   
		    LCD_WriteReg(0x001D,0x0003);   
		    LCD_WriteReg(0x001E,0x0000);  
		    LCD_WriteReg(0x001F,0x0009);   
		    LCD_WriteReg(0x0044,0x0053);  
		    LCD_WriteReg(0x0045,0x0010);   
		    delay_ms(10);  
		    LCD_WriteReg(0x001C,0x0004);  
		    delay_ms(20); 
		    LCD_WriteReg(0x0043,0x0080);    
		    delay_ms(5); 
		    LCD_WriteReg(0x001B,0x000a);    
		    delay_ms(40);  
		    LCD_WriteReg(0x001B,0x0012);    
		    delay_ms(40);   
		    /* Display On Setting */ 
		    LCD_WriteReg(0x0090,0x007F); 
		    LCD_WriteReg(0x0026,0x0004); 
		    delay_ms(40);  
		    LCD_WriteReg(0x0026,0x0024); 
		    LCD_WriteReg(0x0026,0x002C); 
		    delay_ms(40);   
		    LCD_WriteReg(0x0070,0x0008); 
		    LCD_WriteReg(0x0026,0x003C);  
		    LCD_WriteReg(0x0057,0x0002); 
		    LCD_WriteReg(0x0055,0x0000); 
		    LCD_WriteReg(0x0057,0x0000); 
		}	 
	}  						
    delay_ms(50);   /* delay 50 ms */	
}

void LCD_SetClip(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	
	ClipLeft = x0;
	ClipRight = x1;
	ClipTop = y0;
	ClipBottom = y1;
	
	if (LCD_Code == ILI9325){
		LCD_WriteReg(0x50,x0); //
		LCD_WriteReg(0x51,x1);
		LCD_WriteReg(0x52,y0); 
		LCD_WriteReg(0x53,y1);
	} else 
	if (LCD_Code == SSD1298){
	LCD_WriteReg(0x44,((x1) <<8) + x0);	//HEA, HSA

	LCD_WriteReg(0x45,y0); 			//VSA
	LCD_WriteReg(0x46,y1);			//VEA
	}
	LCD_SetCursor(x0,y0);
}
/*******************************************************************************
* Function Name  : LCD_Clear

* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;
	
	switch (LCD_Code){
		case HX8347D:
		case HX8347A:
		case HX8346A:
				LCD_WriteReg(0x02,0x00);                                                  
		LCD_WriteReg(0x03,0x00);  
		                
		LCD_WriteReg(0x04,0x00);                           
		LCD_WriteReg(0x05,0xEF);  
		                 
		LCD_WriteReg(0x06,0x00);                           
		LCD_WriteReg(0x07,0x00);    
		               
		LCD_WriteReg(0x08,0x01);                           
		LCD_WriteReg(0x09,0x3F);     
		break;
		case SSD1289:
		case SSD1298:
			LCD_WriteReg(0x44,0Xef00);
			LCD_WriteReg(0x45,0x0000); 
			LCD_WriteReg(0x46,0x013F);
		
		LCD_SetCursor(0,0); 
		break;
		
		case ILI9325:
			LCD_WriteReg(0x50,0x0000); //
			LCD_WriteReg(0x51,0x00EF);
			LCD_WriteReg(0x52,0x0000); 
			LCD_WriteReg(0x53,0x013F);
			LCD_SetCursor(0,0);
		break;
	}
	
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for( index = 0; index < (MAX_X) * (MAX_Y); index++ )
	{
		LCD_RAM = Color;
	}
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB ¸ÄÎª BBBBBGGGGGGRRRRR ¸ñÊ½
* Input          : - color: BRG ÑÕÉ«Öµ  
* Output         : None
* Return         : RGB ÑÕÉ«Öµ
* Attention		 : ÄÚ²¿º¯Êýµ÷ÓÃ
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}


uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);

	switch( LCD_Code )
	{
		case ST7781:
		case LGDP4531:
		case LGDP4535:
		case SSD1289:
		case SSD1298:
 		     return  LCD_ReadRAM();

	    case HX8346A:			 	
	    case HX8347A:
	    case HX8347D:
             {		
		        uint8_t red,green,blue; 
				/* Write 16-bit Index (then Read Reg) */
                LCD_REG = R34;  /* Select GRAM Reg */

				dummy = LCD_RAM;

		        red = LCD_RAM >> 3; 
                green = LCD_RAM >> 2; 
                blue = LCD_RAM >> 3; 	
                dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue ); 
			 }
 	         return  dummy;
        default:	/* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
 		     return  LCD_BGR2RGB( LCD_ReadRAM() );
	}
}


void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
		
	if ((Xpos < ClipLeft) || (Xpos > ClipRight-1)) return;
	if ((Ypos < ClipTop) || (Ypos > ClipBottom-1)) return;
	
	
	LCD_SetCursor(Xpos,Ypos);
    LCD_WriteRAM_Prepare();
    LCD_WriteRAM(point);
}


void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
 
	
	  int deltaX = abs(x1 - x0);
    int deltaY = abs(y1 - y0);
    int signX = x0 < x1 ? 1 : -1;
    int signY = y0 < y1 ? 1 : -1;
		int error2;
    //
    int error = deltaX - deltaY;
    //
    LCD_SetPoint(x1, y1, color);
    while(x0 != x1 || y0 != y1) {
        LCD_SetPoint(x0, y0, color);
        error2 = error * 2;
        //
        if(error2 > -deltaY) {
            error -= deltaY;
            x0 += signX;
        }
        if(error2 < deltaX) {
            error += deltaX;
            y0 += signY;
        }
    }

} 

/******************************************************************************
* Function Name  : PutChar
* Description    : ½«LcdÆÁÉÏÈÎÒâÎ»ÖÃÏÔÊ¾Ò»¸ö×Ö·û
* Input          : - Xpos: Ë®Æ½×ø±ê 
*                  - Ypos: ´¹Ö±×ø±ê  
*				   - ASCI: ÏÔÊ¾µÄ×Ö·û
*				   - charColor: ×Ö·ûÑÕÉ«   
*				   - bkColor: ±³¾°ÑÕÉ« 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_PutChar( uint16_t Xpos, uint16_t Ypos, char ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
  char buffer[16], tmp_char;
	
	if (Ypos<(ClipTop+1)) return;
	if (Ypos>(ClipBottom-1 - 16)) return;
	
    GetASCIICode(buffer,ASCI);  /* È¡×ÖÄ£Êý¾Ý */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* ×Ö·ûÑÕÉ« */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* ±³¾°ÑÕÉ« */
            }
        }
    }
}

void LCD_PutTransparentChar( uint16_t Xpos, uint16_t Ypos, char ASCI, uint16_t charColor)
{
	uint16_t i, j;
  char buffer[16], tmp_char;
	
	
	if (Ypos<(ClipTop+1)) return;
	if (Ypos>(ClipBottom-1 - 16)) return;
	
	
    GetASCIICode(buffer,ASCI);  /* È¡×ÖÄ£Êý¾Ý */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* ×Ö·ûÑÕÉ« */
            }
            
        }
    }
}
/******************************************************************************
* Function Name  : GUI_Text
* Description    : ÔÚÖ¸¶¨×ù±êÏÔÊ¾×Ö·û´®
* Input          : - Xpos: ÐÐ×ù±ê
*                  - Ypos: ÁÐ×ù±ê 
*				   - str: ×Ö·û´®
*				   - charColor: ×Ö·ûÑÕÉ«   
*				   - bkColor: ±³¾°ÑÕÉ« 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Text(uint16_t Xpos, uint16_t Ypos, char *str,uint16_t Color, uint16_t bkColor)
{
    char TempChar;
    do
    {
        TempChar = *str++;  
        LCD_PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

void LCD_TransparentText(uint16_t Xpos, uint16_t Ypos, char *str,uint16_t Color)
{
    char TempChar;
    do
    {
        TempChar = *str++;  
        LCD_PutTransparentChar( Xpos, Ypos, TempChar, Color);    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}
void LCD_FillRect(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1, uint16_t Color){
	uint32_t index=0;
	
	LCD_SetClip(x0,y0,x1-1,y1-1);
	LCD_SetCursor(x0,y0); 
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for( index = 0; index < (x1-x0)*(y1-y0); index++ )
	{
		LCD_RAM = Color;
	}
	
	LCD_SetClip(0,0,MAX_X-1, MAX_Y-1);
}

void LCD_Rect(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1, uint16_t Color){
	
	LCD_DrawHLine( x0, x1, y0, Color );
	LCD_DrawVLine( y0, y1, x1, Color );
	
	
	LCD_DrawHLine( x0, x1, y1, Color );
	LCD_DrawVLine( y0, y1, x0, Color );
}

void LCD_DrawImage(uint16_t x, uint16_t y,uint16_t size_w, uint16_t size_h, const unsigned short *data){
	uint16_t i,j;
	
	for (i=0; i< size_h; i++)
	 for (j= 0; j<size_w; j++){
		 LCD_SetPoint(j,i, data[i*size_w+j]);
		  LCD_WriteRAM(data[i*size_w+j]);
			 
	 }
	

	 
}

void LCD_DrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color){
	short sign = x0 < x1 ? 1: -1;
	
	while (x0 != x1){		
		LCD_SetPoint(x0,y,color);		
		x0 += sign;
	}
}

void LCD_DrawVLine(uint16_t y0, uint16_t y1, uint16_t x, uint16_t color){
	short sign = y0 < y1 ? 1: -1;
	
	while (y0 != y1){		
		LCD_SetPoint(x,y0,color);		
		y0 += sign;
	}
}

void LCD_DrawDottedHLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t dash, uint16_t color){
	short sign = x0 < x1 ? 1: -1;
	
	while (x0 != x1){
		if (x0%dash > (dash/2))
				LCD_SetPoint(x0,y0,color);		
		x0 += sign;
	}
}

void LCD_DrawDottedVLine(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t dash, uint16_t color){
	short sign = y0 < y1 ? 1: -1;
	
	while (y0 != y1){
		if (y0%dash > (dash/2))
			LCD_SetPoint(x0,y0,color);
		
		y0 += sign;
	}
}

void DrawImage(uint16_t x, uint16_t y,uint16_t size_w, uint16_t size_h, const unsigned short *data){
	uint16_t i,j;
	 
/*	for (i=0; i< size_h; i++)
	 for (j= 0; j<size_w; j++){
		 LCD_SetPoint(x+j,y+i, data[i*size_w+j]);
			 
	 }
	*/
	LCD_SetClip(x,y,x+size_w,y+size_h);
	LCD_SetCursor(x,y); 
	LCD_WriteRAM_Prepare(); 

	for (i=0; i< size_h; i++)
	 for (j= 0; j<size_w; j++){
		 LCD_SetPoint(x+j,y+i, data[i*size_w+j]);			 
	 }
	
	LCD_SetClip(0,0,MAX_X-1, MAX_Y-1);
}

void LCD_DrawTime(uint16_t x, uint16_t y, int timeMS){
	char ctime[12];

	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t ss;
	
		sec = (timeMS/1000)%60;
		min = (timeMS/60000)%60;
		hour = ((timeMS/3600000))%60;
		ss = (timeMS/100)%10;
		
		sprintf(ctime, "%3d:%2d:%2d:%d",hour,min,sec,ss);
	
	//GUI_Text(5,300,ctime, COLOR_BLACK, COLOR_SKY);
	LCD_Text(x, y,ctime, Black, Green);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

