#include "stm32f4xx.h"
#include "Gpio.h"
#include "Exti.h"

/******************************************************************************************
*�������ƣ�void ExtiLine0_IRQ(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵�����ⲿ�ж�0������
*******************************************************************************************/
void ExtiLine0_IRQ(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        if(GPIO_ReadOutputDataBit(LED1_GPIO,LED1_GPIO_PIN) == Bit_SET)     //�жϵƵĵ�ǰ״̬��Ȼ��Ե�ȡ��
            LED1_ONOFF(Bit_RESET);
        else
            LED1_ONOFF(Bit_SET);

        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/******************************************************************************************
*�������ƣ�void ExtiLine6_IRQ(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵�����ⲿ�ж�6������
*******************************************************************************************/
void ExtiLine6_IRQ(void)
{
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        if(GPIO_ReadOutputDataBit(LED2_GPIO,LED2_GPIO_PIN) == Bit_SET)        //�жϵƵĵ�ǰ״̬��Ȼ��Ե�ȡ��
            LED2_ONOFF(Bit_RESET);
        else
            LED2_ONOFF(Bit_SET);

        EXTI_ClearITPendingBit(EXTI_Line6);
    }
}

/******************************************************************************************
*�������ƣ�void EXTILine0_Config(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵�����ⲿ�ж�0��ʼ��
*******************************************************************************************/
void EXTILine0_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��IOʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //�����Ͻ�
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;             //IO��Ϊ0
  GPIO_Init(GPIOA, &GPIO_InitStructure);                //��������gpio

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//ʵʼ���ж���0

  EXTI_InitStructure.EXTI_Line = EXTI_Line0;                   //�����ж���Ϊ�ж���0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //�����ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //����Ϊ�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //�����ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);                              

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;             
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************************
*�������ƣ�void EXTILine6_Config(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵�����ⲿ�ж�6��ʼ��
*******************************************************************************************/
void EXTILine6_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource6);

  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}
