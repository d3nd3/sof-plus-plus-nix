#pragma once

/**************************************************************
 * palette info
 **************************************************************/

class paletteRGBA_c
{
public:
	paletteRGBA_t	color;

	paletteRGBA_c(void) { color.c = 0xffffffff; }
	paletteRGBA_c(byte r, byte g, byte b) { color.r = r; color.g = g; color.b = b; color.a = 0xff; }
	paletteRGBA_c(byte r, byte g, byte b, byte a) { color.r = r; color.g = g; color.b = b; color.a = a; }
	paletteRGBA_c(unsigned c) { color.c = c; }
	paletteRGBA_c(byte *c) { color.r = c[0]; color.g = c[1]; color.b = c[2]; color.a = c[3]; }
	paletteRGBA_c(paletteRGBA_t c) { color = c; }
	paletteRGBA_c(char *s) { color.c = strtoul(s, NULL, 0); }
	paletteRGBA_c(const char *s) { color.c = strtoul(s, NULL, 0); }
	paletteRGBA_c(float *shade)
	{
		color.r = 255;//Q_ftol(shade[0] * 255.0); 
		color.g = 255;//Q_ftol(shade[1] * 255.0); 
		color.b = 255;//Q_ftol(shade[2] * 255.0); 
		color.a = 255; 
	}
	paletteRGBA_c brighten()
	{
		paletteRGBA_c	col;

		col.color.r = ClampI(color.r * 1.30, 0, 255);
		col.color.g = ClampI(color.g * 1.30, 0, 255);
		col.color.b = ClampI(color.b * 1.30, 0, 255);
		col.color.a = color.a;
		return(col);
	}
	paletteRGBA_c darken()
	{
		paletteRGBA_c	col;

		col.color.r = ClampI(color.r * 0.70, 0, 255);
		col.color.g = ClampI(color.g * 0.70, 0, 255);
		col.color.b = ClampI(color.b * 0.70, 0, 255);
		col.color.a = color.a;
		return(col);
	}
};

#define P_DONT_USE0	'\000'		/* DONT USE !! */

#define P_WHITE		'\001'
#define P_RED  		'\002'
#define P_GREEN		'\003'
#define P_YELLOW	'\004'
#define P_BLUE		'\005'
#define P_PURPLE	'\006'
#define P_CYAN		'\007'

#define P_BLACK		'\010'
#define P_HWHITE  	'\011'
#define P_DONT_USE1	'\012'		/* DONT USE !! */
#define P_HRED    	'\013'
#define P_HGREEN  	'\014'
#define P_DONT_USE2	'\015'		/* DONT USE !! */
#define P_HYELLOW	'\016'
#define P_HBLUE		'\017'

#define P_CAMOBROWN	'\020'
#define P_CAMOGREEN	'\021'
#define P_SEAGREEN 	'\022'
#define P_SEABLUE  	'\023'
#define P_METAL    	'\024'
#define P_DBLUE    	'\025'
#define P_DPURPLE  	'\026'
#define P_DGREY    	'\027'
           
#define P_PINK	  	'\030'
#define P_BLOODRED 	'\031'
#define P_RUSSET   	'\032'
#define P_BROWN    	'\033'
#define P_TEXT     	'\034'
#define P_BAIGE    	'\035'
#define P_LBROWN   	'\036'
#define P_ORANGE   	'\037'

// end