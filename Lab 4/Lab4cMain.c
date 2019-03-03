/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include "library.h"
#include "graphics.h"

// Function to implement in assembly: Returns (mtop*x + mbtm/2)/mbtm + b
extern int32_t MxPlusB(int32_t x, int32_t mtop, int32_t mbtm, int32_t b) ;

typedef int BOOL ;
#define	FALSE	0
#define	TRUE	1

#define	RCC_BASE	0x40023800
#define	ADC1_BASE	0x40012000

uint32_t *RCC_APB2ENR	= ((uint32_t *) (RCC_BASE + 0x44)) ;
#define	ADC1EN		(1 << 8)

uint32_t *ADC1_SR	= ((uint32_t *) (ADC1_BASE + 0x00)) ;
#define	ADC1EOC		(1 << 1)

uint32_t *ADC1_DR = ((uint32_t *) (ADC1_BASE + 0x4C)) ;

uint32_t *ADC1_SQR1 = ((uint32_t *) (ADC1_BASE + 0x2C)) ;
#define	ADC1_LEN	(1 << 20)

uint32_t *ADC1_SQR3 = ((uint32_t *) (ADC1_BASE + 0x34)) ;
#define	ADC1_IN18	18	// Input channel for temperature sensor
#define	ADC1_IN17	17	// Input channel for internal voltage reference

uint32_t *ADC1_CR2 = ((uint32_t *) (ADC1_BASE + 0x08)) ;
#define	ADON		(1 << 0)
#define	EOCS		(1 << 10)
#define	SWSTART		(1 << 30)

uint32_t *ADC_CCR = ((uint32_t *) (ADC1_BASE + 0x300 + 0x04)) ;
#define	TSVREFE		(1 << 23)

int16_t *ADC_VREFIN_CAL	=  ((int16_t *) 0x1FFF7A2A) ;	// pointer to Raw data acquired at temperature of 30 °C VDDA = 3.3 V
int16_t *ADC_TEMP_3V3_030C	=  ((int16_t *) 0x1FFF7A2C) ;	// pointer to ADC reading for 30 degrees C w/ref voltage = 3.3
int16_t *ADC_TEMP_3V3_110C	=  ((int16_t *) 0x1FFF7A2E) ;	// pointer to ADC reading for 30 degrees C w/ref voltage = 3.3

static void		DelayCycles(uint32_t cycles) ;
static int32_t	ADC_Reading(int32_t channel) ;
static int32_t	PutStringAt(int32_t x, int32_t y, char *fmt, ...) ;
static void		PlotDegreesC(int32_t rawTemp, int32_t scaled025, int32_t plus5) ;
static int32_t *PixelAddress(int32_t x, int32_t y) ;
static void		SanityChecksOK(void) ;

int main(void)
	{
	int32_t curVref, calVref, cal030, cal110, scaled110, scaled030, scaled025, scaled045, y, savey ;

	InitializeHardware(NULL, "Lab 4c: Linear Interpolation") ;
	SanityChecksOK() ;

	curVref = ADC_Reading(ADC1_IN17) ;	// Get current reference voltage reading
	calVref = *ADC_VREFIN_CAL ;			// Get calibrated reference voltage reading

	cal030 = *ADC_TEMP_3V3_030C ;		// Get calibrated temp reading for  30 degrees C
	cal110 = *ADC_TEMP_3V3_110C ;		// Get calibrated temp reading for 110 degrees C

	// Scale calibrated temp readings to current reference voltage
	scaled030 = MxPlusB(cal030, curVref, calVref, 0) ;
	scaled110 = MxPlusB(cal110, curVref, calVref, 0) ;

	// Compute scaled vertical limits of graphical display
	scaled025 = MxPlusB(25 - 30, scaled110 - scaled030, 80, scaled030) ;
	scaled045 = MxPlusB(45 - 30, scaled110 - scaled030, 80, scaled030) ;

	y = 50 ;

	y = PutStringAt(20, y, "REF VOLTAGE A/D MEASUREMENTS:") + 2 ;
	DrawRect(19, y - 3, 201, 39) ;
	SetForeground(COLOR_LIGHTGREEN) ;
	FillRect(20, y - 2, 200, 38) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTGREEN) ;
	y = PutStringAt(20, y, "       Current Vref: %5d", (int) curVref) ;
	y = PutStringAt(20, y, "    Calibrated Vref: %5d", (int) calVref) ;
	y = PutStringAt(20, y, "       Scale Factor: %5.2f", (float) curVref /(float) calVref) ;
	SetBackground(COLOR_WHITE) ;
	y += 4 ;

	y = PutStringAt(20, y, "TEMPERATURE A/D MEASUREMENTS:") + 2 ;
	DrawRect(19, y - 3, 201, 83) ;
	SetForeground(COLOR_LIGHTGREEN) ;
	FillRect(20, y - 2, 200, 82) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_LIGHTGREEN) ;
	y = PutStringAt(20, y, "  Calibrated ( 30C): %5d", (int) cal030) ;
	y = PutStringAt(20, y, "  Calibrated (110C): %5d", (int) cal110) ;
	y += 4 ;
	y = PutStringAt(20, y, "      Scaled ( 30C): %5d", (int) scaled030) ;
	y = PutStringAt(20, y, "      Scaled (110C): %5d", (int) scaled110) ;
	y += 4 ;

	savey = y ;
	while (1)
		{
		int32_t rawTemp, degreesC ;

		y = savey ;
		SetBackground(COLOR_LIGHTGREEN) ;

		// Get raw temp reading from A/D converter
		rawTemp = ADC_Reading(ADC1_IN18) ;
		y = PutStringAt(20, y, "    Raw A/D Reading: %5d", (int) rawTemp) ;

		// Convert to temp in degrees C
		degreesC = MxPlusB(rawTemp - scaled030, 80, scaled110 - scaled030, 30) ;
		y = PutStringAt(20, y, "   Temp (degrees C): %5d", (int) degreesC) ;

		PlotDegreesC(rawTemp, scaled025, scaled045) ;
		}

	return 0 ;
	}

static int32_t ADC_Reading(int32_t channel)
	{
	*RCC_APB2ENR |= ADC1EN ;			// Turn on ADC1 clock
	*ADC_CCR  |= TSVREFE ;				// Select temperature sensor and internal voltage reference as input 

	*ADC1_CR2 |= (ADON | EOCS) ;		// Turn on ADC and configure EndOfConversion signal
	*ADC1_SQR1 = ADC1_LEN ;				// Set # channels in seequence to 1
	*ADC1_SQR3 = channel ;				// Select channel IN18 as only input

	DelayCycles(10000000) ;

	*ADC1_CR2 |= SWSTART ;				// Start the ADC EndOfConversion=1
	DelayCycles(100) ;					// Give it a chance to convert

	while ((*ADC1_SR & ADC1EOC) == 0) ;	// Wait for conversion
	return *ADC1_DR ;					// Read data and clear EndOfConversion
	}

static void DelayCycles(uint32_t cycles)
	{
	uint32_t timeout = GetClockCycleCount() + cycles ;
	while ((int) (timeout - GetClockCycleCount()) > 0) ;
	}

static int32_t	PutStringAt(int32_t x, int32_t y, char *fmt, ...)
	{
	char text[100] ;
	va_list aptr ;

	va_start(aptr, fmt);
	vsprintf(text, fmt, aptr) ;
	va_end(aptr) ;
	DisplayStringAt(x, y, text) ;
	return y + 12 ;
	}

static void PlotDegreesC(int32_t rawTemp, int32_t scaled025, int32_t scaled045)
	{
	const int32_t ymin = 204 ;
	const int32_t ymax = 297 ;
	const int32_t xmin = 20 ;
	const int32_t xmax = 219 ;
	int32_t height, *px, row, ypos, plus5 ;
	int32_t width = xmax - xmin + 1 ;
	static BOOL first = TRUE ;
	static int32_t pxl[100] ;
	static int32_t y ;

	plus5  = MxPlusB(scaled045 - scaled025, ymax - ymin, 4*(scaled045 - scaled025), 0) ;
	height = 4 * plus5 ;
	ypos   = ymax - height + 1 ;

	if (first)
		{
		int32_t degreesC = 25 ;
		char text[100] ;

		SetForeground(COLOR_LIGHTYELLOW) ;
		FillRect(xmin, ypos, width, height) ;
		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		DrawRect(xmin - 1, ypos - 1, width + 1, height) ;
		for (row = 0; row <= height; row += plus5)
			{
			DrawHLine(xmin, ymax - row, width) ;
			sprintf(text, "%d", (int) degreesC) ;
			degreesC += 5 ;
			DisplayStringAt(4, ymax - row - 5, text) ;
			}
		for (row = 0; row < height; row++)
			{
			pxl[row] = *PixelAddress(xmax - 1, ymax - row) ;
			}
		first = FALSE ;
		y = ymax ;
		}

	px = PixelAddress(xmin + 1, ymin) ;
	for (row = ypos; row <= ymax; row++, px += 240)
		{
		memmove(px, px + 1, (xmax - xmin - 1)*sizeof(uint32_t)) ;
		}

	*PixelAddress(xmax, y) = pxl[ymax - y] ;

	if (rawTemp < scaled025) y = ymax ;
	else if (rawTemp >= scaled025 + height) y = ymin ;
	else y = ymax - (rawTemp - scaled025) ;

	*PixelAddress(xmax, y) = COLOR_RED ;
	}

static int32_t *PixelAddress(int32_t x, int32_t y)
	{
	return ((int32_t *) 0xD0000000) + x + 240*y ;
	}

static void SanityChecksOK(void)
	{
#	define	COUNT(a)	(sizeof(a)/sizeof(a[0]))
	typedef struct {int	x, mtop, mbtm, b, correct, result; } CHECK ;
	static CHECK check[] =
		{
		{0,	 0,	 1,	 1,	 1},	{2,	 3,	 1,	 0,	 6},	{2,	-3,	 1,	 0,	-6},	{2,	 3,	-1,	0, -6},
		{1,	 5,	 3,	 0,	 2},	{1,	-5,	 3,	 0,	-2},	{1,	 5,	-3,	 0,	-2},	{1,	-5,	-3,	0,  2},
		{1,	 4,	 3,	 0,	 1},	{1,	-4,	 3,	 0,	-1},	{1,	 4,	-3,	 0,	-1},	{1,	-4,	-3,	0,  1}
		} ;
	int which, errors ;
	CHECK *p ;

	errors = 0 ;
	p = check ;
	for (which = 0; which < COUNT(check); which++, p++)
		{
		p->result = MxPlusB(p->x, p->mtop, p->mbtm, p->b) ;
		if (p->result != p->correct) errors++ ;
		}
	if (errors == 0) return ;

	printf("\n       SANITY CHECK ERRORS:\n\n") ;
	printf("       x mtop mbtm b  result\n") ;
	printf("       - ---- ---- -  ------\n") ;

	p = check ;
	for (which = 0; which < COUNT(check); which++, p++)
		{
		if (p->result != p->correct) printf("       %2d  %+d   %+d  %d  %+4d\n", p->x, p->mtop, p->mbtm, p->b, p->result) ;
		}
	exit(0) ;
	}


