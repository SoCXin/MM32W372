#ifndef __DRV_LCD_GUI_H__
#define __DRV_LCD_GUI_H__

#include "drv_lcd.h"
#include "ncov_data.h"
#include "math.h"

typedef enum {
    DIR_X = 0,
    DIR_Y
}direction;

extern const uint8_t F8X16[][16];

void gui_draw_point(uint8_t x0, uint8_t y0, uint8_t en, uint16_t color);
void gui_drawLine(uint8_t x0, uint8_t y0, uint8_t len, uint8_t dir, uint16_t color);
void gui_drawRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

void gui_show_F6X8_Char(uint8_t x0, uint8_t y0, uint8_t idx, uint16_t color);
void gui_show_F6X8_String(uint8_t x0, uint8_t y0, uint8_t *str, uint16_t color);

void gui_show_F8X16_Char(uint8_t x0, uint8_t y0, uint8_t idx, uint16_t color);
void gui_show_F8X16_String(uint8_t x0, uint8_t y0, uint8_t *str, uint16_t color);
void gui_show_num(uint16_t x, uint16_t y, long num, uint16_t color);

void gui_show_F16X16_Char(uint8_t x0, uint8_t y0, uint8_t idx, uint16_t color);
void gui_show_dataChina(void);
void gui_show_dataGlobal(void);

void my_getNum(long num, uint8_t *cnt, bool *minusFlag);
uint32_t my_pow(uint8_t m, uint8_t n);


#endif

