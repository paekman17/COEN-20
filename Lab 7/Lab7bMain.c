/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "library.h"
#include "graphics.h"
#include "touch.h"

extern uint32_t	Times45(uint32_t mcnd) ;
extern uint32_t	Times55(uint32_t mcnd) ;
extern uint32_t	Times106(uint32_t mcnd) ;
extern uint32_t	Times43691(uint32_t mcnd) ;

typedef int BOOL ;
#define	FALSE	0
#define	TRUE	1

typedef struct
	{
	const uint8_t *table ;
	const uint16_t Width ;
	const uint16_t Height ;
	} sFONT;

extern sFONT Font8, Font12, Font16, Font20, Font24 ;

static void		ClockCycles(unsigned *xpos, unsigned *ypos, char *label, unsigned cycles) ;
static uint32_t	PutStringAt(uint32_t xpos, uint32_t ypos, char *fmt, ...) ;
static uint32_t RandomMultiplicand(void) ;
static void		ResultsBox(uint32_t xpos, uint32_t ypos, char *label) ;
static void		SetFontSize(sFONT *pFont) ;
static unsigned	TestTimes45(unsigned *xpos, unsigned *ypos) ;
static unsigned	TestTimes55(unsigned *xpos, unsigned *ypos) ;
static unsigned	TestTimes106(unsigned *xpos, unsigned *ypos) ;
static unsigned	TestTimes43691(unsigned *xpos, unsigned *ypos) ;

static sFONT *font ;

int main(void)
	{
	unsigned xpos, ypos, cycles[4], results[2], ovhd ;

	InitializeHardware(NULL, "Lab7b: Multiply by Constant") ;

	ovhd = CountCycles(CallReturnOverhead, results, results, results) ;

	SetFontSize(&Font20) ;
	xpos = font->Width / 2 ;
	ypos = 60 ;

	ResultsBox(xpos, ypos, "Product Checks:") ;
	ypos += font->Height ;
	cycles[0] = TestTimes45(&xpos, &ypos) - ovhd ;
	cycles[1] = TestTimes55(&xpos, &ypos) - ovhd ;
	cycles[2] = TestTimes106(&xpos, &ypos) - ovhd ;
	cycles[3] = TestTimes43691(&xpos, &ypos) - ovhd ;
	ypos += font->Height ;

	ResultsBox(xpos, ypos, "Clock Cycles:") ;
	ypos += font->Height ;
	ClockCycles(&xpos, &ypos, "Times45:", cycles[0]) ;
	ClockCycles(&xpos, &ypos, "Times55:", cycles[1]) ;
	ClockCycles(&xpos, &ypos, "Times106:", cycles[2]) ;
	ClockCycles(&xpos, &ypos, "Times43691:", cycles[3]) ;

	return 0 ;
	}

static void ResultsBox(uint32_t xpos, uint32_t ypos, char *label)
	{
	uint32_t height, width ;

	SetBackground(COLOR_WHITE) ;
	PutStringAt(xpos, ypos, label) ;
	ypos += font->Height ;

	width = 240 - font->Width ;
	height = 4*font->Height ;

	SetForeground(COLOR_LIGHTYELLOW) ;
	FillRect(xpos, ypos, width, height) ;
	SetForeground(COLOR_RED) ;
	DrawRect(xpos - 1, ypos - 1, width + 1, height + 1) ;
	}

static void ClockCycles(unsigned *xpos, unsigned *ypos, char *label, unsigned cycles)
	{
	uint32_t xlen ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTYELLOW) ;
	xlen = PutStringAt(*xpos, *ypos, "%-11s ", label) ;
	PutStringAt(*xpos + xlen, *ypos, "%4u", cycles) ;
	*ypos += font->Height ;
	}

static unsigned TestTimes45(unsigned *xpos, unsigned *ypos)
	{
	unsigned prod[2], mcnd, diff, xlen ;
	const unsigned mpier = 45 ;

	mcnd = RandomMultiplicand() ;
	diff = CountCycles(Times45, &mcnd, &mcnd, prod) ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTYELLOW) ;
	xlen = PutStringAt(*xpos, *ypos, "%5dx%2u:", mpier, mcnd) ;
	if (prod[0] != mpier*mcnd)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground( COLOR_RED) ;
		}
	PutStringAt(*xpos + xlen, *ypos, "%7u", prod[0]) ;
	*ypos += font->Height ;
	return diff ;
	}

static unsigned TestTimes55(unsigned *xpos, unsigned *ypos)
	{
	unsigned prod[2], mcnd, diff, xlen ;
	const unsigned mpier = 55 ;

	mcnd = RandomMultiplicand() ;
	diff = CountCycles(Times55, &mcnd, &mcnd, prod) ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTYELLOW) ;
	xlen = PutStringAt(*xpos, *ypos, "%5dx%2u:", mpier, mcnd) ;
	if (prod[0] != mpier*mcnd)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground( COLOR_RED) ;
		}
	PutStringAt(*xpos + xlen, *ypos, "%7u", prod[0]) ;
	*ypos += font->Height ;
	return diff ;
	}

static unsigned TestTimes106(unsigned *xpos, unsigned *ypos)
	{
	unsigned prod[2], mcnd, diff, xlen ;
	const unsigned mpier = 106 ;

	mcnd = RandomMultiplicand() ;
	diff = CountCycles(Times106, &mcnd, &mcnd, prod) ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTYELLOW) ;
	xlen = PutStringAt(*xpos, *ypos, "%5dx%2u:", mpier, mcnd) ;
	if (prod[0] != mpier*mcnd)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground( COLOR_RED) ;
		}
	PutStringAt(*xpos + xlen, *ypos, "%7u", prod[0]) ;
	*ypos += font->Height ;
	return diff ;
	}

static unsigned TestTimes43691(unsigned *xpos, unsigned *ypos)
	{
	unsigned prod[2], mcnd, diff, xlen ;
	const unsigned mpier = 43691 ;

	mcnd = RandomMultiplicand() ;
	diff = CountCycles(Times43691, &mcnd, &mcnd, prod) ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTYELLOW) ;
	xlen = PutStringAt(*xpos, *ypos, "%5dx%2u:", mpier, mcnd) ;
	if (prod[0] != mpier*mcnd)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground( COLOR_RED) ;
		}
	PutStringAt(*xpos + xlen, *ypos, "%7u", prod[0]) ;
	*ypos += font->Height ;
	return diff ;
	}

static uint32_t RandomMultiplicand(void)
	{
	uint32_t mcnd ;

	do
		{
		mcnd = 3 + GetRandomNumber() % 100 ;
		} while (mcnd >= 100) ;

	return mcnd ;
	}

static void SetFontSize(sFONT *pFont)
	{
	extern void BSP_LCD_SetFont(sFONT *) ;
	BSP_LCD_SetFont(font = pFont) ;
	}

static uint32_t PutStringAt(uint32_t xpos, uint32_t ypos, char *fmt, ...)
	{
	char text[100] ;
	va_list ap ;

	va_start(ap, fmt) ;
	vsprintf(text, fmt, ap) ;
	va_end(ap) ;

	DisplayStringAt(xpos, ypos, text) ;
	return strlen(text) * font->Width ;
	}

