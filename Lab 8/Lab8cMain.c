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
#include "library.h"
#include "graphics.h"

typedef int32_t	Q16 ;

// Functions implemented in assembly
extern Q16 Discriminant(Q16 a, Q16 b, Q16 c) ;
extern Q16 Root1(Q16 a, Q16 b, Q16 c) ;
extern Q16 Root2(Q16 a, Q16 b, Q16 c) ;
extern Q16 Quadratic(Q16 x, Q16 a, Q16 b, Q16 c) ;

// Public Functions (may be called from assembly)
Q16 Q16Product(Q16 multiplier, Q16 multiplicand) ;
Q16	Q16Quotient(Q16 dividend, Q16 divisor) ;
Q16	Q16SquareRoot(Q16 x) ;

// Private functions (may NOT be called from assembly)
static int	Close(Q16 a, Q16 b) ;
static void	DrawXAxis(void) ;
static void	DrawYAxis(void) ;
static Q16	GuessSqrt(Q16 x, Q16 guess) ;
static void	PlotQuadratic(Q16 a, Q16 b, Q16 c) ;
static void	PlotRoot(Q16 x) ;
static int	PutStatus(int row, int col, int ok) ;
static int	PutStringAt(int row, int col, char *fmt, ...) ;
static Q16	UQ16Quotient(Q16 dividend, Q16 divisor) ;

#define	ENTRIES(a)	(sizeof(a)/sizeof(a[0]))

#define	GFXCOL1		0
#define	GFXCOLN		(XPIXELS-1)
#define	GFXCOLS		(GFXCOLN - GFXCOL1 + 1)

#define	GFXROW1		45
#define	GFXROWN		305
#define	GFXROWS		(GFXROWN - GFXROW1 + 1)

#define	AXISROW		(GFXROW1 + GFXROWS/2)
#define	AXISCOL		(GFXCOL1 + GFXCOLS/2)

#define	XMIN		-30.0
#define	XMAX		+30.0
#define	YMIN		-130.0
#define	YMAX		+130.0

#define	TXT_HEIGHT	15
#define	TXT_WIDTH	7

#define	PARAM_ROW1	(GFXROW1 + 10)
#define	VALUE_ROW1	(GFXROWN - 3*TXT_HEIGHT)

#define	LBL_COL		10
#define	STS_COL		(LBL_COL + 25*TXT_WIDTH)

#define	TICKRATE	20
#define	TICKSIZE	4

#define	PLOT_RAD	1
#define	ROOT_RAD	4

#define	COLOR_PLOT	COLOR_BLUE
#define	COLOR_ROOT	COLOR_RED
#define	COLOR_AXIS	COLOR_BLACK

#define Q16ONE			0x00010000
#define	FloatToQ16(x)	((Q16) ((x) * ((float) Q16ONE)))
#define Q16ToFloat(x)	(((float) (x)) / ((float) Q16ONE))

Q16 Q16Product(Q16 multiplier, Q16 multiplicand)
	{
	int64_t product = ((int64_t) multiplier) * ((int64_t) multiplicand) ;
	return (Q16) (product >> 16) ;
	}

Q16 Q16Quotient(Q16 dividend, Q16 divisor)
	{
	Q16 quotient ;
	int negate = 0 ;

	if ((dividend ^ divisor) < 0) negate = 1 ;

	if (dividend < 0) dividend = -dividend ;
	if (divisor  < 0) divisor  = -divisor  ;

	quotient = UQ16Quotient(dividend, divisor) ;

	return negate ? -quotient : quotient ;
	}

Q16 Q16SquareRoot(Q16 x)
	{
	return GuessSqrt(x, Q16ONE) ;
	}

int main(void)
	{
	typedef struct
		{
		float	a, b, c ;
		float	d ;
		float	r1, r2 ;
		} TESTCASE ;
	static TESTCASE testcase[] =
		{
		{+1.0,   0.0, -100.0, +400.0, +10.0, -10.0},	// 2 roots
		{-1.0, +30.0, -225.0,    0.0, +15.0, +15.0},	// 1 root
		{+1.0, +30.0, +240.0,  -60.0,   0.0,   0.0}		// Imaginary root
		} ;
	Q16 my_d, my_r1, my_r2 ;
	Q16 a, b, c, d, r1, r2 ;
	int k, row ;

	InitializeHardware(HEADER, "Lab 8c: Q16 Fixed Point Quadratics") ;

	k = 0 ;
	while (1)
		{
		SetForeground(COLOR_WHITE) ;
		FillRect(0, 48, XPIXELS, YPIXELS - 64) ;
		SetForeground(COLOR_BLACK) ;

		a  = FloatToQ16(testcase[k].a) ;
		b  = FloatToQ16(testcase[k].b) ;
		c  = FloatToQ16(testcase[k].c) ;
		d  = FloatToQ16(testcase[k].d) ;
		r1 = FloatToQ16(testcase[k].r1) ;
		r2 = FloatToQ16(testcase[k].r2) ;

		row = PARAM_ROW1 ;
		row = PutStringAt(row, LBL_COL, "Coefficient A:%10.2f (x^2)", Q16ToFloat(a)) ;
		row = PutStringAt(row, LBL_COL, "Coefficient B:%10.2f (x^1)", Q16ToFloat(b)) ;
		row = PutStringAt(row, LBL_COL, "Coefficient C:%10.2f (x^0)", Q16ToFloat(c)) ;

		row = VALUE_ROW1 ;
		my_d = Discriminant(a, b, c) ;
		PutStringAt(row, LBL_COL, " Discriminant:%10.2f", Q16ToFloat(my_d)) ;
		row = PutStatus(row, STS_COL, Close(my_d, d)) ;

		if (my_d < 0)
			{
			PutStringAt(row, LBL_COL, "        Roots:%10s", "Imag") ;
			row = PutStatus(row, STS_COL, 1) ;
			}
		else if (my_d > 0)
			{
			my_r1 = Root1(a, b, c) ;
			PutStringAt(row, LBL_COL, "   First root:%10.2f", Q16ToFloat(my_r1)) ;
			row = PutStatus(row, STS_COL, Close(my_r1, r1)) ;

			my_r2 = Root2(a, b, c) ;
			PutStringAt(row, LBL_COL, "  Second root:%10.2f", Q16ToFloat(my_r2)) ;
			row = PutStatus(row, STS_COL, Close(my_r2, r2)) ;

			PlotRoot(my_r1) ;
			PlotRoot(my_r2) ;
			}
		else // if (my_d == 0)
			{
			my_r1 = Root1(a, b, c) ;
			PutStringAt(row, LBL_COL, "  Single root:%10.2f", Q16ToFloat(my_r1)) ;
			row = PutStatus(row, STS_COL, Close(my_r1, r1)) ;

			PlotRoot(my_r1) ;
			}

		PlotQuadratic(a, b, c) ;
		WaitForPushButton() ;
		k = (k + 1) % ENTRIES(testcase) ;
		}

	return 0 ;
	}

static void PlotQuadratic(Q16 a, Q16 b, Q16 c)
	{
	int oldRow = 0 ;

	DrawXAxis() ;
	DrawYAxis() ;

	SetColor(COLOR_PLOT) ;
	for (int col = 0; col < GFXCOLS; col++)
		{
		Q16 x, y ;
		int row ;

		x = FloatToQ16(XMIN + col / 4.0) ;
		y = Quadratic(x, a, b, c) ;

		row = (int) (0.5 + GFXROWN + (YMIN - Q16ToFloat(y))) ;
		do
			{
			if (!oldRow) oldRow = row ;
			if (GFXROW1 <= oldRow && oldRow <= GFXROWN) FillCircle(col, oldRow, PLOT_RAD) ;
			if (oldRow < row) oldRow++ ;
			else if (oldRow > row) oldRow-- ;
			} while (row != oldRow) ;
		}
	}

static void PlotRoot(Q16 x)
	{
	int col ;

	col = (int) (0.5 + 4.0*(Q16ToFloat(x) - XMIN)) ;

	if (col < ROOT_RAD || col > GFXCOLS - ROOT_RAD) return ;

	SetColor(COLOR_ROOT) ;
	FillCircle(col, AXISROW, ROOT_RAD) ;
	}

static void DrawXAxis(void)
	{
	int col, rowMin, rowMax ;

	SetColor(COLOR_AXIS) ;
	rowMin = AXISROW - TICKSIZE/2 ;
	rowMax = rowMin + TICKSIZE ;
	DrawLine(GFXCOL1, AXISROW, GFXCOLN, AXISROW) ;
	for (col = TICKRATE; col < GFXCOLS/2; col += TICKRATE)
		{
		DrawLine(AXISCOL + col, rowMin, AXISCOL + col, rowMax) ;
		DrawLine(AXISCOL - col, rowMin, AXISCOL - col, rowMax) ;
		}

	DisplayStringAt(AXISCOL - 4*TICKRATE - 12, AXISROW + 8, "-20") ;
	DisplayStringAt(AXISCOL - 2*TICKRATE - 12, AXISROW + 8, "-10") ;
	DisplayStringAt(AXISCOL + 2*TICKRATE - 12, AXISROW + 8, "+10") ;
	DisplayStringAt(AXISCOL + 4*TICKRATE - 12, AXISROW + 8, "+20") ;
	}

static void DrawYAxis(void)
	{
	int row, colMin, colMax ;

	SetColor(COLOR_AXIS) ;
	colMin = AXISCOL - TICKSIZE/2 ;
	colMax = colMin + TICKSIZE ;
	DrawLine(AXISCOL, GFXROW1, AXISCOL, GFXROWN) ;
	for (row = TICKRATE; row < GFXROWS/2; row += TICKRATE)
		{
		DrawLine(colMin, AXISROW + row, colMax, AXISROW + row) ;
		DrawLine(colMin, AXISROW - row, colMax, AXISROW - row) ;
		}

	DisplayStringAt(AXISCOL + 6, AXISROW - 2*TICKRATE - 5, "+40") ;
	DisplayStringAt(AXISCOL + 6, AXISROW + 2*TICKRATE - 5, "-40") ;
	}

static int PutStatus(int row, int col, int ok)
	{
	if (!ok)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		}

	PutStringAt(row, col, "%s", ok ? "CORRECT" : " WRONG ") ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;

	return row + TXT_HEIGHT ;
	}

static int PutStringAt(int row, int col, char *fmt, ...)
	{
	va_list args ;
	char text[100] ;

	va_start(args, fmt) ;
	vsprintf(text, fmt, args) ;
	va_end(args) ;

	DisplayStringAt(col, row, text) ;

	return row + TXT_HEIGHT ;
	}

static Q16 GuessSqrt(Q16 x, Q16 guess)
	{
	Q16 xoverg = UQ16Quotient(x, guess) ;
	if (Close(xoverg, guess)) return guess ;
	else return GuessSqrt(x, (guess + xoverg) >> 1) ;
	}

static Q16 UQ16Quotient(Q16 dividend, Q16 divisor)
	{
	uint64_t quotient = (((uint64_t) dividend) << 16) / divisor ;
	return (Q16) quotient ;
	}

static int Close(Q16 a, Q16 b)
	{
	Q16 largest = abs((abs(a) > abs(b)) ? a : b) ;
	Q16 threshold = Q16Product(largest, FloatToQ16(0.001)) ;
	return abs(a - b) <= threshold ;
	}


