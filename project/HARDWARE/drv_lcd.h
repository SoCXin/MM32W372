#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__

#include "sys.h"

/*
SCL - PA4 - A0
SDA - PA5 - A1
RES - PA6 - A2
DC  - PA7 - A3
CS  - PC4 - A4
BL  - PC5 - A5
*/
#define LCD_SCL PCout(1)
#define LCD_SDA PCout(0)
#define LCD_RST PCout(14)
#define LCD_DC  PCout(15)
#define LCD_CS  PCout(13)
#define LCD_BL  PBout(0)

#define LCD_SIZE_X  128
#define LCD_SIZE_Y  160

#define RED  	0xF800
#define GREEN	0x07E0
#define BLUE 	0x001F
#define WHITE	0xFFFF
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D
#define GRAY1   0x8410
#define GRAY2   0x4208

void lcd_gpio_init(void);void lcd_spi_write(uint8_t dat);
void lcd_init(uint16_t color);
void lcd_write_data(uint8_t dat);
void lcd_write_cmd(uint8_t dat);
void lcd_write_16bit(uint16_t dat);
void lcd_write_reg(uint8_t addr, uint8_t dat);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_set_region(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_set_xy(uint16_t x, uint16_t y);
void lcd_clear(uint16_t color);

#endif

