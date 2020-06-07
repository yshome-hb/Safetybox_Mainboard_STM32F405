/*
 * lcdGdi.c:
 *	Graphics-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based on the generic 12864H chips
 *
 *	There are many variations on these chips, however they all mostly
 *	seem to be similar.
 *	This implementation has the Pins from the Pi hard-wired into it,
 *	in particular wiringPi pins 0-7 so that we can use
 *	digitalWriteByete() to speed things up somewhat.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "delay.h"
#include "stmflash.h"
#include "lcd192x64.h"

// Hardware Pins
#define LCD_PIN_RST -1
#define LCD_PIN_CS 	6
#define LCD_PIN_DC 	4
#define LCD_PIN_SCL 3
#define LCD_PIN_SDA 5
#define LCD_PIN_LIGHT 7

#if (LCD_PIN_RST > -1)
	#define LCD_GPIO_RST 	PBout(LCD_PIN_RST)
#else
	#define LCD_GPIO_RST 	
#endif

#define LCD_GPIO_CS 	PBout(LCD_PIN_CS)
#define LCD_GPIO_DC 	PBout(LCD_PIN_DC)
#define LCD_GPIO_SCL 	PBout(LCD_PIN_SCL)
#define LCD_GPIO_SDA 	PBout(LCD_PIN_SDA)
#define LCD_GPIO_LIGHT 	PBout(LCD_PIN_LIGHT)

// Software copy of the framebuffer
static uint8_t frameBuffer[LCD_DRV_PAGE_MAX][LCD_DRV_MAX_X] = {0};

static int32_t lastX = 0, lastY = 0;
static int32_t mirrorX = 0, mirrorY = 0;

/*
 * spi_write_data:
 *	Send an data byte by spi.
 *********************************************************************************
 */
static inline void lcd_drv_spi(uint8_t data)
{
  uint8_t i = 0;
  LCD_GPIO_CS = 0;
  for (i = 0; i < 8; i++)
  {
    LCD_GPIO_SCL = 0;
    LCD_GPIO_SDA = ((data & 0x80) > 0);
    LCD_GPIO_SCL = 1;
    data <<= 1;
  }
  LCD_GPIO_CS = 1;
}

/*
 * lcd_send_data:
 *	Send an data byte to the display.
 *********************************************************************************
 */
static inline void lcd_drv_send_data(uint8_t data)
{
  LCD_GPIO_DC = 1;
	lcd_drv_spi(data);
}

/*
 * lcd_send_cmd:
 *	Send an data byte to the display.
 *********************************************************************************
 */
static inline void lcd_drv_send_cmd(uint8_t cmd)
{
  LCD_GPIO_DC = 0;
	lcd_drv_spi(cmd);
}


static void lcd_drv_hw_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#if (LCD_PIN_RST > -1)	
  LCD_GPIO_RST = 1;
  delay_ms(10);
  LCD_GPIO_RST = 0;
  delay_ms(10);
  LCD_GPIO_RST = 1;
#endif	

//	LCD_GPIO_LIGHT = 0;
//  delay_ms(10);
//	LCD_GPIO_LIGHT = 1;

  delay_ms(200);
  lcd_drv_send_cmd(0xE2);
  delay_ms(200);
  lcd_drv_send_cmd(0x2F);
  delay_ms(200);
}


/*
 * lcd_drv_open:
 *	Open hardware display.
 *********************************************************************************
 */
static void lcd_drv_hw_open(void)
{
  lcd_drv_send_cmd(0xA0);	
  lcd_drv_send_cmd(0x81);
  lcd_drv_send_cmd(0x65);	
  lcd_drv_send_cmd(0xEB);	
  lcd_drv_send_cmd(0xC4);
  lcd_drv_send_cmd(0xAF);
  lcd_drv_send_cmd(0x40);	
}

/*
 * lcd_drv_close:
 *	Cloase the hardware display.
 *********************************************************************************
 */
static void lcd_drv_hw_close(void)
{
#if 0
  lcd_drv_send_data(0X8D, LCD_SEND_MODE_CMD); //SET DCDC
  lcd_drv_send_data(0X10, LCD_SEND_MODE_CMD); //DCDC OFF
  lcd_drv_send_data(0XAE, LCD_SEND_MODE_CMD); //DISPLAY OFF
#endif
}

/*
 * lcd_drv_hw_clear:
 *	Clear the hardware display.
 *********************************************************************************
 */
static void lcd_drv_hw_clear(void)
{
#if 0
  int32_t i, n;
  for (i = 0; i < 8; i++)
  {
    lcd_drv_send_data(0xb0 + i, LCD_SEND_MODE_CMD);
    lcd_drv_send_data(0x02, LCD_SEND_MODE_CMD);
    lcd_drv_send_data(0x10, LCD_SEND_MODE_CMD);
    for (n = 0; n < 128; n++)
    {
      lcd_drv_send_data(0, LCD_DISP_MODE_DAT);
    }
  }
#endif
}


static void lcd_drv_address(uint8_t page, uint8_t col)
{
   lcd_drv_send_cmd(0xb0+page);	
	 lcd_drv_send_cmd(((col>>4)&0x0f)+0x10);	
	 lcd_drv_send_cmd(col&0x0f);	
}

/*
 * lcd_drv_init:
 *	Initialise the display and GPIO.
 *********************************************************************************
 */
void lcd_drv_init(void)
{
  lcd_drv_hw_init();
  lcd_drv_hw_clear();
  lcd_drv_hw_open();

  lcd_fb_set_orientation(0);
  lcd_fb_clear(0);
	lcd_drv_update();
}

/*
 * lcd_drv_update:
 *	Copy our software version to the real display
 *********************************************************************************
 */
void lcd_drv_update(void)
{
  uint8_t x = 0, y = 0;
  for (y = 0; y < LCD_DRV_PAGE_MAX; y++)
  {
		lcd_drv_address(y, 0);
    for (x = 0; x < LCD_DRV_MAX_X; x++)
    {
      lcd_drv_send_data(frameBuffer[y][x]);
    }
  }
}


/*
 * lcd_drv_clear:
 *	Clear the display to the given colour.
 *********************************************************************************
 */
void lcd_fb_clear(uint8_t mask)
{
  if(mask == 0)
    memset(frameBuffer, 0x00, sizeof(frameBuffer));
  else
    memset(frameBuffer, 0xFF, sizeof(frameBuffer));
}

/*
 * lcd_fb_page:
 *	
 *********************************************************************************
 */
void lcd_fb_page(const uint8_t *page)
{
  if(page == NULL)
    return;
  
  memcpy(frameBuffer, page, sizeof(frameBuffer));
}

/*
 * lcd_drv_set_orientation:
 *	Set the display orientation:
 *	0: Normal, the display is portrait mode, 0,0 is top left
 *	1: Mirror x
 *	2: Mirror y
 *	3: Mirror x and y
 *********************************************************************************
 */
void lcd_fb_set_orientation(uint8_t orientation)
{
  switch (orientation)
  {
  case 0:
    mirrorX = 0;
    mirrorY = 0;
    break;

  case 1:
    mirrorX = 1;
    mirrorY = 0;
    break;

  case 2:
    mirrorX = 0;
    mirrorY = 1;
    break;

  case 3:
    mirrorX = 1;
    mirrorY = 1;
    break;

  default:
    break;
  }
}

/*
 * lcd_fb_set_point:
 *	Plot a pixel.
 *********************************************************************************
 */
void lcd_fb_set_point(int32_t x, int32_t y, uint8_t mask)
{
  uint8_t bitmsk = 0;
  if (mirrorX)
    x = (LCD_DRV_MAX_X - x - 1);

  if (mirrorY)
    y = (LCD_DRV_MAX_Y - y - 1);

  lastX = x;
  lastY = y;

  if ((x < 0) || (x >= LCD_DRV_MAX_X) || (y < 0) || (y >= LCD_DRV_MAX_Y))
    return;

  bitmsk = 0x01 << (y & 0x07);
	if(mask > 0)
		frameBuffer[y / LCD_DRV_PAGE_ROW][x] |= bitmsk;		
	else
		frameBuffer[y / LCD_DRV_PAGE_ROW][x] &= (~bitmsk);	
}

/*
 * lcd_fb_set_point:
 *	Plot a pixel.
 *********************************************************************************
 */
uint8_t lcd_fb_get_point(int32_t x, int32_t y)
{
  uint8_t bitmsk = 0;

  if (mirrorX)
    x = (LCD_DRV_MAX_X - x - 1);

  if (mirrorY)
    y = (LCD_DRV_MAX_Y - y - 1);

  if ((x < 0) || (x >= LCD_DRV_MAX_X) || (y < 0) || (y >= LCD_DRV_MAX_Y))
    return 0;

  bitmsk = 0x01 << (y & 0x07);
  return ((frameBuffer[y / LCD_DRV_PAGE_ROW][x]&bitmsk) > 0);
}

/*
 * lcd_fb_line: lcd_fb_lineto:
 *	Classic Bressenham Line code
 *********************************************************************************
 */
void lcd_fb_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t mask)
{
  int32_t dx, dy;
  int32_t sx, sy;
  int32_t err, e2;

  lastX = x1;
  lastY = y1;

  dx = abs(x1 - x0);
  dy = abs(y1 - y0);

  sx = (x0 < x1) ? 1 : -1;
  sy = (y0 < y1) ? 1 : -1;

  err = dx - dy;
  for (;;)
  {
    lcd_fb_set_point(x0, y0, mask);
    if ((x0 == x1) && (y0 == y1))
      break;

    e2 = 2 * err;

    if (e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }

    if (e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}

void lcd_fb_lineto(int32_t x, int32_t y, uint8_t mask)
{
  lcd_fb_line(lastX, lastY, x, y, mask);
}

/*
 * lcd_drv_rectangle:
 *	A rectangle is a spoilt days fishing
 *********************************************************************************
 */
void lcd_fb_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t mask, uint8_t filled)
{
  int32_t x;
  if (filled)
  {
    if (x1 == x2)
    {
      lcd_fb_line(x1, y1, x2, y2, mask);
    }
    else if (x1 < x2)
    {
      for (x = x1; x <= x2; ++x)
      {
        lcd_fb_line(x, y1, x, y2, mask);
      }
    }
    else
    {
      for (x = x2; x <= x1; ++x)
      {
        lcd_fb_line(x, y1, x, y2, mask);
      }
    }
  }
  else
  {
    lcd_fb_line(x1, y1, x2, y1, mask);
    lcd_fb_lineto(x2, y2, mask);
    lcd_fb_lineto(x1, y2, mask);
    lcd_fb_lineto(x1, y1, mask);
  }
}

/*
 * lcd_drv_circle:
 *      This is the midpoint32 circle algorithm.
 *********************************************************************************
 */
void lcd_fb_circle(int32_t x, int32_t y, int32_t r, uint8_t mask, uint8_t filled)
{
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;

  int32_t f = 1 - r;
  int32_t x1 = 0;
  int32_t y1 = r;

  if (filled)
  {
    lcd_fb_line(x, y + r, x, y - r, mask);
    lcd_fb_line(x + r, y, x - r, y, mask);
  }
  else
  {
    lcd_fb_set_point(x, y + r, mask);
    lcd_fb_set_point(x, y - r, mask);
    lcd_fb_set_point(x + r, y, mask);
    lcd_fb_set_point(x - r, y, mask);
  }

  while (x1 < y1)
  {
    if (f >= 0)
    {
      y1--;
      ddF_y += 2;
      f += ddF_y;
    }
    x1++;
    ddF_x += 2;
    f += ddF_x;
    if (filled)
    {
      lcd_fb_line(x + x1, y + y1, x - x1, y + y1, mask);
      lcd_fb_line(x + x1, y - y1, x - x1, y - y1, mask);
      lcd_fb_line(x + y1, y + x1, x - y1, y + x1, mask);
      lcd_fb_line(x + y1, y - x1, x - y1, y - x1, mask);
    }
    else
    {
      lcd_fb_set_point(x + x1, y + y1, mask);
      lcd_fb_set_point(x - x1, y + y1, mask);
      lcd_fb_set_point(x + x1, y - y1, mask);
      lcd_fb_set_point(x - x1, y - y1, mask);
      lcd_fb_set_point(x + y1, y + x1, mask);
      lcd_fb_set_point(x - y1, y + x1, mask);
      lcd_fb_set_point(x + y1, y - x1, mask);
      lcd_fb_set_point(x - y1, y - x1, mask);
    }
  }
}

/*
 * lcd_drv_ellipse:
 *	Fast ellipse drawing algorithm by 
 *      John Kennedy
 *	Mathematics Department
 *	Santa Monica College
 *	1900 Pico Blvd.
 *	Santa Monica, CA 90405
 *	jrkennedy6@gmail.com
 *	-Confirned in email this algorithm is in the public domain -GH-
 *********************************************************************************
 */
static void plot_for_ellipse_points(int32_t cx, int32_t cy, int32_t x, int32_t y, uint8_t mask, uint8_t filled)
{
  if (filled)
  {
    lcd_fb_line(cx + x, cy + y, cx - x, cy + y, mask);
    lcd_fb_line(cx - x, cy - y, cx + x, cy - y, mask);
  }
  else
  {
    lcd_fb_set_point(cx + x, cy + y, mask);
    lcd_fb_set_point(cx - x, cy + y, mask);
    lcd_fb_set_point(cx - x, cy - y, mask);
    lcd_fb_set_point(cx + x, cy - y, mask);
  }
}

void lcd_fb_ellipse(int32_t cx, int32_t cy, int32_t xRadius, int32_t yRadius, uint8_t mask, uint8_t filled)
{
  int32_t x, y;
  int32_t xChange, yChange, ellipseError;
  int32_t twoAsquare, twoBsquare;
  int32_t stoppingX, stoppingY;

  twoAsquare = 2 * xRadius * xRadius;
  twoBsquare = 2 * yRadius * yRadius;

  x = xRadius;
  y = 0;

  xChange = yRadius * yRadius * (1 - 2 * xRadius);
  yChange = xRadius * xRadius;

  ellipseError = 0;
  stoppingX = twoBsquare * xRadius;
  stoppingY = 0;

  while (stoppingX >= stoppingY) // 1st set of point32s
  {
    plot_for_ellipse_points(cx, cy, x, y, mask, filled);
    ++y;
    stoppingY += twoAsquare;
    ellipseError += yChange;
    yChange += twoAsquare;

    if ((2 * ellipseError + xChange) > 0)
    {
      --x;
      stoppingX -= twoBsquare;
      ellipseError += xChange;
      xChange += twoBsquare;
    }
  }

  x = 0;
  y = yRadius;

  xChange = yRadius * yRadius;
  yChange = xRadius * xRadius * (1 - 2 * yRadius);

  ellipseError = 0;
  stoppingX = 0;
  stoppingY = twoAsquare * yRadius;

  while (stoppingX <= stoppingY) //2nd set of point32s
  {
    plot_for_ellipse_points(cx, cy, x, y, mask, filled);
    ++x;
    stoppingX += twoBsquare;
    ellipseError += xChange;
    xChange += twoBsquare;

    if ((2 * ellipseError + yChange) > 0)
    {
      --y;
      stoppingY -= twoAsquare;
      ellipseError += yChange;
      yChange += twoAsquare;
    }
  }
}

/*
 * lcd_drv_putc:
 *	Print a single character to the screen
 *********************************************************************************
 */
void lcd_fb_putc(int32_t x, int32_t y, ascii_type_e font, char c, uint8_t bg, uint8_t fg)
{
  int32_t x1, x2;
  int32_t y1, y2;
  uint8_t line;
	uint8_t size;
  uint8_t *fontPtr = NULL;
	uint8_t *mallocPtr = NULL;
	font_t *disp_font = NULL;

	disp_font = font_get(font);
	c -= disp_font->cmin;
  line = (disp_font->width >> 3) + ((disp_font->width&0x07) > 0);
	size = disp_font->height * line;
	fontPtr = (uint8_t *)pvPortMalloc(size);
	stmflash_read(disp_font->addr+c*size, fontPtr, size);
	
	mallocPtr = fontPtr;
  for (y1 = 0; y1 < disp_font->height; y1++)
  {
    y2 = y + y1;
		for (x1 = 0; x1 < disp_font->width; x1++)
		{	
			x2 = x + x1;			
			lcd_fb_set_point(x2, y2, mallocPtr[x1 >> 3] & (0x80 >> (x1 & 0x07))? fg : bg);
		}
    mallocPtr += line;
  }
	
	vPortFree(fontPtr);
}

/*
 * lcd_drv_puts:
 *	Send a string to the display. Obeys \n and \r formatting
 *********************************************************************************
 */
void lcd_fb_puts(int32_t x, int32_t y, ascii_type_e font, const char *str, uint8_t bg, uint8_t fg)
{
  int32_t c, mx, my;
  mx = x;
  my = y;

	font_t *disp_font = NULL;

	disp_font = font_get(font);	
	
  while (*str)
  {
    c = *str++;

    if (c == '\r')
    {
      mx = x;
      continue;
    }

    if (c == '\n')
    {
      my += disp_font->height;
      continue;
    }

    lcd_fb_putc(mx, my, font, c, bg, fg);

    mx += disp_font->width;
    //if (mx >= (LCD_DRV_MAX_X - fontWidth))
    if (mx > (LCD_DRV_MAX_X - disp_font->width))
    {
      mx = 0;
      my += disp_font->width;
    }
  }
}

/*
 * lcd_drv_putn:
 *	Send a number to the display. 
 *********************************************************************************
 */
void lcd_fb_putn(int32_t x, int32_t y, ascii_type_e font, int32_t num, uint8_t bg, uint8_t fg)
{
  char numString[32] = {0};
  if (sprintf(numString, "%d", num) < 0)
    return;
  lcd_fb_puts(x, y, font, numString, bg, fg);
}

/*
 * lcd_drv_puthz:
 *	Print a single character to the screen
 *********************************************************************************
 */
void lcd_fb_puthz(int32_t x, int32_t y, uint16_t code, uint8_t bg, uint8_t fg)
{
  int32_t x1, x2;
  int32_t y1, y2;
	uint32_t foffset;
  uint8_t *fontPtr = NULL;
	uint8_t *mallocPtr = NULL;
	uint8_t ql = code>>8;
	uint8_t qh = code&0xFF;	
	
	if(qh < 0xA1 || ql < 0xA1)
		return;
	
	qh -= 0xA1;
	ql -= 0xA1;
	foffset=((uint32_t)94*qh + ql)*32;
	fontPtr = (uint8_t *)pvPortMalloc(32);
	stmflash_read(FLASH_ADDR_GB2312_16x16+foffset, fontPtr, 32);
	
	mallocPtr = fontPtr;
  for (y1 = 0; y1 < 16; y1++)
  {
    y2 = y + y1;
		for (x1 = 0; x1 < 16; x1++)
		{	
			x2 = x + x1;			
			lcd_fb_set_point(x2, y2, mallocPtr[x1 >> 3] & (0x80 >> (x1 & 0x07))? fg : bg);
		}
    mallocPtr += 2;
  }
	
	vPortFree(fontPtr);
}

/*
 * lcd_drv_puts:
 *	Send a string to the display. Obeys \n and \r formatting
 *********************************************************************************
 */
void lcd_fb_puthzs(int32_t x, int32_t y, const char *str, uint8_t bg, uint8_t fg)
{
  uint8_t c1, c2 = *str;
  int32_t mx = x;
  int32_t my = y;
	
  while (c2)
  {
    c1 = c2;
		c2 = *(++str);

    if (c1 == '\r')
    {
      mx = x;
      continue;
    }
		else if (c1 == '\n')
    {
      my += 16;
      continue;
    }
		else if(c1 <= 0x80)
		{
			lcd_fb_putc(mx, my+3, ASCII_10X12, c1, bg, fg);
			mx += 10;	
		}
		else if(c1 > 0xA1 && c2 > 0xA1)
		{
			uint16_t code = (((uint16_t)c2<<8)|c1);
			lcd_fb_puthz(mx, my, code, bg, fg);
			c2 = *(++str);
			mx += 16;	
		}

    if (mx > (LCD_DRV_MAX_X - 16))
    {
      mx = 0;
      my += 16;
    }
  }
}


/*
 * lcd_drv_bmp:
 *	Send a picture to the display. 
 *********************************************************************************
 */
void lcd_fb_bmp(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t *bmp, uint8_t bg, uint8_t fg)
{
  int32_t x1, x2;
  int32_t y1, y2;
  uint8_t line;
  uint8_t *bmpPtr = NULL;

  bmpPtr = (uint8_t *)bmp;
  line = (width >> 3) + ((width&0x07) > 0);
  for (y1 = 0; y1 < height; y1++)
  {
    y2 = y + y1;
		for (x1 = 0; x1 < width; x1++)
		{	
			x2 = x + x1;			
			lcd_fb_set_point(x2, y2, bmpPtr[x1 >> 3] & (0x80 >> (x1 & 0x07))? fg : bg);
		}
    bmpPtr += line;
  }
}


/*
 * lcd_drv_puthz:
 *	Print a single character to the screen
 *********************************************************************************
 */
void lcd_fb_fbmp(int32_t x, int32_t y, bmp_type_e type, uint8_t bg, uint8_t fg)
{
  uint8_t line;
	uint8_t size;
  uint8_t *bmpPtr = NULL;
	bmp_t *disp_bmp = bmp_get(type);

  line = (disp_bmp->width >> 3) + ((disp_bmp->width&0x07) > 0);
	size = disp_bmp->height * line;
	bmpPtr = (uint8_t *)pvPortMalloc(size);
	stmflash_read(disp_bmp->addr, bmpPtr, size);
	
	lcd_fb_bmp(x, y, disp_bmp->width, disp_bmp->height, bmpPtr, bg, fg);
	
	vPortFree(bmpPtr);
}


/*
 * lcd_drv_bmp_speed:
 *	Send a picture to the display. 
 *********************************************************************************
 */
void lcd_fb_bmp_speed(int32_t x0, int32_t y0, int32_t width, int32_t height, uint8_t *bmp, int32_t colour)
{
//  int32_t x = 0, y = 0;
//  uint8_t dat = 0;

//  x0 = ((x0 >= LCD_DRV_MAX_X) ? (LCD_DRV_MAX_X - 1) : ((x0 < 0) ? 0 : x0));
//  y0 = ((y0 >= LCD_DRV_MAX_Y) ? (LCD_DRV_MAX_Y - 1) : ((y0 < 0) ? 0 : y0));

//  height = ((height + y0) >= LCD_DRV_MAX_Y) ? LCD_DRV_MAX_Y - y0 : height;
//  y0 = ((y0 % LCD_DRV_PAGE_ROW == 0) ? (y0 / LCD_DRV_PAGE_ROW) : (y0 / LCD_DRV_PAGE_ROW + 1));
//  width = ((width + x0) >= LCD_DRV_MAX_X) ? LCD_DRV_MAX_X - x0 : width;
//  height = ((height % LCD_DRV_PAGE_ROW == 0) ? (height / LCD_DRV_PAGE_ROW) : (height / LCD_DRV_PAGE_ROW + 1));

//  //lcd_drv_set_mode();
//  #if 1
//  for (y = y0; y < y0 + height; y++)
//  {
//    lcd_drv_set_pos(x0, y);
//    lcd_drv_send_data(0x5C, LCD_SEND_MODE_CMD); // write data to lcd
//    for (x = x0; x < x0 + width; x++)
//    {
//      // if ((x >= LCD_DRV_MAX_X) || (y >= LCD_DRV_PAGE_MAX))
//      // {
//      //   break;
//      // }
//      dat = *bmp++;
//      lcd_drv_send_data(((colour != 0) ? dat : ~dat), LCD_DISP_MODE_DAT);
//    }
//  }
//  #else
//  lcd_drv_set_pos(0, 0);
//  lcd_drv_send_data(0x5C, LCD_SEND_MODE_CMD); // write data to lcd
//  for (y = 0; y < LCD_DRV_PAGE_MAX; y++)
//  {
//    for (x = 0; x < LCD_DRV_MAX_X; x++)
//    {
//      if ((x < x0) || (x >= (width + x0)) || (y < y0) || (y0 >= (height + y0)))
//      {
//        dat = (colour != 0) ? 0x00 : (~0x00);
//      } 
//      else
//      {
//        dat = *bmp++;
//      }
//      lcd_drv_send_data(((colour != 0) ? dat : ~dat), LCD_DISP_MODE_DAT);
//    }
//  }
//  #endif
//  
}
