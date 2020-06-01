/*
 * lcdGdi.h:
 *
 * Copyright (c) 2015 WHJWNAVY.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */
#ifndef __LCD192X64_H__
#define __LCD192X64_H__

#include "sys.h"
#include "font.h"

// Size
#define LCD_DRV_MAX_X (192)
#define LCD_DRV_MAX_Y (64)
#define LCD_DRV_PAGE_ROW (8)
#define LCD_DRV_PAGE_MAX (LCD_DRV_MAX_Y / LCD_DRV_PAGE_ROW)

extern void lcd_drv_init(void);
extern void lcd_drv_update(void);

extern void lcd_fb_clear(uint8_t mask);
extern void lcd_fb_page(const uint8_t *page);
extern void lcd_fb_set_orientation(uint8_t orientation);
extern void lcd_fb_set_point(int32_t x, int32_t y, uint8_t mask);
extern uint8_t lcd_fb_get_point(int32_t x, int32_t y);
extern void lcd_fb_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t mask);
extern void lcd_fb_lineto(int32_t x, int32_t y, uint8_t mask);
extern void lcd_fb_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t mask, uint8_t filled);
extern void lcd_fb_circle(int32_t x, int32_t y, int32_t r, uint8_t mask, uint8_t filled);
extern void lcd_fb_ellipse(int32_t cx, int32_t cy, int32_t xRadius, int32_t yRadius, uint8_t mask, uint8_t filled);
extern void lcd_fb_putc(int32_t x, int32_t y, ascii_type_e font, char c, uint8_t bg, uint8_t fg);
extern void lcd_fb_puts(int32_t x, int32_t y, ascii_type_e font, const char *str, uint8_t bg, uint8_t fg);
extern void lcd_fb_putn(int32_t x, int32_t y, ascii_type_e font, int32_t num, uint8_t bg, uint8_t fg);
extern void lcd_fb_puthz(int32_t x, int32_t y, uint16_t code, uint8_t bg, uint8_t fg);
extern void lcd_fb_puthzs(int32_t x, int32_t y, const char *str, uint8_t bg, uint8_t fg);
extern void lcd_fb_bmp(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t *bmp, uint8_t bg, uint8_t fg);
extern void lcd_fb_fbmp(int32_t x, int32_t y, bmp_type_e type, uint8_t bg, uint8_t fg);
extern void lcd_drv_bmp_speed(int32_t x0, int32_t y0, int32_t width, int32_t height, uint8_t *bmp, int32_t colour);

#endif
