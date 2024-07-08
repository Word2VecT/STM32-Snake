#include "SysTick.h"
#include "lcd.h"
#include "font.h"

#define MAX_HZ_POSX 480
#define MAX_HZ_POSY 227 

#define LCD_RAM    *(__IO uint16_t *) (0x6C000002)    	//��ַ�Ĵ���
#define LCD_REG    *(__IO uint16_t *) (0x6C000000)	 	//ָ��Ĵ���

#define rw_data_prepare()               write_cmd(34)

void write_cmd(unsigned short cmd);
unsigned short read_data(void);
unsigned short DeviceCode;

static void delay(int cnt)
{
	volatile unsigned int dl;
	while(cnt--)
	{
		for(dl=0; dl<500; dl++);
	}
}

/*
 *	
 */
void write_cmd(unsigned short cmd)
{	
	LCD_REG = cmd;	
}

unsigned short read_data(void)
{
	unsigned short temp;	
	temp = LCD_RAM;
	temp = LCD_RAM;
	return temp;
}

void write_data(unsigned short data_code )
{	
	LCD_RAM = data_code;
}

void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
	write_cmd(reg_addr);
	write_data(reg_val);
}

unsigned short read_reg(unsigned char reg_addr)
{
	unsigned short val=0;
	write_cmd(reg_addr);
	val = read_data();
	return (val);
}


void lcd_SetCursor(unsigned int x,unsigned int y)
{
	write_reg(0x004e,x);    /* 0-239 */
	write_reg(0x004f,y);    /* 0-319 */
}
/* ��ȡָ����ַ��GRAM */
static unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
	unsigned short temp;
	lcd_SetCursor(x,y);
	rw_data_prepare();
	/* dummy read */
	temp = read_data();
	temp = read_data();
	return temp;
}
static void lcd_data_bus_test(void)
{
	unsigned short temp1;
	unsigned short temp2;
	/* wirte */
	lcd_SetCursor(0,0);
	rw_data_prepare();
	write_data(0x5555);

	lcd_SetCursor(1,0);
	rw_data_prepare();
	write_data(0xAAAA);

	/* read */
	lcd_SetCursor(0,0);
	temp1 = lcd_read_gram(0,0);
	temp2 = lcd_read_gram(1,0);

	if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
	{
		//printf(" data bus test pass!\r\n");
	}
	else
	{
		//printf(" data bus test error: %04X %04X\r\n",temp1,temp2);
	}
}

void lcd_clear(unsigned short Color)
{
    unsigned int count;

	write_cmd(0x002a);	
	write_data(0);	    
	write_data(0);
	write_data(HDP>>8);	    
	write_data(HDP&0x00ff);
    write_cmd(0x002b);	
	write_data(0);	    
	write_data(0);
	write_data(VDP>>8);	    
	write_data(VDP&0x00ff);
	write_cmd(0x002c);	
	write_cmd(0x002c);


	for(count=0;count<130560;count++){
		
		write_data(Color);
	}
}

void port_fsmc_init(void)
{	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	//FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure;

	
	FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;

	FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);

	/*--------------------- read timings configuration ---------------------*/
	Timing_read.FSMC_AddressSetupTime = 0;  /* [3:0] F2/F4 1~15 HCLK */
	Timing_read.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
	Timing_read.FSMC_DataSetupTime = 3;     /* [15:8] F2/F4 0~255 HCLK */
	/* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
	Timing_read.FSMC_BusTurnAroundDuration = 0;
	Timing_read.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
	Timing_read.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
	Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;

	/*--------------------- write timings configuration ---------------------*/
	Timing_write.FSMC_AddressSetupTime = 0;  /* [3:0] F2/F4 1~15 HCLK */
	Timing_write.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
	Timing_write.FSMC_DataSetupTime = 0;     /* [15:8] F2/F4 0~255 HCLK */
	/* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
	Timing_write.FSMC_BusTurnAroundDuration = 0;
	Timing_write.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
	Timing_write.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
	Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;

	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);	
	/* Configure the BL pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	

	GPIO_ResetBits(GPIOF,GPIO_Pin_9);

	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                  +
	+-------------------+--------------------+------------------+------------------+
	| PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0 <-> FSMC_A10 |
	| PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1 <-> FSMC_A11 |
	| PD4  <-> FSMC_NOE | PE7  <-> FSMC_D4   | PF2  <-> FSMC_A2 | PG2 <-> FSMC_A12 |
	| PD5  <-> FSMC_NWE | PE8  <-> FSMC_D5   | PF3  <-> FSMC_A3 | PG3 <-> FSMC_A13 |
	| PD8  <-> FSMC_D13 | PE9  <-> FSMC_D6   | PF4  <-> FSMC_A4 | PG4 <-> FSMC_A14 |
	| PD9  <-> FSMC_D14 | PE10 <-> FSMC_D7   | PF5  <-> FSMC_A5 | PG5 <-> FSMC_A15 |
	| PD10 <-> FSMC_D15 | PE11 <-> FSMC_D8   | PF12 <-> FSMC_A6 | PG9 <-> FSMC_NE2 |
	| PD11 <-> FSMC_A16 | PE12 <-> FSMC_D9   | PF13 <-> FSMC_A7 |------------------+
	| PD12 <-> FSMC_A17 | PE13 <-> FSMC_D10  | PF14 <-> FSMC_A8 | 
	| PD14 <-> FSMC_D0  | PE14 <-> FSMC_D11  | PF15 <-> FSMC_A9 | 
	| PD15 <-> FSMC_D1  | PE15 <-> FSMC_D12  |------------------+
	+-------------------+--------------------+
	*/
	/*
	+-------------------+--------------------+------------------+-------------------+
	+                       STM32 FSMC pins assignment                              +
	+-------------------+--------------------+------------------+-------------------+
	| PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
	| PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
	| PD4  <-> FSMC_NOE | PE3  <-> FSMC_A19  | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
	| PD5  <-> FSMC_NWE | PE4  <-> FSMC_A20  | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
	| PD8  <-> FSMC_D13 | PE7  <-> FSMC_D4   | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
	| PD9  <-> FSMC_D14 | PE8  <-> FSMC_D5   | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
	| PD10 <-> FSMC_D15 | PE9  <-> FSMC_D6   | PF12 <-> FSMC_A6 | PG9  <-> FSMC_NE2 |
	| PD11 <-> FSMC_A16 | PE10 <-> FSMC_D7   | PF13 <-> FSMC_A7 | PG12 <-> FSMC_NE4 |
	| PD12 <-> FSMC_A17 | PE11 <-> FSMC_D8   | PF14 <-> FSMC_A8 |-------------------+
	| PD13 <-> FSMC_A18 | PE12 <-> FSMC_D9   | PF15 <-> FSMC_A9 |
	| PD14 <-> FSMC_D0  | PE13 <-> FSMC_D10  |------------------+
	| PD15 <-> FSMC_D1  | PE14 <-> FSMC_D11  |
	| PD7  <-> FSMC_NE1 | PE15 <-> FSMC_D12  |
	+-------------------+--------------------+
	*/
	
	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	/* GPIOE configuration */
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	/*GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);*/

	/* GPIOG configuration */
	/*GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);*/
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
		| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
		| GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
		/*| GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13*/
		| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = /*GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3 |*/
		GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
		GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
		GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  /*| GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
		GPIO_Pin_4  | GPIO_Pin_5  |
		GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15*/;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =/* GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
		GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_9 |*/ GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

	/*!< Enable FSMC Bank1_SRAM1 Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);	
}

void lcd_init(void)
{
	port_fsmc_init();

	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	delay(100);
	DeviceCode = read_reg(0x0000);

  write_cmd(0x002b);	
	write_data(0);

	Delay(5); // delay 50 ms 
	write_cmd(0x00E2);					//PLL multiplier, set PLL clock to 120M
	write_data(0x001D);					//N=0x36 for 6.5M, 0x23 for 10M crystal
	write_data(0x0002);
	write_data(0x0004);
	
	write_cmd(0x00E0);					//PLL enable
	write_data(0x0001);
	Delay(1);
	write_cmd(0x00E0);
	write_data(0x0003);
	Delay(5);
	write_cmd(0x0001);  					//software reset
	Delay(5);
	write_cmd(0x00E6);					//PLL setting for PCLK, depends on resolution
//	LCD_WriteRAM(0x0001);
//	LCD_WriteRAM(0x0033);
//	LCD_WriteRAM(0x0032);
	write_data(0x0000);
	write_data(0x00D9);
	write_data(0x0016);

	write_cmd(0x00B0);					//LCD SPECIFICATION
	write_data(0x0020);
	write_data(0x0000);
	write_data((HDP>>8)&0X00FF);			//Set HDP
	write_data(HDP&0X00FF);
    write_data((VDP>>8)&0X00FF);			//Set VDP
	write_data(VDP&0X00FF);
    write_data(0x0000);

	write_cmd(0x00B4);					//HSYNC
	write_data((HT>>8)&0X00FF); 			//Set HT
	write_data(HT&0X00FF);
	write_data((HPS>>8)&0X00FF);			//Set HPS
	write_data(HPS&0X00FF);
	write_data(HPW);						//Set HPW
	write_data((LPS>>8)&0X00FF); 			//Set HPS
	write_data(LPS&0X00FF);
	write_data(0x0000);

	write_cmd(0x00B6);					//VSYNC
	write_data((VT>>8)&0X00FF);   		//Set VT
	write_data(VT&0X00FF);
	write_data((VPS>>8)&0X00FF); 			//Set VPS
	write_data(VPS&0X00FF);
	write_data(VPW);						//Set VPW
	write_data((FPS>>8)&0X00FF);			//Set FPS
	write_data(FPS&0X00FF);

	//=============================================

	//=============================================
	write_cmd(0x00BA);
	write_data(0x0005);//0x000F);    //GPIO[3:0] out 1

	write_cmd(0x00B8);
	write_data(0x0007);    //GPIO3=input, GPIO[2:0]=output
	write_data(0x0001);    //GPIO0 normal

	write_cmd(0x0036); //rotation
	write_data(0x0000);

	Delay(50);

	write_cmd(0x00BE); //set PWM for B/L
	write_data(0x0006);
	write_data(0x0080);
	
	write_data(0x0001);
	write_data(0x00f0);
	write_data(0x0000);
	write_data(0x0000);

	write_cmd(0x00d0);//���ö�̬����������� 
	write_data(0x000d);
   
	write_cmd(0x00F0); //pixel data interface
	write_data(0x0003); //03:16λ   02:24λ

	write_cmd(0x0029); //display on

	//lcd_data_bus_test();
	lcd_clear(Red);	
}
/*********************************************************************************************************
** Functoin name:       LCD_SetCursor
** Descriptions:        ��������
** input paraments:     Xpos��Ypos ����
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	write_cmd(0x002A);	
	write_data(Xpos>>8);	    
	write_data(Xpos&0x00ff);
	write_data(479>>8);	    
	write_data(479&0x00ff);
    write_cmd(0x002b);	
	write_data(Ypos>>8);	    
	write_data(Ypos&0x00ff);
	write_data(271>>8);	    
	write_data(271&0x00ff);
}
/*********************************************************************************************************
** Functoin name:       LCD_DrawPoint
** Descriptions:        ����
** input paraments:     x.y:���������
						color����ɫ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/

void LCD_DrawPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	LCD_SetCursor(xsta, ysta);  		/*���ù��λ��  */
	write_data_Prepare();          	/*��ʼд��GRAM */
	write_data(color); 

}
/**
	*���ƣ�void WriteOneHz(uint16_t x0, uint16_t y0, uint8_t *pucMsk, uint16_t PenColor, uint16_t BackColor)
	*������x0,y0     ��ʼ����
		   *pucMsk   ָ��
		   PenColor	 �ַ���ɫ
		   BackColor ������ɫ
	*���ܣ�
	*��ע���˺������ܵ�����Ϊ�����ַ���ʾ											  
**/					
void WriteOneHz(u16 x0, u16 y0, u8 *pucMsk, u16 PenColor, u16 BackColor)
{
    u16 i,j;
    u16 mod[16];                                      /* ��ǰ��ģ 16*16 */
    u16 *pusMsk;                                      /* ��ǰ�ֿ��ַ  */
    u16 y;

    pusMsk = (u16 *)pucMsk;


    for(i=0; i<16; i++)                                    /* ���浱ǰ���ֵ���ʽ��ģ       */
    {
        mod[i] = *pusMsk;                                /* ȡ�õ�ǰ��ģ�����ֶ������   */
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);/* ��ģ�����ߵ��ֽ�*/
		pusMsk = pusMsk+1;
    }
    y = y0;
    for(i=0; i<16; i++)                                    /* 16��   */
    {                                              
        for(j=0; j<16; j++)                                /* 16��   */
        {
		    if((mod[i] << j) & 0x8000)       /* ��ʾ��i�� ��16���� */
            {
				LCD_DrawPoint(x0+j, y0+i, PenColor); 
            } 
			else 
			{
				LCD_DrawPoint(x0+j, y0+i, BackColor);
			}
        }
        y++;
    }
}
/**
	*����: u16 findHzIndex(u8 *hz)
	*������hz
	*���ܣ��������ִ洢���ڴ��ַ
	*��ע��
**/
u16 findHzIndex(u8 *hz)                            /* ���Զ��庺�ֿ��ڲ�����Ҫ��ʾ */
                                                      /* �ĺ��ֵ�λ�� */
{
	u16 i=0;
	FNT_GB16 *ptGb16 = (FNT_GB16 *)GBHZ_16;		  /*ptGb16ָ��GBHZ_16*/
	while(ptGb16[i].Index[0] > 0x80)
	{
	    if ((*hz == ptGb16[i].Index[0]) && (*(hz+1) == ptGb16[i].Index[1])) /*��������λ����ʾ��ַ��*/
		{
	        return i;
	    }
	    i++;
	    if(i > (sizeof((FNT_GB16 *)GBHZ_16) / sizeof(FNT_GB16) - 1))  /* �����±�Լ�� */
	    {
		    break;
	    }
	}
	return 0;
}
/**
	*���ƣ�void LCD_ShowHzString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
	*������x0��y0    ��ʼ����
		   pcStr     ָ��
		   PenColor  ������ɫ
		   BackColor ���屳��
	*���ܣ���ʾ�����ַ���
	*��ע������������ܵ�������	       
**/
void LCD_ShowHzString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
	u16 usIndex;
	u8 size = 16; 
	FNT_GB16 *ptGb16 = 0;    
    ptGb16 = (FNT_GB16 *)GBHZ_16; 

	if(x0>MAX_HZ_POSX){x0=0;y0+=size;}			         /*����X��������С��λ������*/
    if(y0>MAX_HZ_POSY){y0=x0=0;lcd_clear(White);}	   /*����Y��������С��λ���ص�ԭ�㣬��������*/

	usIndex = findHzIndex(pcStr);
	WriteOneHz(x0, y0, (u8 *)&(ptGb16[usIndex].Msk[0]),  PenColor, BackColor); /* ��ʾ�ַ� */
}
/**
	*���ƣ�void LCD_ShowChar(u8 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor)
	*������x��y      ��ʼ���꣨x:0~234 y:0~308��
		   num       �ַ�ASCII��ֵ
		   size      �ַ���С��ʹ��Ĭ��8*16
		   PenColor  ������ɫ
		   BackColor ���屳����ɫ
	*���ܣ�
	*��ע��ע����Ļ��С
**/
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor)
{       
#define MAX_CHAR_POSX (272-8)
#define MAX_CHAR_POSY (480-16) 
    u8 temp;
    u8 pos,t;
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;		    
	num=num-' ';                         /*�õ�ƫ�ƺ��ֵ */
	for(pos=0;pos<size;pos++)
	{
		if(size==12)
			temp=asc2_1206[num][pos];/*����1206����*/
		else 
			temp=asc2_1608[num][pos];		 /*����1608����	*/
		for(t=0;t<size/2;t++)
	    {                 
	        if(temp&0x01)			   /*�ӵ�λ��ʼ*/
			{
				LCD_DrawPoint(x+t, y+pos, PenColor);  /*��������ɫ һ����*/
			}
			else 
				LCD_DrawPoint(x+t, y+pos, BackColor);	   /*��������ɫ һ����*/     
	        temp>>=1; 
	    }
	}			
}
/**
	���ƣ�void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
	������x��y      ��ʼ����
	      p         ָ���ַ�����ʼ��ַ
		  PenColor  �ַ���ɫ
		  BackColor ������ɫ
	���ܣ�
	��ע����16���壬���Ե��� �˺������ܵ�������
**/
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
{   
	uint8_t size = 16;     /*---�ַ���СĬ��16*8---*/
	 
    if(x>MAX_HZ_POSX){x=0;y+=size;}			         /*����X��������С��λ������*/
    if(y>MAX_HZ_POSY){y=x=0;lcd_clear(White);}	 /*����Y��������С��λ���ص�ԭ�㣬��������*/
    LCD_ShowChar(x, y, *p, size, PenColor, BackColor);			   /*0��ʾ�ǵ��ӷ�ʽ*/
}

/**
	*���ƣ�void LCD_ShowString(u16 x0, u16 y0, u8 *pcstr, u16 PenColor, u16 BackColor)
	*������x0 y0     ��ʼ����
		   pcstr     �ַ���ָ��
		   PenColor  ������ɫ
		   BackColor ���屳��ɫ
	*���ܣ������ַ��ͺ�����ʾ������ʵ���ַ�����ʾ
	*��ע��	
**/
void LCD_ShowString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
	while(*pcStr!='\0')
	{
	 	if(*pcStr>0x80) /*��ʾ����*/
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;	
		}
		else           /*��ʾ�ַ�*/
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	
	}	

}
//
///*********************************************************************************************************
//** Functoin name:       write_data_Prepare
//** Descriptions:        д���ݿ�ʼ
//** input paraments:     ��
//** output paraments:    ��    
//** Returned values:     ��
//*********************************************************************************************************/

void write_data_Prepare(void)
{
	write_cmd(0x002C);
}
/*********************************************************************************************************
** Functoin name:       LCD_WindowMax
** Descriptions:        ���ô���
** input paraments:     ���ڵ�λ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void LCD_WindowMax (unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend) 
{
	write_cmd(0X002A);
	write_data(xsta>>8);
	write_data(xsta&0X00FF);
	write_data(xend>>8);
	write_data(xend&0X00FF);
		
	write_cmd(0X002B);	
	write_data(ysta>>8);	
	write_data(ysta&0X00FF);
	write_data(yend>>8);	
	write_data(yend&0X00FF);			
}
/*********************************************************************************************************
** Functoin name:       LCD_Fill
** Descriptions:       ��䴰��
** input paraments:     ���ڵ�λ��
						colour����ɫ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void LCD_Fill(uint8_t xsta, uint16_t ysta, uint8_t xend, uint16_t yend, uint16_t colour)
{                    
    u32 n;

	/*���ô���	*/	
	LCD_WindowMax (xsta, ysta, xend, yend); 
	write_data_Prepare();         /*��ʼд��GRAM*/	 	   	   
	n=(u32)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){write_data(colour);} /*��ʾ��������ɫ*/	 
} 
/*********************************************************************************************************
** Functoin name:       LCD_DrawLine
** Descriptions:        ָ������(����)������
** input paraments:     xsta X��ʼ����
	   	   				ysta Y��ʼ����
						xend X�յ�����
						yend Y�յ�����
						colour����ɫ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/

void LCD_DrawLine(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    u16 x, y, t;	 
	if((xsta==xend)&&(ysta==yend))
		LCD_DrawPoint(xsta, ysta, color);
	else 
		if(abs(yend-ysta)>abs(xend-xsta))/*б�ʴ���1 */ 
		{
			if(ysta>yend) 
			{
				t=ysta;
				ysta=yend;
				yend=t; 
				t=xsta;
				xsta=xend;
				xend=t; 
			}
			for(y=ysta;y<yend;y++)            /*��y��Ϊ��׼*/ 
			{
				x=(u32)(y-ysta)*(xend-xsta)/(yend-ysta)+xsta;
				LCD_DrawPoint(x, y, color);  
			}
		}
		else     /*б��С�ڵ���1 */
		{
			if(xsta>xend)
			{
				t=ysta;
				ysta=yend;
				yend=t;
				t=xsta;
				xsta=xend;
				xend=t;
			}   
			for(x=xsta;x<=xend;x++)  /*��x��Ϊ��׼*/ 
			{
				y =(u32)(x-xsta)*(yend-ysta)/(xend-xsta)+ysta;
				LCD_DrawPoint(x, y, color); 
			}
		} 
}
/**
	*���ƣ�void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
	*������x0 ���ĵ������
	       y0 ���ĵ�������
		   r  �뾶
	*���أ���
	*���ܣ���ָ��λ�û�һ��ָ����С��Բ
	*��ע��������ɫ�������Ƿ�����
**/
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             /*�ж��¸���λ�õı�־*/
	while(a<=b)
	{
		LCD_DrawPoint(x0-b, y0-a, color);             //3           
		LCD_DrawPoint(x0+b, y0-a, color);             //0           
		LCD_DrawPoint(x0-a, y0+b, color);             //1       
		LCD_DrawPoint(x0-b, y0-a, color);             //7           
		LCD_DrawPoint(x0-a, y0-b, color);             //2             
		LCD_DrawPoint(x0+b, y0+a, color);             //4               
		LCD_DrawPoint(x0+a, y0-b, color);             //5
		LCD_DrawPoint(x0+a, y0+b, color);             //6 
		LCD_DrawPoint(x0-b, y0+a, color);             
		a++;

		/*ʹ��Bresenham�㷨��Բ*/     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a, y0+b, color);
	}
}
/**
	*���ƣ�void LCD_DrawRectangle(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend)
	*������xsta X��ʼ����
	       ysta Y��ʼ����
		   xend X��������
		   yend Y��������
	*���أ���
	*���ܣ���ָ�����򻭾���
	*��ע��

**/
void LCD_DrawRectangle(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
	LCD_DrawLine(xsta, ysta, xend, ysta, color);
	LCD_DrawLine(xsta, ysta, xsta, yend, color);
	LCD_DrawLine(xsta, yend, xend, yend, color);
	LCD_DrawLine(xend, ysta, xend, yend, color);
} 
/****************************************************************************
* ��    �ƣ�void LCD_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* ��    �ܣ���ָ�����귶Χ��ʾһ��ͼƬ
* ��ڲ�����StartX     ����ʼ����
*           StartY     ����ʼ����
*           EndX       �н�������
*           EndY       �н�������
            pic        ͼƬͷָ��
* ���ڲ�������
* ˵    ����ͼƬȡģ��ʽΪˮƽɨ�裬16λ��ɫģʽ
* ���÷�����LCD_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void LCD_DrawPicture(u16 StartX,u16 StartY,u16 Xend,u16 Yend,u8 *pic)
{
	static	u16 i=0,j=0;
	u16 *bitmap = (u16 *)pic;
	for(j=0; j<Yend-StartY; j++)
	{
		for(i=0; i<Xend-StartX; i++) 
		LCD_DrawPoint(StartX+i, StartY+j, *bitmap++); 	
	}
}
