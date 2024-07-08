#include "Gpio.h"
#include "SysTick.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SNAKE_LENGTH 100
#define GRID_SIZE 10
#define WIDTH 240
#define HEIGHT 240
#define DIRECTION_UP 0
#define DIRECTION_LEFT 1
#define DIRECTION_DOWN 2
#define DIRECTION_RIGHT 3

#define RCC_KEY5 RCC_AHB1Periph_GPIOC
#define GPIO_KEY5_PORT GPIOC
#define GPIO_KEY5 GPIO_Pin_13
#define RCC_KEY3 RCC_AHB1Periph_GPIOI
#define GPIO_KEY3_PORT GPIOI
#define GPIO_KEY3 GPIO_Pin_9

typedef struct {
    int x;
    int y;
} Point;

Point snake[MAX_SNAKE_LENGTH];
Point food;
int snake_length = 1;
int direction = DIRECTION_RIGHT;
int i;
char text[10] = "score:";
char textOver[15] = "Game Over!!!";

void GPIO_KEY_Config(void);
void TIM2_Config(void);
void Init_Game(void);
void Draw_Snake(void);
void Move_Snake(void);
void Generate_Food(void);
void Draw_Food(void);
int Check_Collision(void);
void LEDGpio_Init(void);
void Delay1(__IO uint32_t nCount);

void GPIO_KEY_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure KEY3 Button */
    RCC_AHB1PeriphClockCmd(RCC_KEY3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_KEY3;
    GPIO_Init(GPIO_KEY3_PORT, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource9);

    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure KEY5 Button */
    RCC_AHB1PeriphClockCmd(RCC_KEY5, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_KEY5;
    GPIO_Init(GPIO_KEY5_PORT, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 4200 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void Init_Game(void)
{
    direction = DIRECTION_RIGHT;
    snake_length = 1;

    for (i = 0; i < snake_length; i++) {
        snake[i].x = 50 - i * GRID_SIZE;
        snake[i].y = 50;
    }

    Generate_Food();
}

void Generate_Food(void)
{
    food.x = (rand() % (HEIGHT / GRID_SIZE)) * GRID_SIZE;
    food.y = (rand() % (WIDTH / GRID_SIZE)) * GRID_SIZE;
}

void Draw_Snake(void)
{
    sprintf(text + 6, "%d", snake_length);
    LCD_ShowString(10, 10, text, White, Black);
    for (i = 0; i < snake_length; i++) {
        LCD_Fill(snake[i].x, snake[i].y, snake[i].x + GRID_SIZE, snake[i].y + GRID_SIZE, Green);
    }
}

void Draw_Food(void)
{
    LCD_Fill(food.x, food.y, food.x + GRID_SIZE, food.y + GRID_SIZE, Red);
}

void Move_Snake(void)
{
    for (i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
    case DIRECTION_UP:
        snake[0].y -= GRID_SIZE;
        break;
    case DIRECTION_DOWN:
        snake[0].y += GRID_SIZE;
        break;
    case DIRECTION_LEFT:
        snake[0].x -= GRID_SIZE;
        break;
    case DIRECTION_RIGHT:
        snake[0].x += GRID_SIZE;
        break;
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length++;
        Generate_Food();
    }
}

int Check_Collision(void)
{
    if (snake[0].x < 0 || snake[0].x >= HEIGHT || snake[0].y < 0 || snake[0].y >= WIDTH) {
        return 1;
    }

    for (i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            while (1) {
                LCD_ShowString(120, 135, textOver, Red, Black);
            }
        }
    }

    return 0;
}

void Delay1(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--)
        ;
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        direction = (direction + 1) % 4;
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        direction = (direction - 1 + 4) % 4;
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        lcd_clear(Black);
        Move_Snake();
        if (Check_Collision()) {
            TIM_Cmd(TIM2, DISABLE);
        } else {
            Draw_Snake();
            Draw_Food();
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

int main(void)
{
    SysTick_Init();
    lcd_init();
    LEDGpio_Init();
    GPIO_KEY_Config();
    TIM2_Config();
    Init_Game();

    while (1) {
    }
}
