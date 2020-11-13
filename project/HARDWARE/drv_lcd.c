#include "drv_lcd.h"
/*
SCL - PA4 - A0
SDA - PA5 - A1
RES - PA6 - A2
DC  - PA7 - A3
CS  - PC4 - A4
BL  - PC5 - A5
*/

extern void delay_ms(uint16_t nms);
void lcd_init(uint16_t color)
{
    lcd_gpio_init();

    LCD_BL = 1;

    /* reset ST7735R */
    LCD_RST = 0;
    delay_ms(100);
    LCD_RST = 1;
    delay_ms(100);

    lcd_write_cmd(0x11);//Sleep exit
    delay_ms (120);

    //ST7735R Frame Rate
    lcd_write_cmd(0xB1);
    lcd_write_data(0x01);
    lcd_write_data(0x2C);
    lcd_write_data(0x2D);

    lcd_write_cmd(0xB2);
    lcd_write_data(0x01);
    lcd_write_data(0x2C);
    lcd_write_data(0x2D);

    lcd_write_cmd(0xB3);
    lcd_write_data(0x01);
    lcd_write_data(0x2C);
    lcd_write_data(0x2D);
    lcd_write_data(0x01);
    lcd_write_data(0x2C);
    lcd_write_data(0x2D);

    lcd_write_cmd(0xB4); //Column inversion
    lcd_write_data(0x07);

    //ST7735R Power Sequence
    lcd_write_cmd(0xC0);
    lcd_write_data(0xA2);
    lcd_write_data(0x02);
    lcd_write_data(0x84);
    lcd_write_cmd(0xC1);
    lcd_write_data(0xC5);

    lcd_write_cmd(0xC2);
    lcd_write_data(0x0A);
    lcd_write_data(0x00);

    lcd_write_cmd(0xC3);
    lcd_write_data(0x8A);
    lcd_write_data(0x2A);
    lcd_write_cmd(0xC4);
    lcd_write_data(0x8A);
    lcd_write_data(0xEE);

    lcd_write_cmd(0xC5); //VCOM
    lcd_write_data(0x0E);

    lcd_write_cmd(0x36); //MX, MY, RGB mode
    
    /*设置显示方向:C0/A0,00/60*/
    lcd_write_data(0xA0);   
    
    lcd_write_cmd(0xe0);
    
    lcd_write_data(0x0f);
    lcd_write_data(0x1a);
    lcd_write_data(0x0f);
    lcd_write_data(0x18);
    lcd_write_data(0x2f);
    lcd_write_data(0x28);
    lcd_write_data(0x20);
    lcd_write_data(0x22);
    lcd_write_data(0x1f);
    lcd_write_data(0x1b);
    lcd_write_data(0x23);
    lcd_write_data(0x37);
    lcd_write_data(0x00);
    lcd_write_data(0x07);
    lcd_write_data(0x02);
    lcd_write_data(0x10);

    lcd_write_cmd(0xe1);
    lcd_write_data(0x0f);
    lcd_write_data(0x1b);
    lcd_write_data(0x0f);
    lcd_write_data(0x17);
    lcd_write_data(0x33);
    lcd_write_data(0x2c);
    lcd_write_data(0x29);
    lcd_write_data(0x2e);
    lcd_write_data(0x30);
    lcd_write_data(0x30);
    lcd_write_data(0x39);
    lcd_write_data(0x3f);
    lcd_write_data(0x00);
    lcd_write_data(0x07);
    lcd_write_data(0x03);
    lcd_write_data(0x10);

    lcd_write_cmd(0x2a);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x7f);

    lcd_write_cmd(0x2b);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x9f);

    lcd_write_cmd(0xF0); //Enable test command
    lcd_write_data(0x01);
    lcd_write_cmd(0xF6); //Disable ram power save mode
    lcd_write_data(0x00);

    lcd_write_cmd(0x3A); //65k mode
    lcd_write_data(0x05);

    lcd_write_cmd(0x29);//Display on
    lcd_clear(color);
}

void lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    LCD_BL = 0;    
}

void lcd_spi_write(uint8_t dat)
{
    uint8_t i = 0;
    for(i = 8; i > 0; i--)
    {
        if(dat & 0x80)
            LCD_SDA = 1; 
        else
            LCD_SDA = 0;

        LCD_SCL = 0;
        LCD_SCL = 1;
        dat <<= 1;
    }
}

void lcd_write_cmd(uint8_t dat)
{
    LCD_CS = 0;
    LCD_DC = 0;
    lcd_spi_write(dat);
    LCD_CS = 1;
}

void lcd_write_data(uint8_t dat)
{
    LCD_CS = 0;
    LCD_DC = 1;
    lcd_spi_write(dat);
    LCD_CS = 1;
}

void lcd_write_16bit(uint16_t dat)
{
    LCD_CS = 0;
    LCD_DC = 1;
    lcd_spi_write(dat >> 8);
    lcd_spi_write(dat);
    LCD_CS = 1;
}


void lcd_write_reg(uint8_t addr, uint8_t dat)
{
    lcd_write_cmd(addr);
    lcd_write_data(dat);
}

void lcd_set_region(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(0x00);
    lcd_write_data(x1 + 2);
    lcd_write_data(0x00);
    lcd_write_data(x2 + 2);

    lcd_write_cmd(0x2b);
    lcd_write_data(0x00);
    lcd_write_data(y1 + 1);
    lcd_write_data(0x00);
    lcd_write_data(y2 + 1);

    lcd_write_cmd(0x2c);
}

void lcd_set_xy(uint16_t x, uint16_t y)
{
    lcd_set_region(x, y, x, y);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_region(x, y, x + 1, y + 1);
    lcd_write_16bit(color);
}

void lcd_clear(uint16_t color)
{
    uint8_t x, y;
    lcd_set_region(0, 0, LCD_SIZE_Y-1, LCD_SIZE_X-1);
    lcd_write_cmd(0x2C);
    for(x = 0; x < LCD_SIZE_Y; x++)
    {
        for(y = 0; y < LCD_SIZE_X; y++)
            lcd_write_16bit(color);
    }
}
