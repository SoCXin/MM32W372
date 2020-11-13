#include "drv_led.h"

/*
LD1 - PA15
LD2 - PC10
LD3 - PC11
LD4 - PC12
*/
extern void delay_ms(uint16_t ms);

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    led_all_off();
}

void led_set(uint8_t num, status sts)
{
    switch(num)
    {
       case 1: LD1_OUT = (sts == ON) ? 0 : 1;break;
       case 2: LD2_OUT = (sts == ON) ? 0 : 1;break;
       case 3: LD3_OUT = (sts == ON) ? 0 : 1;break;
       case 4: LD4_OUT = (sts == ON) ? 0 : 1;break;
    }
} 

void led_all_off(void)
{
    LD1_OUT = 1;
    LD2_OUT = 1;
    LD3_OUT = 1;
    LD4_OUT = 1;
}

//LD1 - PA15
void led1_toggle(void)
{
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) == 0)
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
    else 
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void led_blink(uint16_t time)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
    GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
    delay_ms(time);
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
    delay_ms(time);    
}
