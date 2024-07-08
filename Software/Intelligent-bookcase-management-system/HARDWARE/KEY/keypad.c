#include "keypad.h"
#include "stm32f10x.h"
#include "delay.h"
#include "stdio.h"
void KEY_Init(void)
{
    /*
B 3 4 5 8 \ 9 10 11 A8
    */

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    //    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}
// B 3 4 5 8 \ 9 10 11 A8
uint8_t keyvalue = 99;
uint8_t set_free = 0, key_press = 99;
uint8_t Key_Scan(void)
{ // 扫描
    key_press = 99;
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)
        key_press = 1;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)
        key_press = 4;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
        key_press = 7;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 1)
        key_press = 10;
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);

    GPIO_SetBits(GPIOB, GPIO_Pin_10);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)
        key_press = 2;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)
        key_press = 5;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
        key_press = 8;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 1)
        key_press = 0;
    GPIO_ResetBits(GPIOB, GPIO_Pin_10);

    GPIO_SetBits(GPIOB, GPIO_Pin_11);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)
        key_press = 3;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)
        key_press = 6;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
        key_press = 9;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 1)
        key_press = 11;
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);

    GPIO_SetBits(GPIOA, GPIO_Pin_8);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)
        key_press = 15;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)
        key_press = 14;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
        key_press = 13;
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 1)
        key_press = 12;
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);

    // 按键消抖
    if (set_free == 0)
    {
        if (key_press != 99)
        {
            keyvalue = key_press;
        }
    } // keyvalue设置成功
    if (key_press == 99)
        set_free = 0; // 上一次是否松开
    else
        set_free = 1;
    return key_press;
}