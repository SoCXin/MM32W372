#include "drv_key.h"
#include "ncov_data.h"
/*
K1 - PB1 - 下拉
K2 - PB2 - 上拉
K3 - PB10 - 上拉
K4 - PB11 - 上拉
*/
extern void delay_ms(uint16_t nms);
extern void MyUartPrintf(UART_TypeDef *UARTx, char *fmt, ...);
extern void gui_show_dataChina(void);
extern void gui_show_dataGlobal(void);

void key_init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能复用功能时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef USE_EXIT
    /* 外部中断配置 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line10 | EXTI_Line11;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_Init(&EXTI_InitStructure);

    /* EXTI_Line1 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //01/02/03
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    /* EXTI_Line2 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* EXTI_Line10 & EXTI_Line11 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}
#ifdef USE_EXIT
/* K1-EXTI_Line1 */
void EXTI1_IRQHandler(void)
{
    delay_ms(10);
    if(K1 == 1)
    {
        MyUartPrintf(UART1, "K1: 更新疫情数据\r\n");
    }
    get_ncov_api(api_addr, parse_ncov_data);
    EXTI_ClearITPendingBit(EXTI_Line1);
}

/* K2-EXTI_Line2 */
void EXTI2_IRQHandler(void)
{
    delay_ms(10);
    if(K2 == 0)
    {
        MyUartPrintf(UART1, "K2: 显示国内疫情数据\r\n");
        gui_show_dataChina();
    }
    EXTI_ClearITPendingBit(EXTI_Line2);
}

/* K3-EXTI_Line10 & K4-EXTI_Line11 */
void EXTI15_10_IRQHandler(void)
{
    delay_ms(10);
    if(K3 == 0)
    {
        MyUartPrintf(UART1, "K3: 显示全球疫情数据\r\n");
        gui_show_dataGlobal();
    }
    if(K4 == 0)
    {
        MyUartPrintf(UART1, "K4: 重新建立连接\r\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11);
}
#endif

uint8_t key_get(uint8_t mode)
{
    static uint8_t key_up = 1;

    if (mode)
        key_up = 1;
    if (key_up && (K1 == 1 || K2 == 0 || K3 == 0 || K4 == 0))
    {
        delay_ms(10);//去抖动
        key_up = 0;
        if (K1 == 1)
            return 1;
        else if (K2 == 0)
            return 2;
        else if (K3 == 0)
            return 3;
        else if (K4 == 0)
            return 4;
    }
    else if (K1 == 0 && K2 == 1 && K3 == 1 && K4 == 1)
        key_up = 1;

    return 0;
}
