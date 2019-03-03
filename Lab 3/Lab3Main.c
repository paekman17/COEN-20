/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "library.h"
#include "graphics.h"

extern void		UseLDRB(void *dst, void *src) ;
extern void		UseLDRH(void *dst, void *src) ;
extern void		UseLDR(void *dst, void *src) ;
extern void		UseLDRD(void *dst, void *src) ;
extern void		UseLDMIA(void *dst, void *src) ;

static int		Check(uint8_t *src, uint8_t *dst) ;
static void		Setup(uint8_t *src, uint8_t *dst) ;
static void		ShowResult(int which, int index[], unsigned cycles[], unsigned maxCycles) ;

#define	VERSIONS			5
#define	BAR_OFFSET			70
#define	BAR_WIDTH			30
#define	MAX_HEIGHT			210

#define ENTRIES(a) (sizeof(a)/sizeof(a[0]))

uint8_t src[512] __attribute__ ((aligned (4))) ; // Place on a word boundary
uint8_t dst[512] __attribute__ ((aligned (4))) ; // Place on a word boundary
uint32_t overhead ;

int main(void)
	{
	static void (*use[])(void *dst, void *src) = {UseLDRB, UseLDRH, UseLDR, UseLDRD, UseLDMIA} ;
	unsigned cycles[VERSIONS], maxCycles, ovhd, dummy[2] ;
	int which, index[VERSIONS], srcErr, dstErr ;
	uint8_t *iparams[] = {dst, src} ;

	InitializeHardware(HEADER, "Lab 3: Copying Data Quickly") ;

	srcErr = (((unsigned) src) & 0x3) != 0 ;
	dstErr = (((unsigned) src) & 0x3) != 0 ;
	if (srcErr) printf("src NOT word-aligned (%08X)\n", (unsigned) src) ;
	if (dstErr) printf("dst NOT word-aligned (%08X)\n", (unsigned) dst) ;
	if (srcErr || dstErr) while (1) ;

	ovhd = CountCycles(CallReturnOverhead, dummy, dummy, dummy) ;
	for (which = 0; which < ENTRIES(use); which++)
		{
		Setup(src, dst) ;
		cycles[which] = CountCycles(use[which], iparams, dummy, dummy) - ovhd ;
		index[which]  = Check(src, dst) ;
		}

	maxCycles = 0 ;
	for (which = 0; which < VERSIONS; which++)
		{
		if (cycles[which] > maxCycles) maxCycles = cycles[which] ;
		}

	for (which = 0; which < VERSIONS; which++)
		{
		ShowResult(which, index, cycles, maxCycles) ;
		}

	SetColor(COLOR_BLACK) ;
	DrawHLine(0, BAR_OFFSET + MAX_HEIGHT, XPIXELS) ;

	return 0 ;
	}

static void Setup(uint8_t *src, uint8_t *dst)
	{
	int k ;

	for (k = 0; k < 512; k++)
		{
		*src++ = rand() ;
		*dst++ = rand() ;
		}
	}

static int Check(uint8_t *src, uint8_t *dst)
	{
	int k ;

	for (k = 0; k < 512; k++)
		{
		if (*src++ != *dst++) return k ;
		}
	return -1 ;
	}

static void ShowResult(int which, int index[], unsigned cycles[], unsigned maxCycles)
	{
	static char *opcode[] = {"LDRB", "LDRH", "LDR", "LDRD", "LDMIA"} ;
	float percent = ((float) cycles[which]) / maxCycles ;
	char text[100] ;
	int x, y ;

	x = (XPIXELS / VERSIONS)*which + XPIXELS / (2*VERSIONS) - BAR_WIDTH / 2 ;
	y = MAX_HEIGHT*(1 - percent) + BAR_OFFSET ;

	SetColor(index[which] >= 0 ? COLOR_RED : COLOR_GREEN) ;
	FillRect(x, y, BAR_WIDTH, (unsigned) (percent*MAX_HEIGHT)) ;
	SetColor(COLOR_BLACK) ;
	DrawRect(x, y, BAR_WIDTH - 1, (unsigned) (percent*MAX_HEIGHT)) ;

	sprintf(text, "%4u", cycles[which]) ;
	DisplayStringAt(x, y - 12, text) ;
	DisplayStringAt(x, BAR_OFFSET + MAX_HEIGHT + 5, opcode[which]) ;

	if (index[which] < 0) return ;
	sprintf(text, "Use%s failed at Index %d!", opcode[which], index[which]) ;
	SetColor(COLOR_BLACK) ;
	DisplayStringAt(15, YPIXELS/3 + 15*which, text) ;
	}


