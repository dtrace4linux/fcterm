/**********************************************************************/
/*                                                                    */
/*  File:          ctw.c                                              */
/*  Author:        P. D. Fox                                          */
/*  Created:       25 Nov 1991                     		      */
/*                                                                    */
/*  Copyright (c) 1990-2011 Paul Fox                                  */
/*                All Rights Reserved.                                */
/*                                                                    */
/*   $Header: Last edited: 20-Oct-2011 1.64 $ 			      */
/*--------------------------------------------------------------------*/
/*  Description:  Color terminal widget.                              */
/*                                                                    */
/*   Sequences supported:					      */
/*   								      */
/*   ESC ESC x  Print character x literally.    		      */
/*   ESC 7	Save cursor position    			      */
/*   ESC 8	Restore cursor position 			      */
/*   ESC @0	Not in curses.					      */
/*   ESC @1     In curses emulation mode.       		      */
/*   ESC @2     Receive ioctl.					      */
/*   ESC @3     Reply to ioctl. 				      */
/*   ESC c	Reset terminal					      */
/*   ESC D	Index (down cursor or scroll).   	      	      */
/*   ESC H	Set tab stop at current position.	      	      */
/*   ESC M	Reverse index (up cursor or insline).   	      */
/*   ESC Z	Identify terminal.      			      */
/*   ESC >	Numeric keypad					      */
/*   ESC =	Application keypad      			      */
/*   ^H		Backspace					      */
/*   ^I		Tabs (fixed-width -- 8 chars wide).		      */
/*   ^J		Line feed					      */
/*   ^L		Clear screen.					      */
/*   ^M 	Carriage return 				      */
/*   ESC(0   ESC)0   ESC*0   ESC+0  Set character sets.      	      */
/*              0  Normal line drawing font set              	      */
/*              1  Alternate fonts                            	      */
/*              2  Alternate fonts                            	      */
/*              A  American ASCII                             	      */
/*              B  British ASCII                              	      */
/*   ESC [n @	Insert characters.      			      */
/*   ESC [n A	Move cursor up					      */
/*   ESC [n B	Move cursor down				      */
/*   ESC [n C	Move cursor right       			      */
/*   ESC [n D	Move cursor left				      */
/*   ESC [n d	Goto (x, n)             			      */
/*   ESC [n E	Goto (0,y+n)                   			      */
/*   ESC [n F	Goto (0,y-n)                   			      */
/*   ESC [n G	Goto specified column.  			      */
/*   ESC [n;m H Move cursor to absolute position.		      */
/*   ESC [n J   Erase screen 0==clear below; 1==clear above; 2==clear all*/
/*   ESC [n L	Insert lines at current row     		      */
/*   ESC [n M	Delete lines at current row     		      */
/*   ESC [n P	Delete characters.              		      */
/*   ESC [S	Index (down cursor or scroll).   	      	      */
/*   ESC [T	Reverse index (up cursor or insline).   	      */
/*   ESC [n X	Erase to blank (no cursor move) 		      */
/*   ESC [n Y	Set mouse button mask.          		      */
/****ESC [n Z	Request mouse position.         		      */
/*   ESC [Z  	Backtab                         		      */
/*   								      */
/*   ESC [ c	Ask for answer back message.            	      */
/*   ESC [n;m f Move cursor to absolute position.		      */
/*   ESC [n g	Print ASCII char 'n' (SCO mode only)    	      */
/*   ESC [3 g	Clear TAB stops.                       	              */
/*   ESC [4 h	Enter insert mode                      	              */
/*   ESC [4 l	Exit insert mode                      	              */
/*   ESC [0 m	Reset attributes.                      	              */
/*   ESC [1 m	Bold                                   	              */
/*   ESC [4 m	Underline                              	              */
/*   ESC [5 m	Blink                                  	              */
/*   ESC [7 m	Reverse video.                         	              */
/*   ESC [8 m	Store current colors as the default.		      */
/*   ESC [10 m  Select primary font, don't display ctrl chars, do't set bit 8 */
/*   ESC [11 m  Select first alternate font; lets chars < 32 be displayed */
/*   ESC [21m	Reset bold.                            	              */
/*   ESC [24m	Reset underline.                       	              */
/*   ESC [25m	Reset blink.                           	              */
/*   ESC [27m	Reset reverse video.                   	              */
/*   ESC [3Xm	Set foreground color.                  	      	      */
/*   ESC [38 ; 5 ; XX m	Set foreground from 256 palette.              */
/*   ESC [39m	Restore foreground default color.      	      	      */
/*   ESC [4Xm	Set background color.                  	      	      */
/*   ESC [48 ; 5 ; XX m	Set background from 256 palette.              */
/*   ESC [49m	Restore background default color.      	      	      */
/*   ESC [7Xm	Set cursor color.                      	      	      */
/*   ESC [78m	Restore cursor color.                  	      	      */
/*   ESC [1900 m  Stop literal mode.                     	      */
/*   ESC [1901 m  Go into literal mode.                  	      */
/*   ESC [1902 m  Go into strikethru mode.               	      */
/*   ESC [1903 m  Exit strikethru mode.                   	      */
/*   ESC [1904 m  Enter autoswitch mode.                  	      */
/*   ESC [1905 m  Exit autoswitch mode.                   	      */
/*   ESC [1920 ; x ; y ; w ; h ; m Draw a rectangle.		      */
/*   ESC [1921 ; x ; y ; w ; h ; m Fill a rectangle.		      */
/*   ESC [1922 ; x1 ; y1; x2 ; y2; m Draw a line		      */
/*   ESC [1923 ; x ; y ; m Draw a pixel.			      */
/*   ESC [1924 m Clear graphics chain.				      */
/*   ESC [1925 m Clear graphics chain and refresh.		      */
/*   ESC [1926 ; x; y; w; h; name m Set name			      */
/*   ESC [1927 ; x; y; str; m Draw string			      */
/*   ESC [1928 ; x; y; w; h; arc1; arc2; m Draw arc     	      */
/*   ESC [1929 ; x; y; w; h; arc1; arc2; m Fill arc     	      */
/*   ESC [1930 ; rrggbb; m Set background color			      */
/*   ESC [1931 ; rrggbb; m Set foreground color			      */
/*   ESC [1932 ; fontname; m Set font for drawing.		      */
/*   ESC [1933 ; x; y; str; m Draw image string			      */
/*   ESC [1934 m    Query winsize in pixels (WxH<Enter>).             */
/*   ESC [1935 m    Dump status to /tmp/$USER/fcterm.$CTW_PID	      */
/*   ESC [1936 m    Box each character drawn.                         */
/*   ESC [1937 m    Unox each character drawn.                        */
/*   ESC [n;m r	Set scrolling region to lines n..m.     	      */
/*   ESC [n;m r	Set scrolling region.			      	      */
/*   ESC [s	Saved cursor position.                  	      */
/*   ESC [n t	Shelltool/cmdtool compatable escape sequence          */
/*   		ESC [1t	Open window.				      */
/*   		ESC [2t	Iconise window.				      */
/*   		ESC [3t	Move window.				      */
/*   		ESC [4t	Change size (in pixels) window.		      */
/*   		ESC [5t	Raise window.				      */
/*   		ESC [6t	Lower window.				      */
/*   		ESC [8t	Change size (in chars) window.		      */
/*   ESC [u	Restore cursor position.                	      */
/*   								      */
/*   ESC [ ? 1 h	Application cursor keys			      */
/*   ESC [ ? 3 h	80/132 column mode switch.		      */
/*   ESC [ ? 4 h	Set smooth scroll mode.			      */
/*   ESC [ ? 5 h	Reverse video mode.    			      */
/*   ESC [ ? 7 h	Set autowrap           			      */
/*   ESC [ ? 8 h	Enable autorepeat (no-op) Use XAutoRepeatOn)  */
/*   ESC [ ? 9 h	Turn on application mouse.             	      */
/*   ESC [ ? 12 h	Move this console to the front.        	      */
/*   ESC [ ? 25 h	Turn on cursor.                        	      */
/*   ESC [ ? 47 h	Use alternate screen buffer.           	      */
/*   ESC [ ? 1000 h	Enter keypad mode.                            */
/*   ESC [ ? 1034 h	SMM mode                                      */
/*   ESC [ ? 1049 h	Use alternate screen buffer and screen clear. */
/*   								      */
/*   ESC [ ? 1 l	Normal cursor keys			      */
/*   ESC [ ? 3 l	80/132 column mode switch.		      */
/*   ESC [ ? 4 l	Set jump scroll mode.			      */
/*   ESC [ ? 5 l	Normal video mode.     			      */
/*   ESC [ ? 7 l	Disable autowrap       			      */
/*   ESC [ ? 8 l	Disable autorepeat (no-op) Use XAutoRepeatOn) */
/*   ESC [ ? 9 l        Turn off application mouse.            	      */
/*   ESC [ ? 25 l	Turn off cursor.                       	      */
/*   ESC [ ? 47 l	Use normal screen buffer.              	      */
/*   ESC [ ? 1000 l	Exit  keypad mode.                            */
/*   ESC [ ? 1049 l	Use alternate screen buffer/save cursor/scr clear */
/*   								      */
/*   ESC [ 6 n          Ask terminal for rows,col - ESC[r;cR          */
/*   ESC [ ? 3 r	Disable window resize.  		      */
/*   ESC [ ? 4 r	Enable window resize.			      */
/*   								      */
/*   ESC [ ? r;c;h;w;n S Scroll rectangular region up or down n lines */
/*   								      */
/*   ESC ] 0;<window/icon-title><BEL>				      */
/*   ESC ] 1;<icon name><BEL>				      	      */
/*   ESC ] 2;<window name><BEL>				      	      */
/*   ESC ] 46;<log-file><BEL>    Change log file name.                */
/*   ESC ] 50;<font-name><BEL>   Change font name.                    */
/*   ESC ] 100;<mini-label><BEL> Change label on status line.         */
/*   								      */
/*   ESC [ ? 0 f	Enable Graphics Expose processing.            */
/*   ESC [ ? 0 F	Disable Graphics Expose processing.           */
/*   ESC [ ? 1 f	Toggle fg/bg color                            */
/*   ESC [ ? 2 f	Push current font onto stack.                 */
/*   ESC [ ? 3 f	Pop font from stack.                          */
/*   								      */
/*   ESC [ = C    	Hide cursor                                   */
/*   ESC [ = 1C    	Invisible cursor.                             */
/*   								      */
/*   ESC # 3    	Top-half of double height                     */
/*   ESC # 4    	Bottom-half of double height                  */
/*   ESC # 5    	Normal height                                 */
/*   ESC # 6    	Double width                                  */
/*   								      */
/*   ESC [ > 0 c	Device attributes request. (secondary)	      */
/*   								      */
/*   Sequences reported:					      */
/*     ESC [ b; m; s; r; c M  Mouse position report   		      */
/*   			b == Button being reported (1..3)	      */
/*   			s == 0 button pressed  			      */
/*   			  == 1 button released 			      */
/*   			  == 2 button motion   			      */
/*   								      */
/*   			m == Modifier status as a set of bits         */
/*   			     0x01 Shift down    		      */
/*   			     0x02 Ctrl down     		      */
/*   			     0x04 Meta down     		      */
/*   								      */
/*   			r == current row (decimal 0..n)		      */
/*   			c == current col (decimal 0..n)		      */
/*   								      */
/**********************************************************************/

# include	"machine.h"
# include	<stdio.h>
# include	<fcntl.h>
# undef sscanf
# include	<assert.h>
# include	<dlfcn.h>
# include	<X11/Xlib.h>
# include 	<X11/IntrinsicP.h>
# include 	<X11/StringDefs.h>
# include 	<X11/Shell.h>
# include	<X11/Xatom.h>
# if HAVE_FREETYPE
#include <ft2build.h>  
#include FT_FREETYPE_H 
# endif
# if HAVE_FREETYPE_XFT
# define FT_FREETYPE_H "/usr/include/freetype2/freetype/freetype.h"
# include       <X11/Xft/Xft.h>
# endif

# include	<signal.h>
# include	<time.h>
# include	<sys/time.h>
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<unistd.h>
# include	<errno.h>
# if HAVE_POLL_H
#	include	<sys/poll.h>
# endif
# include	<malloc.h>
# include	<stdlib.h>
# include 	"ctwP.h"
# include 	<dstr.h>
# include 	<hash.h>
# include	<X11/cursorfont.h>

# define	DO_VERIFY	0
# if !DO_VERIFY
#	define	verify(x)
# endif

# if !defined(MIN)
#	define	MIN(a, b)	((a) < (b) ? (a) : (b))
#	define	MAX(a, b)	((a) > (b) ? (a) : (b))
# endif
# define 	SWAP(a, b, t) {t = a; a = b; b = t; }

/**********************************************************************/
/*   Following  is  used  if  we  want  to  use my version of malloc  */
/*   checking.							      */
/**********************************************************************/
# if defined(USE_CHKALLOC)
# 	include	"chkalloc.h"
# else
#	define	chk_calloc	calloc
#	define	chk_alloc	malloc
#	define	chk_zalloc(x)	calloc(x, 1)
#	define	chk_realloc	realloc
#	define	chk_free	free
#	define	chk_free_ptr(x)	if (*(x)) { free(*x); *x = NULL;}
#	define	chk_strdup	strdup
# endif
# include	<X11/keysym.h>
# include	"keyboard.c"
# include	<ctype.h>
# include	<memory.h>
# if defined(GOT_STDLIB)
# include	<stdlib.h>
# endif
# include	<string.h>

char	*mktemp();

/**********************************************************************/
/*   Macros  to  convert  row/col  positions  to pixel co-ordinates.  */
/*   These can be used for drawing strings.			      */
/**********************************************************************/
# define	X_PIXEL(w, x) ((w->ctw.font_width * (x)) + w->ctw.internal_width)
# define	Y_PIXEL(w, y) ((w->ctw.font_height * (y)) + w->ctw.font_ascent + w->ctw.internal_height)

/**********************************************************************/
/*   ROW_TO_PIXEL  is  similar  to  Y_PIXEL  but  mustnt be used for  */
/*   drawing strings. It is suitable for drawing boxes.		      */
/**********************************************************************/
# define	ROW_TO_PIXEL(w, r)	\
	((r) * w->ctw.font_height + w->ctw.internal_height)
# define	BLACK	0
# define	WHITE	7

# define	MAX_ARGS	32

char sequences[] = {
#include "include/sequence.h"
};
char code_text[] = {
#include "include/code.h"
};

/**********************************************************************/
/*   Default size of window if too small or not defined.	      */
/**********************************************************************/
# define	DEFAULT_HEIGHT	(13 * 24)
# define	DEFAULT_WIDTH	(7 * 80)
# define	DEFAULT_FONT	XtDefaultFont

static XtIntervalId wheel_timer;
static int 	default_rows = 24;
static int 	default_spill_size = 25 * 1024 * 1024;
static int 	default_columns = 80;
static int	default_max_lines = 10000;
static Boolean	defaultFALSE = FALSE;
static int	default_flashrate = 500;
static int	default_multiClickTime = 250;
static int	default_internal_ht = 0;
static int	default_internal_wd = 0;
static int	default_gzip_rollover = TRUE;
static int	might_have_selection;
static Atom	atom_clipboard;
int	enable_primary = TRUE;
int	enable_secondary = FALSE;
int	enable_clipboard = FALSE; /* XView ? */
int	enable_cut_buffer0 = FALSE;
static int	fill_to_black = 0;
extern int	version_build_no;

/**********************************************************************/
/*   Get bitmaps for visuals.					      */
/**********************************************************************/
static int      rshift, gshift, bshift;
static int      rshift1, gshift1, bshift1;

static XtResource resources[] = {
#define offset(field) XtOffset(CtwWidget, ctw.field)
	/* {name, class, type, size, offset, default_type, default_addr}, */
	{ XtNinternalHeight, XtCInternalHeight, XtRInt, sizeof(int),
	  offset(internal_height), XtRString, (char *) &default_internal_ht},
	{ XtNinternalWidth, XtCInternalWidth, XtRInt, sizeof(int),
	  offset(internal_width), XtRString, (char *) &default_internal_wd},
	{ XtNfont, XtCFont, XtRString, sizeof(char*),
	  offset(font), XtRString, "7x13bold"},
	{ XtNttyName, XtCTtyName, XtRString, sizeof(char*),
	  offset(ttyname), XtRString, "qq"},
	{ XtNgeometry, XtCGeometry, XtRString, sizeof(char*),
	  offset(geometry), XtRString, ""},
	{ XtNgridLineColor, XtCGridLineColor, XtRPixel, sizeof(unsigned long),
	  offset(gridLine_color), XtRString, "#303050"},
	{ XtNcursorColor, XtCCursorColor, XtRPixel, sizeof(unsigned long),
	  offset(cursor_color), XtRString, "red"},
	{ XtNhiliteBackground, XtCHiliteBackground, XtRPixel, sizeof(unsigned long),
	  offset(hilite_bg), XtRString, "RoyalBlue"},
	{ XtNhiliteForeground, XtCHiliteForeground, XtRPixel, sizeof(unsigned long),
	  offset(hilite_fg), XtRString, "yellow"},

	{ XtNspillForeground, XtCSpillForeground, XtRPixel, sizeof(unsigned long),
	  offset(spill_fg), XtRString, "#ffff80"},
	{ XtNspillBackground, XtCSpillBackground, XtRPixel, sizeof(unsigned long),
	  offset(spill_bg), XtRString, "#303030"},

	{ XtNcontForeground, XtCContForeground, XtRPixel, sizeof(unsigned long),
	  offset(cont_fg), XtRString, "red"},
	{ XtNcontBackground, XtCContBackground, XtRPixel, sizeof(unsigned long),
	  offset(cont_bg), XtRString, "yellow"},

	{ XtNsearchForeground, XtCSearchForeground, XtRPixel, sizeof(unsigned long),
	  offset(search_fg), XtRString, "#1010ff"},
	{ XtNsearchBackground, XtCSearchBackground, XtRPixel, sizeof(unsigned long),
	  offset(search_bg), XtRString, "#ffff80"},
	{ XtNspillSize, XtCSpillSize, XtRInt, sizeof(int),
	  offset(c_spill_size), XtRInt, (char *) &default_spill_size},
	{ XtNrows, XtCRows, XtRInt, sizeof(int),
	  offset(rows), XtRInt, (char *) &default_rows},
	{ XtNcolumns, XtCColumns, XtRInt, sizeof(int),
	  offset(columns), XtRInt, (char *) &default_columns},
	{ XtNkbdCallback, XtCKbdCallback, XtRCallback, sizeof(caddr_t),
	  offset(kbd_callback), XtRCallback, (caddr_t) NULL},
	{ XtNresizeCallback, XtCResizeCallback, XtRCallback, sizeof(caddr_t),
	  offset(resize_callback), XtRCallback, (caddr_t) NULL},
	{ XtNtopCallback, XtCTopCallback, XtRCallback, sizeof(caddr_t),
	  offset(top_callback), XtRCallback, (caddr_t) NULL},
	{ XtNmouseCallback, XtCMouseCallback, XtRCallback, sizeof(caddr_t),
	  offset(mouse_callback), XtRCallback, (caddr_t) NULL},
	{ XtNapplCallback, XtCApplCallback, XtRCallback, sizeof(caddr_t),
	  offset(appl_callback), XtRCallback, (caddr_t) NULL},
	{ XtNtranslations, XtCTranslations, XtRTranslationTable, sizeof(char*),
	  offset(table), XtRString, ""},
	{ XtNsunFunctionKeys, XtCSunFunctionKeys, XtRBoolean, sizeof(Boolean),
	  offset(sun_function_keys), XtRBoolean, &defaultFALSE},
	{ XtNsaveLines, XtCSaveLines, XtRInt, sizeof(int),
	  offset(max_lines), XtRInt, (char *) &default_max_lines},
	{ XtNflashrate, XtCFlashrate, XtRInt, sizeof(int),
	  offset(flashrate), XtRInt, (char *) &default_flashrate},
	{ XtNfont1, XtCFont1, XtRString, sizeof(char *),
	  offset(font1), XtRString, "5x8"},
	{ XtNfont2, XtCFont2, XtRString, sizeof(char *),
	  offset(font2), XtRString, "6x9"},
	{ XtNfont3, XtCFont3, XtRString, sizeof(char *),
	  offset(font3), XtRString, "6x10"},
	{ XtNfont4, XtCFont4, XtRString, sizeof(char *),
	  offset(font4), XtRString, "6x13"},
	{ XtNfont5, XtCFont5, XtRString, sizeof(char *),
	  offset(font5), XtRString, "7x13bold"},
	{ XtNfont6, XtCFont6, XtRString, sizeof(char *),
	  offset(font6), XtRString, "8x13bold"},
	{ XtNmultiClickTime, XtCMultiClickTime, XtRInt, sizeof(int),
	  offset(multiClickTime), XtRInt, (char *) &default_multiClickTime},

	{ XtNcolor1, XtCColor1, XtRString, sizeof(char *),
	  offset(color_names[0]), XtRString, ""},
	{ XtNcolor2, XtCColor2, XtRString, sizeof(char *),
	  offset(color_names[1]), XtRString, ""},
	{ XtNcolor3, XtCColor3, XtRString, sizeof(char *),
	  offset(color_names[2]), XtRString, ""},
	{ XtNcolor4, XtCColor4, XtRString, sizeof(char *),
	  offset(color_names[3]), XtRString, ""},
	{ XtNcolor5, XtCColor5, XtRString, sizeof(char *),
	  offset(color_names[4]), XtRString, ""},
	{ XtNcolor6, XtCColor6, XtRString, sizeof(char *),
	  offset(color_names[5]), XtRString, ""},
	{ XtNcolor7, XtCColor7, XtRString, sizeof(char *),
	  offset(color_names[6]), XtRString, ""},
	{ XtNcolor8, XtCColor8, XtRString, sizeof(char *),
	  offset(color_names[7]), XtRString, ""},
	{ XtNcolor9, XtCColor9, XtRString, sizeof(char *),
	  offset(color_names[8]), XtRString, ""},
	{ XtNcolor10, XtCColor10, XtRString, sizeof(char *),
	  offset(color_names[9]), XtRString, ""},
	{ XtNcolor11, XtCColor11, XtRString, sizeof(char *),
	  offset(color_names[10]), XtRString, ""},
	{ XtNcolor12, XtCColor12, XtRString, sizeof(char *),
	  offset(color_names[11]), XtRString, ""},
	{ XtNcolor13, XtCColor13, XtRString, sizeof(char *),
	  offset(color_names[12]), XtRString, ""},
	{ XtNcolor14, XtCColor14, XtRString, sizeof(char *),
	  offset(color_names[13]), XtRString, ""},
	{ XtNcolor15, XtCColor15, XtRString, sizeof(char *),
	  offset(color_names[14]), XtRString, ""},
	{ XtNcolor16, XtCColor16, XtRString, sizeof(char *),
	  offset(color_names[15]), XtRString, ""},
	{ XtNcolor17, XtCColor17, XtRString, sizeof(char *),
	  offset(color_names[16]), XtRString, ""},
	{ XtNcolor18, XtCColor18, XtRString, sizeof(char *),
	  offset(color_names[17]), XtRString, ""},
	{ XtNcolor19, XtCColor19, XtRString, sizeof(char *),
	  offset(color_names[18]), XtRString, ""},
	{ XtNcolor20, XtCColor20, XtRString, sizeof(char *),
	  offset(color_names[19]), XtRString, ""},
	{ XtNcolor21, XtCColor21, XtRString, sizeof(char *),
	  offset(color_names[20]), XtRString, ""},
	{ XtNcolor22, XtCColor22, XtRString, sizeof(char *),
	  offset(color_names[21]), XtRString, ""},
	{ XtNcolor23, XtCColor23, XtRString, sizeof(char *),
	  offset(color_names[22]), XtRString, ""},
	{ XtNcolor24, XtCColor24, XtRString, sizeof(char *),
	  offset(color_names[23]), XtRString, ""},
	{ XtNcolor25, XtCColor25, XtRString, sizeof(char *),
	  offset(color_names[24]), XtRString, ""},
	{ XtNcolor26, XtCColor26, XtRString, sizeof(char *),
	  offset(color_names[25]), XtRString, ""},
	{ XtNcolor27, XtCColor27, XtRString, sizeof(char *),
	  offset(color_names[26]), XtRString, ""},
	{ XtNcolor28, XtCColor28, XtRString, sizeof(char *),
	  offset(color_names[27]), XtRString, ""},
	{ XtNcolor29, XtCColor29, XtRString, sizeof(char *),
	  offset(color_names[28]), XtRString, ""},
	{ XtNcolor30, XtCColor30, XtRString, sizeof(char *),
	  offset(color_names[29]), XtRString, ""},
	{ XtNcolor31, XtCColor31, XtRString, sizeof(char *),
	  offset(color_names[30]), XtRString, ""},
	{ XtNcolor32, XtCColor32, XtRString, sizeof(char *),
	  offset(color_names[31]), XtRString, ""},

	{ XtNclientData, XtCClientData, XtRString, sizeof(char *),
	  offset(client_data), XtRString, ""},

	{ XtNlogFile, XtCLogFile, XtRString, sizeof(char *),
	  offset(log_file), XtRString, "/tmp/CtwLog.XXXXXX"},
	{ XtNlogDir, XtCLogDir, XtRString, sizeof(char *),
	  offset(log_dir), XtRString, "/tmp"},
	{ XtNlogging, XtCLogging, XtRBoolean, sizeof(int),
	  offset(flags[CTW_LOGGING]), XtRInt, &defaultFALSE},
#undef offset
};

static struct keymap_t keytbl[] = {
	{~(ShiftMask | ControlMask), XK_R13, "\033[mR13~"},
	{~(ShiftMask | ControlMask), XK_End, "\033[mR13~"},

	{~(ShiftMask | ControlMask), XK_R7, "\033[mR7~"},
	{~(ShiftMask | ControlMask), XK_Home, "\033[mR7~"},

	{~(ShiftMask | ControlMask), XK_R15, "\033[mR15~"},
	{~(ShiftMask | ControlMask), XK_Next, "\033[mR15~"},

	{~(ShiftMask | ControlMask), XK_R9, "\033[mR9~"},
	{~(ShiftMask | ControlMask), XK_Prior, "\033[mR9~"},
	{~(ShiftMask | ControlMask), '[', "\033[m[~"},
	{~(ShiftMask | ControlMask), ']', "\033[m]~"},

	{ShiftMask, XK_R15, "\033[SR15~"},
	{ShiftMask, XK_Next, "\033[SR15~"},

	{ShiftMask, XK_R9, "\033[SR9~"},
	{ShiftMask, XK_Prior, "\033[SR9~"},
	};

/**********************************************************************/
/*   Indirection for Freetype.					      */
/**********************************************************************/
# if HAVE_FREETYPE
FT_Library	ft_lib;
FT_Face		face;
int	(*FT_Init_FreeType_ptr)();
int	(*FT_New_Face_ptr)();
int	(*FT_Load_Char_ptr)();
int	(*FT_Set_Char_Size_ptr)();
# endif
# if HAVE_FREETYPE_XFT
XftFont	*(*XftFontOpen_ptr)();
void	*(*XftTextExtents8_ptr)();
int	*(*XftColorAllocValue_ptr)();
int	*(*XftDrawCreate_ptr)();
int	*(*XftDrawStringUtf8_ptr)();
# endif
/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
# if !defined(verify)
void verify(CtwWidget ctw);
# endif
static long map_rgb_to_visual_rgb(CtwWidget ctw, long rgb);
static void graph_destroy(CtwWidget ctw);
static graph_t *graph_click(CtwWidget ctw, int x, int y, int execute);
static void graph_draw(CtwWidget ctw, graph_t *g);
static void graph_redraw(CtwWidget ctw, XRectangle *rectp);
static void hilite(CtwWidget w, int x1, int y1, int x2, int y2, int set_flag, XRectangle *rectp);
char	*basename(char *);
static void command_input(CtwWidget ctw, int keymod_mask, int keysym, int ch);
static void command_draw(CtwWidget ctw);
static void unhilite(CtwWidget ctw);
static line_t * dsp_get_row(CtwWidget w, int ln);
static int dsp_get_top(CtwWidget w);
static int dsp_get_max_lines(CtwWidget w);
static void read_modifier_keys(Display *);
static void label_line(CtwWidget ctw);
static int is_ctw_modifier(int mask);
static void	initialize();
static void	realize();
static void	Destroy();
static void	Resize();
static void	HandleExpose();
static  Boolean	Set_values();
static void	HandleFocusChange();

static void CtwInput(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectStart(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectExtend(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwSelectEnd(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwString(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwGraphicsExpose(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonDown(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonUp(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwWheel(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwButtonMotion(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwMotion(/* Widget, void *data, XEvent * */);
static void CtwFocusChange(/* Widget, XEvent*, String*, Cardinal* */);
static void CtwInsertSelection(/* Widget, XEvent*, String*, Cardinal* */);

static  int	handle_blinking PROTO((CtwWidget));
static void	destroy_pixmap_array PROTO((CtwWidget, Pixmap *));
static int	hist_save_line(CtwWidget, int);
static 	void	ctw_add_char PROTO((CtwWidget, char *));
static 	void	do_logging PROTO((CtwWidget, int));
static  Atom 	FetchAtom();
static  Boolean convert_proc();
static  char	*handle_escape PROTO((CtwWidget, char *, char *));
static  char	*handle_utf8 PROTO((CtwWidget, char *, char *));
static void	set_reverse_video PROTO((CtwWidget, int));
static  int	compute_length PROTO((CtwWidget, int, int, int));
static  int	do_at_parms PROTO((CtwWidget));
static  int	do_escequ PROTO((CtwWidget, int, int, int *));
static  int	do_csi_quest PROTO((CtwWidget, int, int, int *));
static  int	do_csi_gt PROTO((CtwWidget, int, int, int *));
static  int	do_text_parms PROTO((Widget, char *));
static  int	funcvalue PROTO((int));
static  int	process_escape PROTO((CtwWidget));
static  int	reset_font PROTO((CtwWidget, int));
static  int	setup_x11_colors PROTO((CtwWidget, Display *));
static  int	sunfuncvalue PROTO((int));
static void	alloc_screen PROTO((CtwWidget, int));
static void	application_mouse PROTO((CtwWidget, int, int, XEvent *, int, int));
static void	blank_line PROTO((CtwWidget, int));
static void	blank_line_ptr(CtwWidget w, line_t *lp);
static void	change_name PROTO((CtwWidget, int, char *));
static void	clear_from_beginning PROTO((CtwWidget));
static void	clear_lines PROTO((CtwWidget, int, int));
static void	clear_screen PROTO((CtwWidget));
static void	clear_to_eol PROTO((CtwWidget));
static void	convert_click PROTO((CtwWidget));
static void	ctw_add_string2 PROTO((CtwWidget, char *, int));
static void	cursor_flash_proc();
static void	delete_line PROTO((CtwWidget, int, int));
static void	do_index PROTO((CtwWidget));
static void	do_insert_space PROTO((CtwWidget, int));
static void	do_rev_index PROTO((CtwWidget));
static void	down_line PROTO((CtwWidget, char *, char *));
static void	draw_line PROTO((CtwWidget, int, int, char *, int, Pixel, Pixel, int));
static void	draw_string PROTO((CtwWidget, int, int, char *, int, Pixel, Pixel, int));
static void	exposed_region PROTO((CtwWidget, int, int, int, int));
static void	handle_focus_change PROTO((CtwWidget, int));
static void	insert_line PROTO((CtwWidget, int, int, int, int));
static void	lose_selection();
static void	print_string PROTO((CtwWidget, line_t *, int, int, int));
static void	requestor_callback();
static void	reset_screen PROTO((CtwWidget));
static void	scroll_down PROTO((CtwWidget, int, int));
static void	scroll_rectangle PROTO((CtwWidget, int, int, int, int, int));
static void	scroll_up PROTO((CtwWidget, int));
static void	scroll_up_local  PROTO((CtwWidget, int, int));
static void	send_input PROTO((CtwWidget, char *, int));
static void	send_str PROTO((CtwWidget, char *, int));
static void	show_cursor PROTO((CtwWidget));
static void	toggle_cursor();
static void	turn_off_cursor PROTO((CtwWidget));
static void	turn_on_cursor PROTO((CtwWidget));
static void	update_region PROTO((CtwWidget, int, int, int, int));
static void	wait_for_exposure PROTO((CtwWidget));
static  Pixmap	create_cached_pixmap PROTO((CtwWidget, line_t *, int, int, int));
static void reframe_screen PROTO((CtwWidget));

static XtActionsRec actions[] = {
  /* {name, procedure}, */
    {"input",		CtwInput},
    {"graphicsexpose",	CtwGraphicsExpose},
    {"string",		CtwString},
    {"select-start",	CtwSelectStart},
    {"select-extend",	CtwSelectExtend},
    {"select-end",	CtwSelectEnd},
    {"insert-selection",CtwInsertSelection},
    {"button_motion",	CtwButtonMotion},
    {"button_down",	CtwButtonDown},
    {"button_up",	CtwButtonUp},
    {"wheel",		CtwWheel},
    {"focus_change",	CtwFocusChange},
};
static char translations[] =
"\
<Key>F18:		insert-selection()\n\
<KeyPress>:		input()\n\
<GraphicsExpose>:	graphicsexpose()\n\
<NoExpose>:		graphicsexpose()\n\
<Btn1Down>:		select-start()\n\
<Btn1Motion>:		select-extend()\n\
<Btn1Up>:		select-end(PRIMARY, CUT_BUFFER0)\n\
<Btn2Up>:		button_up()\n\
<Btn2Down>:		button_down()\n\
<Btn3Up>:		button_up()\n\
<Btn3Down>:		button_down()\n\
<Btn4Down>:		wheel(0)\n\
<Btn5Down>:		wheel(1)\n\
<BtnMotion>:		button_motion()\n\
<EnterWindow>:		focus_change(1)\n\
<LeaveWindow>:		focus_change(0)\n";

CtwClassRec ctwClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Ctw",
    /* widget_size		*/	sizeof(CtwRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	XtExposeCompressMaximal,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	HandleExpose,
    /* set_values		*/	Set_values,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL /*ctw_accept_focus*/,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* ctw fields */
    /* empty			*/	0
  }
};

WidgetClass ctwWidgetClass = (WidgetClass)&ctwClassRec;

static int	mouse_emulate;

/**********************************************************************/
/*   Array  of  characters  used  to  map  from  PC character set to  */
/*   line-drawing character set.				      */
/**********************************************************************/
static char pc_chars[256];

static char *word_chars = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/";

/**********************************************************************/
/*   Structure  to  keep  track of whether certain X11 modifier keys  */
/*   are depressed.						      */
/**********************************************************************/
typedef struct special_key_t {
	int	sk_keysym;	/* Value of key.		*/
	int	sk_mask;	/* Mask to see if button down.	*/
	int	sk_pressed;	/* Button pressed.		*/
	} special_key_t;
# define	SKEY_NUMLOCK		0
# define	SKEY_MODE_SWITCH	1

# define	MAX_SPECIAL_KEYS	2

static XModifierKeymap	*keymap;
static special_key_t key_specials[MAX_SPECIAL_KEYS];

/**********************************************************************/
/*   Index  array  by  char  value  to  see  whether we need special  */
/*   processing.						      */
/**********************************************************************/
static unsigned char	parse_tbl[256];

int ctw_history = 1;
static int crwin_debug;
static int crwin_do_cont;
static int draw_watch;

static int	ctw_usleep(int, CtwWidget);
int	nanosleep();

/**********************************************************************/
/*   Function to allocate (or re-allocate) screen memory structure.   */
/**********************************************************************/
static void
alloc_screen(CtwWidget ctw, int clear_flag)
{	int	i, j, c;
	int	rows;
	int	old_rows_count = 0;
	int	old_max_lines;
	ctw_callback_t	reason;
	line_t	*old_lines = (line_t *) NULL;
	vbyte_t	normal_space;
	int	do_redraw = FALSE;

	/***********************************************/
	/*   Dont  do  anything if screen is the same  */
	/*   size.				       */
	/***********************************************/
	if (ctw->ctw.rows == ctw->ctw.old_rows && ctw->ctw.columns == ctw->ctw.old_columns)
		return;

	turn_off_cursor(ctw);
	/***********************************************/
	/*   If   window   is  changing  width,  then  */
	/*   maybe  avoid  lots of hardwork if we are  */
	/*   either   getting   smaller   or  getting  */
	/*   larger   but   we've   got   some  spare  */
	/*   columns handy.			       */
	/***********************************************/
	if (ctw->ctw.rows == ctw->ctw.old_rows) {
		if (ctw->ctw.columns < ctw->ctw.old_columns) {
			goto tidy_up;
			}
		/***********************************************/
		/*   If  window  getting  wider but we've got  */
		/*   the  memory  allocated  just blacken out  */
		/*   the column.			       */
		/***********************************************/
		if (ctw->ctw.columns < ctw->ctw.alloc_columns) {
			vbyte_clear(&normal_space);
			normal_space.vb_fcolor = ctw->ctw.blank.vb_bcolor;
			normal_space.vb_bcolor = ctw->ctw.blank.vb_bcolor;
			for (i = 0; i < ctw->ctw.max_lines + ctw->ctw.rows; i++)
				for (j = ctw->ctw.old_columns; j < ctw->ctw.columns; j++) {
					ctw->ctw.c_lines[i].l_text[j] = normal_space;
					}
			goto tidy_up;
			}
		}
	else if (ctw->ctw.columns == ctw->ctw.old_columns) {
		line_t	*lp;
		int	nend;
		/*int	new_rows = ctw->ctw.rows - ctw->ctw.old_rows;*/

		/***********************************************/
		/*   If  window  keeping  same width but just  */
		/*   got  taller/shorter  then  just allocate  */
		/*   extra/free   excess   and  avoid  paging  */
		/*   whole memory space.		       */
		/***********************************************/
		if (ctw->ctw.rows < ctw->ctw.old_rows) {
/*printf("resize smaller %d -> %d (dtop=%d)\n", ctw->ctw.old_rows, ctw->ctw.rows, dsp_get_top(ctw));*/
			for (i = ctw->ctw.max_lines + ctw->ctw.rows; 
			     i < ctw->ctw.max_lines + ctw->ctw.old_rows; i++)
				chk_free_ptr((void **) &ctw->ctw.c_lines[i].l_text);
			/*
			for (i = dsp_get_top(ctw) + ctw->ctw.rows; 
				i < dsp_get_max_lines(ctw) + ctw->ctw.rows; i++) {
				assert(i != ctw->ctw.curtop);
				blank_line(ctw, i);
				}
			*/
			if (ctw->ctw.curtop >= ctw->ctw.max_lines + ctw->ctw.rows) {
/*printf("....shoving top\n");*/
				ctw->ctw.curtop = ctw->ctw.max_lines + ctw->ctw.rows - 1;
				}
//			ctw->ctw.curtop = ctw->ctw.max_lines + ctw->ctw.rows - 1;
			do_redraw = TRUE;
			ctw->ctw.bot_line = dsp_get_top(ctw) + ctw->ctw.rows;
			goto tidy_up;
			}
/*printf("resize BIGGER %d -> %d\n", ctw->ctw.old_rows, ctw->ctw.rows);*/
		/***********************************************/
		/*   Must  be  getting  taller allocate extra  */
		/*   lines and blacken them.		       */
		/***********************************************/
		rows = ctw->ctw.rows + ctw->ctw.max_lines;
		ctw->ctw.c_lines = (line_t *) chk_realloc((void *) ctw->ctw.c_lines, 
			rows * sizeof(line_t));
		lp = ctw->ctw.c_lines;
		nend = ctw->ctw.max_lines + ctw->ctw.old_rows - ctw->ctw.curtop;
		if (nend < 0) {
			printf("nend=%d\n", nend);
			assert(nend > 0);
			}
		memmove(&lp[rows - nend], &lp[ctw->ctw.curtop], nend * sizeof(line_t));
		for (i = ctw->ctw.curtop; i < rows - nend; i++) {
			ctw->ctw.c_lines[i].l_attr = 0;
			ctw->ctw.c_lines[i].l_text = (vbyte_t *) 
				chk_alloc(ctw->ctw.alloc_columns * sizeof(vbyte_t));
			blank_line_ptr(ctw, &ctw->ctw.c_lines[i]);
			}
		ctw->ctw.curtop = i;
		goto tidy_up;
		}
	/***********************************************/
	/*   Save  copy  of  old  screen  contents in  */
	/*   case   we   want  to  keep  as  much  as  */
	/*   possible.				       */
	/***********************************************/
	old_max_lines = ctw->ctw.max_lines;
	if (ctw->ctw.c_lines) {
		rows = ctw->ctw.old_rows + ctw->ctw.max_lines;
		old_lines = (line_t *) chk_alloc(rows * sizeof(line_t));
		old_rows_count = rows;
		memcpy((char *) old_lines, 
			(char *) ctw->ctw.c_lines, rows * sizeof(line_t));
		}

	if (ctw->ctw.tab_stops == (char *) NULL) {
		ctw->ctw.tab_stops = (char *) chk_calloc(ctw->ctw.columns, 1);
		}
	else {
		i = ctw->ctw.columns - ctw->ctw.old_columns;
		if (i > 0) {
			ctw->ctw.tab_stops = (char *) chk_realloc(ctw->ctw.tab_stops,
				ctw->ctw.columns);
			memset(ctw->ctw.tab_stops + ctw->ctw.old_columns, 0, i);
			}
		}
	ctw->ctw.alloc_columns = ctw->ctw.columns + SPARE_COLUMNS;
	rows = ctw->ctw.rows + ctw->ctw.max_lines;
	ctw->ctw.tot_rows = rows;
	ctw->ctw.c_lines = (line_t *) chk_zalloc(rows * sizeof(line_t));
	for (i = 0; i < rows; i++) {
		ctw->ctw.c_lines[i].l_text = (vbyte_t *) chk_zalloc(ctw->ctw.alloc_columns * sizeof(vbyte_t));
		}
	/***********************************************/
	/*   Set  whole  screen  to  white  spaces on  */
	/*   black background.			       */
	/***********************************************/
	blank_line(ctw, 0);
	for (i = 1; i < rows; i++)
		memcpy(ctw->ctw.c_lines[i].l_text, 
			ctw->ctw.c_lines[0].l_text, 
			sizeof(vbyte_t) * ctw->ctw.columns);

	/***********************************************/
	/*   If  we're  resizing the screen then copy  */
	/*   back  as  much  of  the  old  screen  as  */
	/*   possible.				       */
	/***********************************************/
	if (!clear_flag) {
		int	last_line;
		c = ctw->ctw.old_columns;
		if (ctw->ctw.columns < c)
			c = ctw->ctw.columns;
		if (ctw->ctw.rows < ctw->ctw.old_rows)
			last_line = ctw->ctw.rows;
		else
			last_line = ctw->ctw.old_rows;
		last_line += old_max_lines;
		for (i = 0; i < last_line; i++) {
			for (j = 0; j < c; j++)
				ctw->ctw.c_lines[i].l_text[j] = old_lines[i].l_text[j];
			for ( ; j < ctw->ctw.columns; j++)
				ctw->ctw.c_lines[i].l_text[j].vb_byte = ' ';
			}
		for ( ; i < rows; i++) {
			for (j = 0; j < ctw->ctw.columns; j++)
				ctw->ctw.c_lines[i].l_text[j].vb_byte = ' ';
			}
		}
	/***********************************************/
	/*   Keep cursor within bounds of the screen.  */
	/***********************************************/
tidy_up:
	ctw->ctw.tot_rows = ctw->ctw.max_lines + ctw->ctw.rows;
	if (ctw->ctw.y >= ctw->ctw.rows)
		ctw->ctw.y = ctw->ctw.rows - 1;
	if (ctw->ctw.saved_y >= ctw->ctw.rows)
		ctw->ctw.saved_y = ctw->ctw.rows - 1;
	if (ctw->ctw.x >= ctw->ctw.columns)
		ctw->ctw.x = ctw->ctw.columns - 1;
	if (ctw->ctw.saved_x >= ctw->ctw.columns)
		ctw->ctw.saved_x = ctw->ctw.columns - 1;
	/***********************************************/
	/*   Save size in case we get a resize event.  */
	/***********************************************/
	ctw->ctw.old_rows = ctw->ctw.rows;
	ctw->ctw.old_columns = ctw->ctw.columns;

	if (old_lines) {
		for (i = 0; i < old_rows_count; i++)
			chk_free((void *) old_lines[i].l_text);
		chk_free((void *) old_lines);
		}
	turn_on_cursor(ctw);
	/***********************************************/
	/*   Allow   owner   of  widget  to  see  the  */
	/*   resize event.			       */
	/***********************************************/
	reason.reason = CTWR_RESIZE;
	reason.client_data = ctw->ctw.client_data;
	XtCallCallbacks((Widget) ctw, XtNresizeCallback, (caddr_t) &reason);

	if (do_redraw) {
		update_region(ctw, 0, 0, ctw->ctw.rows, ctw->ctw.columns);
		}
}
/**********************************************************************/
/*   Initialise  freetype but handle that we may not have it on this  */
/*   system.							      */
/**********************************************************************/
static int freetype_enabled;
static void
init_freetype(CtwWidget new)
{	char	*cp;

	if ((cp = getenv("CTW_FREETYPE")) == NULL || atoi(cp) == 0)
		return;

# if defined(HAVE_FREETYPE)
	{
	Display *dpy = XtDisplay((Widget) new);
	int	error;

	void	*handle = dlopen("/usr/lib/libfreetype.so", RTLD_LAZY);
	if (handle == NULL) {
		printf("Couldnt open libXft.so\n");
		return;
		}

	FT_Init_FreeType_ptr = dlsym(handle, "FT_Init_FreeType");
	FT_New_Face_ptr = dlsym(handle, "FT_New_Face");
	FT_Load_Char_ptr = dlsym(handle, "FT_Load_Char");
	FT_Set_Char_Size_ptr = dlsym(handle, "FT_Set_Char_Size");

	if (!FT_Init_FreeType_ptr)
		return;


	if ((error = FT_Init_FreeType_ptr(&ft_lib)) != 0) {
		printf("ctw:FT_Init_FreeType: error=%d\n", error);
		return;
 		}
	error = FT_New_Face_ptr(ft_lib, 
		"/usr/share/fonts/truetype/freefont/FreeSans.ttf", 0, 
		&face ); 
	if (error) {
		printf("ctw:FT_New_Face: error=%d\n", error);
		return;
		}

	error = FT_Set_Char_Size_ptr( face, /* handle to face object */  
		0,	/* char_width in 1/64th of points */  
		16*64,	/* char_height in 1/64th of points */  
		300,	/* horizontal device resolution */  
		300 );	/* vertical device resolution */ 
	if (error) {
		printf("ctw:FT_Set_Char_Size: error=%d\n", error);
		return;
		}
	}
# endif
# if defined(HAVE_FREETYPE_XFT)
	{
	Display *dpy = XtDisplay((Widget) new);
	int scr = DefaultScreen(dpy);
	void	*handle = dlopen("/usr/lib/libXft.so", RTLD_LAZY);
	if (handle == NULL) {
		printf("Couldnt open libXft.so\n");
		return;
		}

	XftFontOpen_ptr = dlsym(handle, "XftFontOpen");
	XftColorAllocValue_ptr = dlsym(handle, "XftColorAllocValue");
	XftDrawCreate_ptr = dlsym(handle, "XftDrawCreate");
	XftDrawStringUtf8_ptr = dlsym(handle, "XftDrawStringUtf8");
	XftTextExtents8_ptr = dlsym(handle, "XftTextExtents8");

	if (XftFontOpen_ptr) {
		/* Fonts: bitstream vera sans mono */
		/* Sans Serif-7 */
		/* Free Mono-12 */
static char *old_font;

		char	*font = new->ctw.font;
		char *size = NULL;
		if (font) {
			chk_free_ptr((void **) &old_font);
			old_font = font = chk_strdup(font);
			size = strchr(font, '-');
			if (size)
				*size++ = '\0';
			}
		if (!font || strcmp(font, "7x13bold") == 0)
			font = "DejaVuSansMono";

		new->ctw.xft_fontp = XftFontOpen_ptr(dpy, scr,
	//	                XFT_FAMILY, XftTypeString, "dejavuserifcondensed",
		                XFT_FAMILY, XftTypeString, font,
	        	        XFT_SIZE, XftTypeDouble, size ? atof(size) : 12.0,
	                	NULL);
		if (new->ctw.xft_fontp == NULL) {
			printf("fcterm: cannot load FreeType font: %s\n",
				new->ctw.font);
			}
		freetype_enabled = TRUE;
		printf("Xft: initialised (%s)\n", new->ctw.font);
		}
	}
# endif
}
/**********************************************************************/
/*   Main entry to create a new widget.				      */
/**********************************************************************/
/* ARGSUSED */
static void
initialize(Widget treq, Widget tnew)
{	CtwWidget	new = (CtwWidget) tnew;
	Display	*dpy = XtDisplay(new);
	char	*cp = getenv("CTW_WORD_CHARS");

	if (cp != NULL)
		word_chars = cp;

	UNUSED_PARAMETER(treq);

	if ((cp = getenv("CTW_FILL_TO_BLACK")) != NULL)
		fill_to_black = atoi(cp);

	if ((cp = getenv("CTW_DEBUG")) != NULL)
		crwin_debug = atoi(cp);
	if ((cp = getenv("CTW_DO_CONT")) != NULL)
		crwin_do_cont = atoi(cp);
	if ((cp = getenv("CTW_GZIP_ROLLOVER")) != NULL)
		default_gzip_rollover = atoi(cp);
	if ((cp = getenv("CTW_HISTORY")) != NULL) 
		ctw_history = atoi(cp);
	if ((cp = getenv("CTW_DRAW_WATCH")) != NULL)
		draw_watch = atoi(cp);
        if ((cp = getenv("CTW_ENABLE_PRIMARY")) != (char *) NULL)
                enable_primary = atoi(cp);
        if ((cp = getenv("CTW_ENABLE_SECONDARY")) != (char *) NULL)
                enable_secondary = atoi(cp);
        if ((cp = getenv("CTW_ENABLE_CLIPBOARD")) != (char *) NULL)
                enable_clipboard = atoi(cp);
        if ((cp = getenv("CTW_ENABLE_CUT_BUFFER0")) != (char *) NULL)
                enable_cut_buffer0 = atoi(cp);

	if (atom_clipboard == 0)
		atom_clipboard = FetchAtom((Widget) new, "CLIPBOARD");

	/***********************************************/
	/*   Initialise the parse table.	       */
	/***********************************************/
//	parse_tbl['\0'] = 1;
	parse_tbl[BEL] = 1;
	parse_tbl['\b'] = 1;
	parse_tbl['\t'] = 1;
	parse_tbl['\f'] = 1;
	parse_tbl['\r'] = 1;
	parse_tbl['\n'] = 1;
	parse_tbl['n' & 0x1f] = 1;
	parse_tbl['o' & 0x1f] = 1;
	parse_tbl[ESC] = 1;
	parse_tbl[XON] = 1;
	parse_tbl[XOFF] = 1;

	memset((char *) new->ctw.flags, 0, sizeof new->ctw.flags);
	new->ctw.font_stack = (struct font_stack *) NULL;
	new->ctw.line_fontp = (XFontStruct *) NULL;
	new->ctw.have_focus = TRUE;
	new->ctw.timestamp = 0;
	new->ctw.nest_level = 0;
	new->ctw.old_rows = 0;
	new->ctw.c_spill_bytes = 0;
	new->ctw.old_columns = 0;
	new->ctw.c_lines = (line_t *) NULL;
	new->ctw.c_search = NULL;
	new->ctw.tab_stops = NULL;
	new->ctw.c_dw_pixmaps = NULL;
	new->ctw.c_th_pixmaps = NULL;
	new->ctw.c_bh_pixmaps = NULL;
	new->ctw.pixmap_mode = FALSE;
	new->ctw.flags[CTW_COLOR] = setup_x11_colors(new, dpy);
	new->ctw.flags[CTW_KEYBOARD_SCROLL] = FALSE;
	new->ctw.flags[CTW_SCROLLING_REGION_ENABLE] = TRUE;
	new->ctw.flags[CTW_SPEED] = 100;
	new->ctw.flags[CTW_UTF8] = TRUE;
	new->ctw.blink_state = 0;

	new->ctw.curr_cset = 0;
	memset(new->ctw.char_sets, 0, sizeof new->ctw.char_sets);

	new->ctw.mouse_mask = 0;
	new->ctw.doing_ge = 0;

	if (new->ctw.flags[CTW_COLOR] == FALSE) {
		/***********************************************/
		/*   Monochrome screen.			       */
		/***********************************************/
		new->ctw.cursor_color = new->ctw.x11_colors[7];
		}
	new->ctw.save_cursor_color = new->ctw.cursor_color;

	/***********************************************/
	/*   Set up  the  PC  character  set  mapping  */
	/*   matrix.				       */
	/***********************************************/
	pc_chars[213] = 'l';
	pc_chars[184] = 'k';
	pc_chars[212] = 'm';
	pc_chars[190] = 'j';
	pc_chars[179] = 'x';
	pc_chars[0xcd] = 'q';
	pc_chars[0xd1] = 'w';
	pc_chars[0xcf] = 'v';
	pc_chars[0xd8] = 'n';
	pc_chars[0xb5] = 'u';
	pc_chars[0xc6] = 't';

	if (new->core.height < 10)
		new->core.height = DEFAULT_HEIGHT + 2 * new->ctw.internal_height;
	if (new->core.width < 10)
		new->core.width = DEFAULT_WIDTH + 2 * new->ctw.internal_width;
	new->core.background_pixel = new->ctw.x11_colors[0];
/*	XSetWindowBackground(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.x11_colors[0]);*/
	new->ctw.win_height_allocated = new->core.height;
	new->ctw.win_width_allocated = new->core.width;
	new->ctw.alloc_columns = 0;
	/***********************************************/
	/*   Set   initial   position  and  state  of  */
	/*   cursor.				       */
	/***********************************************/
	new->ctw.x = new->ctw.y = 0;
	new->ctw.cursor_visible = TRUE;


	init_freetype(new);
	if ((new->ctw.fontp = XLoadQueryFont(XtDisplay(new), new->ctw.font)) != NULL ||
	    (new->ctw.fontp = XLoadQueryFont(XtDisplay(new), "fixed")) != NULL) {
	    	reset_font(new, 0);
		}
	/***********************************************/
	/*   If  rows  or  columns attribute set then  */
	/*   allow   these   to  define  the  widgets  */
	/*   dimensions.			       */
	/***********************************************/
	if (new->ctw.rows > 0)
		new->core.height = new->ctw.rows * new->ctw.font_height 
			+ 2 * new->ctw.internal_height;
	if (new->ctw.columns > 0)
		new->core.width = new->ctw.columns * new->ctw.font_width
			+ 2 * new->ctw.internal_width;
	new->ctw.bot_line = new->ctw.rows;

	/***********************************************/
	/*   Make sure everything is initialized.      */
	/***********************************************/
	new->ctw.num_exposures = 0;
	new->ctw.saved_x = 0;
	new->ctw.saved_y = 0;
	new->ctw.cursor_timer = 0;
	new->ctw.cursor_state = CURSOR_OFF;
	new->ctw.scroll_top = 0;
	new->ctw.scroll_bot = new->ctw.rows;
	new->ctw.sel_string = (char *) NULL;
	new->ctw.c_spill_name = (char *) NULL;
	new->ctw.sel_length = 0;
	new->ctw.sel_start_x = 0;
	new->ctw.sel_start_y = 0;
	new->ctw.sel_cur_x = 0;
	new->ctw.sel_cur_y = 0;
	new->ctw.num_clicks = 0;
	new->ctw.old_color_avail = FALSE;
	new->ctw.c_graph_fg = 0x7fffffff;
	new->ctw.c_graph_bg = 0x7fffffff;

	/***********************************************/
	/*   Look  for  focus  related  events on the  */
	/*   parent     widget.     We     want    to  */
	/*   enable/disable      flashing      cursor  */
	/*   depending   on   whether   we  have  the  */
	/*   keyboard focus or not.		       */
	/***********************************************/
/*	XtAddEventHandler(XtParent(new), FocusChangeMask, FALSE,
		HandleFocusChange, new);*/

	XtAddEventHandler((Widget) new, 
		PointerMotionMask,
		FALSE,
		CtwMotion, NULL);

	/***********************************************/
	/*   Start the log file going if necessary.    */
	/***********************************************/
	new->ctw.log_fp = (FILE *) NULL;
	if (new->ctw.flags[CTW_LOGGING])
		do_logging(new, TRUE);

	read_modifier_keys(dpy);

	/***********************************************/
	/*   Compute RGB visual bitmaps.	       */
	/***********************************************/
	{Visual *visual = DefaultVisual(XtDisplay(new), DefaultScreen(XtDisplay(new)));
	int	j;
	/***********************************************/
	/*   For  24  or  32 bit displays, we need to  */
	/*   get the RGB in the right order.	       */
	/***********************************************/
	j = visual->red_mask;
	for (rshift = 0; rshift < 16; rshift++) {
		if (j & 0x1)
			break;
		j >>= 1;
		}
	for (rshift1 = rshift; j & 0x01; j >>= 1) {
		rshift1++;
		}

	j = visual->green_mask;
	for (gshift = 0; gshift < 16; gshift++) {
		if (j & 0x1)
			break;
		j >>= 1;
		}
	for (gshift1 = gshift; j & 0x01; j >>= 1) {
		gshift1++;
		}

	j = visual->blue_mask;
	for (bshift = 0; bshift < 16; bshift++) {
		if (j & 0x1)
			break;
		j >>= 1;
		}
	for (bshift1 = bshift; j & 0x01; j >>= 1) {
		bshift1++;
		}

	}
}
/**********************************************************************/
/*   Try  and  keep  track of whether we have the focus or not so we  */
/*   can avoid flashing cursor when its not our responsibility.	      */
/**********************************************************************/
static void
HandleFocusChange(Widget widget, CtwWidget w, XFocusChangeEvent *event)
{
	UNUSED_PARAMETER(widget);

	handle_focus_change(w, event->type == FocusIn);
}
# define	superclass	(&widgetClassRec)
static GC gc;
static void
realize(Widget w, XtValueMask *valueMask, XSetWindowAttributes *attributes)
{	CtwWidget	new = (CtwWidget) w;
	XGCValues	values;
	Arg	args[10];
	int	n, i;
	XSetWindowAttributes	win_attr;

	/***********************************************/
	/*   Make sure we have a sane window size.     */
	/***********************************************/
	if (new->ctw.rows < 0)
		new->ctw.rows = 1;
	if (new->ctw.columns < 0)
		new->ctw.columns = 1;
	reset_font(new, FALSE);
	new->core.width = new->ctw.columns * new->ctw.font_width
			+ 2 * new->ctw.internal_width;
	new->core.height = new->ctw.rows * new->ctw.font_height
			+ 2 * new->ctw.internal_height;

	(*superclass->core_class.realize)(w, valueMask, attributes);
	/***********************************************/
	/*   Need  graphics  exposure  events because  */
	/*   we  support  the  insert and delete line  */
	/*   functionality, which uses XCopyArea.      */
	/***********************************************/
	values.graphics_exposures = TRUE;
	values.background = new->ctw.x11_colors[0];
	new->ctw.gc = XCreateGC(XtDisplay(new), XtWindow(new), 
		GCBackground | GCGraphicsExposures, &values);
	values.graphics_exposures = FALSE;
	values.background = new->ctw.cursor_color;
	new->ctw.cursor_gc = XCreateGC(XtDisplay(new), XtWindow(new), 
		GCBackground | GCGraphicsExposures, &values);
	new->ctw.line_gc = XCreateGC(XtDisplay(new), XtWindow(new), GCGraphicsExposures, &values);

	/***********************************************/
	/*   Create   a  spare  GC  for  the  graphic  */
	/*   drawing.				       */
	/***********************************************/
	if (gc == 0) {
		gc = XCreateGC(XtDisplay(new), XtWindow(new), 
			GCBackground | GCGraphicsExposures, &values);
		}
	XSetFont(XtDisplay(new), new->ctw.gc, new->ctw.fontp->fid);
	XSetFont(XtDisplay(new), new->ctw.cursor_gc, new->ctw.fontp->fid);
	XSetFont(XtDisplay(new), new->ctw.line_gc, new->ctw.fontp->fid);

	win_attr.bit_gravity = NorthWestGravity;
	XChangeWindowAttributes(XtDisplay(w), XtWindow(w),
		CWBitGravity, &win_attr);
	
	n = 0;
	XtSetArg(args[n], XtNinput, 1); n++;
	XtSetValues(w, args, n);

# if 0
	n = 0;
	XtSetArg(args[n], XtNwidthInc, new->ctw.font_width); n++;
	XtSetArg(args[n], XtNheightInc, new->ctw.font_height); n++;
	XtSetValues(XtParent(w), args, n);
# endif

	new->ctw.old_top_line = -1;
	new->ctw.sel_string = (char *) NULL;
	new->ctw.top_line = 0;

	new->ctw.flags[CTW_ERASE_BLACK] = TRUE;
	new->ctw.flags[CTW_CUT_NEWLINES] = TRUE;

	/***********************************************/
	/*   Be    careful    in   case   we   change  */
	/*   reset_screen()   such  that  it  assumes  */
	/*   screen memory has been allocated.	       */
	/***********************************************/
	new->ctw.cursor_state = CURSOR_OFF;
	reset_screen(new);
	alloc_screen(new, TRUE);

	/***********************************************/
	/*   Set up default TAB stops.		       */
	/***********************************************/
	for (i = 0; i < new->ctw.columns; i += 8)
		new->ctw.tab_stops[i] = TRUE;


	/***********************************************/
	/*   Timer for the flashing cursor.	       */
	/***********************************************/
	new->ctw.cursor_timer = 0;
	if (new->ctw.flashrate)
		new->ctw.cursor_timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) new),
			(long) new->ctw.flashrate, cursor_flash_proc, new);
}
/**********************************************************************/
/*   Function called when widget is destroyed.			      */
/**********************************************************************/
static void
Destroy(CtwWidget ctw)
{	int	i;

	if (wheel_timer) {
		XtRemoveTimeOut(wheel_timer);
		wheel_timer = 0;
		}

	graph_destroy(ctw);
	chk_free_ptr((void **) &ctw->ctw.c_lcache);

	for (i = 0; i < ctw->ctw.max_lines + ctw->ctw.rows; i++)
		chk_free((void *) ctw->ctw.c_lines[i].l_text);
	chk_free((void *) ctw->ctw.c_lines);
	chk_free_ptr((void **) &ctw->ctw.c_search);
	chk_free_ptr((void **) &ctw->ctw.sel_string);
	chk_free_ptr((void **) &ctw->ctw.c_spill_name);
	if (ctw->ctw.c_spill_fp)
		fclose(ctw->ctw.c_spill_fp);

	if (ctw->ctw.tab_stops)
		chk_free((void *) ctw->ctw.tab_stops);
	/***********************************************/
	/*   Free up the pixmaps.		       */
	/***********************************************/
	destroy_pixmap_array(ctw, ctw->ctw.c_dw_pixmaps);
	destroy_pixmap_array(ctw, ctw->ctw.c_th_pixmaps);
	destroy_pixmap_array(ctw, ctw->ctw.c_bh_pixmaps);

	XFreeGC(XtDisplay((Widget) ctw), ctw->ctw.gc);
	XFreeGC(XtDisplay((Widget) ctw), ctw->ctw.cursor_gc);
	XFreeGC(XtDisplay((Widget) ctw), ctw->ctw.line_gc);
	if (ctw->ctw.cursor_timer)
		XtRemoveTimeOut(ctw->ctw.cursor_timer);

	XtRemoveEventHandler(XtParent(ctw), FocusChangeMask, FALSE,
		(XtEventHandler) HandleFocusChange, ctw);
}
/**********************************************************************/
/*   Free  up  any  allocated  pixmaps  for  the double height/width  */
/*   stuff.							      */
/**********************************************************************/
static void
destroy_pixmap_array(CtwWidget ctw, Pixmap *pa)
{	int	i;

	if (pa == (Pixmap *) NULL)
		return;

	for (i = 0; i < 256; i++) {
		if (pa[i])
			XFreePixmap(XtDisplay(ctw), pa[i]);
		}
	chk_free((void *) pa);
}
static void
HandleExpose(Widget w, XExposeEvent *event)
{
	exposed_region((CtwWidget) w, event->x, event->y, event->width, event->height);
}
/* ARGSUSED */
static void
Resize(CtwWidget w)
{	int	x, y;
	int	diff;

	if (!XtIsRealized((Widget) w))
		return;
	/***********************************************/
	/*   Stop  ctw_get_xy()  from stopping window  */
	/*   from getting bigger.		       */
	/***********************************************/
	w->ctw.rows = 32767;
	w->ctw.columns = 32767;
	ctw_get_xy(w, &y, &x, w->core.width, w->core.height);
	if (y < 1)
		y = 1;
	if (x < 1)
		x = 1;
	w->ctw.rows = y;
	w->ctw.columns = x;
	w->ctw.scroll_top = 0;
	w->ctw.scroll_bot = y;
	w->ctw.flags[CTW_SCROLLING_REGION] = FALSE;

	/***********************************************/
	/*   try  and  make  the bottom of the scroll  */
	/*   area take up the slack so we can now see  */
	/*   more text.                                */
	/***********************************************/
	diff = w->ctw.rows - w->ctw.old_rows;
	if (diff < 0) {
		turn_off_cursor(w);
		ctw_set_top_line(w, dsp_get_top(w) - diff);
		w->ctw.old_top_line = -1;
		turn_on_cursor(w);
		}
	alloc_screen(w, FALSE);
	if (diff > 0) {
		int n = dsp_get_top(w) - diff;
		if (n > 0) {
			turn_off_cursor(w);
			w->ctw.y += diff;
			ctw_set_top_line(w, n);
			w->ctw.old_top_line = -1;
			turn_on_cursor(w);
			}
		}
}
/**********************************************************************/
/*   Method   called  when  application  wants  to  change  resource  */
/*   values. Intercept font change.				      */
/**********************************************************************/
static  Boolean
Set_values(CtwWidget cur, CtwWidget req, CtwWidget new, ArgList args, Cardinal *num_args)
{	int	refresh_needed = FALSE;
	XFontStruct	*fp;
	Dimension	req_width, req_height;

# define	NE(field)	(req->field != new->field)

	UNUSED_PARAMETER(args);
	UNUSED_PARAMETER(num_args);

	if (cur->ctw.font != new->ctw.font && 
	    strcmp(cur->ctw.font, new->ctw.font) != 0) {
		if ((fp = XLoadQueryFont(XtDisplay(new), new->ctw.font)) != NULL) {
			new->ctw.fontp = fp;
			reset_font(new, FALSE);
			XSetFont(XtDisplay(new), new->ctw.gc, fp->fid);
			XSetFont(XtDisplay(new), new->ctw.cursor_gc, fp->fid);
			XSetFont(XtDisplay(new), new->ctw.line_gc, fp->fid);
			refresh_needed = TRUE;
			/***********************************************/
			/*   Try and resize our window.		       */
			/***********************************************/
			req_width = new->ctw.columns * new->ctw.font_width;
			req_height = new->ctw.rows * new->ctw.font_height;
			}
		}
	/***********************************************/
	/*   Watch for row/column changes.	       */
	/***********************************************/
	if (NE(ctw.rows) || NE(ctw.columns)) {
		new->core.width = new->ctw.rows * new->ctw.font_height;
		new->core.height = new->ctw.columns * new->ctw.font_width;
		}
# undef NE
	/***********************************************/
	/*   Check for Log file state change.	       */
	/***********************************************/
	if (cur->ctw.flags[CTW_LOGGING] != new->ctw.flags[CTW_LOGGING])
		do_logging(new, new->ctw.flags[CTW_LOGGING]);
	return refresh_needed;
}
/**********************************************************************/
/*   Convert mouse button presses into pseudo keystrokes.	      */
/**********************************************************************/
static void 
CtwButtonDown(CtwWidget w, XEvent *event, String *x, Cardinal *y)
{	ctw_callback_t reason;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, event->xbutton.state, 0, event, 
			event->xbutton.x,
			event->xbutton.y);
		return;
		}
	w->ctw.timestamp = event->xbutton.time;

	reason.reason = CTWR_BUTTON_DOWN;
	reason.client_data = w->ctw.client_data;
	reason.event = event;
	XtCallCallbacks((Widget) w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Event  handler  to  handle  the mouse moving whilst a button is  */
/*   held down.							      */
/**********************************************************************/
static void 
CtwButtonMotion(CtwWidget w, XEvent *event, String *x, Cardinal *y)
{	ctw_callback_t reason;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, 
			event->xmotion.state & Button1Mask ? Button1 : 
			event->xmotion.state & Button2Mask ? Button2 : 
			event->xmotion.state & Button3Mask ? Button3 : 
			event->xmotion.state & Button4Mask ? Button4 : 
				       	Button5,
			2, event,
			event->xmotion.x,
			event->xmotion.y);
		return;
		}

	if (event->xmotion.state == 0)
		return;

	reason.reason = CTWR_BUTTON_MOTION;
	reason.client_data = w->ctw.client_data;
	reason.event = event;
	XtCallCallbacks((Widget) w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Convert mouse button presses into pseudo keystrokes.	      */
/**********************************************************************/
static void 
CtwButtonUp(CtwWidget w, XEvent *event, String *x, Cardinal *y)
{	ctw_callback_t reason;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, event->xbutton.state, 1, event,
			event->xbutton.x,
			event->xbutton.y);
		return;
		}
	w->ctw.timestamp = event->xbutton.time;
	reason.reason = CTWR_BUTTON_UP;
	reason.client_data = w->ctw.client_data;
	reason.event = event;
	XtCallCallbacks((Widget) w, XtNmouseCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Work function to allow user to customise focus-in/focus-out.     */
/**********************************************************************/
static void 
CtwFocusChange(CtwWidget w, XEvent *event, String *argv, Cardinal *argc)
{
	UNUSED_PARAMETER(event);

	/***********************************************/
	/*   Do nothing if no argument specified       */
	/***********************************************/
	if (argc == (Cardinal *) NULL)
		return;
	switch (atoi(argv[0])) {
	  case 0:
	  	handle_focus_change(w, FALSE);
		break;
	  case 1:
	  	handle_focus_change(w, TRUE);
		break;
	  }
}
/**********************************************************************/
/*   Action routine to handle redrawing of window.		      */
/**********************************************************************/
static void 
CtwGraphicsExpose(Widget w, XGraphicsExposeEvent *event, String *x, Cardinal *y)
{	CtwWidget	cw = (CtwWidget) w;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (!XtIsRealized(w) /*|| need_resize*/)
		return;

	/***********************************************/
	/*   Handle a NoExpose by doing nothing.       */
	/***********************************************/
	if (event->type == NoExpose) {
		if (cw->ctw.num_exposures > 0)
			cw->ctw.num_exposures--;
		return;
		}

	if (event->count == 0 && cw->ctw.num_exposures > 0)
		cw->ctw.num_exposures--;
	/***********************************************/
	/*   If  window  has  changed  size,  then we  */
	/*   need  to  resize  it.  Also  we  need to  */
	/*   clear  any  garbage  which may be at the  */
	/*   bottom or right edge of the window.       */
	/***********************************************/
	if (cw->ctw.win_height_allocated != cw->core.height ||
	    cw->ctw.win_width_allocated != cw->core.width) {
/*		need_resize = TRUE;*/
		cw->ctw.win_height_allocated = cw->core.height;
		cw->ctw.win_width_allocated = cw->core.width;
		return;
		}
	exposed_region(cw, event->x, event->y, 
		event->width,
		event->height);
}
/**********************************************************************/
/*   Come here on a keypress event.				      */
/**********************************************************************/
static void 
CtwInput(Widget w, XEvent *event, String *x, Cardinal *y)
{	CtwWidget	ctw = (CtwWidget) w;
	char	buf[BUFSIZ];
	char	reply[BUFSIZ];
	int	n, k;
	char	*rp = reply;
	static XComposeStatus compose_status;
	KeySym	keysym;
	int	nbytes;
	int	mask, keymod_mask, state;
	ctw_callback_t	reason;
static char *kypd_num = " XXXXXXXX\tXXX\rXXXxxxxXXXXXXXXXXXXXXXXXXXXX*+,-.\\0123456789XXX=";
static char *kypd_apl = " ABCDEFGHIJKLMNOPQRSTUVWXYZ??????abcdefghijklmnopqrstuvwxyzXXX";
static char *cur = "DACB";

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	reason.reason = CTWR_INPUT;
	reason.client_data = ctw->ctw.client_data;
	reason.len = 0;
	reason.ptr = reply;

	nbytes = XLookupString((XKeyEvent *) event, buf, sizeof buf,
		&keysym, &compose_status);

# define TRACE(str) if (crwin_debug) {printf("%s(%d): " #str, __FILE__, __LINE__); }
# define RETURN() if (crwin_debug) {printf("%s(%d): return\n", __FILE__, __LINE__); } return;
	/***********************************************/
	/*   If  we  were  frozen from scrolling then  */
	/*   allow us to scroll again.		       */
	/***********************************************/
	ctw->ctw.c_flags &= ~CTWF_FROZEN;

	/***********************************************/
	/*   If   keyboard  scrolling  enabled  (e.g.  */
	/*   scrollbar  on  display),  then intercept  */
	/*   the directional keys.		       */
	/***********************************************/
	mask = event->xkey.state & (ShiftMask | ControlMask | 
		Mod1Mask | Mod2Mask | Mod3Mask);

	/***********************************************/
	/*   Turn off NumLock.			       */
	/***********************************************/
	state = event->xkey.state;
	state &= ~key_specials[SKEY_NUMLOCK].sk_mask;

	keymod_mask = 0;
	if (state & ShiftMask)
		keymod_mask |= KEYMOD_SHIFT;
	if (state & ControlMask)
		keymod_mask |= KEYMOD_CTRL;
	if (state & (Mod1Mask | Mod2Mask | Mod3Mask))
		keymod_mask |= KEYMOD_META;

	if (keymod_mask == (KEYMOD_META | KEYMOD_SHIFT) &&
	    keysym == XK_F10) {
	    	ctw->ctw.c_flags |= CTWF_CMD_MODE;
	    	ctw->ctw.c_idx = 0;
		command_input(ctw, keymod_mask, keysym, 0);
		return;
	    	}
	if (ctw->ctw.c_flags & CTWF_CMD_MODE) {
		command_input(ctw, keymod_mask, keysym, buf[0]);
		return;
		}

	if (crwin_debug) {
		printf("%s(%d): nbytes=%d keysym=0x%lx state=0x%x mask=0x%x\n", 
			__FILE__, __LINE__, nbytes, (long) keysym, event->xkey.state,
			keymod_mask);
		}

	if (ctw->ctw.flags[CTW_KEYBOARD_SCROLL] ||
	    !ctw->ctw.flags[CTW_APPL_KEYPAD]) {
		switch (keysym) {
		  case XK_Home:
		  case XK_KP_Home:
		  	ctw_set_top_line(ctw, 0);
			TRACE("XK_Home pressed");
			RETURN();
		  case XK_End:
		  case XK_KP_End:
			TRACE("XK_End pressed");
			reframe_screen(ctw);
			RETURN();
		  case XK_KP_Up:
		  case XK_Up:
		  case XK_R8:
		  	if (mask == 0)
				break;
			ctw_set_top_line(ctw, dsp_get_top(ctw) - 1);
		  	RETURN();
		  case XK_R9:
		  case XK_Prior:
		  case XK_KP_Prior:
			ctw_set_top_line(ctw, dsp_get_top(ctw) - ctw->ctw.rows);
		  	RETURN();
		  case XK_KP_Down:
		  case XK_Down:
		  case XK_R14:
		  	if (mask == 0)
				break;
			ctw_set_top_line(ctw, dsp_get_top(ctw) + 1);
		  	RETURN();
		  case XK_R15:
		  case XK_Next:
		  case XK_KP_Next:
			ctw_set_top_line(ctw, dsp_get_top(ctw) + ctw->ctw.rows);
		  	RETURN();
		  case XK_Insert:
		  case XK_KP_Insert:
		  	ctw_get_selection(ctw);
		  	RETURN();
		  default:
		  	break;
		  }
		}
	else {
		switch (is_ctw_modifier(mask) ? keysym : 0) {
		  case XK_Insert:
		  case XK_KP_Insert:
		  	ctw_get_selection(ctw);
		  	RETURN();
		  }
		}

	/***********************************************/
	/*   These  apply  no matter what mode we are  */
	/*   in.				       */
	/***********************************************/
	if (is_ctw_modifier(mask)) {
		switch (keysym) {
		  case XK_F1:
		  case XK_F2:
		  case XK_F3:
		  case XK_F4:
		  case XK_F5:
		  case XK_F6:
		  case XK_F7:
		  case XK_F8:
		  case XK_F9:
		  case XK_F10:
		  case XK_F11:
		  case XK_F12:
		  	reason.reason = CTWR_FUNCTION_KEY;
			reason.client_data = ctw->ctw.client_data;
			reason.key = keysym - XK_F1;
		  	XtCallCallbacks((Widget) w, XtNapplCallback, (caddr_t) &reason);
			RETURN();
		  }
		}

# if defined(XK_KP_Insert)
	if (keysym == XK_KP_Insert)
		keysym = XK_Insert;
# endif
# if defined(XK_KP_Delete)
	if (keysym == XK_KP_Delete) {
		keysym = XK_Delete;
		buf[0] = 0x7f;
		nbytes = 1;
		}
# endif
	if (keysym == XK_ISO_Left_Tab) {
		*rp++ = ESC;
		*rp++ = '\t';
		reason.len = 2;
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
		RETURN();
		}

	/***********************************************/
	/*   Map Linux num keypad keys.		       */
	/***********************************************/
	switch (keysym) {
	  case XK_KP_Prior: keysym = XK_Prior; break;
	  case XK_KP_Next: keysym = XK_Next; break;
	  case XK_KP_Left: keysym = XK_Left; break;
	  case XK_KP_Right: keysym = XK_Right; break;
	  case XK_KP_Down: keysym = XK_Down; break;
	  case XK_KP_Up: keysym = XK_Up; break;
	  }

	/***********************************************/
	/*   Handle special keys here.		       */
	/***********************************************/
	for (k = 0; k < (int) (sizeof keytbl / sizeof keytbl[0]); k++) {
		if (keytbl[k].k_flags & mask &&
		    keytbl[k].k_keysym == keysym) {
		    	strcpy(rp, keytbl[k].k_value);
			reason.len = strlen(rp);
			XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
			RETURN();
			}
		}
/*printf("   %d: keysym=%x mask=%x\n", k, keysym, keymod_mask);*/
	for (k = 0; k < (int) (sizeof keymapping / sizeof keymapping[0]); k++) {
/*if (keymap[k].k_keysym == keysym) {
printf("   %d: keysym=%x mask=%x flags=%02x\n", k, keysym, keymod_mask, keymap[k].k_flags);
}*/
		if (keymapping[k].k_flags == keymod_mask &&
		    keymapping[k].k_keysym == keysym &&
		    keymapping[k].k_value) {
/*printf("%d: mask=%x flags=%02x\n", k, keymod_mask, keymap[k].k_flags);*/
		    	strcpy(rp, keymapping[k].k_value);
			reason.len = strlen(rp);
			XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
			RETURN();
			}
		}

	if (IsPFKey(keysym)) {
		*rp++ = ESC;
		*rp++ = 'O';
		*rp = keysym - XK_KP_F1 + 'P';
		reason.len = 3;
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
		RETURN();
		}
	
	if (IsKeypadKey(keysym)) {
		if (ctw->ctw.flags[CTW_APPL_KEYPAD]) {
			*rp++ = ESC;
			*rp++ = 'O';
			*rp = kypd_apl[keysym - XK_KP_Space];
			reason.len = 3;
			}
		else  {
			*rp = kypd_num[keysym - XK_KP_Space];
			reason.len = 1;
			}
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
		RETURN();
		}
	
	if (IsCursorKey(keysym) && 
	        keysym != XK_Prior && keysym != XK_Next &&
		keysym != XK_Home && keysym != XK_End && keysym != XK_Insert) {
		switch (keysym) {
		  default:
			*rp++ = ESC;
			if (ctw->ctw.flags[CTW_CURSOR_KEYPAD])
				*rp++ = 'O';
			else
				*rp++ = '[';
			*rp = cur[keysym - XK_Left];
			reason.len = 3;
			break;
		  }
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
		RETURN();
		}
	
	if (IsFunctionKey(keysym) || IsMiscFunctionKey(keysym) ||
		keysym == XK_Prior || keysym == XK_Next ||
		keysym == XK_Home || keysym == XK_End) {
		*rp++ = ESC;
		*rp++ = '[';
		if (ctw->ctw.sun_function_keys) {
			n = sunfuncvalue(keysym);
			if (n < 0)
				RETURN();
			sprintf(rp, "%dz", n);
			}
		else {
			sprintf(rp, "%d~", funcvalue(keysym));
			}
		rp += strlen(rp);
		reason.len = rp - reply;
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
		RETURN();
		}
	
	if (nbytes > 0) {
		reason.ptr = buf;
		reason.len = nbytes;
		}

	if (crwin_debug) {
		printf("%s(%d): Return %d: '%*.*s'\n",
			__FILE__, __LINE__, reason.len, reason.len, reason.len, reason.ptr);
		}
	if (reason.len)
		XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Function  to  grab  contents  of  selection  and give to owning  */
/*   application.						      */
/**********************************************************************/
static void 
CtwInsertSelection(Widget w, XEvent *event, String *x, Cardinal *y)
{
	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, requestor_callback, 
		NULL, event->xbutton.time);
}
/**********************************************************************/
/*   Detect mouse moving over a hot spot.			      */
/**********************************************************************/
static void 
CtwMotion(CtwWidget ctw, void *data, XEvent *event)
{	graph_t	*g;
static Cursor cursor;

	UNUSED_PARAMETER(data);

	g = graph_click(ctw, event->xmotion.x, event->xmotion.y, FALSE);
	if (g == NULL) {
		if (ctw->ctw.c_chain_mouse == NULL)
			return;
		ctw->ctw.c_chain_mouse = NULL;
		XDefineCursor(XtDisplay(ctw),  XtWindow(ctw), None);
		return;
		}
	if (g == ctw->ctw.c_chain_mouse)
		return;

	if (cursor == (Cursor) 0)
		cursor = XCreateFontCursor(XtDisplay(ctw), XC_right_ptr);
	XDefineCursor(XtDisplay(ctw),  XtWindow(ctw), cursor);
	ctw->ctw.c_chain_mouse = g;
}
/**********************************************************************/
/*   Xterm compatible routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectEnd(CtwWidget w, XEvent *event, String *str, Cardinal *num)
{	int	r, t, len;
	int	y, x, end_x;
	vbyte_t	*vp;
	int	cur;
	int	start;
	char	*cp;
	int	nl;
	ctw_callback_t reason;

	if (!mouse_emulate) {
		reason.reason = CTWR_BUTTON_UP;
		reason.client_data = w->ctw.client_data;
		reason.event = event;
		XtCallCallbacks((Widget) w, XtNmouseCallback, (caddr_t) &reason);
		}

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		CtwButtonUp(w, event, str, num);
		return;
		}

	/***********************************************/
	/*   Turn cursor back on.		       */
	/***********************************************/
	w->ctw.cursor_state = CURSOR_OFF;
	turn_on_cursor(w);

	if (w->ctw.sel_start_x < 0 || w->ctw.sel_start_y < 0)
		return;

	/***********************************************/
	/*   Look   to  ensure  sel_start  is  before  */
	/*   sel_cur.				       */
	/***********************************************/
	start = w->ctw.sel_start_y * w->ctw.columns + w->ctw.sel_start_x;
	cur = w->ctw.sel_cur_y * w->ctw.columns + w->ctw.sel_cur_x;
	if (cur < start) {
		t = w->ctw.sel_start_x;
		w->ctw.sel_start_x = w->ctw.sel_cur_x;
		w->ctw.sel_cur_x = t;

		t = w->ctw.sel_start_y;
		w->ctw.sel_start_y = w->ctw.sel_cur_y;
		w->ctw.sel_cur_y = t;
		}

	/***********************************************/
	/*   Work  out  how  much  room is needed for  */
	/*   copy.  We  may  allocate  too  much  but  */
	/*   thats better than too little.	       */
	/***********************************************/
	if (w->ctw.sel_start_y == w->ctw.sel_cur_y) {
		len = compute_length(w, 
			w->ctw.sel_start_y, 
			w->ctw.sel_start_x, 
			w->ctw.sel_cur_x) + 1;
		if (len <= 1)
			return;
		}
	else {
		len = compute_length(w, w->ctw.sel_start_y, w->ctw.sel_start_x, 
			w->ctw.columns) + 1;
		for (r = w->ctw.sel_start_y + 1; r < w->ctw.sel_cur_y; r++)
			len += compute_length(w, r, 0, w->ctw.columns) + 1;
		len += compute_length(w, r, 0, w->ctw.sel_cur_x) + 1;
		}
	if (len <= 1)
		return;

	/***********************************************/
	/*   Dont  touch  the  clipboard if we are in  */
	/*   virtual slave mode.		       */
	/***********************************************/
	if (mouse_emulate)
		return;

	/***********************************************/
	/*   Free up any previous selection.	       */
	/***********************************************/
	chk_free_ptr((void **) &w->ctw.sel_string);
	w->ctw.sel_length = len;
	w->ctw.sel_string = (char *) chk_alloc(len + 1);
/*printf("len allocated=%d\n", len + 1);*/
	cp = w->ctw.sel_string;
	y = w->ctw.sel_start_y;
	x = w->ctw.sel_start_x;
	while (y <= w->ctw.sel_cur_y) {
		line_t	*lp;

		if (y == w->ctw.sel_cur_y)
			end_x = w->ctw.sel_cur_x;
		else {
			end_x = w->ctw.columns;
			}
		lp = dsp_get_row(w, y);
		vp = &lp->l_text[x];
		/***********************************************/
		/*   Remove trailing spaces.		       */
		/***********************************************/
		len = compute_length(w, y, x, end_x);
		if (y != w->ctw.sel_cur_y && 
		    (x + len != w->ctw.columns || w->ctw.flags[CTW_CUT_NEWLINES]))
			nl = '\n';
		else
			nl = '\0';
		end_x = x + len;
		end_x = MIN(end_x, w->ctw.columns);
		while (x++ < end_x) {
			if ((vp->vb_attr & VB_LINE) == 0)
				*cp++ = vp->vb_byte;
			vp++;
			}
		/***********************************************/
		/*   Trim trailing white space.		       */
		/***********************************************/
		if ((lp->l_attr & LA_CONTINUED) == 0) {
			while (cp > w->ctw.sel_string + 1 && cp[-1] == ' ')
				cp--;
			}
		if (nl && ((lp->l_attr & LA_CONTINUED) == 0 || 
		    w->ctw.flags[CTW_APPL_KEYPAD])) {
			*cp++ = (char) nl;
			}
		y++;
		x = 0;
		}
	*cp = '\0';
/*printf("len used=%d\n", cp - w->ctw.sel_string + 1);*/

	/***********************************************/
	/*   Try and grab ownership of selection.      */
	/***********************************************/
	{static int sels[3] = {XA_PRIMARY, XA_SECONDARY};
	int i;
	sels[2] = atom_clipboard;
	might_have_selection = 0;
	for (i = 0; i < (int) (sizeof sels / sizeof sels[0]); i++) {
		might_have_selection++;
		if (XtOwnSelection((Widget) w, sels[i], event->xbutton.time,
			convert_proc, lose_selection, NULL) == FALSE) {
			XBell(XtDisplay(w), 100);
			}
		}
	}
}
/**********************************************************************/
/*   Xterm compatable routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectExtend(CtwWidget ctw, XEvent *event, String *str, Cardinal *num)
{	vbyte_t	*vpleft, *vp, *vpright;
	int	x, y;
	int	start, cur;
	int	is_word;
	char	*word_table;
	char	word_chars2[3];
	ctw_callback_t reason;
	line_t	*lp;

	UNUSED_PARAMETER(str);
	UNUSED_PARAMETER(num);

	if (!mouse_emulate) {
		reason.reason = CTWR_BUTTON_MOTION;
		reason.client_data = ctw->ctw.client_data;
		reason.event = event;
		XtCallCallbacks((Widget) ctw, XtNmouseCallback, (caddr_t) &reason);
		}

	if (ctw->ctw.flags[CTW_APPL_MOUSE]) {
		CtwButtonMotion(ctw, event, str, num);
		return;
		}

	ctw_get_xy(ctw, &y, &x, event->xbutton.x, event->xbutton.y);
	y += dsp_get_top(ctw);

	start = ctw->ctw.sel_start_y * ctw->ctw.columns + ctw->ctw.sel_start_x;
	cur = ctw->ctw.sel_cur_y * ctw->ctw.columns + ctw->ctw.sel_cur_x;

	lp = dsp_get_row(ctw, y);

	switch (ctw->ctw.num_clicks) {
	  case CLICK_WORD:
# define	IS_WORD(vp)	(strchr(word_table, vp->vb_byte) != (char *) NULL)
	  	vp = &lp->l_text[x];
	  	vpleft = &lp->l_text[0];
	  	vpright = &lp->l_text[ctw->ctw.columns];
		if (strchr(word_chars, vp->vb_byte))
			word_table = word_chars;
		else {
			word_chars2[0] = vp->vb_byte;
			word_chars2[1] = '\0';
			word_table = word_chars2;
			}
		is_word = IS_WORD(vp);
	  	if (cur < start) {
			while (vp >= vpleft) {
				if (is_word && !IS_WORD(vp)) {
					vp++;
					break;
					}
				else if (!is_word && IS_WORD(vp)) {
					vp++;
					break;
					}
				vp--;
				}
			if (vp < vpleft)
				vp = vpleft;
			}
		else {
			while (vp < vpright) {
				if (is_word && !IS_WORD(vp)) {
					break;
					}
				else if (!is_word && IS_WORD(vp)) {
					break;
					}
				vp++;
				}
			}
		x = vp - vpleft;
# undef IS_WORD
		break;
	  case CLICK_LINE:
		x = ctw->ctw.columns;
		ctw->ctw.sel_start_x = 0;
		break;
	  }

	
	{
	char	*s;
	int	sy, ey;
	int	ox, oy;

	hilite(ctw, ctw->ctw.sel_start_x, ctw->ctw.sel_start_y, 
		ctw->ctw.sel_cur_x, ctw->ctw.sel_cur_y, FALSE, NULL);
	
	sy = MIN(y, ctw->ctw.sel_start_y);
	sy = MIN(sy, ctw->ctw.sel_cur_y);
	ey = MAX(y, ctw->ctw.sel_start_y);
	ey = MAX(ey, ctw->ctw.sel_cur_y);

	ox = ctw->ctw.sel_cur_x;
	oy = ctw->ctw.sel_cur_y;

	hilite(ctw, ctw->ctw.sel_start_x, ctw->ctw.sel_start_y, x, y, TRUE, NULL);

	ctw->ctw.sel_cur_x = x;
	ctw->ctw.sel_cur_y = y;

	s = ctw->ctw.sel_string;
	ctw->ctw.sel_string = "";
	if (ctw->ctw.sel_start_y <= y && oy == y) {
		update_region(ctw, 
			ey - dsp_get_top(ctw),
			ctw->ctw.num_clicks ? 0 : MIN(x, ox),
			ey + 1 - dsp_get_top(ctw),
			ctw->ctw.columns);
		}
	else if (ctw->ctw.sel_start_y <= y && oy < y) {
		update_region(ctw, 
			oy - dsp_get_top(ctw),
			0,
			ey + 1 - dsp_get_top(ctw),
			ctw->ctw.columns);
		}
	else if (ctw->ctw.sel_start_y > y && oy == y) {
		update_region(ctw, 
			oy - dsp_get_top(ctw),
			MIN(x, ox),
			oy + 1 - dsp_get_top(ctw),
			ctw->ctw.columns);
		}
	else if (ctw->ctw.sel_start_y > y && oy > y) {
		update_region(ctw, 
			y - dsp_get_top(ctw),
			0,
			oy + 1 - dsp_get_top(ctw),
			ctw->ctw.columns);
		}
	else {
		update_region(ctw, 
			sy - dsp_get_top(ctw),
			0,
			ey + 1 - dsp_get_top(ctw),
			ctw->ctw.columns);
		}
	ctw->ctw.sel_string = s;
	}
	graph_redraw(ctw, NULL);
}
/**********************************************************************/
/*   Xterm compatable routine for making a selection.		      */
/**********************************************************************/
static void 
CtwSelectStart(CtwWidget ctw, XEvent *event, String *x, Cardinal *y)
{	int	r, c;
	ctw_callback_t	reason;

	if (ctw->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(ctw, event->xbutton.state, 0, event, 
			event->xbutton.x,
			event->xbutton.y);
		return;
		}

	if (graph_click(ctw, event->xbutton.x, event->xbutton.y, TRUE))
		return;

	memset((char *) &reason, 0, sizeof reason);

	if ((unsigned long) ((long) event->xbutton.time - ctw->ctw.timestamp) >
		(unsigned long) ctw->ctw.multiClickTime)
		ctw->ctw.num_clicks = 0;
	else
		ctw->ctw.num_clicks = (ctw->ctw.num_clicks + 1) & 3;
	ctw->ctw.timestamp = event->xbutton.time;

	/***********************************************/
	/*   Unhilite  any  previous selection before  */
	/*   starting the new one.		       */
	/***********************************************/
	if (ctw->ctw.sel_string || mouse_emulate) {
		if (ctw->ctw.sel_start_y >= 0) {
			hilite(ctw, ctw->ctw.sel_start_x, ctw->ctw.sel_start_y,
				ctw->ctw.sel_cur_x + 1,
				ctw->ctw.sel_cur_y, FALSE, NULL);
			unhilite(ctw);
			}
		}

	ctw_get_xy(ctw, &r, &c, event->xbutton.x, event->xbutton.y);

	ctw->ctw.sel_start_x = c;
	ctw->ctw.sel_start_y = dsp_get_top(ctw) + r;

	ctw->ctw.sel_cur_x = ctw->ctw.sel_start_x;
	ctw->ctw.sel_cur_y = ctw->ctw.sel_start_y;

	convert_click(ctw);

	/***********************************************/
	/*   Turn  off  cursor  and stop the flashing  */
	/*   code from turning it back on.	       */
	/***********************************************/
	turn_off_cursor(ctw);
	ctw->ctw.cursor_state = CURSOR_HIDDEN;

	/***********************************************/
	/*   Tell  user  that we're doing a selection  */
	/*   so  its  best not to keep adding text to  */
	/*   the window to avoid scrolling problems.   */
	/***********************************************/
	if (!mouse_emulate) {
		reason.reason = CTWR_SELECTION;
		reason.event = event;
		reason.client_data = ctw->ctw.client_data;
		XtCallCallbacks((Widget) ctw, XtNmouseCallback, (caddr_t) &reason);
		}

	/***********************************************/
	/*   Need   to  highlight  current  word/line  */
	/*   depending on click state.		       */
	/***********************************************/
	switch (ctw->ctw.num_clicks) {
	  case CLICK_WORD:
	  	/***********************************************/
	  	/*   Need  to do word to the left and word to  */
	  	/*   the right.                                */
	  	/***********************************************/
		ctw->ctw.sel_start_x = ctw->ctw.sel_cur_x + 1;
		CtwSelectExtend(ctw, event, x, y);
		ctw->ctw.sel_start_x = ctw->ctw.sel_cur_x;
		CtwSelectExtend(ctw, event, x, y);
		break;
	  case CLICK_LINE:
		CtwSelectExtend(ctw, event, x, y);
		break;
	  }
	graph_redraw(ctw, NULL);
}
/**********************************************************************/
/*   Xterm compatable string action routine.			      */
/**********************************************************************/
static void 
CtwString(Widget w, XEvent *event, String *x, Cardinal *y)
{	register int	i;
	register char	*cp;
	ctw_callback_t	reason;
	KeySym	keysym;
	int	nbytes;
	char	buf[BUFSIZ];
	static XComposeStatus compose_status;
	CtwWidget	ctw = (CtwWidget) w;
	int	mask;

	/***********************************************/
	/*   Let us do special keys even if remapped.  */
	/***********************************************/
	nbytes = XLookupString((XKeyEvent *) event, buf, sizeof buf,
		&keysym, &compose_status);
	mask = event->xkey.state & (ShiftMask | ControlMask | 
		Mod1Mask | Mod2Mask | Mod3Mask);
	if ((keysym == XK_Insert || keysym == XK_KP_Insert) &&
	    !ctw->ctw.flags[CTW_APPL_KEYPAD]) {
	  	ctw_get_selection(ctw);
		return;
		}
	
	switch (is_ctw_modifier(mask) ? keysym : 0) {
	  case XK_Insert:
	  case XK_KP_Insert:
	  	ctw_get_selection(ctw);
	  	return;
	  case XK_F1:
	  case XK_F2:
	  case XK_F3:
	  case XK_F4:
	  case XK_F5:
	  case XK_F6:
	  case XK_F7:
	  case XK_F8:
	  case XK_F9:
	  case XK_F10:
	  case XK_F11:
	  case XK_F12:
	  	reason.reason = CTWR_FUNCTION_KEY;
		reason.client_data = ctw->ctw.client_data;
		reason.key = keysym - XK_F1;
	  	XtCallCallbacks((Widget) w, XtNapplCallback, (caddr_t) &reason);
		return;
	  }

	memset((char *) &reason, 0, sizeof reason);
	reason.reason = CTWR_INPUT;
	reason.client_data = ctw->ctw.client_data;
	for (i = 0; i < (int) *y; i++) {
		cp = x[i];
		reason.ptr = cp;
		reason.len = strlen(cp);
		XtCallCallbacks(w, XtNkbdCallback, (caddr_t) &reason);
		}
}
/**********************************************************************/
/*   Handle mouse wheel events.					      */
/**********************************************************************/
static int wheel_decay;
static	ctw_callback_t wheel_reason;
static int ctw_wheel_init_decay = 5;
static int ctw_wheel_incr_decay = 10;
static void
CtwWheel_timeout_proc(XtPointer client_data, XtIntervalId *timer)
{	CtwWidget	ctw = (CtwWidget) client_data;

	UNUSED_PARAMETER(timer);

# define WHEEL_DECAY(decay) (decay >= 10 ? 5 : (100 - decay * 10 + 10))

/*printf("CtwWheel_timeout_proc decay=%d\n", wheel_decay);*/
	if (wheel_decay-- < 0) {
		wheel_timer = 0;
		return;
		}

	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &wheel_reason);
	wheel_timer = XtAppAddTimeOut(
		XtWidgetToApplicationContext((Widget) ctw),
		(long) WHEEL_DECAY(wheel_decay), CtwWheel_timeout_proc, ctw);
}
static void 
CtwWheel(CtwWidget w, XEvent *event, String *argv, Cardinal y)
{static int first_time = TRUE;

	UNUSED_PARAMETER(y);

	if (first_time) {
		char *cp;
		if ((cp = getenv("CTW_WHEEL_INIT_DECAY")) != NULL)
			ctw_wheel_init_decay = atoi(cp);
		if ((cp = getenv("CTW_WHEEL_INCR_DECAY")) != NULL)
			ctw_wheel_incr_decay = atoi(cp);
		first_time = FALSE;
		}

/*printf("==\n");*/
	if (wheel_timer) {
		XtRemoveTimeOut(wheel_timer);
		wheel_timer = 0;
		wheel_decay += ctw_wheel_incr_decay;
		}
	else
		wheel_decay = ctw_wheel_init_decay;

	if (w->ctw.flags[CTW_APPL_MOUSE]) {
		application_mouse(w, event->xbutton.state, 0, event, 
			event->xbutton.x,
			event->xbutton.y);
		return;
		}
	w->ctw.timestamp = event->xbutton.time;

	wheel_reason.reason = atoi(argv[0]) ? CTWR_WHEEL_UP : CTWR_WHEEL_DOWN;
	wheel_reason.client_data = w->ctw.client_data;
	wheel_reason.event = event;
	XtCallCallbacks((Widget) w, XtNapplCallback, (caddr_t) &wheel_reason);

	wheel_timer = XtAppAddTimeOut(
		XtWidgetToApplicationContext((Widget) w),
		(long) WHEEL_DECAY(wheel_decay), CtwWheel_timeout_proc, w);

}
/**********************************************************************/
/*   Function to handle the application mouse mode.		      */
/**********************************************************************/
static void
application_mouse(CtwWidget w, int state, int flag, XEvent *event, int x, int y)
{	ctw_callback_t	reason;
	char	buf[64];
	int	r, c;
	int	m = 0;

	ctw_get_xy(w, &r, &c, x, y);
	/***********************************************/
	/*   Calculate modifer byte value.	       */
	/***********************************************/
	if (state & ShiftMask)
		m |= 0x01;
	if (state & ControlMask)
		m |= 0x02;
	if (state & (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask))
		m |= 0x04;

	snprintf(buf, sizeof buf, "\033[%d;%d;%d;%d;%dM", 
		event->xbutton.button, 
		flag,
		m, r, c);
	reason.reason = CTWR_INPUT;
	reason.client_data = w->ctw.client_data;
	reason.ptr = buf;
	reason.len = strlen(buf);
	XtCallCallbacks((Widget) w, XtNkbdCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Blank a line, e.g. because we are scrolling up.		      */
/**********************************************************************/
static void
blank_line(CtwWidget w, int row)
{	line_t	*lp = dsp_get_row(w, row);
	blank_line_ptr(w, lp);
}
/**********************************************************************/
/*   Blank a physical line pointer - so we can handle resize.	      */
/**********************************************************************/
static void
blank_line_ptr(CtwWidget ctw, line_t *lp)
{
	vbyte_t	*vp = lp->l_text;
	int	i;
	struct copy {
		vbyte_t	array[80];
		};
	static int	space_fg = -1;
	static int	space_bg = -1;
	static struct copy blank;
	int	black_fg;
	int	black_bg;

/*printf("%p BLANK: row=%d ", ctw, row); for (i = 0; i < 20; i++) putchar(lp->l_text[i].vb_byte); printf("\n");*/
	if (fill_to_black) {
		black_fg = ctw->ctw.blank.vb_fcolor;
		black_bg = ctw->ctw.blank.vb_bcolor;
		}
	else {
		black_fg = ctw->ctw.attr.vb_fcolor;
		black_bg = ctw->ctw.attr.vb_bcolor;
		}
	if (black_fg != space_fg && black_bg != space_bg) {
		space_fg = black_fg;
		space_bg = black_bg;
		for (i = 0; i < 80; i++) {
			vbyte_clear(&blank.array[i]);
			blank.array[i].vb_fcolor = space_fg;
			blank.array[i].vb_bcolor = space_bg;
			}
		}

	lp->l_attr &= ~LA_CONTINUED;
	for (i = ctw->ctw.columns; i >= 80; i -= 80) {
		*(struct copy *) vp = blank;
		vp += 80;
		}
	while (i-- > 0) {
		*vp++ = blank.array[0];
		}
}
/**********************************************************************/
/*   Routine  to  change  icon  title or window name but only if its  */
/*   different from the current one.				      */
/**********************************************************************/
static void
change_name(CtwWidget ctw, int flags, char *str)
{	ctw_callback_t	reason;

	reason.reason = CTWR_SET_TITLE;
	reason.client_data = ctw->ctw.client_data;
	reason.flags = flags;
	reason.ptr = str;
	reason.len = strlen(str);
	/***********************************************/
	/*   Let  the owner decide to set the title -  */
	/*   since he may want to augment.	       */
	/***********************************************/
	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Draw the command line prompt.				      */
/**********************************************************************/
static void
command_input(CtwWidget ctw, int keymod_mask, int keysym, int ch)
{
	UNUSED_PARAMETER(keymod_mask);

/*
      printf("draw command %04x %02x idx=%d '%s'\n", 
		keysym, ch,
		ctw->ctw.c_idx,
		ctw->ctw.c_ibuf);
*/
	if (ch == 0x0d) {
		chk_free_ptr((void **) &ctw->ctw.c_search);
		if (strncmp(ctw->ctw.c_ibuf, "cmap", 4) == 0) {
			int	r, g, b;
			char	buf[64];

			ctw_add_string(ctw, "Color map ESC [ 38 ; 5 ; nn m\r\n", -1);
			ctw_add_string(ctw, "Color cube 6x6x6\r\n", -1);
			for (g = 0; g < 6; g++) {
				for (r = 0; r < 6; r++) {
					for (b = 0; b < 6; b++) {
						int c = 16 + r * 36 + g * 6 + b;
						snprintf(buf, sizeof buf, "\x1b[48;5;%dm  ", c);
						ctw_add_string(ctw, buf, -1);
						}
					ctw_add_string(ctw, "\x1b[48;5;0m ", -1);
					}
				ctw_add_string(ctw, "\r\n", -1);
				}

			ctw_add_string(ctw, "Grayscale ramp\r\n", -1);
			for (r = 232; r < 256; r++) {
				snprintf(buf, sizeof buf, "\x1b[48;5;%dm  ", r);
				ctw_add_string(ctw, buf, -1);
				}
			ctw_add_string(ctw, "\r\n", -1);
			ctw_add_string(ctw, "Normal colors\r\n", -1);
			for (r = 0; r < 8; r++) {
				snprintf(buf, sizeof buf, "\x1b[%d;%dm  %d  ", 
					r == 0 ? 37 : 30,
					r + 40,
					r);
				ctw_add_string(ctw, buf, -1);
			}
			ctw_add_string(ctw, "\r\n", -1);
			ctw_add_string(ctw, "\x1b[48;5;0m\n", -1);
			}
		else if (strncmp(ctw->ctw.c_ibuf, "code", 4) == 0) {
			ctw_add_string(ctw, code_text, -1);
			}
		else if (strncmp(ctw->ctw.c_ibuf, "esc", 3) == 0) {
			ctw_add_string(ctw, sequences, -1);
			}
		else if (strncmp(ctw->ctw.c_ibuf, "help", 4) == 0) {
			ctw_add_string(ctw, "\r\nCommands available:\r\n", -1);
			ctw_add_string(ctw, "  cmap   show colormap.\r\n", -1);
			ctw_add_string(ctw, "  code   show sample code for drawing graphics.\r\n", -1);
			ctw_add_string(ctw, "  esc    show escape sequence summary.\r\n", -1);
			ctw_add_string(ctw, "\r\n", -1);
			}
		else if (ctw->ctw.c_ibuf[0]) {
			ctw->ctw.c_search = chk_strdup(ctw->ctw.c_ibuf);
			}
	    	ctw->ctw.c_flags &= ~CTWF_CMD_MODE;
		update_region(ctw, 0, 0, ctw->ctw.rows, ctw->ctw.columns);
		return;
		}
	if (ch == 0x0d || ch == 0x1b) {
	    	ctw->ctw.c_flags &= ~CTWF_CMD_MODE;
		update_region(ctw, ctw->ctw.rows - 1, 0, ctw->ctw.rows - 1, ctw->ctw.columns);
		return;
		}
	ctw->ctw.c_flags |= CTWF_CMD_MODE;

	if (keysym == XK_Home || keysym == XK_KP_Home)
		ctw->ctw.c_idx = 0;
	else if (keysym == XK_End || keysym == XK_KP_End)
		ctw->ctw.c_idx = strlen(ctw->ctw.c_ibuf);
	else if (keysym == XK_Left || keysym == XK_KP_Left)
		ctw->ctw.c_idx = MAX(0, ctw->ctw.c_idx - 1);
	else if (keysym == XK_Right || keysym == XK_KP_Right) {
		if (ctw->ctw.c_ibuf[ctw->ctw.c_idx])
			ctw->ctw.c_idx++;
		}
	else {
		switch (ch) {
		  case 0:
		  	break;
		  case '\b':
			if (ctw->ctw.c_idx)
				ctw->ctw.c_idx--;
			memmove(&ctw->ctw.c_ibuf[ctw->ctw.c_idx],
				&ctw->ctw.c_ibuf[ctw->ctw.c_idx + 1],
				strlen(&ctw->ctw.c_ibuf[ctw->ctw.c_idx] + 1) + 1);
			break;
		  case 0x7f:
			memmove(&ctw->ctw.c_ibuf[ctw->ctw.c_idx],
				&ctw->ctw.c_ibuf[ctw->ctw.c_idx + 1],
				strlen(&ctw->ctw.c_ibuf[ctw->ctw.c_idx] + 1) + 1);
		  	break;
		  case 'u' & 0x1f:
			memmove(&ctw->ctw.c_ibuf[0],
				&ctw->ctw.c_ibuf[ctw->ctw.c_idx + 1],
				strlen(&ctw->ctw.c_ibuf[ctw->ctw.c_idx] + 1) + 1);
			ctw->ctw.c_idx = 0;
		  	break;
		  default:
			if (ctw->ctw.c_ibuf[ctw->ctw.c_idx] == '\0') {
				ctw->ctw.c_ibuf[ctw->ctw.c_idx++] = ch;
				ctw->ctw.c_ibuf[ctw->ctw.c_idx] = '\0';
				}
			else {
				memmove(&ctw->ctw.c_ibuf[ctw->ctw.c_idx+1],
					&ctw->ctw.c_ibuf[ctw->ctw.c_idx],
					strlen(&ctw->ctw.c_ibuf[ctw->ctw.c_idx]) + 1);
				ctw->ctw.c_ibuf[ctw->ctw.c_idx++] = ch;
				}
			break;
		  }
		}
	command_draw(ctw);
}
static void
command_draw(CtwWidget ctw)
{	line_t	lbuf;
	vbyte_t vbytes[80];
	char	*cp;
	int	c = 0;

	memset(&lbuf, 0, sizeof lbuf);
	memset(vbytes, 0, sizeof vbytes);
	lbuf.l_text = vbytes;
	for (c = 0; c < 80; c++) {
		vbytes[c].vb_byte = ' ';
		vbytes[c].vb_fcolor = 1;
		vbytes[c].vb_fcolor = 3;
		}

	c = 0;
	for (cp = "Command: "; *cp; c++) {
		vbytes[c].vb_byte = *cp++;
		}
	for (cp = ctw->ctw.c_ibuf; *cp; c++) {
		vbytes[c].vb_byte = *cp++;
		}
	print_string(ctw, &lbuf, ctw->ctw.rows-1, 0, 80);

}
/**********************************************************************/
/*   Function  to  compute  the  length  of text in a line ready for  */
/*   cutting.							      */
/**********************************************************************/
static int
compute_length(CtwWidget ctw, int row, int start_col, int end_col)
{	vbyte_t	*vp;
	vbyte_t	*svp;
	line_t	*lp;

	if (end_col != ctw->ctw.columns)
		return end_col - start_col;

/*printf("compute_length %d %d %d ", row, start_col, end_col);*/
	lp = dsp_get_row(ctw, row);
	if (lp->l_attr & LA_CONTINUED) {
		return end_col - start_col;
		}

	end_col = MIN(end_col, ctw->ctw.columns - 1);
	vp = lp->l_text + end_col;
	svp = lp->l_text + start_col;
	while (vp > svp) {
		if (vp->vb_byte != ' ') {
/*printf(" = %d (short)\n", (vp - svp) + 1);*/
			return vp - svp + 1;
			}
		vp--;
		}
	/***********************************************/
	/*   The  +1  is  because  the  positions are  */
	/*   inclusive.                                */
	/***********************************************/
/*printf(" = %d\n", (vp - svp));*/
	return (vp - svp) + 1;
}
/**********************************************************************/
/*   Routine   to  convert  co-ordinates  got  via  a  button  click  */
/*   depending on how many clicks were typed.			      */
/**********************************************************************/
static void
convert_click(CtwWidget ctw)
{
	if (ctw->ctw.num_clicks == CLICK_LINE) {
		ctw->ctw.sel_start_x = 0;
		ctw->ctw.sel_cur_x = ctw->ctw.columns;
		update_region(ctw, 
			ctw->ctw.sel_start_y - dsp_get_top(ctw),
			0,
			ctw->ctw.sel_start_y - dsp_get_top(ctw) + 1,
			ctw->ctw.columns);
		}
}
/**********************************************************************/
/*   Function   called   when   some  other  application  wants  the  */
/*   selection.							      */
/**********************************************************************/
static Boolean
convert_proc(CtwWidget ctw, Atom *selection, Atom *target, Atom *type, XtPointer *value, unsigned long *length, int *format)
{	static Atom	targets = 0;

	UNUSED_PARAMETER(selection);

	if (ctw->ctw.sel_string == (char *) NULL)
		return FALSE;

	if (targets == 0) {
		targets = FetchAtom((Widget) ctw, "TARGETS");
		}

	if (*target == targets) {
		*type = XA_ATOM;
		*value = (XtPointer) XtNew(Atom);
		*(Atom *) *value = XA_STRING;
		*length = 1;
		*format = 32;
		return TRUE;
		}

	if (*target == XA_STRING) {
		*type = XA_STRING;
		*value = (XtPointer) XtNewString(ctw->ctw.sel_string);
		*length = strlen(*value);
		*format = 8;
		return TRUE;
		}
	return FALSE;
}
/**********************************************************************/
/*   Create the container dir for fcterm.			      */
/**********************************************************************/
static void
create_fcterm_dir(CtwWidget ctw)
{	char	buf[BUFSIZ];
	struct stat sbuf;

	snprintf(buf, sizeof buf, "%s/%s", 
			ctw->ctw.log_dir,
			getenv("USER") ? getenv("USER") : "fcterm");
	if (stat(buf, &sbuf) == 0)
		return;
	if (mkdir(buf, 0700) < 0) {
		fprintf(stderr, "fcterm: mkdir(%s) failed\n", buf);
		perror("error");
	}
}
/**********************************************************************/
/*   Do a fast delete line.					      */
/**********************************************************************/
static void
delete_line(CtwWidget ctw, int start_line, int num_lines)
{	int	width, height;
	int	src_x, src_y, dst_y;
	int	h;

	if (ctw->ctw.nodraw)
		return;

	width = ctw->ctw.font_width * ctw->ctw.columns;
	height = ctw->ctw.font_height * (ctw->ctw.scroll_bot - start_line - num_lines);

	src_x = 0;
	src_y = ctw->ctw.font_height * (start_line + num_lines);
	dst_y = ctw->ctw.font_height * start_line;
	if (height > 0) {
		wait_for_exposure(ctw);
		XCopyArea(XtDisplay(ctw), XtWindow(ctw), XtWindow(ctw), ctw->ctw.gc,
			ctw->ctw.internal_width, src_y + ctw->ctw.internal_height, width, height,
			ctw->ctw.internal_width, dst_y + ctw->ctw.internal_height);
		}
	h = ctw->ctw.font_height * ctw->ctw.scroll_bot - (dst_y + height);
	if (h > 0)
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			ctw->ctw.internal_width, dst_y + height + ctw->ctw.internal_height,
			ctw->ctw.internal_width + ctw->ctw.font_width * ctw->ctw.columns,
			h,
			FALSE);
}
/**********************************************************************/
/*   Handle  ESC@n  escape  sequences.  Not  sure  what  they do but  */
/*   USL's termcap contains a description of them for xterms.	      */
/**********************************************************************/
static  int
do_at_parms(CtwWidget ctw)
{
	switch (ctw->ctw.escbuf[1]) {
	  case '0':
		break;
	  case '1':
		break;
	  case '2':
		break;
	  case '3':
		break;
	  }
	return 1;
}
static  int
do_csi_gt(CtwWidget ctw, int cmd, int arg_no, int *args)
{
	UNUSED_PARAMETER(ctw);
	UNUSED_PARAMETER(arg_no);
	UNUSED_PARAMETER(args);

	switch (cmd) {
	  case 'c':
	  	/***********************************************/
	  	/*   Secondary device request.		       */
	  	/***********************************************/
	  	return TRUE;
	  }
	return FALSE;
}
/**********************************************************************/
/*   Handle ESC ? escape sequences.				      */
/**********************************************************************/
static  int
do_csi_quest(CtwWidget ctw, int cmd, int arg_no, int *args)
{	int	flag = TRUE;
	int	r, c, h, width, num;
	struct font_stack	*fp;
	ctw_callback_t	reason;
	int	n;
	Arg	arg[20];

	switch (cmd) {
	  case 'f':
	  case 'F':
	  	switch (args[0]) {
		  case 0: 
		  	ctw->ctw.flags[CTW_GRAPHICS_EXPOSE] = cmd == 'F';
		  	return TRUE;
		  case 1:
	  		n = ctw->ctw.attr.vb_fcolor;
	  		ctw->ctw.attr.vb_fcolor = ctw->ctw.attr.vb_bcolor;
	  		ctw->ctw.attr.vb_fcolor = n;
		  	return TRUE;
		  case 2:
		  	/***********************************************/
		  	/*   Push current font onto stack.	       */
		  	/***********************************************/
		  	fp = (struct font_stack *) chk_zalloc(sizeof(struct font_stack));
			fp->fnt_name = chk_strdup(ctw->ctw.font);
			fp->fnt_next = ctw->ctw.font_stack;
			ctw->ctw.font_stack = fp;
			return TRUE;
		  case 3:
		  	/***********************************************/
		  	/*   Pop current font from stack.	       */
		  	/***********************************************/
		  	if (ctw->ctw.font_stack == (struct font_stack *) NULL)
				return TRUE;
		  	fp = ctw->ctw.font_stack;
			ctw->ctw.font_stack = fp->fnt_next;
			n = 0;
			XtSetArg(arg[n], XtNfont, fp->fnt_name); n++;
			XtSetValues((Widget) ctw, arg, n);
			chk_free((void *) fp);
			return TRUE;
		  }
		return FALSE;
	  case 'l':
	  	flag = FALSE;
		/* Fallthru.. */
	  case 'h':
	  	switch (args[0]) {
		  case 1:
		  	ctw->ctw.flags[CTW_CURSOR_KEYPAD] = flag;
			return TRUE;
		  case 3:
		  	/***********************************************/
		  	/*   80/132 column mode switch.		       */
		  	/***********************************************/
		  	reason.reason = CTWR_SIZE_WINDOW_CHARS;
			reason.height = ctw->ctw.rows;
			reason.width = flag ? 132 : 80;
			reason.client_data = ctw->ctw.client_data;
			if (reason.width == ctw->ctw.columns)
				return TRUE;
			turn_off_cursor(ctw);
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			turn_on_cursor(ctw);
			return TRUE;
		  case 4:
/*		  	ctw->ctw.flags[CTW_SMOOTH_SCROLL] = flag;*/
			return TRUE;
		  case 5:
		  	if (ctw->ctw.flags[CTW_REVERSE_VIDEO] == flag)
				return TRUE;
			set_reverse_video(ctw, flag);
			update_region(ctw, 0, 0, ctw->ctw.rows, ctw->ctw.columns);
		  	return TRUE;
		  case 6:
		  	/***********************************************/
		  	/*   ESC[?6l  DECOM  - Set cursor at position  */
		  	/*   taking into account scrolling region.     */
		  	/***********************************************/
			ctw->ctw.x = 0;
			ctw->ctw.y = ctw->ctw.scroll_top;
		  	return TRUE;
		  case 7:
		  	ctw->ctw.flags[CTW_AUTOWRAP] = flag;
			return TRUE;
		  case 8:
		  	/***********************************************/
		  	/*   Enable/disable autorepeat.		       */
		  	/***********************************************/
			return TRUE;
		  case 9:
		  	/***********************************************/
		  	/*   My birthyear! Mouse report mode.	       */
		  	/***********************************************/
			ctw->ctw.flags[CTW_APPL_MOUSE] = flag;
			return TRUE;
		  case 12:
		  	/***********************************************/
		  	/*   Move us to the front.		       */
		  	/***********************************************/
		  	reason.reason = CTWR_FRONT_WINDOW;
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			return TRUE;
		  case 25:
		  	/***********************************************/
		  	/*   Set cursor visible/invisible	       */
		  	/***********************************************/
			if (ctw->ctw.cursor_state == CURSOR_ON && flag)
				return TRUE;
			if (ctw->ctw.cursor_state == CURSOR_HIDDEN && !flag)
				return TRUE;
			ctw->ctw.cursor_state = flag ? CURSOR_ON : CURSOR_HIDDEN;
			if (flag)
				show_cursor(ctw);
			return TRUE;
		  case 47:
		  	/***********************************************/
		  	/*   Alternate screen buffer.		       */
		  	/***********************************************/
			return TRUE;

		  case 1000:
			ctw->ctw.flags[CTW_APPL_KEYPAD] = flag;
		  	return TRUE;

		  case 1034:
		  	/***********************************************/
		  	/*   smm/rmm meta mode.			       */
		  	/***********************************************/
		  	return TRUE;

		  case 1047:
		  case 1048:
		  	/***********************************************/
		  	/*   Not  sure  what this does but Linux does  */
		  	/*   it.				       */
		  	/***********************************************/
			return TRUE;

		  case 1049:
		  	return TRUE;
		  }
		break;
	  case 'r':
	  case 's':
	  	/***********************************************/
	  	/*   This is definitely wrong!		       */
	  	/***********************************************/
	  	switch (args[0]) {
		  case 3:	/* smcup() */
		  	return TRUE;
		  case 4:	/* rmcup() */
		  	return TRUE;
		  case 1001:
		  	/***********************************************/
		  	/*   For compatibility with w3m.	       */
			/*   This is wrong, this is what w3m defines:  */
			/* #define XTERM_ON   {fputs("\033[?1001s\033[?1000h",ttyf); flush_tty();}*/
			/* #define XTERM_OFF  {fputs("\033[?1000l\033[?1001r",ttyf); flush_tty();}*/
		  	/***********************************************/
			ctw->ctw.flags[CTW_APPL_MOUSE] = cmd == 'r';
			return TRUE;
		  }
		break;
	  case 'S':
	  	/***********************************************/
	  	/*   Scroll a rectangular region.	       */
	  	/***********************************************/
	  	if (arg_no != 5)
			break;
		r = args[0];
		c = args[1];
		h = args[2];
		width = args[3];
		num = args[4];
		/***********************************************/
		/*   Make sure arguments in range.	       */
		/***********************************************/
		if (r < 0)
			r = 0;
		else if (r >= ctw->ctw.rows)
			r = ctw->ctw.rows - 1;
		if (c < 0)
			c = 0;
		else if (c >= ctw->ctw.columns)
			c = ctw->ctw.columns - 1;
		if (h <= 0)
			h = 1;
		else if (h >= ctw->ctw.rows - r)
			h = ctw->ctw.rows - r;
		if (width <= 0)
			width = 1;
		else if (c + width >= ctw->ctw.columns)
			width = ctw->ctw.columns - c;
		if (num < -h)
			num = -h;
		else if (num > h)
			num = h;
		scroll_rectangle(ctw, r, c, width, h, num);
		return TRUE;
	  }
	return FALSE;
}
/**********************************************************************/
/*   Handle ISC's ESC[=... sequence.				      */
/**********************************************************************/
static  int
do_escequ(CtwWidget w, int cmd, int arg_no, int *args)
{
/*	if (w->ctw.flags[CTW_CSI_EQUAL_C] == FALSE)
		return FALSE;
*/
	UNUSED_PARAMETER(arg_no);
	UNUSED_PARAMETER(args);

	switch (cmd) {
	  case 'C':
	  	/***********************************************/
	  	/*   Ignore cursor size changes for now.       */
	  	/***********************************************/
		w->ctw.cursor_state = arg_no < 0 || args[0] == 0 ? CURSOR_HIDDEN : CURSOR_ON;
	  	return TRUE;
	  }
	return FALSE;
}
/**********************************************************************/
/*   Function  to  execute  the  Index  escape  sequences  (ESC D or  */
/*   ESC[S).  Move  down  a  line  or  scroll screen if at bottom of  */
/*   window.							      */
/**********************************************************************/
static void
do_index(CtwWidget w)
{
	w->ctw.escp = NULL;
	if (w->ctw.y >= w->ctw.scroll_bot-1)
		scroll_up(w, 1);
	else
		w->ctw.y++;
}
/**********************************************************************/
/*   Function  to  insert  a  number  of  blanks for insert mode and  */
/*   escape sequence.						      */
/**********************************************************************/
static void
do_insert_space(CtwWidget w, int num)
{	int	y, n, i;
	vbyte_t	space;

	y = dsp_get_top(w) + w->ctw.y;
	vbyte_clear(&space);
	space.vb_fcolor = w->ctw.blank.vb_fcolor;
	space.vb_bcolor = w->ctw.blank.vb_bcolor;
	for (n = num; n-- > 0; ) {
		line_t *lp = dsp_get_row(w, y);
		for (i = w->ctw.columns - 1; i > w->ctw.x; i--) {
			lp->l_text[i] = lp->l_text[i-1];
			}
		lp->l_text[w->ctw.x] = space;
		}
	update_region(w, w->ctw.y, w->ctw.x, w->ctw.y+1, w->ctw.columns);
}
/**********************************************************************/
/*   Function  to  execute a Reverse Index escape sequence (ESC M or  */
/*   ESC[T).  Move  up a line unless at top of region, in which case  */
/*   scroll region down.					      */
/**********************************************************************/
static void
do_rev_index(CtwWidget w)
{
	w->ctw.escp = NULL;
	if (w->ctw.y > w->ctw.scroll_top)
		w->ctw.y--;
	else
		scroll_down(w, w->ctw.scroll_top, 1);
}
/**********************************************************************/
/*   Handle  Sun  compatable escape sequences where we have a string  */
/*   to process.						      */
/**********************************************************************/
static  int
do_text_parms(Widget w, char *str)
{	int	n;
	Arg	args[20];
	int	type = atoi(str);
	char	*cp;
	CtwWidget	ctw = (CtwWidget) w;

	/***********************************************/
	/*   Check for Sun style shell tool sequence.  */
	/***********************************************/
	if (str[0] == 'l') {
		type = 1;
		str++;
		str[strlen(str) - 2] = '\0';
		}
	else if (str[0] == 'L') {
		type = 2;
		str++;
		str[strlen(str) - 2] = '\0';
		}
	else {
		while (*str && *str != ';')
			str++;
			
		if (*str++ == '\0')
			return 0;
		for (cp = str; *cp && *cp != BEL; )
			cp++;
		if (*cp == '\0')
			return 0;
		*cp = '\0';
		}

	/***********************************************/
	/*   Find top level shell.		       */
	/***********************************************/
	while (XtParent(w))
		w = XtParent(w);

	switch (type) {
	  case 0:
	  	/***********************************************/
	  	/*   Window Name and Title.		       */
	  	/***********************************************/
# define	CHANGE_TITLE	0x01
# define	CHANGE_ICON	0x02
		change_name(ctw, CHANGE_TITLE | CHANGE_ICON, str);
		break;
	  case 1:
	  	/***********************************************/
	  	/*   Window name.			       */
	  	/***********************************************/
		change_name(ctw, CHANGE_ICON, str);
		break;
	  case 2:
	  	/***********************************************/
	  	/*   Window title.			       */
	  	/***********************************************/
		change_name(ctw, CHANGE_TITLE, str);
		break;
	  case 46:
	  	/***********************************************/
	  	/*   Set log file name.			       */
	  	/***********************************************/
		do_logging(ctw, FALSE);
		/***********************************************/
		/*   memleak.				       */
		/***********************************************/
		ctw->ctw.log_file = (char *) chk_strdup(str);
		if (ctw->ctw.flags[CTW_LOGGING])
			do_logging(ctw, TRUE);
		break;
	  case 50:
	  	/***********************************************/
	  	/*   Set font name.			       */
	  	/***********************************************/
		n = 0;
		str = chk_strdup(str);
		XtSetArg(args[n], XtNfont, str); n++;
		XtSetValues((Widget) ctw, args, n);
		break;

	  case 100: {
	  	/***********************************************/
	  	/*   Status line labels.		       */
	  	/***********************************************/
		ctw_callback_t	reason;
		reason.reason = CTWR_SET_STATUS_LABEL;
		reason.client_data = ctw->ctw.client_data;
		reason.ptr = str;
		reason.len = strlen(str);
		/***********************************************/
		/*   Let  the owner decide to set the title -  */
		/*   since he may want to augment.	       */
		/***********************************************/
		XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
		break;
		}
	  }
	return 1;
}
/**********************************************************************/
/*   Function  to  move  cursor  down  a  line,  possibly  forcing a  */
/*   scroll.  Also  look  ahead  for more new-lines to see if we can  */
/*   bit-blt more than a line at a time.			      */
/**********************************************************************/
static void
down_line(CtwWidget ctw, char *str, char *str_end)
{	int	y = ctw->ctw.y;
	int	num_scrolls;
	int	max_rows;
	int	col, i;
	int	last_col;
	line_t	*lp;
# if COLLECT_STATS
	ctw_stats.lines_scrolled++;
# endif
	if (y < ctw->ctw.scroll_bot-1 || ctw->ctw.c_flags & CTWF_FROZEN) {
		ctw->ctw.y++;
		return;
		}

	/***********************************************/
	/*   Look  ahead  to  see  if  we  can scroll  */
	/*   more than one line.		       */
	/***********************************************/
	num_scrolls = 1;
	max_rows = ctw->ctw.scroll_bot - ctw->ctw.scroll_top;
	last_col = ctw->ctw.columns + (ctw->ctw.flags[CTW_NEWLINE_GLITCH] != 0);
	col = 0;
	while (str < str_end) {
		switch (*str++) {
		  case ESC:
			{int n;
			
		  	if (*str++ != '[')
			  	goto after_loop;
			while (1) {
				if (isdigit(*str)) {
					n = 0;
					while (isdigit(*str))
						n = 10 * n + *str++ - '0';
					}
				else
					n = 1;
				if (*str != ';')
					break;
				str++;
				}
			switch (*str++) {
			  case 'A':
				y -= n;
				if (y < ctw->ctw.scroll_top)
					y = ctw->ctw.scroll_top;
				break;
			  case 'B':
				y += n;
				if (y >= ctw->ctw.scroll_bot)
				  	goto after_loop;
				break;
			  case 'm':
			  	break;
			  default:
			  	goto after_loop;
			  }
			break;
			}

		  case '\n':
		  	y++;
		  	if (++num_scrolls >= max_rows || 
			    ctw->ctw.flags[CTW_SMOOTH_SCROLL])
				goto after_loop;
//printf("%s(%d): remove me\n", __FILE__, __LINE__); goto after_loop;
			col = 0;
			break;
		  case '\0':
		  	goto after_loop;
		  case '\b':
		  	if (col)
				col--;
			break;
		  default:
		  	if (col++ >= last_col) {
				if (ctw->ctw.flags[CTW_AUTOWRAP] == FALSE) {
					col--;
					break;
					}
				lp = dsp_get_row(ctw, y + dsp_get_top(ctw));
				lp->l_attr |= LA_CONTINUED;
				y++;
			  	if (++num_scrolls >= max_rows 
				    || ctw->ctw.flags[CTW_SMOOTH_SCROLL])
					goto after_loop;
				col = 0;
				}
			break;
		  }
		}
after_loop:

	/***********************************************/
	/*   If  we  have  a  hilited  selection then  */
	/*   adjust place where its displayed.	       */
	/***********************************************/
	if (ctw->ctw.sel_string && dsp_get_top(ctw) + ctw->ctw.rows >= dsp_get_max_lines(ctw)) {
		ctw->ctw.sel_start_y -= num_scrolls;
		ctw->ctw.sel_cur_y -= num_scrolls;
		if (ctw->ctw.sel_start_y < 0 ||
		    ctw->ctw.sel_cur_y < 0) {
			ctw->ctw.sel_start_y -= -1;
			ctw->ctw.sel_cur_y -= -1;
			}
		}

	/***********************************************/
	/*   Save lines which scroll off screen.       */
	/***********************************************/
/*printf("scrolls %d %d %d\n", ctw->ctw.spill_cnt, ctw->ctw.c_scr_line_cnt, num_scrolls);*/
	if (ctw->ctw.flags[CTW_SCROLLING_REGION] == FALSE &&
	    dsp_get_top(ctw) < dsp_get_max_lines(ctw) - 1) {
		if (ctw->ctw.flags[CTW_SCROLLING_REGION] == FALSE) {
			if (crwin_debug)
				printf("[Start-block num_scrolls=%d]\n", num_scrolls);
	/*exposed_region(ctw, 0, 0, 400, 300);
	XSync(XtDisplay(ctw), 1);
	*/
			for (i = 0; i < num_scrolls; i++) {
				hist_save_line(ctw, i);
				ctw->ctw.c_scr_line_cnt++;
				}
			if (crwin_debug)
				printf("[End-block]\n");
			}

/*printf(" .. spill_cnt=%d\n", ctw->ctw.spill_cnt);*/
		delete_line(ctw, 0, num_scrolls);
		if (dsp_get_top(ctw) + num_scrolls > dsp_get_max_lines(ctw)) {
/*update_region(ctw, 0,0, 8888, 80);*/
			ctw->ctw.y -= dsp_get_max_lines(ctw) - ctw->ctw.top_line - 1;
			ctw->ctw.top_line = dsp_get_max_lines(ctw);
			}
		else {
			ctw->ctw.y -= num_scrolls-1;
			ctw->ctw.top_line += num_scrolls;
			}
		ctw->ctw.bot_line = dsp_get_top(ctw) + ctw->ctw.rows;
		}
	else {
		for (i = 0; i < num_scrolls; i++) {
			hist_save_line(ctw, i);
			ctw->ctw.c_scr_line_cnt++;
			}
		ctw->ctw.y -= num_scrolls-1;
# if COLLECT_STATS
		ctw_stats.lines_jumped += num_scrolls;
		ctw_stats.scroll_ups++;
# endif
		scroll_up(ctw, num_scrolls);
		}

}
/**********************************************************************/
/*   Routine to handle start/stop of log-file.			      */
/**********************************************************************/
static void
do_logging(CtwWidget w, int flag)
{	char	buf[BUFSIZ];
	int	fd;

	if (flag) {
		if (w->ctw.log_fp != (FILE *) NULL)
			return;
		if (w->ctw.log_file == NULL || w->ctw.log_file[0] == '\0')
			return;
		strncpy(buf, w->ctw.log_file, sizeof buf);
		if (strlen(buf) > 6 && strcmp(buf+strlen(buf)-6, "XXXXXX") != 0) {
			printf("ctw[%d]: log to '%s'\n", getpid(), w->ctw.log_file);
			fd = open(w->ctw.log_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			}
		else {
			fd = mkstemp(buf);
			printf("ctw[%d]: log to '%s'\n", getpid(), buf);
			}
		if (fd < 0) {
			fprintf(stderr, "ctw: create-log: ");
			perror(buf);
			return;
			}
		w->ctw.log_fp = fdopen(fd, "a");
		return;
		}
	if (w->ctw.log_fp == NULL)
		return;
	fclose(w->ctw.log_fp);
	w->ctw.log_fp = NULL;
}
/**********************************************************************/
/*   Function to draw the line-drawing characters.		      */
/**********************************************************************/
static void
draw_line(CtwWidget w, int row, int col, char *str, int len, Pixel fg, Pixel bg, int attr)
{

	if (w->ctw.line_fontp == (XFontStruct *) NULL) {
		draw_string(w, row, col, str, len, fg, bg, attr);
		return;
		}

	XSetForeground(XtDisplay(w), w->ctw.line_gc, fg);
	XSetBackground(XtDisplay(w), w->ctw.line_gc, bg);
	XDrawImageString(XtDisplay(w), XtWindow(w), w->ctw.line_gc, 
		X_PIXEL(w, col),
		Y_PIXEL(w, row),
		str, len);
}
/**********************************************************************/
/*   Draw line-drawing or the special outline/grid charset.	      */
/**********************************************************************/
static void
draw_special_line_chars(CtwWidget ctw, int row, int c, char *buf, int len, 
	Pixel fg, Pixel bg, int attr, int hstate)
{	char	*bp;
	char	*bufend = buf + len;

	while (buf < bufend) {
		len = bufend - buf;
		switch (*buf) {
		  case '|':
		  case '}':
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				ctw->ctw.font_width, 
				ctw->ctw.font_height + 1);
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, ctw->ctw.gridLine_color);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1), Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				X_PIXEL(ctw, c + len - 1), Y_PIXEL(ctw, row + 1) - ctw->ctw.font_ascent);
			buf++;
			continue;

		  case 0x01:
		  case 0x04:
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				ctw->ctw.font_width, 
				ctw->ctw.font_height + 1);
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
			XDrawRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + 1,
				ctw->ctw.font_width - 1, 
				ctw->ctw.font_height - 2);
			if (*buf == 0x04)
				XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
					X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, 
					Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + 3,
					X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, 
					Y_PIXEL(ctw, row + 1) - ctw->ctw.font_ascent - 4);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + 2,
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width - 3, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2);
			buf++;
			continue;

		  case 0x02:
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				ctw->ctw.font_width, 
				ctw->ctw.font_height + 1);
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, Y_PIXEL(ctw, row + 1) - ctw->ctw.font_ascent);
			buf++;
			continue;

		  case 0x03:
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				ctw->ctw.font_width, 
				ctw->ctw.font_height + 1);
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2, Y_PIXEL(ctw, row + 1) - ctw->ctw.font_ascent);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2 + 1, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2 + 1, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2 + 4);
			XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2 + 2, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2 + 1,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2 + 2, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2 + 3);
			XDrawPoint(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1) + ctw->ctw.font_width / 2 + 3, 
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent + ctw->ctw.font_height / 2 + 2);
			buf++;
			continue;

		  case ' ':
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c + len - 1),
				Y_PIXEL(ctw, row) - ctw->ctw.font_ascent,
				ctw->ctw.font_width, 
				ctw->ctw.font_height + 1);
			buf++;
			continue;
		  }
		/***********************************************/
		/*   Normal line drawing.		       */
		/***********************************************/
		for (bp = buf; bp < bufend; bp++) {
			if (*bp == 0x01 || *bp == 0x02 || *bp == 0x03 ||
			    *bp == '|' || *bp == '}' || *bp == ' ')
				break;
			if (*bp >= 0x5f && *bp <= 0x7e)
				*bp = *bp == 0x5f ? 0x7f : (*bp - 0x5f);
			}
		if (hstate || attr & VB_SELECT) {
			fg = ctw->ctw.hilite_fg;
			bg = ctw->ctw.hilite_bg;
			}
		if (draw_watch || ctw->ctw.flags[CTW_WATCH_DRAWING])
			draw_line(ctw, row, c, buf, len, 
				ctw->ctw.x11_colors[0],
				ctw->ctw.x11_colors[7],
				attr);
		draw_line(ctw, row, c, buf, bp - buf, fg, bg, attr);
		c += bp - buf;
		buf = bp;
		}
}
/**********************************************************************/
/*   Public  function  to  draw  a  string  at the specified row/col  */
/*   with the specified foreground and background colors.	      */
/**********************************************************************/
static void
draw_string(CtwWidget ctw, int row, int col, char *str, int len, Pixel fg, Pixel bg, int attr)
{	int	x = X_PIXEL(ctw, col);
	int	y = Y_PIXEL(ctw, row);
	int	i;
	line_t	*lp;

	XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
	XSetBackground(XtDisplay(ctw), ctw->ctw.gc, bg);

	lp = dsp_get_row(ctw, row + dsp_get_top(ctw));
# if defined(HAVE_FREETYPE)
	if (freetype_enabled) {
		FT_GlyphSlot slot = face->glyph;
		int	pen_x = 300;
		int	pen_y = row * ctw->ctw.font_height;
		int	n;
		int	error;

		for (n = 0; n < len; n++) {
			error = FT_Load_Char( face, str[n], FT_LOAD_RENDER ); 
			if ( error ) continue; /* ignore errors */  
			
			/* now, draw to our target surface */  
			my_draw_bitmap( &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top ); 
			
			/* increment pen position */  
			pen_x += slot->advance.x >> 6; 
		}
		return;
		}
# endif

# if defined(HAVE_FREETYPE_XFT)
	if (freetype_enabled) {
		Display *dpy = XtDisplay(ctw);
		int scr = DefaultScreen(dpy);
	        Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
	        XftColor color;
	        XftDraw *draw;
	        XRenderColor rendcol = { 0x4c00, 0x7800, 0x9900, 0xffff};

		rendcol.red = ((fg >> 16) & 0xff) * 0x101;
		rendcol.green = ((fg >> 8) & 0xff) * 0x101;
		rendcol.blue = ((fg >> 0) & 0xff) * 0x101;

        	XftColorAllocValue_ptr(dpy, DefaultVisual(dpy, scr), cmap,
                	&rendcol, &color);

	        draw = (XftDraw *) XftDrawCreate_ptr(dpy, XtWindow(ctw), DefaultVisual(dpy, scr), cmap);
		XSetForeground(XtDisplay(ctw), ctw->ctw.gc, bg);
		XFillRectangle(dpy, XtWindow(ctw),
			ctw->ctw.gc,
			x, y - ctw->ctw.font_ascent,
			len * ctw->ctw.font_width, ctw->ctw.font_height);
                XftDrawStringUtf8_ptr(draw, &color, ctw->ctw.xft_fontp, x, y,
                                (FcChar8*)str, len);
		return;
		}
# endif

	/***********************************************/
	/*   Disable double height/double width code.  */
	/***********************************************/
	if (0 && lp->l_attr) {
		for (i = 0; i < len; i++) {
			Pixmap p;
			p = create_cached_pixmap(ctw, lp, str[i] & 0xff, fg, bg);
			if (p == (Pixmap) 0)
				continue;
			XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
			XSetBackground(XtDisplay(ctw), ctw->ctw.gc, bg);
			XCopyArea(XtDisplay(ctw), p,
				XtWindow(ctw), ctw->ctw.gc,
				0, 0,
				ctw->ctw.font_width, ctw->ctw.font_height,
				x,
				row * ctw->ctw.font_height);
			x += ctw->ctw.font_width;
			}
		}
	else {
		XDrawImageString(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc, 
			x, y,
			str, len);
		}
	if (attr & VB_BOLD && 0) {
		XDrawString(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc, 
			x + 1, y,
			str, len);
		}
	if (attr & VB_UNDERLINE) {
		XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			x, y + 1,
			x + len * ctw->ctw.font_width, y + 1);
		}
	if (attr & VB_STRIKETHRU) {
		XDrawLine(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			x, y + 1 - ctw->ctw.font_height / 2,
			x + len * ctw->ctw.font_width, y + 1 - ctw->ctw.font_height / 2);
		}
	if (attr & VB_BOXED) {
		for (i = 0; i < len; i++) {
			XDrawRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
				x + i * ctw->ctw.font_width, y - ctw->ctw.font_height,
				ctw->ctw.font_width,
				ctw->ctw.font_height);
			}
		}
	if (attr & VB_FLASHING)
		lp->l_attr |= LA_FLASHING;
}
static int
dsp_get_max_lines(CtwWidget ctw)
{
	return ctw->ctw.spill_cnt + ctw->ctw.max_lines;
//	return ctw->ctw.spill_cnt + ctw->ctw.tot_rows;
}
/**********************************************************************/
/*   Get virtual line.						      */
/**********************************************************************/
static line_t *
dsp_get_row(CtwWidget ctw, int ln)
{	int	i, idx, len;
	char	buf[BUFSIZ];
	vbyte_t	space;
static int old_columns;
static line_t	lbuf;

	if (ln >= ctw->ctw.spill_cnt) {
		ln -= ctw->ctw.spill_cnt;
		return &ctw->ctw.c_lines[(ctw->ctw.curtop + ln) % ctw->ctw.tot_rows];
		}
//printf("dsp_get_row(%d) spill_cnt=%d\n", ln, ctw->ctw.spill_cnt);
	if (lbuf.l_text == NULL || ctw->ctw.columns != old_columns) {
		old_columns = ctw->ctw.columns;
		chk_free_ptr((void **) &lbuf.l_text);
		lbuf.l_text = (vbyte_t *) chk_zalloc(ctw->ctw.columns * sizeof(vbyte_t) + 1);
		}

# define CHUNK_SIZE 1000

	if (ln < 0)
		return NULL;

	vbyte_clear(&space);
/*	space.vb_color = ctw->ctw.blank.vb_color;*/
	space.vb_fcolor = 4;
	space.vb_bcolor = 6;
	for (i = 0; i < ctw->ctw.columns; i++) {
		lbuf.l_text[i] = space;
		}

	if (ln < ctw->ctw.c_spill_line ||
	    ctw->ctw.c_spill_append) {
	    	if (ctw->ctw.c_spill_fp)
			fclose(ctw->ctw.c_spill_fp);
		ctw->ctw.c_spill_append = FALSE;
		ctw->ctw.c_spill_fp = NULL;
		ctw->ctw.c_spill_line = 0;
		}
	if (ctw->ctw.c_spill_fp == NULL &&
	    (ctw->ctw.c_spill_fp = fopen(ctw->ctw.c_spill_name, "r")) == NULL)
		return &lbuf;

	/***********************************************/
	/*   Use  the  line  cache  hint to get there  */
	/*   close.				       */
	/***********************************************/
	idx = ln / CHUNK_SIZE;
	if (idx > ctw->ctw.c_lcache_size) {
		printf("ctw: lcache problem, ln=%d idx=%d (maxidx=%d)\n", ln, idx, ctw->ctw.c_lcache_size);
		}
	fseek(ctw->ctw.c_spill_fp, ctw->ctw.c_lcache[idx].lc_offset, SEEK_SET);
	i = idx * CHUNK_SIZE;
	for ( ; i <= ln; i++) {
		if (fgets(buf, sizeof buf, ctw->ctw.c_spill_fp) == NULL) {
			printf("fcterm(%d): Error reading line %d from spill file after %d lines\n",
				getpid(), ln, i);
			break;
			}
		}
	ctw->ctw.c_spill_line = ln + 1;

	len = strlen(buf) - 1;
	for (i = 0; i < len; i++) {
		lbuf.l_text[i].vb_byte = buf[i];
		}
	lbuf.l_attr |= LA_SPILT;

	return &lbuf;
}
/**********************************************************************/
/*   Return current line of the top of the screen.		      */
/**********************************************************************/
static int
dsp_get_top(CtwWidget ctw)
{
	return ctw->ctw.top_line;
}
/**********************************************************************/
/*   Handle exposure events. Redraw appropriate sections of window.   */
/**********************************************************************/
static void
exposed_region(CtwWidget w, int x, int y, int width, int height)
{	int	start_row, start_col;
	int	end_row, end_col;

	/***********************************************/
	/*   Convert     pixel     co-ordinates    to  */
	/*   character positions.		       */
	/***********************************************/
	ctw_get_xy(w, &start_row, &start_col, x, y);
	ctw_get_xy(w, &end_row, &end_col, x + width, y + height);

	update_region(w, start_row, start_col, end_row + 1, end_col + 1);
	/***********************************************/
	/*   Redraw cursor if we just obliterated it.  */
	/***********************************************/
	if (w->ctw.doing_ge == 0 &&
	    start_row <= w->ctw.y && w->ctw.y <= end_row + 1 &&
	    start_col <= w->ctw.x && w->ctw.x <= end_col + 1) {
	    	show_cursor(w);
		}
}
static Atom
FetchAtom(Widget w, String name)
{
	Atom	a;
	XrmValue	source, dest;

	source.size = strlen(name) + 1;
	source.addr = name;
	dest.size = sizeof(Atom);
	dest.addr = (caddr_t) &a;

	XtConvertAndStore(w, XtRString, &source, XtRAtom, &dest);
	return a;
}
static  int
funcvalue (int keycode)
{
	switch (keycode) {
		case XK_F1:	return(11);
		case XK_F2:	return(12);
		case XK_F3:	return(13);
		case XK_F4:	return(14);
		case XK_F5:	return(15);
		case XK_F6:	return(17);
		case XK_F7:	return(18);
		case XK_F8:	return(19);
		case XK_F9:	return(20);
		case XK_F10:	return(21);
		case XK_F11:	return(23);
		case XK_F12:	return(24);
		case XK_F13:	return(25);
		case XK_F14:	return(26);
		case XK_F15:	return(28);
		case XK_Help:	return(28);
		case XK_F16:	return(29);
		case XK_Menu:	return(29);
		case XK_F17:	return(31);
		case XK_F18:	return(32);
		case XK_F19:	return(33);
		case XK_F20:	return(34);

		case XK_Find :	return(1);
		case XK_Insert:	return(2);
		case XK_Delete:	return(3);
		case XK_Print:	return(209);
		case XK_Scroll_Lock:	return(210);
# if defined(DXK_Remove)
		case DXK_Remove: return(3);
# endif
		case XK_Select:	return(4);
		case XK_KP_Prior:	return(5);
		case XK_Prior:	return(5);
		case XK_KP_Next:	return(6);
		case XK_Next:	return(6);
		case XK_Home:	return 214;
		case XK_End:	return 220;
		default:	return(-1);
	}
}
/**********************************************************************/
/*   Add a drawable to the graph chain.				      */
/**********************************************************************/
static XFontStruct *graph_fontp;

static graph_t *
graph_add(CtwWidget ctw, int *args, char **sargs)
{	graph_t	*g, *g1;
	int	i;

	switch (args[0]) {
	  case DRAW_SET_BACKGROUND:
	  	if (ctw->ctw.c_graph_bg == (Pixel) args[1])
			return NULL;
		ctw->ctw.c_graph_bg = args[1];
		break;

	  case DRAW_SET_FOREGROUND:
	  	if (ctw->ctw.c_graph_fg == (Pixel) args[1])
			return NULL;
		ctw->ctw.c_graph_fg = args[1];
		break;
	  }

	g = chk_zalloc(sizeof *g);
	g->g_code = *args++;
	sargs++;
	for (i = 0; i < MAX_GARGS; i++) {
		g->g_args[i] = args[i];
		}
	switch (g->g_code) {
	  case DRAW_RECT:
	  case DRAW_FILL_RECT:
	  	g->g_rect.x = args[0];
	  	g->g_rect.y = args[1];
	  	g->g_rect.width = args[2];
	  	g->g_rect.height = args[3];
		break;

	  case DRAW_LINE:
	  	g->g_rect.x = MIN(args[0], args[2]);
	  	g->g_rect.y = MIN(args[1], args[3]);
	  	g->g_rect.width = MAX(args[0], args[2]) - g->g_rect.x;
	  	g->g_rect.height = MAX(args[1], args[3]) - g->g_rect.y;
		break;

	  case DRAW_PIXEL:
	  	g->g_rect.x = args[0];
	  	g->g_rect.y = args[1];
		break;

	  case DRAW_NAME:
	  	g->g_rect.x = args[0];
	  	g->g_rect.y = args[1];
	  	g->g_rect.width = args[2];
	  	g->g_rect.height = args[3];
	  	if (sargs[4])
			g->g_str = chk_strdup(sargs[4]);
		break;

	  case DRAW_STRING:
	  case DRAW_IMAGE_STRING:
	  	g->g_rect.x = args[0];
	  	g->g_rect.y = args[1];
	  	if (sargs[2] && graph_fontp) {
			g->g_str = chk_strdup(sargs[2]);
			g->g_rect.width = XTextWidth(graph_fontp, g->g_str, strlen(g->g_str));
			g->g_rect.height = graph_fontp->ascent + graph_fontp->descent;
			}
		break;

	  case DRAW_ARC:
	  case DRAW_FILL_ARC:
	  	g->g_rect.x = args[0];
	  	g->g_rect.y = args[1];
	  	g->g_rect.width = args[2];
	  	g->g_rect.height = args[3];
		g->g_args[0] = args[4];
		g->g_args[1] = args[5];
		break;

	  case DRAW_SET_FONT:
	  	if (sargs[0])
			g->g_str = chk_strdup(sargs[0]);
		break;
	  }

	if (ctw->ctw.c_chain == NULL)
		ctw->ctw.c_chain = g;
	else {
		for (g1 = ctw->ctw.c_chain; g1->g_next; )
			g1 = g1->g_next;
		g1->g_next = g;
		}
	graph_draw(ctw, g);
	return g;
}
/**********************************************************************/
/*   Detect a mouse click on a region/graph.			      */
/**********************************************************************/
static graph_t *
graph_click(CtwWidget ctw, int x, int y, int execute)
{  	graph_t *g;

  	for (g = ctw->ctw.c_chain; g; g = g->g_next) {
		if (g->g_code != DRAW_NAME)
			continue;
/*printf("%d %d,%d %d %d\n", g->g_code, g->g_args[0], g->g_args[1], g->g_args[2], g->g_args[3]);*/
		if (x >= g->g_rect.x && x <= g->g_rect.x + g->g_rect.width &&
		    y >= g->g_rect.y && y <= g->g_rect.y + g->g_rect.height) {
		    	if (execute) {
			  	send_input(ctw, g->g_str, strlen(g->g_str));
			  	send_input(ctw, "\n", 1);
			}
			return g;
			}
		}
	return NULL;
}
static void
graph_destroy(CtwWidget ctw)
{  	graph_t *g, *g1;

	if (ctw->ctw.c_chain_mouse) {
		ctw->ctw.c_chain_mouse = NULL;
		XDefineCursor(XtDisplay(ctw),  XtWindow(ctw), None);
		}

  	for (g = ctw->ctw.c_chain; g; g = g1) {
		g1 = g->g_next;
		chk_free_ptr((void **) &g->g_str);
		chk_free(g);
		}
	ctw->ctw.c_chain = NULL;
	ctw->ctw.c_graph_fg = 0x7fffffff;
	ctw->ctw.c_graph_bg = 0x7fffffff;
}
static void
graph_draw(CtwWidget ctw, graph_t *g)
{	int *args = g->g_args;

	switch (g->g_code) {		
	  case DRAW_RECT:
		XDrawRectangle(XtDisplay(ctw), XtWindow(ctw), gc,
			g->g_rect.x, g->g_rect.y, g->g_rect.width, g->g_rect.height);
		break;
	  case DRAW_FILL_RECT:
		XFillRectangle(XtDisplay(ctw), XtWindow(ctw), gc,
			g->g_rect.x, g->g_rect.y, g->g_rect.width, g->g_rect.height);
		break;
	  case DRAW_LINE:
		XDrawLine(XtDisplay(ctw), XtWindow(ctw), gc,
			args[0], args[1], args[2], args[3]);
		break;
	  case DRAW_PIXEL:
		XDrawPoint(XtDisplay(ctw), XtWindow(ctw), gc,
			g->g_rect.x, g->g_rect.y);
		break;
	  case DRAW_NAME:
	  	break;
	  case DRAW_STRING:
	  	if (g->g_str == NULL)
			break;
//if (g->g_str[0]++ > 'z') g->g_str[0] = 'A';
		XDrawString(XtDisplay(ctw), XtWindow(ctw), gc, 
			g->g_rect.x, g->g_rect.y,
			g->g_str, strlen(g->g_str));
	  	break;
	  case DRAW_ARC:
		XDrawArc(XtDisplay(ctw), XtWindow(ctw), gc,
			g->g_rect.x, g->g_rect.y, g->g_rect.width, g->g_rect.height,
			args[0], args[1]);
		break;
	  case DRAW_FILL_ARC:
		XFillArc(XtDisplay(ctw), XtWindow(ctw), gc,
			g->g_rect.x, g->g_rect.y, g->g_rect.width, g->g_rect.height,
			args[0], args[1]);
		break;
	  case DRAW_SET_BACKGROUND:
		XSetBackground(XtDisplay(ctw), gc, map_rgb_to_visual_rgb(ctw, args[0]));
		break;
	  case DRAW_SET_FOREGROUND:
		XSetForeground(XtDisplay(ctw), gc, map_rgb_to_visual_rgb(ctw, args[0]));
		break;
	  case DRAW_SET_FONT: {
		static hash_t *hash_fonts;
		static int err;

	  	if (g->g_str == NULL)
			break;

		if (hash_fonts == NULL)
			hash_fonts = hash_create(16, 16);
		if ((graph_fontp = hash_lookup(hash_fonts, g->g_str)) == NULL) {
			graph_fontp = XLoadQueryFont(XtDisplay(ctw), g->g_str);
			hash_insert(hash_fonts, chk_strdup(g->g_str), graph_fontp);
			}
		if (graph_fontp == NULL) {
			if (err++ < 10) {
				printf("ctw: pid=%d cannot load font '%s' for DRAW_SET_FONT\n",
					getpid(), g->g_str);
				}
			return;
			}
		XSetFont(XtDisplay(ctw), gc, graph_fontp->fid);
	  	break;
		}
	  case DRAW_IMAGE_STRING:
	  	if (g->g_str == NULL)
			break;
		XDrawImageString(XtDisplay(ctw), XtWindow(ctw), gc, 
			g->g_rect.x, g->g_rect.y,
			g->g_str, strlen(g->g_str));
	  	break;
	  }
}
/**********************************************************************/
/*   Redraw the graph data on an expose event.			      */
/**********************************************************************/
static void
graph_redraw(CtwWidget ctw, XRectangle *rectp)
{	graph_t	*g;

	for (g = ctw->ctw.c_chain; g; g = g->g_next) {
		switch (g->g_code) {
		  case DRAW_SET_FOREGROUND:
		  case DRAW_SET_BACKGROUND:
		  case DRAW_SET_FONT:
			graph_draw(ctw, g);
			break;

		  default:
		  	/***********************************************/
		  	/*   Check for a dirty region.		       */
		  	/***********************************************/
			if (rectp) {
				if (g->g_rect.x + g->g_rect.width < rectp->x)
			    		continue;
				if (g->g_rect.x >= rectp->x + rectp->width)
			    		continue;
				if (g->g_rect.y + g->g_rect.height < rectp->y)
					continue;
				if (g->g_rect.y >= rectp->y + rectp->height)
					continue;
				}

			graph_draw(ctw, g);
			break;
		  }
		}
}
/**********************************************************************/
/*   Handle blinking attributes.				      */
/**********************************************************************/
static  int
handle_blinking(CtwWidget w)
{	int	r;
	int	c;
	vbyte_t	*vp;
	int	len;
	int	attr;
	char	buf[128];
	int	fg, bg;
	int	num_blinks = 0;

	w->ctw.blink_state ^= VB_REVERSE;

	for (r = 0; r < w->ctw.rows; r++) {
		line_t	*lp = dsp_get_row(w, r + dsp_get_top(w));

		if ((lp->l_attr & LA_FLASHING) == 0)
			continue;

		vp = lp->l_text;
		for (c = 0; c < w->ctw.columns; c++) {
			int	n = 0;
			if ((vp[c].vb_attr & VB_FLASHING) == 0)
				continue;
			attr = vp[c].vb_attr;
			if (attr & VB_BOLD)
				n += 8;
			if (w->ctw.blink_state & VB_REVERSE) {
				fg = w->ctw.x11_colors[vp[c].vb_fcolor + n];
				bg = w->ctw.x11_colors[vp[c].vb_bcolor + n];
				}
			else {
				fg = w->ctw.x11_colors[vp[c].vb_bcolor + n];
				bg = w->ctw.x11_colors[vp[c].vb_fcolor + n];
				}
			for (len = 0; len < (int) sizeof buf && c < w->ctw.columns; c++) {
				if (vp[c].vb_attr != attr)
					break;
				buf[len++] = vp[c].vb_byte;
				}
			draw_string(w, r, c - len, buf, len, 
				fg, bg, attr);
			num_blinks++;
			}
		}
	w->ctw.blinking = num_blinks;
/*printf("%x: %d %s\n", w, num_blinks, w->ctw.have_focus ? "Have focus" : "nope");*/
	return num_blinks;
}
/**********************************************************************/
/*   Function to parse or continue parsing an escape sequence.	      */
/**********************************************************************/
static char *
handle_escape(CtwWidget w, char *str, char *str_end)
{	char	*cp = w->ctw.escp;
	char	*cpend = &w->ctw.escbuf[MAX_ESCBUF - 1];
	int	in_str = FALSE;
	int	seen_str = FALSE;

	/***********************************************/
	/*   If    doing    continuation,   recompute  */
	/*   in_str/seen_str.			       */
	/***********************************************/
	if (w->ctw.escbuf[0] == '[') {
		char *cp1;
		for (cp1 = w->ctw.escbuf; cp1 < w->ctw.escp; cp1++) {
			if (*cp1 == BEL) {
				in_str = TRUE;
				seen_str = TRUE;
				}
			else if (*cp1 == ESC) {
				in_str = FALSE;
				}
			}
		}

	while (str < str_end) {
		/***********************************************/
		/*   If sequence too large then just junk it.  */
		/***********************************************/
		if (cp >= cpend) {
			w->ctw.escp = (char *) NULL;
			ctw_add_char(w, "<ESC>");
			send_str(w, w->ctw.escbuf, MAX_ESCBUF);
			return str;
			}
		*cp++ = *str++;
		switch (w->ctw.escbuf[0]) {
		  case '[':
		  	if (!in_str) {
				if (cp[-1] == BEL) {
					in_str = TRUE;
					seen_str = TRUE;
					continue;
					}

			  	if (!isalpha(str[-1]) && str[-1] != '@')
					continue;
				if (str[-1] == '-')
					continue;
				}
			else {
				if (str[-1] == ESC) {
					in_str = FALSE;
					continue;
					}
				continue;
				}
			break;
		  case ']':
		  	if (str[-1] == BEL)
				break;
			/***********************************************/
			/*   Check  for  shelltool  style icon/window  */
			/*   name escape			       */
			/***********************************************/
			if (cp < &w->ctw.escbuf[3])
				continue;
			if (str[-1] == '\\' && str[-2] == ESC)
				break;
			continue;
		  case '@':
		  	if (cp > &w->ctw.escbuf[1]) {
				do_at_parms(w);
				w->ctw.escp = (char *) NULL;
			  	return str;
				}
			continue;
		  case '#':
		  	if (cp > &w->ctw.escbuf[2])
				break;
			continue;
		  case '(':
		  case ')':
		  case '*':
		  case '+':
		  	if (cp > &w->ctw.escbuf[1])
				break;
			continue;
		  case '7':
		  	w->ctw.saved_x = w->ctw.x;
		  	w->ctw.saved_y = w->ctw.y;
			w->ctw.escp = NULL;
		  	return str;
		  case '8':
		  	w->ctw.x = w->ctw.saved_x;
		  	w->ctw.y = w->ctw.saved_y;
			w->ctw.escp = NULL;
		  	return str;
		  case 'c':
			w->ctw.escp = (char *) NULL;
		  	reset_screen(w);
		  	clear_screen(w);
			return str;
		  case '<':
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case '>':
		  	w->ctw.flags[CTW_APPL_KEYPAD] = FALSE;
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case '=':
		  	w->ctw.flags[CTW_APPL_KEYPAD] = TRUE;
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case 'D':
		  	do_index(w);
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case 'H':
		  	w->ctw.tab_stops[w->ctw.x] = TRUE;
			w->ctw.escp = NULL;
		  	return str;
		  case 'M':
		  	do_rev_index(w);
			w->ctw.escp = (char *) NULL;
		  	return str;
		  case 'Z':
		  	send_input(w, "\033[?1;2c", 7);
			w->ctw.escp = NULL;
		  	return str;
		  case '\033':
		  	if (cp > &w->ctw.escbuf[1])
				break;
			continue;
		  	
		  default:
		  	if (w->ctw.flags[CTW_ESC_LITERAL]) {
				char	buf[1];
				buf[0] = w->ctw.escbuf[0];
				w->ctw.escp = NULL;
			  	send_str(w, buf, 1);
				return str;
				}
		  	break;
		  }

		/***********************************************/
		/*   Dont  allow dynamic strings if we cannot  */
		/*   process them.			       */
		/***********************************************/
		*cp = '\0';
		w->ctw.escp = NULL;
		if ((cp[-1] != 'm' && seen_str) || 
		    process_escape(w) == FALSE) {
			ctw_add_char(w, "<ESC>");
			send_str(w, w->ctw.escbuf, cp - w->ctw.escbuf);
			}
		w->ctw.escp = NULL;
		return str;
		}
	w->ctw.escp = cp;
	return str;
}
/**********************************************************************/
/*   Common routine for handling focus change or enter/leave window.  */
/**********************************************************************/
static void
handle_focus_change(CtwWidget w, int entering)
{
	/***********************************************/
	/*   If  mouse  if  moving  out/in  of window  */
	/*   then  flush  log  to  try and keep it up  */
	/*   to date.				       */
	/***********************************************/
	ctw_flush_log(w);

/*printf("\n%x: handle_focus_change %d\n", w, entering);*/
	if (entering) {
		w->ctw.have_focus = TRUE;
		/***********************************************/
		/*   If timer isn't running then restart it.   */
		/***********************************************/
		if (w->ctw.cursor_timer == 0 && w->ctw.flashrate)
			cursor_flash_proc(w, (XtIntervalId *) NULL);
		}
	else {
		w->ctw.have_focus = FALSE;
		if (w->ctw.cursor_timer && w->ctw.flashrate &&
		    w->ctw.blinking == 0) {
			XtRemoveTimeOut(w->ctw.cursor_timer);
			w->ctw.cursor_timer = 0;
			turn_on_cursor(w);
			}
		}
}
/**********************************************************************/
/*   Decode UTF-8 sequences here.				      */
/*
Decode next UTF-8 sequence. Return the value and update the pointer.

U-00000000 - U-0000007F: 0xxxxxxx
U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
/**********************************************************************/
static char *
handle_utf8(CtwWidget w, char *str, char *str_end)
{	unsigned char	*cp = w->ctw.utfp;
	char	buf[16];
	int	len = str_end - str;
	int	nbytes, todo, u;
	int	b1, b2, b3, b4, b5;

	if (w->ctw.utfp == NULL)
		w->ctw.utfp = w->ctw.utfbuf;
	*w->ctw.utfp++ = *str++;
	u = w->ctw.utfbuf[0];

	if ((u & 0xe0) == 0xc0)
		nbytes = 2;
	else if ((u & 0xf0) == 0xe0)
		nbytes = 3;
	else if ((u & 0xf8) == 0xf0)
		nbytes = 4;
	else if ((u & 0xfc) == 0xf8)
		nbytes = 5;
	else if ((u & 0xfe) == 0xfc)
		nbytes = 6;
	else {
		w->ctw.utfp = NULL;
		ctw_add_raw_string(w, str-1, 1);
		return str;
		}

	len = str_end - str;
	todo = nbytes - (w->ctw.utfp - w->ctw.utfbuf);
	if (len >= todo) {
		memcpy(w->ctw.utfp, str, todo);
		w->ctw.utfp += todo;
		str += todo;
		}
	if (w->ctw.utfp - w->ctw.utfbuf < nbytes)
		return str;

	cp = w->ctw.utfbuf + 1;
	if ((u & 0xe0) == 0xc0) {
		/* U-00000080 - U-000007FF: 110xxxxx 10xxxxxx */
		u &= 0x1f;
		b1 = *cp++;
		u = (u << 6) | (b1 & 0x3f);
		}
	else if ((u & 0xf0) == 0xe0) {
		/* U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx */
		u &= 0x0f;
		b1 = *cp++;
		b2 = *cp++;
		u = (u << 12) | ((b1 & 0x3f) << 6) | (b2 & 0x3f);
		}
	else if ((u & 0xf8) == 0xf0) {
		/* U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
		u &= 0x07;
		b1 = *cp++;
		b2 = *cp++;
		b3 = *cp++;
		u = (u << 18) | ((b1 & 0x3f) << 12) | ((b2 & 0x3f) << 6) | (b3 & 0x3f);
		}
	else if ((u & 0xfc) == 0xf8) {
		/* U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
		u &= 0x03;
		b1 = *cp++;
		b2 = *cp++;
		b3 = *cp++;
		b4 = *cp++;
		u = (u << 24) | ((b1 & 0x3f) << 18) | ((b2 & 0x3f) << 12) | ((b3 & 0x3f) << 6) | (b4 & 0x3f);
		}
	else if ((u & 0xfe) == 0xfc) {
		/* U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
		u &= 0x01;
		b1 = *cp++;
		b2 = *cp++;
		b3 = *cp++;
		b4 = *cp++;
		b5 = *cp++;
		u = (u << 30) | ((b1 & 0x3f) << 24) | ((b2 & 0x3f) << 18) | ((b3 & 0x3f) << 12) | ((b4 & 0x3f) << 6) | (b5 & 0x3f);
		}
	if (u < 0x100) {
		buf[0] = u;
		ctw_add_raw_string(w, buf, 1);
		}
	else if (u < 0x10000) {
		sprintf(buf, "\\U%04x", u);
		ctw_add_raw_string(w, buf, strlen(buf));
		}
	else {
		sprintf(buf, "\\U%08x", u);
		ctw_add_raw_string(w, buf, strlen(buf));
		}
	w->ctw.utfp = NULL;
	return str;
}
/**********************************************************************/
/*   Routine to hilite a marked area.				      */
/**********************************************************************/
static void
hilite(CtwWidget ctw, int x1, int y1, int x2, int y2, int set_flag, XRectangle *rectp)
{	int	start_x;
	int	end_x, x, t;
	int	y;
	vbyte_t	*vp;

	if (y2 < y1) {
		SWAP(y1, y2, t);
		SWAP(x1, x2, t);
		}
	if (y1 == y2 && x2 < x1) {
		SWAP(x1, x2, t);
		}
	start_x = x1;
	y = y1;

	if (y < 0)
		return;

	if (y2 >= dsp_get_max_lines(ctw) + ctw->ctw.rows)
		y2 = dsp_get_max_lines(ctw) + ctw->ctw.rows - 1;

	if (rectp) {
		rectp->x = 0;
		rectp->y = y - ctw->ctw.top_line;
		rectp->width = ctw->ctw.columns;
		rectp->height = y2 - y + 1;
		}

	while (y <= y2) {
		line_t	*lp;

		if (y == y2)
			end_x = x2;
		else
			end_x = ctw->ctw.columns;
		end_x = MIN(end_x, ctw->ctw.columns);
		lp = dsp_get_row(ctw, y);
		vp = &lp->l_text[start_x];
		x = start_x;
		if (set_flag) {
			while (start_x++ < end_x) {
				vp->vb_attr |= VB_SELECT;
				vp++;
				}
			}
		else {
			while (start_x++ < end_x) {
				vp->vb_attr &= ~VB_SELECT;
				vp++;
				}
			}
		if (rectp == NULL && 0) {
			update_region(ctw, y - ctw->ctw.top_line, x, 
				y+1 - ctw->ctw.top_line, end_x+1);
			}
		start_x = 0;
		y++;
		}
}
/**********************************************************************/
/*   Save  a  line  about  to  be  scrolled  off the screen into the  */
/*   history buffer.						      */
/**********************************************************************/
static int
hist_save_line(CtwWidget ctw, int ln)
{	line_t	*lp = dsp_get_row(ctw, ln + dsp_get_top(ctw));
//{	line_t	*lp = dsp_get_row(ctw, ln);
	int	cnt = 0;
	char	buf[1024];
	char	buf2[1024];
	vbyte_t *vp = lp->l_text;
	vbyte_t	*vpend;

	ln = ctw->ctw.c_scr_line_cnt;
	if (ctw_history == 0)
		return 0;

if (ln < ctw->ctw.spill_cnt) {
printf("skip %d:%d\n", ln, ctw->ctw.spill_cnt);
	return 0;
	}

	/***********************************************/
	/*   Handle log spilling.		       */
	/***********************************************/
	if (ctw->ctw.c_spill_bytes >= ctw->ctw.c_spill_size &&
	    (ln % CHUNK_SIZE) == 0) {
	    	if (crwin_debug)
			printf("SPill rollover: %d\n", ln);
		if (ctw->ctw.c_spill_fp) {
			fclose(ctw->ctw.c_spill_fp);
			ctw->ctw.c_spill_fp = NULL;
			}
		ctw->ctw.top_line -= ctw->ctw.spill_cnt;
		ctw->ctw.c_scr_line_cnt -= ctw->ctw.spill_cnt;
		ln -= ctw->ctw.spill_cnt;
		ctw->ctw.spill_cnt = 0;
		ctw->ctw.c_spill_bytes = 0;
		chk_free_ptr((void **) &ctw->ctw.c_spill_name);
		chk_free_ptr((void **) &ctw->ctw.c_lcache);
		}

	if (ctw->ctw.c_spill_name == NULL) {
		char	*name = ctw->ctw.ttyname ? basename(ctw->ctw.ttyname) : "ZZ";
		time_t t = time(NULL);
		char	buf3[1024];
		struct stat sbuf;
		int	i;

		for (i = 0; i < 1000; i++) {
			snprintf(buf, sizeof buf, "%s/%s/fcterm-%s%s-%%Y%%m%%d-%03d.log", 
				ctw->ctw.log_dir,
				getenv("USER") ? getenv("USER") : "fcterm",
				isdigit(*name) ? "tty" : "", name, i);
			strftime(buf2, sizeof buf2, buf, localtime(&t));
			sprintf(buf3, "%s.gz", buf2);
			if (stat(buf2, &sbuf) < 0 && stat(buf3, &sbuf) < 0)
				break;
			if (default_gzip_rollover && stat(buf3, &sbuf) < 0) {
				sprintf(buf3, "gzip %s &", buf2);
				if (system(buf3) != 0) 
					perror("fcterm: gzip failed");
				}
			}

		ctw->ctw.c_spill_name = chk_strdup(buf2);
		/***********************************************/
		/*   Make  sure  we  dont have an old file we  */
		/*   are appending to.			       */
		/***********************************************/
		remove(ctw->ctw.c_spill_name);

		/***********************************************/
		/*   Setup symlink to current file.	       */
		/***********************************************/
		create_fcterm_dir(ctw);

		snprintf(buf, sizeof buf, "%s/%s/fcterm-%s%s.log",
				ctw->ctw.log_dir,
				getenv("USER") ? getenv("USER") : "fcterm",
				isdigit(*name) ? "tty" : "", name);
		remove(buf);
		if (symlink(ctw->ctw.c_spill_name, buf) < 0) {
			fprintf(stderr, "fcterm: symlink(%s, %s) error - %s\n",
				ctw->ctw.c_spill_name, buf, strerror(errno));
			}
		}

	/***********************************************/
	/*   Make sure file is in correct mode.	       */
	/***********************************************/
	if (ctw->ctw.c_spill_fp && !ctw->ctw.c_spill_append) {
		fclose(ctw->ctw.c_spill_fp);
		ctw->ctw.c_spill_fp = NULL;
		}

	if (ctw->ctw.c_spill_fp == NULL) {
		if ((ctw->ctw.c_spill_fp = fopen(ctw->ctw.c_spill_name, "a")) == NULL)
			return 0;
		if (ctw->ctw.spill_cnt == 0)
			chmod(ctw->ctw.c_spill_name, 0640);
		}

	if ((ln % CHUNK_SIZE) == 0 || ctw->ctw.c_lcache == NULL) {
		int idx = ln / CHUNK_SIZE;

		if (ctw->ctw.c_lcache == 0)
			ctw->ctw.c_lcache = (line_cache_t *) chk_zalloc((idx + 1) * sizeof(line_cache_t));
		else
			ctw->ctw.c_lcache = (line_cache_t *) chk_realloc(ctw->ctw.c_lcache, 
				(idx + 1) * sizeof(line_cache_t));
		ctw->ctw.c_lcache_size = idx;

		if (crwin_debug)
			printf("lcache resize: ln=%d idx=%d\n", ln, idx);
		ctw->ctw.c_lcache[idx].lc_number = ln;
		ctw->ctw.c_lcache[idx].lc_offset = ftell(ctw->ctw.c_spill_fp);
		}

	vpend = vp + ctw->ctw.columns - 1;
	while (vpend > vp && vpend->vb_byte == ' ')
		vpend--;

	if (crwin_debug) {
		printf("Line(%d,%d,%d,%d): ", 
			ctw->ctw.spill_cnt, 
			ln, 
			ctw->ctw.curtop, 
			ctw->ctw.tot_rows);
		}

	for (; vp <= vpend; vp++) {
		cnt++;
		if (fputc(vp->vb_byte, ctw->ctw.c_spill_fp) == EOF) {
			printf("*** error writing to spill file\n");
			}
		if (crwin_debug && cnt < 50)
			putchar(vp->vb_byte);
		}
	fputc('\n', ctw->ctw.c_spill_fp);
	ctw->ctw.c_spill_bytes += cnt + 1;
	ctw->ctw.c_spill_append = TRUE;
	if (crwin_debug)
		putchar('\n');
	return 1;
}
/**********************************************************************/
/*   Tell us if we are in a hilite.				      */
/**********************************************************************/
static int
in_hilite(CtwWidget ctw, int row, int col)
{	int	sy, ey;
	int	sx, ex;

	if (ctw->ctw.sel_string == NULL)
		return 0;

	row += dsp_get_top(ctw);

	sy = ctw->ctw.sel_start_y;
	sx = ctw->ctw.sel_start_x;
	ey = ctw->ctw.sel_cur_y;
	ex = ctw->ctw.sel_cur_x;
	if (sy > ey || (sy == ey && sx > ex)) {
		int	t;
		SWAP(sy, ey, t);
		SWAP(sx, ex, t);
		}
	
	if (row < sy)
		return 0;
	if (row > ey)
		return 0;
	if (row == sy && col < sx)
		return 0;
	if (row == ey && col >= ex)
		return 0;

	return 1;
}
/**********************************************************************/
/*   Do a fast insert line.					      */
/**********************************************************************/
static void
insert_line(CtwWidget w, int row, int bot, int nchunk, int clear_flag)
{	int	y, dst_y, height;

	UNUSED_PARAMETER(clear_flag);

	y = row * w->ctw.font_height;
	dst_y = y + nchunk * w->ctw.font_height;
	height = (bot - row - nchunk) * w->ctw.font_height;

	if (height > 0) {	
		wait_for_exposure(w);
		XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->ctw.gc,
			w->ctw.internal_width, y + w->ctw.internal_height, 
				w->ctw.columns * w->ctw.font_width, height,
			w->ctw.internal_width, dst_y + w->ctw.internal_height);
		}

	if (nchunk > 0)
		XClearArea(XtDisplay(w), XtWindow(w), 
			w->ctw.internal_width, y + w->ctw.internal_height,
			w->ctw.internal_width + w->ctw.font_width * w->ctw.columns,
			w->ctw.font_height * nchunk, 
			False);

}
/**********************************************************************/
/*   Look to see if its one of our keys.			      */
/**********************************************************************/
static int
is_ctw_modifier(int mask)
{
	if (mask == (ShiftMask | ControlMask))
		return TRUE;

	if ((mask & (ShiftMask | ControlMask)) == ShiftMask &&
	    mask & (Mod1Mask | Mod2Mask | Mod3Mask))
	    	return TRUE;

	return FALSE;
}
/**********************************************************************/
/*   Function  called  when  we lose the selection. Just free up the  */
/*   allocated memory.						      */
/**********************************************************************/
static void
lose_selection(CtwWidget ctw, Atom *selection)
{
	UNUSED_PARAMETER(selection);

	if (ctw->ctw.sel_string && might_have_selection == 1) {
		hilite(ctw, ctw->ctw.sel_start_x, ctw->ctw.sel_start_y, 
			ctw->ctw.sel_cur_x, ctw->ctw.sel_cur_y, FALSE, NULL);
		/***********************************************/
		/*   We  may  scroll  the selected region and  */
		/*   never undraw it.			       */
		/***********************************************/
		hilite(ctw, 0, ctw->ctw.spill_cnt, 
			ctw->ctw.columns, dsp_get_max_lines(ctw) + ctw->ctw.rows, 
			FALSE, NULL);
		unhilite(ctw);
		chk_free_ptr((void **) &ctw->ctw.sel_string);
		ctw->ctw.sel_start_x = -1;
		}
	might_have_selection--;
	if (might_have_selection < 0)
		might_have_selection = 0;
}
/* ARGSUSED */
static void
requestor_callback(Widget w, XtPointer client_data, Atom *selection, Atom *type, XtPointer *value, unsigned long *length, int *format)
{	CtwWidget	ctw = (CtwWidget) w;
	ctw_callback_t	reason;

	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(selection);
	UNUSED_PARAMETER(type);
	UNUSED_PARAMETER(format);

	reason.reason = CTWR_PASTE;
	reason.client_data = ctw->ctw.client_data;

	if (value == NULL || (*value == NULL && *length == 0)) {
		return;
		}
	reason.ptr = (char *) value;
	reason.len = (int) *length;
	XtCallCallbacks(w, XtNkbdCallback, (caddr_t) &reason);

	/***********************************************/
	/*   Free it.				       */
	/***********************************************/
	XtFree((char *) value);
}
static void
label_line(CtwWidget ctw)
{	char buf[64];
	struct tm *tm;
	struct timeval tval;

	if (ctw->ctw.flags[CTW_APPL_KEYPAD])
		return;

	if (ctw->ctw.flags[CTW_NUMBER_LINES]) {
		sprintf(buf, "[%06d]", dsp_get_top(ctw) + ctw->ctw.y);
		ctw_add_string(ctw, buf, strlen(buf));
		}
	if (ctw->ctw.flags[CTW_LABEL_LINES]) {
		gettimeofday(&tval, NULL);
		tm = localtime(&tval.tv_sec);
		sprintf(buf, "%04d", ctw->ctw.c_line_no++);
		strftime(buf + 4, sizeof buf - 5, " %H:%M:%S", tm);
		sprintf(buf + strlen(buf), ".%03ld ", tval.tv_usec / 1000);
		ctw_add_string(ctw, buf, strlen(buf));
		}
}
/**********************************************************************/
/*   Common code to handle a change in the font.		      */
/**********************************************************************/
static  int
reset_font(CtwWidget w, int delete_flag)
{
	UNUSED_PARAMETER(delete_flag);

	w->ctw.font_height = w->ctw.fontp->ascent + w->ctw.fontp->descent;
	w->ctw.font_width = XTextWidth(w->ctw.fontp, "A", 1);
	w->ctw.font_ascent = w->ctw.fontp->ascent;
	w->ctw.font_descent = w->ctw.fontp->descent;
# if defined(HAVE_FREETYPE_XFT)
	if (w->ctw.xft_fontp) {
		XGlyphInfo extents;
		XftTextExtents8_ptr(XtDisplay(w),
			w->ctw.xft_fontp,
			(FcChar8 *) "A", 1, &extents);
		w->ctw.font_width = extents.xOff;

		if (0) {
			printf("extents.x=%d\n", extents.x);
			printf("extents.xOff=%d\n", extents.xOff);
			printf("extents.y=%d\n", extents.y);
			printf("extents.yOff=%d\n", extents.yOff);
			printf("extents.height=%d\n", extents.height);
			printf("ascent=%d\n", w->ctw.xft_fontp->ascent);
			printf("descent=%d\n", w->ctw.xft_fontp->descent);
			}

		w->ctw.font_height = w->ctw.xft_fontp->ascent +
			w->ctw.xft_fontp->descent;
		w->ctw.font_descent = w->ctw.xft_fontp->descent;
		w->ctw.font_ascent = w->ctw.xft_fontp->ascent;
		}
# endif
	return 0;
}
/**********************************************************************/
/*   Create a mask for showing search strings.			      */
/**********************************************************************/
static struct match_t {
	char	*m_str;
	int	m_fg;
	int	m_bg;
	int	m_flag;
	} matches[] = {
	{"fatal", 	34, 45, LA_MATCH},
	{"error", 	34, 45, LA_MATCH},
	{"warning", 	35, 46, LA_MATCH2},
	{0, 0, 0, 0},
	};

static int
print_search(CtwWidget ctw, line_t *lp, unsigned char *map)
{	char	*str;
	int	len;
	vbyte_t	*vp = lp->l_text;
	vbyte_t	*vpend = lp->l_text + ctw->ctw.columns;
	vbyte_t	*vp1;
	int	i, j;
	struct match_t *mp;

	memset(map, 0, ctw->ctw.columns);

	/***********************************************/
	/*   Handle search lines.		       */
	/***********************************************/
	if (ctw->ctw.flags[CTW_APPL_KEYPAD] == FALSE) {
		for (vp = lp->l_text; vp + 1 < vpend; vp++) {
			if (!isalpha(vp->vb_byte))
				continue;
			if (vp > lp->l_text && isalpha(vp[-1].vb_byte))
				continue;
			for (mp = matches; mp->m_str; mp++) {
				char	*str = mp->m_str;
				if (tolower(vp->vb_byte) != *str)
					continue;
				for (i = 0, vp1 = vp; str[i] && vp1 < vpend; vp1++, i++) {
					if (toupper(str[i]) != toupper(vp1->vb_byte))
						break;
					}
				if (str[i])
					continue;
				if (vp1 < vpend && vp1->vb_byte == '_')
					continue;

if(0)				for (j = 0; j < ctw->ctw.columns; j++)
					map[j] |= 1;
				lp->l_attr |= mp->m_flag;
				return TRUE;
				}
			}
		}
	lp->l_attr &= ~(LA_MATCH | LA_MATCH2);

	/***********************************************/
	/*   Handle search string.		       */
	/***********************************************/
	if ((str = ctw->ctw.c_search) == NULL) {
		return FALSE;
		}

	len = strlen(str);
	for (vp = lp->l_text ; vp + len < vpend; vp++) {
		vbyte_t	*vp1;
		int	i, j;

		if (vp->vb_byte != *str)
			continue;
		for (i = 0, vp1 = vp; str[i] && vp1 < vpend; vp1++, i++) {
			if (str[i] != vp1->vb_byte)
				break;
			}
		if (str[i])
			continue;
		for (j = 0; j < len; j++)
			map[vp - lp->l_text + j] |= 1;
		vp += len;
		}
	return FALSE;
}
/**********************************************************************/
/*   Routine  to  draw  a  single  string at the specified position.  */
/*   Handles the changes in attributes.				      */
/**********************************************************************/
static void
print_string(CtwWidget ctw, line_t *lp, int row, int col, int len)
{	char	buf[1024];
	char	*end_buf;
	char	*bp;
	Pixel	fg, bg;
	int	c;
	int	end_col = col + len;
	vbyte_t	*vp;
static unsigned char *sline;
static int	scol;

	if (!XtIsManaged((Widget) ctw) && ctw->ctw.pixmap_mode == FALSE)
		return;

	if (scol == 0) {
		scol = ctw->ctw.columns;
		sline = (unsigned char *) chk_zalloc(scol);
		}
	if (scol < ctw->ctw.columns) {
		scol = ctw->ctw.columns;
		sline = (unsigned char *) chk_realloc(sline, scol);
		}
	
	print_search(ctw, lp, sline);

	vp = lp->l_text + col;

	for (c = col; c < end_col; ) {
		vbyte_t	attr;
		int	hstate = in_hilite(ctw, row, c);
		int	map_state = sline[c];
		int	continued_color = FALSE;
		int	need_hstate = TRUE;
		int	c1 = c;
		int	n = 0;

		attr = *vp;
		/***********************************************/
		/*   Draw  as  many consecutive characters as  */
		/*   we can until we hit a color change.       */
		/***********************************************/
		bp = buf;
		if (end_col - c > (int) sizeof buf)
			end_buf = &buf[sizeof buf];
		else
			end_buf = &buf[end_col - c];
		/***********************************************/
		/*   Handle  'proc'  redraw  vs scrolled back  */
		/*   solid coloring.			       */
		/***********************************************/
		if (ctw->ctw.old_top_line == -1) {
			need_hstate = FALSE;
			hstate = 0;
			}

		while (bp < end_buf) {
			if (vp->vb_fcolor != attr.vb_fcolor ||
			    vp->vb_bcolor != attr.vb_bcolor ||
			    vp->vb_attr != attr.vb_attr)
				break;
			if (sline[c + (bp - buf)] != map_state)
				break;
			if (need_hstate && in_hilite(ctw, row, c1) != hstate)
				break;
			*bp++ = vp->vb_byte;
			vp++;
			c1++;
			}
		len = bp - buf;
		if (lp->l_attr & LA_CONTINUED && 
		    len > 1 &&
		    c + len == ctw->ctw.columns) {
		    	len--;
			bp--;
			vp--;
			}
		else if (lp->l_attr & LA_CONTINUED &&
		    c + 1 == ctw->ctw.columns)
		    	continued_color = TRUE;

		if (attr.vb_attr & VB_BOLD)
			n += 8;
		if (attr.vb_attr & VB_SELECT) {
			fg = ctw->ctw.hilite_fg;
			bg = ctw->ctw.hilite_bg;
			if (fg == bg) {
				bg = ctw->ctw.x11_colors[attr.vb_fcolor];
				fg = ctw->ctw.x11_colors[attr.vb_bcolor];
				}
			}
		else if (lp->l_attr & LA_MATCH) {
			fg = ctw->ctw.x11_colors[1];
			bg = ctw->ctw.x11_colors[attr.vb_bcolor + n];
			}
		else if (lp->l_attr & LA_MATCH2) {
			fg = ctw->ctw.x11_colors[3];
			bg = ctw->ctw.x11_colors[attr.vb_bcolor + n];
			}
		else {
			fg = ctw->ctw.x11_colors[attr.vb_fcolor + n];
			bg = ctw->ctw.x11_colors[attr.vb_bcolor + n];
			}

		/***********************************************/
		/*   Handle searching.			       */
		/***********************************************/
		if (sline[c]) {
			fg = ctw->ctw.search_fg;
			bg = ctw->ctw.search_bg;
			}
		/***********************************************/
		/*   Handle   line   drawing   by   doing  it  */
		/*   ourselves.				       */
		/***********************************************/
		if (attr.vb_attr & VB_LINE) {
			draw_special_line_chars(ctw, row, c, buf, len, 
				fg, bg, attr.vb_attr, hstate);
			c += len;
			continue;
			}

		/***********************************************/
		/*   Normal text.			       */
		/***********************************************/
		if (draw_watch || ctw->ctw.flags[CTW_WATCH_DRAWING])
			draw_string(ctw, row, c, buf, len, 
				ctw->ctw.x11_colors[0],
				ctw->ctw.x11_colors[7],
				attr.vb_attr);
		if (lp->l_attr & LA_SPILT) {
			bg = ctw->ctw.spill_bg;
			fg = ctw->ctw.spill_fg;
			}
		if (hstate || attr.vb_attr & VB_SELECT) {
			fg = ctw->ctw.hilite_fg;
			bg = ctw->ctw.hilite_bg;
			}
		if (continued_color && crwin_do_cont) {
			bg = ctw->ctw.cont_bg;
			fg = ctw->ctw.cont_fg;
			}
		draw_string(ctw, row, c, buf, len, fg, bg, attr.vb_attr);
		c += len;
		}
}

/**********************************************************************/
/*   Function to reset the screen.				      */
/**********************************************************************/
static void
reset_screen(CtwWidget w)
{

	w->ctw.x = w->ctw.y = 0;
	w->ctw.saved_x = w->ctw.saved_y = 0;
	w->ctw.flags[CTW_NEWLINE_GLITCH] = TRUE;

	vbyte_clear(&w->ctw.attr);
	w->ctw.attr.vb_fcolor = WHITE;
	w->ctw.blank.vb_fcolor = WHITE;
	w->ctw.blank.vb_bcolor = BLACK;
	if (w->ctw.flags[CTW_REVERSE_VIDEO])
		set_reverse_video(w, FALSE);
	w->ctw.old_color_avail = TRUE;
	w->ctw.old_fg_color = WHITE;
	w->ctw.old_bg_color = BLACK;
	w->ctw.curr_cset = 0;
	w->ctw.char_sets[0] = 'B';
	/***********************************************/
	/*   Reset the saved color to a known state.   */
	/***********************************************/
	w->ctw.saved_attr = w->ctw.attr;

	w->ctw.escp = NULL;
	w->ctw.utfp = NULL;

	w->ctw.scroll_top = 0;
	w->ctw.scroll_bot = w->ctw.rows;

	w->ctw.flags[CTW_AUTOWRAP] = TRUE;
	w->ctw.flags[CTW_AUTOLINEFEED] = FALSE;
	w->ctw.flags[CTW_APPL_KEYPAD]  = FALSE;
	w->ctw.flags[CTW_CURSOR_KEYPAD] = FALSE;
	w->ctw.flags[CTW_LITERAL_MODE] = FALSE;
	w->ctw.flags[CTW_SCROLLING_REGION] = FALSE;
	w->ctw.flags[CTW_DESTRUCTIVE_TABS] = FALSE;
	w->ctw.flags[CTW_APPL_MOUSE] = FALSE;

	graph_destroy(w);
}

/**********************************************************************/
/*   Function to display cursor at current position.		      */
/**********************************************************************/
static void
show_cursor(CtwWidget w)
{	vbyte_t	attr;
	int	y;
	int	x;
	int	draw_hollow = FALSE;
	int	fg, bg;
	line_t	*lp;
	int	sy;

	if (w->ctw.y < 0)
		w->ctw.y = 0;
	y = dsp_get_top(w) + w->ctw.y;
	/***********************************************/
	/*   Dont do anything if cursor not visible.   */
	/***********************************************/
	if (w->ctw.have_focus) {
		if (w->ctw.cursor_visible == FALSE || 
	    	w->ctw.cursor_state == CURSOR_HIDDEN ||
	    	w->ctw.old_top_line >= 0)
			return;
		}
	x = w->ctw.x >= w->ctw.columns ? w->ctw.columns - 1 : w->ctw.x;
	lp = dsp_get_row(w, y);
	attr = lp->l_text[x];
	if (!w->ctw.have_focus)
		draw_hollow = TRUE;

	sy = w->ctw.y;
	if (w->ctw.c_flags & CTWF_CMD_MODE) {
		attr.vb_byte = w->ctw.c_ibuf[w->ctw.c_idx] ? w->ctw.c_ibuf[w->ctw.c_idx] : ' ';
		sy = w->ctw.rows - 1;
		x = w->ctw.c_idx + 9;
		}

	fg = w->ctw.x11_colors[attr.vb_bcolor];
	bg = w->ctw.x11_colors[attr.vb_fcolor];
	if (w->ctw.flags[CTW_COLOR]) {
		bg = w->ctw.cursor_color;
		if (fg == bg)
			bg = w->ctw.x11_colors[0];
		}
	else {
		/***********************************************/
		/*   If   foreground  and  background  colors  */
		/*   are  the  same  then  the  user  wont be  */
		/*   able  to  see the cursor! Draw an out of  */
		/*   focus  flashing  cursor  to let the user  */
		/*   know.				       */
		/***********************************************/
		if (fg == bg) {
			draw_hollow = TRUE;
			}
		}
	XSetForeground(XtDisplay(w), w->ctw.cursor_gc, fg);
	XSetBackground(XtDisplay(w), w->ctw.cursor_gc, bg);

	if (!draw_hollow) {
		XDrawImageString(XtDisplay(w), XtWindow(w), w->ctw.cursor_gc, 
			X_PIXEL(w, x),
			Y_PIXEL(w, sy),
			(char *) &attr.vb_byte, 1);
		}

	if (draw_hollow) {
		update_region(w, y, x, y+1, x+1);
		XSetForeground(XtDisplay(w), w->ctw.cursor_gc, bg);
		XDrawRectangle(XtDisplay(w), XtWindow(w), w->ctw.cursor_gc,
			X_PIXEL(w, x),
			ROW_TO_PIXEL(w, sy),
			w->ctw.font_width - 1,
			w->ctw.font_height - 1);
		}
	w->ctw.cursor_state = CURSOR_ON;
}
static void
scroll_down(CtwWidget w, int start_line, int num_lines)
{	int	i, j;
	vbyte_t	*vp = NULL;
	int	top = dsp_get_top(w);
	line_t	*lp, *lp1;

verify(w);
	if (num_lines > w->ctw.scroll_bot - w->ctw.scroll_top)
		num_lines = w->ctw.scroll_bot - w->ctw.scroll_top;

	insert_line(w, start_line, w->ctw.scroll_bot, num_lines, TRUE);

	for (j = 0; j < num_lines; j++) {
		i = top + w->ctw.scroll_bot - 1;
		for (; i > top + start_line; i--) {
			lp = dsp_get_row(w, i);
			lp1 = dsp_get_row(w, i-1);
			if (vp == NULL)
				vp = lp->l_text;
			lp->l_text = lp1->l_text;
			}
		lp = dsp_get_row(w, top + start_line);
		lp->l_text = vp;
		blank_line(w, top + start_line);
		vp = NULL;
		}
verify(w);
}
/**********************************************************************/
/*   Scroll screen up number of lines. Called on '\n'.		      */
/**********************************************************************/
static void
scroll_up(CtwWidget ctw, int num_lines)
{	int	j;
	int	start_line;
	int	disp_start;

verify(ctw);
	if (ctw->ctw.flags[CTW_SCROLLING_REGION]) {
		line_t	lbuf;
		line_t	*lp;
		int top = dsp_get_top(ctw);
		int end = ctw->ctw.scroll_bot;

		start_line = ctw->ctw.scroll_top;
		disp_start = ctw->ctw.scroll_top;

		delete_line(ctw, disp_start, num_lines);

		for (j = 0; j < num_lines; j++) {
			int	k;

			blank_line(ctw, top + start_line);
			lp = dsp_get_row(ctw, top + start_line);
			lbuf = *lp;
			for (k = start_line; k < end; k++) {
				line_t *lp = dsp_get_row(ctw, top + k);
				line_t *lp1 = dsp_get_row(ctw, top + k + 1);
				*lp = *lp1;
				}
			lp = dsp_get_row(ctw, top + end - 1);
			*lp = lbuf;
			}
		return;
		}

	start_line = 0;
	disp_start = 0;

	delete_line(ctw, disp_start, num_lines);

	ctw->ctw.curtop = (ctw->ctw.curtop + num_lines) % ctw->ctw.tot_rows;
	ctw->ctw.top_line += num_lines;

	/***********************************************/
	/*   Bump the selection string.		       */
	/***********************************************/
	if (ctw->ctw.sel_string) {
		ctw->ctw.sel_start_y += num_lines;
		ctw->ctw.sel_cur_y += num_lines;
		}

	for (j = 0; j < num_lines; j++) {
		blank_line(ctw, ctw->ctw.spill_cnt + ctw->ctw.tot_rows - 
			num_lines + j);
		}
	if (ctw_history)
		ctw->ctw.spill_cnt += num_lines;

	if (!ctw_history)
		ctw->ctw.top_line = MIN(ctw->ctw.top_line, ctw->ctw.max_lines);
	ctw->ctw.bot_line = dsp_get_top(ctw) + ctw->ctw.rows;
/*printf("         scroll_up %d curtop=%d tot_rows=%d\n", num_lines, ctw->ctw.curtop, ctw->ctw.tot_rows);*/

/*printf("%s: top_line=%d end=%d num_lines=%d\n", __func__, ctw->ctw.top_line, end, num_lines);*/
verify(ctw);
}
/**********************************************************************/
/*   Scroll screen up number of lines. Called on ESC[M		      */
/**********************************************************************/
static void
scroll_up_local(CtwWidget ctw, int start_line, int num_lines)
{	int	j, k;
	line_t	lbuf;
	line_t	*lp;
	int	end;
	int	top = dsp_get_top(ctw);

verify(ctw);
	if (ctw->ctw.flags[CTW_SCROLLING_REGION]) {
		end = ctw->ctw.scroll_bot;
		}
	else {
		end = ctw->ctw.rows;
		}
	if (num_lines > ctw->ctw.scroll_bot - ctw->ctw.scroll_top)
		num_lines = ctw->ctw.scroll_bot - ctw->ctw.scroll_top;

	delete_line(ctw, start_line, num_lines);

	for (j = 0; j < num_lines; j++) {
		blank_line(ctw, top + start_line);
		lp = dsp_get_row(ctw, top + start_line);
		lbuf = *lp;
		for (k = start_line; k < end; k++) {
			line_t *lp = dsp_get_row(ctw, top + k);
			line_t *lp1 = dsp_get_row(ctw, top + k + 1);
			*lp = *lp1;
			}
		lp = dsp_get_row(ctw, top + end - 1);
		*lp = lbuf;
		}
verify(ctw);
}
/**********************************************************************/
/*   Execute a compiled escape sequence.			      */
/**********************************************************************/
static  int
process_escape(CtwWidget ctw)
{	int	args[MAX_ARGS];
	char	*sargs[MAX_ARGS];
	char	*sargs2[MAX_ARGS];
	int	sarg_no = 0;
	int	arg1;
	int	arg_no = 0;
	int	n, i, y;
	int	target = 0;
	int	esc_equ = FALSE;
	int	isneg;
	char	*cp;
  	vbyte_t space, attr;
	ctw_callback_t	reason;
	line_t	*lp;

	args[0] = args[1] = 0;
	cp = ctw->ctw.escbuf;
	switch (*cp++) {
	  case '[':
	  	if (*cp == '?' || *cp == '>') {
			target = *cp++;
			break;
			}
		break;
	  case '#':
		lp = dsp_get_row(ctw, ctw->ctw.y + dsp_get_top(ctw));
	  	lp->l_attr &= ~(LA_TOP_HALF | LA_BOTTOM_HALF | LA_DOUBLE_WIDTH);
	  	switch (*cp) {
		  case '3':
		  	lp->l_attr |= LA_TOP_HALF;
		  	break;
		  case '4':
		  	lp->l_attr |= LA_BOTTOM_HALF;
		  	break;
		  case '5':
			return TRUE;
		  case '6':
		  	lp->l_attr |= LA_DOUBLE_WIDTH;
		  	break;
		  }
	  	return TRUE;
	  case ']':
	  	do_text_parms((Widget) ctw, cp);
		return TRUE;
	  case '@':
	  	do_at_parms(ctw);
		return TRUE;
	  case '\033':
	  	/***********************************************/
	  	/*   ESC ESC x prints x literally.	       */
	  	/***********************************************/
		ctw_add_raw_string(ctw, cp, 1);
		return TRUE;
	  case '(': case ')': case '*': case '+':
	  	if (ctw->ctw.flags[CTW_FONT_LOCK])
			return TRUE;
	  	i = cp[-1] - '(';
	  	ctw->ctw.char_sets[i] = *cp;
		if (cp[-1] == ')')
			return TRUE;
		switch (*cp) {
		  case '0':
			ctw->ctw.attr.vb_attr |= VB_LINE;
			break;
		  default:
			ctw->ctw.attr.vb_attr &= ~VB_LINE;
			break;
		  }
	  	return TRUE;
	  default:
	  	return FALSE;
	  }

	/***********************************************/
	/*   Handle ISC cursor size changes.	       */
	/***********************************************/
	if (*cp == '=') {
		cp++;
		esc_equ = TRUE;
		}
# define MAX_STR_SIZE 256
	while (!isalpha(*cp) && *cp != '@') {
		int	is_hex = *cp == '#';
		/***********************************************/
		/*   String  args for 'm' sequence only since  */
		/*   we  want  to  avoid expensive reclaim of  */
		/*   memory.				       */
		/***********************************************/
		if (*cp == BEL) {
			char	*bp;
			sargs[arg_no] = chk_alloc(MAX_STR_SIZE);
			bp = sargs[arg_no];
			sargs2[sarg_no++] = sargs[arg_no];
			cp++;
			while (*cp && *cp != ESC) {
				if (bp < &sargs[arg_no][MAX_STR_SIZE-2])
					*bp++ = *cp;
				cp++;
				}
			if (*cp == ESC)
				cp++;
			*bp = '\0';
			arg_no++;
			}
		else {
			if (is_hex) {
				cp++;
				}
			n = 0;
			isneg = FALSE;
			if (*cp == '-') {
				cp++;
				isneg = TRUE;
				}
			while (isdigit(*cp) || (is_hex && *cp >= 'a' && *cp <= 'f')) {
				if (is_hex) {
					n = 16 * n;
					if (isdigit(*cp))
						n += *cp++ - '0';
					else
						n += *cp++ - 'a' + 10;
					}
				else
					n = 10 * n + *cp++ - '0';
				}
			sargs[arg_no] = NULL;
			args[arg_no++] = isneg ? -n : n;
			}
		if (*cp != ';')
			break;
		cp++;
		}
	if (target == '?') {
		return do_csi_quest(ctw, *cp, arg_no, args);
		}
	if (target == '>') {
		return do_csi_gt(ctw, *cp, arg_no, args);
		}
	if (esc_equ)
		return do_escequ(ctw, *cp, arg_no, args);

	if (arg_no > 0)
		arg1 = args[0];
	else
		arg1 = 1;
	switch (*cp) {
	  case '@':
	  	do_insert_space(ctw, arg1);
	  	break;
	  case 'A':
	  	ctw->ctw.y -= arg1;
		goto check_cursor;
	  case 'B':
	  	ctw->ctw.y += arg1;
		goto check_cursor;
	  case 'C':
	  	ctw->ctw.x += arg1;
		goto check_cursor;
	  case 'D':
	  	ctw->ctw.x -= arg1;
		goto check_cursor;
	  case 'd':
	  	ctw->ctw.y = arg1 - 1;
		goto check_cursor;
	  case 'E':
	  	ctw->ctw.x = 0;
	  	ctw->ctw.y += arg1;
		goto check_cursor;
	  case 'F':
	  	ctw->ctw.x = 0;
	  	ctw->ctw.y -= arg1;
		goto check_cursor;
	  case 'G':
	  	ctw->ctw.x = arg1 ? arg1 - 1 : 0;
		goto check_cursor;
	  case 'H':
	  case 'f':
	  	if (arg_no > 1)
			ctw->ctw.x = args[1] - 1;
		else
			ctw->ctw.x = 0;
		if (arg_no > 0)
			ctw->ctw.y = args[0] - 1;
		else {
			ctw->ctw.x = 0;
			ctw->ctw.y = 0;
			}
check_cursor:
		if (ctw->ctw.x < 0)
			ctw->ctw.x = 0;
		else if (ctw->ctw.x >= ctw->ctw.columns)
			ctw->ctw.x = ctw->ctw.columns - 1;
		if (ctw->ctw.y < ctw->ctw.scroll_top)
			ctw->ctw.y = ctw->ctw.scroll_top;
		else if (ctw->ctw.y >= ctw->ctw.scroll_bot)
			ctw->ctw.y = ctw->ctw.scroll_bot - 1;
		if (ctw->ctw.y > ctw->ctw.rows)
			ctw->ctw.y = ctw->ctw.rows - 1;
		break;
	  case 'J':
		if (arg_no == 0)
			args[0] = 0;
		switch (args[0]) {
		  case 0:
		  	clear_to_eol(ctw);
			clear_lines(ctw, ctw->ctw.y + 1, ctw->ctw.rows);
		  	break;
		  case 1:
		  	clear_from_beginning(ctw);
			clear_lines(ctw, 0, ctw->ctw.y - 1);
			break;
		  case 2:
		  	clear_screen(ctw);
			break;
		  }
	  	break;
	  case 'K':
	  	if (arg_no == 0)
			args[0] = 0;
		switch (args[0]) {
		  case 0:
		  	clear_to_eol(ctw);
			break;
		  case 1:
		  	clear_from_beginning(ctw);
			break;
		  case 2:
		  	clear_from_beginning(ctw);
			clear_to_eol(ctw);
			break;
		  }
	  	break;
	  case 'L':
	  	if (ctw->ctw.y >= ctw->ctw.scroll_top && ctw->ctw.y < ctw->ctw.scroll_bot)
		  	scroll_down(ctw, ctw->ctw.y, arg1);
	  	break;
	  case 'M':
	  	if (ctw->ctw.y >= ctw->ctw.scroll_top && ctw->ctw.y < ctw->ctw.scroll_bot)
		  	scroll_up_local(ctw, ctw->ctw.y, arg1);
	  	break;
	  case 'P': {
		y = dsp_get_top(ctw) + ctw->ctw.y;
		vbyte_clear(&space);
		space.vb_fcolor = ctw->ctw.blank.vb_fcolor;
		space.vb_bcolor = ctw->ctw.blank.vb_bcolor;

	  	for (n = arg1; n-- > 0; ) {
			lp = dsp_get_row(ctw, y);
		  	for (i = ctw->ctw.x; i < ctw->ctw.columns - 1; i++) {
				lp->l_text[i] = lp->l_text[i+1];
				}
			lp->l_text[i] = space;
			}
		update_region(ctw, ctw->ctw.y, ctw->ctw.x, ctw->ctw.y+1, ctw->ctw.columns);
	  	break;
		}
	  case 'S':
	  	do_index(ctw);
		break;
	  case 'T':
	  	do_rev_index(ctw);
		break;

	  case 'X':
	  	/***********************************************/
	  	/*   ISC  &  SCO  allow ESC[X have to erase a  */
	  	/*   sequence of blanks. So does Linux.	       */
	  	/***********************************************/
		if (ctw->ctw.flags[CTW_CSI_X_SUPPORTED] == FALSE)
			return FALSE;

		attr = ctw->ctw.attr;
		attr.vb_byte = ' ';
		attr.vb_attr &= ~VB_LINE;
		y = dsp_get_top(ctw) + ctw->ctw.y;
		lp = dsp_get_row(ctw, y);
		for (i = 0; i < arg1; i++) {
			if (ctw->ctw.x + i >= ctw->ctw.columns)
				break;
			lp->l_text[ctw->ctw.x + i] = attr;
			}
		update_region(ctw, ctw->ctw.y, ctw->ctw.x, ctw->ctw.y+1, ctw->ctw.x + arg1);
		break;

	  case 'Y':
	  	/***********************************************/
	  	/*   Set mask for mouse buttons.	       */
	  	/***********************************************/
		ctw->ctw.mouse_mask = args[0];
		break;

	  case 'Z':
	  	/***********************************************/
	  	/*   ESC[Z Backtab			       */
	  	/***********************************************/
		while (ctw->ctw.x > 0 &&
		      ctw->ctw.tab_stops[ctw->ctw.x] == FALSE)
		      	ctw->ctw.x--;
		break;

	  case 'c':
	  	/***********************************************/
	  	/*   ESC[c -- ask for answerback message.      */
	  	/***********************************************/
		send_input(ctw, "\033[?1;2c", 7);
		break;
	  case 'g':
	  	if (ctw->ctw.flags[CTW_CSI_g]) {
			/***********************************************/
			/*   ESC[3g  is  VT220 mechanism for clearing  */
			/*   tabs.				       */
			/***********************************************/
			switch (arg1) {
			  case 3:
			  	memset(ctw->ctw.tab_stops, 0, ctw->ctw.columns);
				return TRUE;
			  default:
			  	break;
			  }
			return FALSE;
			}

		{
		vbyte_t *vp_start = dsp_get_row(ctw, dsp_get_top(ctw) + ctw->ctw.y)->l_text;
		vbyte_t *vp = vp_start + ctw->ctw.x;
		vp->vb_byte = arg1;
		ctw->ctw.x++;
		update_region(ctw, ctw->ctw.y, ctw->ctw.x - 1, ctw->ctw.y+1, ctw->ctw.x);
		}
	  	return TRUE;
	  case 'h':
	  	for (i = 0; i < arg_no; i++) {
			switch (args[i]) {
			  case 4:
			  	ctw->ctw.flags[CTW_INSERT_MODE] = TRUE;
				break;
			  default:
			  	return FALSE;
			  }
			}
	  	break;
	  case 'l':
	  	for (i = 0; i < arg_no; i++) {
			switch (args[i]) {
			  case 4:
			  	ctw->ctw.flags[CTW_INSERT_MODE] = FALSE;
				break;
			  default:
			  	return FALSE;
			  }
			}
	  	break;
	  case 'm':
	  	if (arg_no == 0) {
		  	int	lflag = ctw->ctw.attr.vb_attr & VB_LINE;
			ctw->ctw.attr.vb_attr = lflag;
			if (ctw->ctw.old_color_avail) {
			  	ctw->ctw.attr.vb_fcolor = ctw->ctw.old_fg_color;
			  	ctw->ctw.attr.vb_bcolor = ctw->ctw.old_bg_color;
				}
			ctw->ctw.old_color_avail = FALSE;
/*			arg_no = 1;
			args[0] = 0;*/
			}
	  	for (i = 0; i < arg_no; i++) {
			switch (args[i]) {
			  case 0: {
			  	int	lflag = ctw->ctw.attr.vb_attr & VB_LINE;
				ctw->ctw.attr.vb_attr = lflag;
				if (ctw->ctw.flags[CTW_COLOR_RESET] ||
				    ctw->ctw.old_color_avail == FALSE) {
				  	ctw->ctw.attr.vb_fcolor = ctw->ctw.blank.vb_fcolor;
				  	ctw->ctw.attr.vb_bcolor = ctw->ctw.blank.vb_bcolor;
					}
				else {
				  	ctw->ctw.attr.vb_fcolor = ctw->ctw.old_fg_color;
				  	ctw->ctw.attr.vb_bcolor = ctw->ctw.old_bg_color;
					}
				ctw->ctw.old_color_avail = 0;
				break;
				}
			  case 1:
			  	ctw->ctw.attr.vb_attr |= VB_BOLD;
				break;
			  case 4:
			  	ctw->ctw.attr.vb_attr |= VB_UNDERLINE;
				break;
			  case 5:
			  	ctw->ctw.attr.vb_attr |= VB_FLASHING;
				ctw->ctw.blinking = TRUE;
				/***********************************************/
				/*   If  we  dont  have input focus then kick  */
				/*   off  the  cursor  flasher  so  that  the  */
				/*   blinking text gets to flash as well.      */
				/***********************************************/
				if (ctw->ctw.have_focus == FALSE)
					handle_focus_change(ctw, TRUE);
				break;
			  case 7: {
			  	if (ctw->ctw.old_color_avail == FALSE) {
					int	n = ctw->ctw.attr.vb_fcolor;
					ctw->ctw.old_color_avail = TRUE;
			  		ctw->ctw.old_fg_color = ctw->ctw.attr.vb_fcolor;
			  		ctw->ctw.old_bg_color = ctw->ctw.attr.vb_bcolor;
			  		ctw->ctw.attr.vb_fcolor = ctw->ctw.attr.vb_bcolor;
			  		ctw->ctw.attr.vb_bcolor = n;
					}
				break;
				}
			  case 8:
			  	/***********************************************/
			  	/*   ESC  [  8m - Save current fg/bg color in  */
			  	/*   case of later 39/49m		       */
			  	/***********************************************/
				ctw->ctw.saved_attr = ctw->ctw.attr;
				break;
			  case 10: /* SCO */
			  	/***********************************************/
			  	/*   Select primary font.		       */
			  	/***********************************************/
			  	ctw->ctw.curr_cset = 0;
			  	break;
			  case 11: /* SCO */
			  	/***********************************************/
			  	/*   Select first alternate font.	       */
			  	/***********************************************/
			  	ctw->ctw.curr_cset = 1;
			  	break;
			  case 21:
			  	ctw->ctw.attr.vb_attr &= ~VB_BOLD;
				break;
			  case 24:
			  	ctw->ctw.attr.vb_attr &= ~VB_UNDERLINE;
				break;
			  case 25:
			  	ctw->ctw.attr.vb_attr &= ~VB_FLASHING;
				break;
			  case 27: {
				if (ctw->ctw.flags[CTW_COLOR_RESET] ||
				    ctw->ctw.old_color_avail == FALSE) {
				  	ctw->ctw.attr.vb_fcolor = ctw->ctw.blank.vb_fcolor;
				  	ctw->ctw.attr.vb_bcolor = ctw->ctw.blank.vb_bcolor;
					}
				else {
			  		ctw->ctw.old_fg_color = ctw->ctw.attr.vb_fcolor;
			  		ctw->ctw.old_bg_color = ctw->ctw.attr.vb_bcolor;
					}
				ctw->ctw.old_color_avail = 0;
				break;
				}
			  case 30: case 31: case 32: case 33:
			  case 34: case 35: case 36: case 37:
		  		ctw->ctw.old_fg_color = ctw->ctw.blank.vb_fcolor;
		  		ctw->ctw.old_bg_color = ctw->ctw.blank.vb_bcolor;
				ctw->ctw.old_color_avail = TRUE;
			  	ctw->ctw.attr.vb_fcolor = args[i] - 30;
			  	break;
			  case 38: 
			  	/***********************************************/
			  	/*   ESC [ 38 ; 5 ; nn m - palette color       */
			  	/***********************************************/
			  	ctw->ctw.attr.vb_fcolor = args[i+2];
				i += 2;
			  	break;
			  case 39:
			  	/***********************************************/
			  	/*   ESC [ 39m Restore foreground color.       */
			  	/***********************************************/
			  	ctw->ctw.attr.vb_fcolor = ctw->ctw.saved_attr.vb_fcolor;
				break;
			  case 40: case 41: case 42: case 43:
			  case 44: case 45: case 46: case 47:
		  		ctw->ctw.old_fg_color = ctw->ctw.blank.vb_fcolor;
		  		ctw->ctw.old_bg_color = ctw->ctw.blank.vb_bcolor;
				ctw->ctw.old_color_avail = TRUE;
			  	ctw->ctw.attr.vb_bcolor = args[i] - 40;
			  	break;
			  case 48: 
			  	/***********************************************/
			  	/*   ESC [ 48 ; 5 ; nn m - palette color       */
			  	/***********************************************/
			  	ctw->ctw.attr.vb_bcolor = args[i+2];
				i += 2;
			  	break;
			  case 49:
			  	/***********************************************/
			  	/*   ESC [ 49m Restore background color.       */
			  	/***********************************************/
			  	ctw->ctw.attr.vb_bcolor = ctw->ctw.saved_attr.vb_bcolor;
				break;
			  case 70: case 71: case 72: case 73:
			  case 74: case 75: case 76: case 77:
				ctw->ctw.cursor_color = ctw->ctw.x11_colors[args[i] - 70];
			  	break;
			  case 78:
				ctw->ctw.cursor_color = ctw->ctw.save_cursor_color;
				break;

			  case 1900:
			  	ctw->ctw.flags[CTW_LITERAL_MODE] = FALSE;
			  	break;
			  case 1901:
			  	ctw->ctw.flags[CTW_LITERAL_MODE] = 2;
			  	break;
			  case 1902:
			  	ctw->ctw.attr.vb_attr |= VB_STRIKETHRU;
				break;
			  case 1903:
			  	ctw->ctw.attr.vb_attr &= ~VB_STRIKETHRU;
				break;
			  case 1904:
				reason.client_data = ctw->ctw.client_data;
			  	reason.reason = CTWR_SET_AUTOSWITCH;
			  	reason.flags = 1;
			  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
				break;
			  case 1905:
				reason.client_data = ctw->ctw.client_data;
			  	reason.reason = CTWR_SET_AUTOSWITCH;
			  	reason.flags = 0;
			  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
				break;
			  case DRAW_RECT: /* 1920 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 4;
				break;
			  case DRAW_FILL_RECT: /* 1921 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 4;
				break;
			  case DRAW_LINE: /* 1922 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 4;
				break;
			  case DRAW_PIXEL: /* 1923 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 2;
				break;
			  case DRAW_CLEAR: /* 1924 */
			  	graph_destroy(ctw);
				break;
			  case DRAW_CLEAR_REDRAW: /* 1925 */
			  	graph_destroy(ctw);
				ctw_redraw(ctw);
				break;
			  case DRAW_NAME: /* 1926 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 5;
				break;
			  case DRAW_STRING: /* 1927 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 3;
				break;
			  case DRAW_ARC: /* 1928 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 6;
				break;
			  case DRAW_FILL_ARC: /* 1929 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 6;
				break;
			  case DRAW_SET_BACKGROUND: /* 1930 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 1;
				break;
			  case DRAW_SET_FOREGROUND: /* 1931 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 1;
				break;
			  case DRAW_SET_FONT: /* 1932 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 1;
				break;
			  case DRAW_IMAGE_STRING: /* 1933 */
			  	graph_add(ctw, args + i, sargs + i);
				i += 3;
				break;
			  case DRAW_QUERY_WINSIZE: { /* 1934 */
			  	char buf[128];
			  	snprintf(buf, sizeof buf,
					"%dx%d\n", ctw->core.width, ctw->core.height);
				send_input(ctw, buf, strlen(buf));
			  	break;
				}
			  case DRAW_DUMP_STATUS: { /* 1935 */
			  	char buf[BUFSIZ];
			  	char buf2[BUFSIZ];
				char	*user = getenv("USER") ? getenv("USER") : "fcterm";
				graph_t *g;
				int	n;

				/***********************************************/
				/*   Avoid  security  hole  with people being  */
				/*   allowed to write to someone elses files.  */
				/***********************************************/
				snprintf(buf, sizeof buf, "/tmp/%s", user);
				mkdir(buf, 0777);
			  	snprintf(buf, sizeof buf, "/tmp/%s/fcterm.%d.tmp", user, getpid());
			  	snprintf(buf2, sizeof buf2, "/tmp/%s/fcterm.%d", user, getpid());
				remove(buf);
				FILE *fp = fopen(buf, "w");
				if (fp == NULL) {
					printf("fcterm: [%d] Cannot create '%s'\n", getpid(), buf);
					break;
					}
				chmod(buf, 0666);
				fprintf(fp, "rows=%d\n", ctw->ctw.rows);
				fprintf(fp, "cols=%d\n", ctw->ctw.columns);
				fprintf(fp, "width=%d\n", ctw->core.width);
				fprintf(fp, "height=%d\n", ctw->core.height);
				fprintf(fp, "font_width=%d\n", ctw->ctw.font_width);
				fprintf(fp, "font_height=%d\n", ctw->ctw.font_height);
				fprintf(fp, "pid=%d\n", getpid());
				fprintf(fp, "build=%d\n", version_build_no);
			  	for (n = 0, g = ctw->ctw.c_chain; g; g = g->g_next)
					n++;
				fprintf(fp, "graph_chain_length=%d\n", n);
				fclose(fp);
				rename(buf, buf2);
			  	break;
				}
			  case 1936:
			  	ctw->ctw.attr.vb_attr |= VB_BOXED;
			  	break;
			  case 1937:
			  	ctw->ctw.attr.vb_attr &= ~VB_BOXED;
			  	break;
			  }
			}

		for (i = 0; i < sarg_no; i++)
			chk_free_ptr((void **) &sargs2[i]);
	  	break;
	  case 'n':
	  	for (i = 0; i < arg_no; i++) {
			switch (args[i]) {
			  case 6: {
			  	/***********************************************/
			  	/*   xterm/resize send this to find where the  */
			  	/*   cursor is (and get the tty size).	       */
			  	/***********************************************/
			  	char buf[128];
			  	snprintf(buf, sizeof buf,
					"\033[%d;%dR", ctw->ctw.y + 1,
					ctw->ctw.x + 1);
				send_input(ctw, buf, strlen(buf));
				break;
				}
			  }
			}
	  	break;
	  case 'r':
	  	if (arg_no >= 0 && args[0] > args[1])
			break;
		ctw->ctw.scroll_top = 0;
		ctw->ctw.scroll_bot = ctw->ctw.rows;
		if (arg_no == 2 && args[0] == 0 && args[1] == 0)
			break;
		/***********************************************/
		/*   Allow  user  to  ignore scrolling region  */
		/*   requests.				       */
		/***********************************************/
		if (ctw->ctw.flags[CTW_SCROLLING_REGION_ENABLE] == FALSE)
			break;
		ctw->ctw.flags[CTW_SCROLLING_REGION] = FALSE;
		if (arg_no >= 1) {
			ctw->ctw.flags[CTW_SCROLLING_REGION] = TRUE;
			ctw->ctw.scroll_top = args[0] ? args[0] - 1 : 0;
			if (arg_no >= 2)
				ctw->ctw.scroll_bot = args[1] ? args[1] : 0;
			if (ctw->ctw.scroll_top < 0)
				ctw->ctw.scroll_top = 0;
			if (ctw->ctw.scroll_bot < ctw->ctw.scroll_top || 
			    ctw->ctw.scroll_bot > ctw->ctw.rows)
				ctw->ctw.scroll_bot = ctw->ctw.rows;
			/***********************************************/
			/*   If  scrolling  region  to  entire window  */
			/*   then turn flag off.		       */
			/***********************************************/
			if (ctw->ctw.scroll_top == 0 &&
			    ctw->ctw.scroll_bot == ctw->ctw.rows)
				ctw->ctw.flags[CTW_SCROLLING_REGION] = FALSE;
			}
		ctw->ctw.x = 0;
		ctw->ctw.y = 0;
	  	break;
	  case 's':
	  	ctw->ctw.saved_x = ctw->ctw.x;
	  	ctw->ctw.saved_y = ctw->ctw.y;
		break;
	  case 't':
	  	/***********************************************/
	  	/*   Shelltool/cmdtool    compatable   escape  */
	  	/*   sequences.				       */
	  	/***********************************************/
		reason.client_data = ctw->ctw.client_data;
		switch (args[0]) {
		  case 1:
		  	reason.reason = CTWR_OPEN_WINDOW;
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 2:
		  	reason.reason = CTWR_CLOSE_WINDOW;
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 3:
		  	reason.reason = CTWR_MOVE_WINDOW;
			reason.x = args[1];
			reason.y = args[2];
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 4:
		  	reason.reason = CTWR_SIZE_WINDOW_PIXELS;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 5:
		  	reason.reason = CTWR_FRONT_WINDOW;
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 6:
		  	reason.reason = CTWR_BACK_WINDOW;
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
		  	break;
		  case 8:
		  	reason.reason = CTWR_SIZE_WINDOW_CHARS;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 99:
		  	reason.reason = CTWR_SAVE_STATE;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			break;
		  case 100:
		  	reason.reason = CTWR_RESTORE_STATE;
			reason.height = args[1];
			reason.width = args[2];
		  	XtCallCallbacks((Widget) ctw, XtNapplCallback, (caddr_t) &reason);
			break;
		  }
	  	break;
	  case 'u':
	  	ctw->ctw.x = ctw->ctw.saved_x;
	  	ctw->ctw.y = ctw->ctw.saved_y;
		goto check_cursor;
	  default:
	  	return FALSE;
	  }
	return TRUE;
}
/**********************************************************************/
/*   Send input via the callback list to the owner.		      */
/**********************************************************************/
static void
send_input(CtwWidget ctw, char *buf, int len)
{
	ctw_callback_t	reason;
	reason.reason = CTWR_INPUT;
	reason.client_data = ctw->ctw.client_data;
	reason.ptr = buf;
	reason.len = len;

	XtCallCallbacks((Widget) ctw, XtNkbdCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Function to display a string internally.			      */
/**********************************************************************/
static void
send_str(CtwWidget w, char *buf, int len)
{	vbyte_t	attr;
	char	*escp = w->ctw.escp;

	attr = w->ctw.attr;
	w->ctw.attr.vb_fcolor = 7;
	w->ctw.attr.vb_bcolor = 0;
	ctw_add_string2(w, buf, len);
	w->ctw.attr = attr;
	w->ctw.escp = escp;
}
/**********************************************************************/
/*   Put us into reverse video mode.				      */
/**********************************************************************/
static void
set_reverse_video(CtwWidget ctw, int flag)
{
	Pixel	t;
	ctw->ctw.flags[CTW_REVERSE_VIDEO] = flag;
	t = ctw->ctw.x11_colors[BLACK];
	ctw->ctw.x11_colors[BLACK] = ctw->ctw.x11_colors[WHITE];
	ctw->ctw.x11_colors[WHITE] = t;
	XSetWindowBackground(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.x11_colors[BLACK]);
}
/**********************************************************************/
/*   Function to allocate color pixels.				      */
/**********************************************************************/
static Pixel
alloc_color_pixel(Display *dpy, int default_depth, char *color)
{
	XColor		exact_def;
	Colormap default_color_map = DefaultColormap(dpy, DefaultScreen(dpy));

	UNUSED_PARAMETER(default_depth);

	if (!XParseColor(dpy, default_color_map, color, &exact_def)) {
#if 0
		fprintf(stderr, "Color name %s not in database -- trying default: %s\n",
			color,
			color_names[i]);
		if (!XParseColor(dpy, default_color_map, color_names[i], &exact_def)) {
			fprintf(stderr, "Color name %s not in database\n",
				color_names[i]);
/*				exit(1);*/
			}
#endif

		}
	if (!XAllocColor(dpy, default_color_map, &exact_def)) {
		fprintf(stderr, "Can't allocate color: %s -- all color cells allocated.\n",
			color);
		}
	return exact_def.pixel;
}
static  int
setup_x11_colors(CtwWidget new, Display *dpy)
{
	static int is_color;

	int	default_depth;
	int		i, r, g, b;
	static char	*color_tbl[] = {
"#000000", "#b21818", "#18b218", "#b2b218", "#4040ee", "#b218b2", "#18b2b2", "#b2b2b2",
"#000000", "#ff5454", "#54ff54", "#ffff54", "#5454ff", "#ff54ff", "#54ffff", "#ffffff",
"#000000", "#ff0000", "#00ff00", "#ffff00", "#0000ff", "#ff00ff", "#00ffff", "#ffffff",
"#000000", "#ff0000", "#00ff00", "#ffff00", "#0000ff", "#ff00ff", "#00ffff", "#ffffff",
//  "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", 
//  "gray90", "red", "green", "yellow", "blue", "magenta", "cyan", "white", 
		NULL
		};
	int num_colors = sizeof(color_tbl) / sizeof(color_tbl[0]);
	static char	*mono_tbl[] = {
  "black", "gray80", "gray85", "gray90", "gray95", "gray96", "gray98", "white",
  "black", "gray80", "gray85", "gray90", "gray95", "gray96", "gray98", "white",
		NULL
		};
	char	**color_names = color_tbl;
	char	*color;

	/***********************************************/
	/*   If  we've  got  a  mono-only screen then  */
	/*   set  up  the  flag  saying whether we're  */
	/*   running on mono or color screen.	       */
	/***********************************************/
	default_depth = DefaultDepth(dpy, DefaultScreen(dpy));
	if (default_depth == 1 || getenv("CRMONO") != (char *) NULL) {
		color_names = mono_tbl;
		is_color = FALSE;
		/***********************************************/
		/*   Force reverse video selections.	       */
		/***********************************************/
		new->ctw.hilite_fg = new->ctw.hilite_bg;
		}
	else
		is_color = TRUE;

	for (i = 0; i < num_colors; i++) {
		/***********************************************/
		/*   Allow user to override color selection.   */
		/***********************************************/
		color = color_names[i];
		if (new->ctw.color_names[i] && new->ctw.color_names[i][0]) {
			color = new->ctw.color_names[i];
			}
		new->ctw.x11_colors[i] = alloc_color_pixel(dpy, default_depth, color);
		}

	/***********************************************/
	/*   6x6x6 color cube.			       */
	/***********************************************/
	i = 16;
	for (r = 0; r < 6; r++) {
		for (g = 0; g < 6; g++) {
			for (b = 0; b < 6; b++) {
				char	buf[32];
				snprintf(buf, sizeof buf, "#%02x%02x%02x", 
					(int) (((r + 1) / 6.) * 255),
					(int) (((g + 1) / 6.) * 255),
					(int) (((b + 1) / 6.) * 255));
				new->ctw.x11_colors[i++] = alloc_color_pixel(dpy, default_depth, buf);
				}
			}
		}
	for (i = 232; i < 256; i++) {
		int	g = i - 232;
		char	buf[32];
		snprintf(buf, sizeof buf, "#%02x%02x%02x",
			(int) ((g / 24.) * 0xff),
			(int) ((g / 24.) * 0xff),
			(int) ((g / 24.) * 0xff));
		new->ctw.x11_colors[i] = alloc_color_pixel(dpy, default_depth, buf);
		}
	return is_color;
}
static  int 
sunfuncvalue(int keycode)
{
  	switch (keycode) {
		case XK_F1:	return(224);
		case XK_F2:	return(225);
		case XK_F3:	return(226);
		case XK_F4:	return(227);
		case XK_F5:	return(228);
		case XK_F6:	return(229);
		case XK_F7:	return(230);
		case XK_F8:	return(231);
		case XK_F9:	return(232);
		case XK_F10:	return(233);
		case XK_F11:	return(192);
		case XK_F12:	return(193);
		case XK_F13:	return(194);
		case XK_F14:	return(195);
		case XK_F15:	return(196);
		case XK_Help:	return(196);
		case XK_F16:	return(197);
		case XK_Menu:	return(197);
		case XK_F17:	return(198);
		case XK_F18:	return(199);
		case XK_F19:	return(200);
		case XK_F20:	return(201);

		case XK_R1:	return(208);
		case XK_R2:	return(209);
		case XK_R3:	return(210);
		case XK_R4:	return(211);
		case XK_R5:	return(212);
		case XK_R6:	return(213);
		case XK_R7:	return(214);
		case XK_R8:	return(215);
		case XK_R9:	return(216);
		case XK_R10:	return(217);
		case XK_R11:	return(218);
		case XK_R12:	return(219);
		case XK_R13:	return(220);
		case XK_R14:	return(221);
		case XK_R15:	return(222);
  
		case XK_Home:	return 214;
		case XK_End:	return 220;

		case XK_Find :	return(1);
		case XK_Insert:	return(2);
		case XK_Delete:	return(3);
# if defined(DXK_Remove)
		case DXK_Remove: return(3);
# endif
		case XK_Select:	return(4);
		case XK_KP_Prior:	return(5);
		case XK_Prior:	return(5);
		case XK_KP_Next:	return(6);
		case XK_Next:	return(6);
		default:	return(-1);
	}
}

/**********************************************************************/
/*   Clear to end of line.					      */
/**********************************************************************/
static void
clear_to_eol(CtwWidget ctw)
{	vbyte_t	space;
	int	i;
	int	y = dsp_get_top(ctw) + ctw->ctw.y;
	line_t	*lp;

	vbyte_clear(&space);
	if (fill_to_black) {
		space.vb_fcolor = ctw->ctw.blank.vb_fcolor;
		space.vb_bcolor = ctw->ctw.blank.vb_bcolor;
		XClearArea(XtDisplay(ctw), XtWindow(ctw), 
			X_PIXEL(ctw, ctw->ctw.x),
			ROW_TO_PIXEL(ctw, ctw->ctw.y),
			ctw->ctw.internal_width + (ctw->ctw.columns - ctw->ctw.x) * ctw->ctw.font_width, 
			ctw->ctw.font_height,
			False);
		}
	else {
		Pixel fg = ctw->ctw.x11_colors[ctw->ctw.attr.vb_bcolor];
		space.vb_fcolor = ctw->ctw.attr.vb_fcolor;
		space.vb_bcolor = ctw->ctw.attr.vb_bcolor;
		XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
		XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			X_PIXEL(ctw, ctw->ctw.x),
			ROW_TO_PIXEL(ctw, ctw->ctw.y),
			ctw->ctw.internal_width + (ctw->ctw.columns - ctw->ctw.x) * ctw->ctw.font_width, 
			ctw->ctw.font_height);
		}

	lp = dsp_get_row(ctw, y);
	for (i = ctw->ctw.x; i < ctw->ctw.columns; i++)
		lp->l_text[i] = space;
}
/**********************************************************************/
/*   Clear from beginning of line to current character.		      */
/**********************************************************************/
static void
clear_from_beginning(CtwWidget ctw)
{	vbyte_t	space;
	int	i;
	int	y = dsp_get_top(ctw) + ctw->ctw.y;
	line_t	*lp;

	vbyte_clear(&space);
	if (fill_to_black) {
		space.vb_fcolor = ctw->ctw.blank.vb_fcolor;
		space.vb_bcolor = ctw->ctw.blank.vb_bcolor;
		}
	else {
		space.vb_fcolor = ctw->ctw.attr.vb_fcolor;
		space.vb_bcolor = ctw->ctw.attr.vb_bcolor;
		}
	lp = dsp_get_row(ctw, y);
	for (i = 0; i <= ctw->ctw.x; i++)
		lp->l_text[i] = space;

	if (fill_to_black) {
		XClearArea(XtDisplay(ctw), XtWindow(ctw), 
			ctw->ctw.internal_width,
			ROW_TO_PIXEL(ctw, ctw->ctw.y),
			X_PIXEL(ctw, ctw->ctw.x + 1),
			ctw->ctw.font_height,
			False);
		}
	else {
		Pixel fg = ctw->ctw.x11_colors[ctw->ctw.attr.vb_bcolor];
		XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
		XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			ctw->ctw.internal_width,
			ROW_TO_PIXEL(ctw, ctw->ctw.y),
			X_PIXEL(ctw, ctw->ctw.x + 1),
			ctw->ctw.font_height);
		}
}
/**********************************************************************/
/*   Function  to  clear  a selected range of lines. Lines specified  */
/*   relative to current window.				      */
/**********************************************************************/
static void
clear_lines(CtwWidget ctw, int top, int bot)
{	int	i;

	if (top >= bot)
		return;

	top += dsp_get_top(ctw);
	bot += dsp_get_top(ctw);
		
	for (i = top; i < bot; i++) {
		blank_line(ctw, i);
		}
	top -= dsp_get_top(ctw);
	bot -= dsp_get_top(ctw);

	if (fill_to_black) {
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			ctw->ctw.internal_width, 
			ROW_TO_PIXEL(ctw, top),
			ctw->ctw.internal_width + (ctw->ctw.columns * ctw->ctw.font_width), 
			ROW_TO_PIXEL(ctw, (bot - top + 1)), 
			FALSE);
		}
	else {
		Pixel fg = ctw->ctw.x11_colors[ctw->ctw.attr.vb_bcolor];
		XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
		XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			ctw->ctw.internal_width, 
			ROW_TO_PIXEL(ctw, top),
			ctw->ctw.internal_width + (ctw->ctw.columns * ctw->ctw.font_width), 
			ROW_TO_PIXEL(ctw, (bot - top + 1)));
		}
}
/**********************************************************************/
/*   Clear entire screen.					      */
/**********************************************************************/
static void
clear_screen(CtwWidget ctw)
{	int	i;
	line_t	*lp2;

	for (i = 0; i < ctw->ctw.rows; i++) {
		lp2 = dsp_get_row(ctw, dsp_get_top(ctw) + i);
		blank_line_ptr(ctw, lp2);
		}

	if (fill_to_black)
		XClearWindow(XtDisplay(ctw), XtWindow(ctw));
	else {
		Pixel fg = ctw->ctw.x11_colors[ctw->ctw.attr.vb_bcolor];
		XSetForeground(XtDisplay(ctw), ctw->ctw.gc, fg);
		XFillRectangle(XtDisplay(ctw), XtWindow(ctw), ctw->ctw.gc,
			0, 0,
			ctw->core.width, ctw->core.height);
		}
}
/**********************************************************************/
/*   Create  a  cached  pixmap  for a character -- needed for double  */
/*   ht/width etc.						      */
/**********************************************************************/
static  Pixmap
create_cached_pixmap(CtwWidget ctw, line_t *lp, int ch, int fg, int bg)
{	Pixmap	*pa;
	Pixmap	**ppa;
	Pixmap	p;
	Display	*dpy = XtDisplay(ctw);
	char	ch2;
	int	width, height;

	if (lp->l_attr & LA_TOP_HALF) {
		ppa = &ctw->ctw.c_th_pixmaps;
		width = ctw->ctw.font_width;
		height = ctw->ctw.font_height;
		}
	else if (lp->l_attr & LA_BOTTOM_HALF) {
		ppa = &ctw->ctw.c_bh_pixmaps;
		width = ctw->ctw.font_width;
		height = ctw->ctw.font_height;
		}
	else {
		ppa = &ctw->ctw.c_dw_pixmaps;
		width = ctw->ctw.font_width * 2;
		height = ctw->ctw.font_height;
		}

	pa = *ppa;
	if (pa == (Pixmap *) NULL) {
		pa = *ppa = (Pixmap *) chk_calloc(256 * sizeof(Pixmap), 1);
		}

	if (pa[ch] == (Pixmap) 0) {
		pa[ch] = XCreatePixmap(dpy, XtWindow(ctw),
			width, height,
			DefaultDepth(dpy, DefaultScreen(dpy)));
		XSetForeground(dpy, ctw->ctw.gc, fg);
		XSetBackground(dpy, ctw->ctw.gc, bg);
		XFillRectangle(dpy, pa[ch],
			ctw->ctw.gc,
			0, 0, width, height);
		}
	p = pa[ch];

	ch2 = ch;
	XDrawImageString(dpy, p, ctw->ctw.gc, 
		0, 5,
		&ch2, 1);
	return p;
}
/**********************************************************************/
/*   Function  called  on  expiration  of timer to cause flashing to  */
/*   occur.							      */
/**********************************************************************/
static void
cursor_flash_proc(XtPointer client_data, XtIntervalId *timer)
{	CtwWidget	ctw = (CtwWidget) client_data;

	UNUSED_PARAMETER(timer);

	/***********************************************/
	/*   If we dont have focus, turn off timer.    */
	/***********************************************/
	if (ctw->ctw.flashrate && (handle_blinking(ctw) || ctw->ctw.have_focus))
		ctw->ctw.cursor_timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) ctw),
			(long) ctw->ctw.flashrate, cursor_flash_proc, ctw);
	else
		ctw->ctw.cursor_timer = 0;

	if (!ctw->ctw.have_focus) {
		turn_on_cursor(ctw);
		return;
		}
	if (ctw->ctw.flashrate) {
		toggle_cursor(ctw);
		}
}
/**********************************************************************/
/*   The accept_focus method.					      */
/**********************************************************************/
# if 0
static int
ctw_accept_focus(Widget w, Time t)
{
	 XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToNone, t);
	 return TRUE;
}
# endif
/**********************************************************************/
/*   Add a character in literal mode (reverse video).		      */
/**********************************************************************/
static void
ctw_add_char(CtwWidget ctw, char *str)
{	int	fg = ctw->ctw.attr.vb_fcolor;
	int	bg = ctw->ctw.attr.vb_bcolor;

  	ctw->ctw.attr.vb_fcolor = bg;
  	ctw->ctw.attr.vb_bcolor = fg;

	ctw_add_string2(ctw, str, strlen(str));

  	ctw->ctw.attr.vb_fcolor = fg;
  	ctw->ctw.attr.vb_bcolor = bg;
}
/**********************************************************************/
/*   Main public function for adding a string to the widget.	      */
/**********************************************************************/
void
ctw_add_string(CtwWidget ctw, char *str, int len)
{	long	t;

	if (len < 0)
		len = strlen(str);

	/***********************************************/
	/*   If  we  have a log file then copy string  */
	/*   to log file.			       */
	/***********************************************/
	if (ctw->ctw.flags[CTW_LOGGING] && ctw->ctw.log_fp) {
		if (fwrite(str, len, 1, ctw->ctw.log_fp) != 1) {
			perror(ctw->ctw.log_file);
			fprintf(stderr, "Log file closed\n");
			fclose(ctw->ctw.log_fp);
			ctw->ctw.log_fp = (FILE *) NULL;
			}
		}

	if (ctw->ctw.flags[CTW_SPEED] >= 100) {
		ctw_add_string2(ctw, str, len);
		return;
		}

	if (ctw->ctw.flags[CTW_SPEED] == 0)
		t = 20000;
	else
		t = (100 - ctw->ctw.flags[CTW_SPEED]) * 20000 / 100;
	while (len-- > 0) {
		ctw_add_string2(ctw, str++, 1);
		XFlush(XtDisplay(ctw));
		ctw_usleep(t, ctw);
		}
}
# define	FLUSH_OUTPUT() \
		{ \
		if (vp != start_vp) { \
			ctw->ctw.x = vp - vp_start; \
			top_y = MIN(top_y, ctw->ctw.y); \
			bot_y = MAX(bot_y, ctw->ctw.y); \
			if (!big_opt) { \
				/*unhilite(ctw);*/ \
				update_region(ctw, ctw->ctw.y, start_vp - vp_start, \
					ctw->ctw.y + 1, ctw->ctw.x + bold_extra); \
				} \
			bold_extra = 0; \
			} \
		}
/**********************************************************************/
/*   Function  to  add  a  string  to  the  display.  Processes ANSI  */
/*   escape sequences.						      */
/**********************************************************************/
static void
ctw_add_string2(CtwWidget ctw, char *str, int len)
{	ctw_callback_t	reason;
	int	x;
	vbyte_t	attr;
	char	*str_end = str + len;
	vbyte_t	*vp_start, *vp_end;
	vbyte_t	*start_vp, *vp;
	int	top_line = dsp_get_top(ctw);
	int	top_y;
	int	bot_y;
	int	pc_charset = ctw->ctw.flags[CTW_PC_CHARSET];
	int	bold_extra = 0;
	int	big_opt = len > 3000;
	int	hilited;

	if (len == 0)
		return;

	if (ctw->ctw.nest_level++ == 0)
		turn_off_cursor(ctw);

	if (big_opt)
		ctw->ctw.nodraw++;

	top_y = ctw->ctw.y;
	bot_y = ctw->ctw.y;
	/***********************************************/
	/*   Reframe   the   area  if  the  user  has  */
	/*   scrolled us.			       */
	/***********************************************/
	reframe_screen(ctw);
	/***********************************************/
	/*   If  we  were in the middle of processing  */
	/*   an  escape  sequence  then carry on from  */
	/*   where we left off.			       */
	/***********************************************/
	if (ctw->ctw.escp) {
		str = handle_escape(ctw, str, str_end);
		}
	if (ctw->ctw.utfp)
		str = handle_utf8(ctw, str, str_end);

	attr = ctw->ctw.attr;

start_again:
	vp_start = dsp_get_row(ctw, dsp_get_top(ctw) + ctw->ctw.y)->l_text;
	start_vp = vp = vp_start + ctw->ctw.x;
	vp_end = vp_start + ctw->ctw.columns;
	hilited = FALSE;
	if (in_hilite(ctw, ctw->ctw.y, ctw->ctw.x))
		hilited = TRUE;

	while (str < str_end) {
		int	ch = *str & 0xff;
		if (parse_tbl[ch] == 0) {
DEFAULT:
			/***********************************************/
			/*   Look for UTF8			       */
			/***********************************************/
			if (ctw->ctw.flags[CTW_UTF8] && ch & 0x80 &&
		  	    ctw->ctw.flags[CTW_LITERAL_MODE] == 0) {
			  	FLUSH_OUTPUT();
				/***********************************************/
				/*   Handle fact we may have scrolled.	       */
				/***********************************************/
				if (top_y > 0)
					top_y--;
				bot_y++;
			  	str = handle_utf8(ctw, str, str_end);
				goto start_again;
				}

		  	if (vp >= vp_end) {
			  	line_t *lp;
				if (ctw->ctw.flags[CTW_AUTOWRAP] == FALSE) {
					FLUSH_OUTPUT();
					start_vp = vp;
					str++;
					continue;
					}
				lp = dsp_get_row(ctw, ctw->ctw.y + dsp_get_top(ctw));
				lp->l_attr |= LA_CONTINUED;
				FLUSH_OUTPUT();
				ctw->ctw.x = 0;
				top_y = 0;
				down_line(ctw, str, str_end);
				goto start_again;
				}
			if (ctw->ctw.flags[CTW_INSERT_MODE]) {
				FLUSH_OUTPUT();
				start_vp = vp;
				do_insert_space(ctw, 1);
				}

			/***********************************************/
			/*   If  character/attributes  are  the  same  */
			/*   dont redraw them.			       */
			/***********************************************/
			attr.vb_byte = ch;
			str++;
			if (vbyte_eq(vp, &attr) &&
			    !ctw->ctw.flags[CTW_INSERT_MODE]) {
				if (vp == start_vp && !hilited) {
					start_vp++;
					ctw->ctw.x++;
					}
				}

			/***********************************************/
			/*   If  we  are  not  bold but something was  */
			/*   there  that was bold then make redrawing  */
			/*   pessimistic.			       */
			/***********************************************/
			if ((attr.vb_attr & VB_BOLD) == 0 &&
			    vp->vb_attr & VB_BOLD) {
			    	bold_extra = 1;
			    	}

			*vp = attr;
			if (pc_charset && pc_chars[attr.vb_byte]) {
				vp->vb_attr |= VB_LINE;
				vp->vb_byte = pc_chars[attr.vb_byte];
				}

			if (++vp < vp_end)
				continue;
			FLUSH_OUTPUT();
			if (ctw->ctw.flags[CTW_AUTOWRAP]) {
				if (ctw->ctw.flags[CTW_NEWLINE_GLITCH])
					continue;
				ctw->ctw.x = 0;
				top_y = 0;
				down_line(ctw, str, str_end);
				goto start_again;
				}
			else
				vp--;
			continue;
			}

		switch (*str++) {
		  case BEL:
			break;
		  case '\b':
		  	FLUSH_OUTPUT();
		  	if (ctw->ctw.flags[CTW_LITERAL_MODE]) {
				ctw_add_char(ctw, "<BS>");
				goto start_again;
				}
			if (vp > vp_start) {
				start_vp = --vp;
				ctw->ctw.x = vp - vp_start;
				}
			break;
		  case '\t':
		  	if (ctw->ctw.flags[CTW_LITERAL_MODE]) {
			  	FLUSH_OUTPUT();
				ctw_add_char(ctw, "<TAB>");
				goto start_again;
				}
			x = vp - vp_start;
			if (ctw->ctw.flags[CTW_DESTRUCTIVE_TABS] == FALSE) {
				while (vp < vp_end) {
					vp++;
					if (ctw->ctw.tab_stops[++x])
						break;
				}
				break;
				}
		  	attr.vb_byte = ' ';
			do {
				*vp++ = attr;
				}
			while (vp < vp_end && ctw->ctw.tab_stops[++x] == 0);
		  	break;
		  case '\r':
		  	FLUSH_OUTPUT();
		  	if (ctw->ctw.flags[CTW_LITERAL_MODE]) {
				ctw_add_char(ctw, "<CR>");
				goto start_again;
				}
			ctw->ctw.x = 0;
			goto start_again;
		  case '\f':
		  	FLUSH_OUTPUT();
		  	if (ctw->ctw.flags[CTW_LITERAL_MODE]) {
				ctw_add_char(ctw, "<FF>");
				goto start_again;
				}
			ctw->ctw.x = 0;
			ctw->ctw.y = 0;
		  	clear_screen(ctw);
			goto start_again;
		  case '\n': {
		  	line_t *lp = dsp_get_row(ctw, ctw->ctw.y + dsp_get_top(ctw));
			lp->l_attr &= ~LA_CONTINUED;
		  	FLUSH_OUTPUT();
			top_y = 0;
			down_line(ctw, str, str_end);
			if (ctw->ctw.flags[CTW_LABEL_LINES] ||
			    ctw->ctw.flags[CTW_NUMBER_LINES])
				label_line(ctw);
			goto start_again;
			}
		  case 'n' & 0x1f:
		  	if (ctw->ctw.flags[CTW_FONT_LOCK]) {
				ctw_add_char(ctw, "<Ctrl-N>");
				break;
				}
		  	ctw->ctw.curr_cset = 1;
		  	switch (ctw->ctw.char_sets[1]) {
			  case 0:
			  case '0':
			  	attr.vb_attr |= VB_LINE;
			  	ctw->ctw.attr.vb_attr |= VB_LINE;
				break;
			  default:
			  	attr.vb_attr &= ~VB_LINE;
			  	ctw->ctw.attr.vb_attr &= ~VB_LINE;
				break;
			  }
		  	break;
		  case 'o' & 0x1f:
		  	if (ctw->ctw.flags[CTW_FONT_LOCK]) {
				ctw_add_char(ctw, "<Ctrl-O>");
				break;
				}
		  	ctw->ctw.curr_cset = 0;
		  	switch (ctw->ctw.char_sets[0]) {
			  case 0:
			  case '0':
			  	attr.vb_attr |= VB_LINE;
			  	ctw->ctw.attr.vb_attr |= VB_LINE;
				break;
			  default:
			  	attr.vb_attr &= ~VB_LINE;
			  	ctw->ctw.attr.vb_attr &= ~VB_LINE;
				break;
			  }
		  	break;
		  case XON:
		  	if (!ctw->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			ctw_add_char(ctw, "<XON>");
			break;
		  case XOFF:
		  	if (!ctw->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			ctw_add_char(ctw, "<XOFF>");
			break;
		  case ESC:
		  	str--;
		  	if (ctw->ctw.flags[CTW_LITERAL_MODE])
				goto DEFAULT;
		  	FLUSH_OUTPUT();
			ctw->ctw.escp = ctw->ctw.escbuf;
		  	str = handle_escape(ctw, str+1, str_end);
			attr = ctw->ctw.attr;
			goto start_again;
		  case '\0':
		  	break;
		  }
		}
	FLUSH_OUTPUT();
	ctw->ctw.attr = attr;
	
	if (big_opt) {
		ctw->ctw.nodraw--;
		update_region(ctw, top_y, 0, bot_y + 1, ctw->ctw.columns);
		}

	if (--ctw->ctw.nest_level == 0)
		turn_on_cursor(ctw);
	/***********************************************/
	/*   If  top  line  changed then tell user so  */
	/*   he/she   can   update  a  scrollbar  for  */
	/*   instance.				       */
	/***********************************************/
	if (top_line != dsp_get_top(ctw)) {
		reason.reason = CTWR_TOP_LINE;
		reason.client_data = ctw->ctw.client_data;
		reason.top_line = dsp_get_top(ctw);
		XtCallCallbacks((Widget) ctw, XtNtopCallback, (caddr_t) &reason);
		}
}
/**********************************************************************/
/*   Add  a  string,  doing  line  wrap stuff but dont interpret any  */
/*   characters and just display the glyph.			      */
/**********************************************************************/
void
ctw_add_raw_string(CtwWidget ctw, char *str, int len)
{	ctw_callback_t	reason;
	vbyte_t	attr;
	char	*str_end = str + len;
	vbyte_t	*vp_start, *vp_end;
	vbyte_t	*start_vp, *vp;
	int	top_y = ctw->ctw.y;
	int	bot_y = ctw->ctw.y;
	int	top_line = dsp_get_top(ctw);
	int	pc_charset = ctw->ctw.flags[CTW_PC_CHARSET];
	int	bold_extra = 0;
	int	big_opt = ctw->ctw.nodraw;

	if (ctw->ctw.nest_level++ == 0)
		turn_off_cursor(ctw);

	/***********************************************/
	/*   Reframe   the   area  if  the  user  has  */
	/*   scrolled us.			       */
	/***********************************************/
	reframe_screen(ctw);

	attr = ctw->ctw.attr;
	ctw->ctw.nodraw++;
start_again:
	vp_start = dsp_get_row(ctw, dsp_get_top(ctw) + ctw->ctw.y)->l_text;
	start_vp = vp = vp_start + ctw->ctw.x;
	vp_end = vp_start + ctw->ctw.columns;
	while (str < str_end) {
	  	if (vp >= vp_end) {
		  	line_t *lp;
			if (ctw->ctw.flags[CTW_AUTOWRAP] == FALSE) {
				FLUSH_OUTPUT();
				start_vp = vp;
				str++;
				continue;
				}
			lp = dsp_get_row(ctw, ctw->ctw.y + dsp_get_top(ctw));
			lp->l_attr |= LA_CONTINUED;
			FLUSH_OUTPUT();
			ctw->ctw.x = 0;
			top_y = 0;
			down_line(ctw, str, str_end);
			goto start_again;
			}
		if (ctw->ctw.flags[CTW_INSERT_MODE]) {
			do_insert_space(ctw, 1);
			}

		/***********************************************/
		/*   If  character/attributes  are  the  same  */
		/*   dont redraw them.			       */
		/***********************************************/
		attr.vb_byte = *str++;
		if (vbyte_eq(vp, &attr) &&
		    !ctw->ctw.flags[CTW_INSERT_MODE]) {
			if (vp == start_vp) {
				start_vp++;
				ctw->ctw.x++;
				}
			}

		/***********************************************/
		/*   If  we  are  not  bold but something was  */
		/*   there  that was bold then make redrawing  */
		/*   pessimistic.			       */
		/***********************************************/
		if ((attr.vb_attr & VB_BOLD) == 0 &&
		    vp->vb_attr & VB_BOLD) {
		    	bold_extra = 1;
		    	}
		*vp = attr;
		if (pc_charset && pc_chars[attr.vb_byte]) {
			vp->vb_attr |= VB_LINE;
			vp->vb_byte = pc_chars[attr.vb_byte];
			}

		if (++vp < vp_end)
			continue;
		FLUSH_OUTPUT();
		if (ctw->ctw.flags[CTW_AUTOWRAP]) {
			if (ctw->ctw.flags[CTW_NEWLINE_GLITCH])
				continue;
			ctw->ctw.x = 0;
			down_line(ctw, str, str_end);
			goto start_again;
			}
		else
			vp--;
		}
	FLUSH_OUTPUT();
	ctw->ctw.attr = attr;

	ctw->ctw.nodraw--;
	/***********************************************/
	/*   If  we  redraw  we  may  not  be able to  */
	/*   Ctrl-C if frantic UTF-8		       */
	/***********************************************/
	/* update_region(ctw, top_y, 0, bot_y + 1, ctw->ctw.columns); */

	if (--ctw->ctw.nest_level == 0)
		turn_on_cursor(ctw);
	/***********************************************/
	/*   If  top  line  changed then tell user so  */
	/*   he/she   can   update  a  scrollbar  for  */
	/*   instance.				       */
	/***********************************************/
	if (top_line != dsp_get_top(ctw)) {
		reason.reason = CTWR_TOP_LINE;
		reason.client_data = ctw->ctw.client_data;
		reason.top_line = dsp_get_top(ctw);
		XtCallCallbacks((Widget) ctw, XtNtopCallback, (caddr_t) &reason);
		}
}
void
ctw_dump(CtwWidget ctw)
{
	printf("spill_cnt: %d spill_name: %s\n", ctw->ctw.spill_cnt, ctw->ctw.c_spill_name);
	printf("curtop:%d y:%d rows:%d tot_rows:%d max_lines:%d top_line=%d bot_line=%d\n",
		ctw->ctw.curtop,
		ctw->ctw.y,
		ctw->ctw.rows,
		ctw->ctw.tot_rows,
		ctw->ctw.max_lines,
		ctw->ctw.top_line,
		ctw->ctw.bot_line);
}
void
ctw_dump_line(CtwWidget ctw, int n)
{	line_t	*lp;
	int	i, j;

	for (j = 0; j < 10 && n < ctw->ctw.tot_rows; j++) {
		printf("[%02d] ", n);
		lp = &ctw->ctw.c_lines[n++];
		for (i = 0; i < 60; i++)
			printf("%c", lp->l_text[i].vb_byte);
		printf("\n");
		}
}
/**********************************************************************/
/*   Return name of an emulation.				      */
/**********************************************************************/
char *
ctw_emulation_name(int type)
{
	switch (type) {
	  case CTW_EMULATE_ISC:
	  	return "ISC";
	  case CTW_EMULATE_SCO:
	  	return "SCO";
	  case CTW_EMULATE_LINUX:
	  	return "Linux";
	  case CTW_EMULATE_DEFAULT:
	  	return "Default";
	  default:
	  	return "not-known";
	  }
}
/**********************************************************************/
/*   Stop/start scrolling.					      */
/**********************************************************************/
int
ctw_freeze_display(CtwWidget ctw, int flag)
{
	if (flag)
		ctw->ctw.c_flags |= CTWF_FROZEN;
	else
		ctw->ctw.c_flags &= ~CTWF_FROZEN;
	return 0;
}
/**********************************************************************/
/*   Public function for flushing log file.			      */
/**********************************************************************/
void
ctw_flush_log(CtwWidget ctw)
{
	if (ctw->ctw.log_fp)
		fflush(ctw->ctw.log_fp);
}
/**********************************************************************/
/*   Function to let user see current flags.			      */
/**********************************************************************/
int
ctw_get_attributes(CtwWidget ctw, int **ip, char ***strp)
{	static char *attr_names[] = {
	"Autowrap",
	"Auto line feed",
	"Application keypad",
	"Application cursor",
	"Application mouse",
	"Color reset",
	"Color",		/* CTW_COLOR */
	"Cut newlines",
	"Destructive tabs",
	"Emulation",
	"Erase black",
	"Esc + literal",
	"Font lock",
	"Font size",
	"Graphics Expose",	/* CTW_GRAPHICS_EXPOSE */
	"Kbd scroll",		/* CTW_KEYBOARD_SCROLL */
	"Insert Mode",
	"Label lines",
	"Literal mode",
	"Logging",
	"Newline glitch",
	"Number lines",
	"PC char set",
	"Reset",
	"Reverse video",
	"Scrolling region",
	"Scroll region enable",
	"Size",
	"Smooth scroll",
	"Speed",		/* CTW_SPEED		*/
	"Sun keys",
	"Video attr",
	"Watch drawing",
	"ESC[X supported",
	"ESC[K uses bg color",
	"ESC[g tabs",
	"ESC[=C cursor settings",
	"UTF-8",
	(char *) NULL
	};

	ctw->ctw.flags[CTW_RESET] = FALSE;
	ctw->ctw.flags[CTW_CURRATTR] = ctw->ctw.attr.vb_attr;
	ctw->ctw.flags[CTW_SUN_FUNCTION_KEYS] = ctw->ctw.sun_function_keys;
	ctw->ctw.flags[CTW_FONT_SIZE] = (ctw->ctw.font_width << 16) | ctw->ctw.font_height;
	ctw->ctw.flags[CTW_SIZE] = (ctw->ctw.columns << 16) | ctw->ctw.rows;
	ctw->ctw.flags[CTW_EMULATION] = ctw->ctw.emulation;
	*ip = &ctw->ctw.flags[0];
	*strp = attr_names;
	return (int) CTW_MAX_ATTR;
}
/**********************************************************************/
/*   Return current font size info.				      */
/**********************************************************************/
void
ctw_get_font_info(ctw, widthp, heightp)
CtwWidget	ctw;
int	*widthp;
int	*heightp;
{
	if (widthp)
		*widthp = ctw->ctw.font_width;
	if (heightp)
		*heightp = ctw->ctw.font_height;
}
GC
ctw_get_gc(CtwWidget ctw)
{
	return ctw->ctw.gc;
}
/**********************************************************************/
/*   Function to retrieve current size of screen.		      */
/**********************************************************************/
void
ctw_get_geometry(CtwWidget ctw, int *pRows, int *pCols)
{
	*pRows = ctw->ctw.rows;
	*pCols = ctw->ctw.columns;
}
/**********************************************************************/
/*   Function to retrieve current size of screen.		      */
/**********************************************************************/
void
ctw_get_size(CtwWidget ctw, int *wp, int *hp)
{
	*hp = ctw->core.height;
	*wp = ctw->core.width;
}
/**********************************************************************/
/*   Return  pointer  to  an  absolute line in the history buffer so  */
/*   that  the  application  can  read  whats on the screen, e.g. to  */
/*   dump the to a file or whatever.				      */
/**********************************************************************/
vbyte_t *
ctw_get_line(CtwWidget ctw, int line_no)
{
	/***********************************************/
	/*   If  we  havent got this far yet then let  */
	/*   caller know line doesn't exist.	       */
	/***********************************************/
	if (line_no >= dsp_get_top(ctw) + ctw->ctw.rows)
		return (vbyte_t *) NULL;
	return dsp_get_row(ctw, line_no)->l_text;
}
/**********************************************************************/
/*   Function to get the selection.				      */
/**********************************************************************/
void
ctw_get_selection(CtwWidget ctw)
{
	if (enable_primary)
		XtGetSelectionValue((Widget) ctw, XA_PRIMARY, XA_STRING, (XtSelectionCallbackProc) requestor_callback, 
			NULL, CurrentTime);
	else if (enable_secondary)
		XtGetSelectionValue((Widget) ctw, XA_SECONDARY, XA_STRING, (XtSelectionCallbackProc) requestor_callback, 
			NULL, CurrentTime);
	else if (enable_clipboard) {
		XtGetSelectionValue((Widget) ctw, atom_clipboard, XA_STRING, (XtSelectionCallbackProc) requestor_callback, 
			NULL, CurrentTime);
		}
	if (enable_cut_buffer0) {
		}
}
/**********************************************************************/
/*   Function to retrieve current playback speed.		      */
/**********************************************************************/
int
ctw_get_speed(CtwWidget ctw)
{
	return ctw->ctw.flags[CTW_SPEED];
}
/**********************************************************************/
/*   Function  to  retrieve  current setting for the line at the top  */
/*   of the screen.						      */
/**********************************************************************/
int
ctw_get_top_line(CtwWidget ctw)
{
	return dsp_get_top(ctw);
}
/**********************************************************************/
/*   Return number of rows in screen + scrollback + spill log.	      */
/**********************************************************************/
int
ctw_get_total_rows(CtwWidget ctw)
{
	return ctw->ctw.spill_cnt + ctw->ctw.tot_rows;
}
/**********************************************************************/
/*   Routine  to  convert  an (x,y) pixel co-ordinate to a character  */
/*   position.							      */
/**********************************************************************/
void
ctw_get_xy(CtwWidget ctw, int *row, int *col, int x, int y)
{	int	r, c;
	
	r = (y - ctw->ctw.internal_height) / ctw->ctw.font_height;
	c = (x - ctw->ctw.internal_width)  / ctw->ctw.font_width;
	if (r >= ctw->ctw.rows)
		r = ctw->ctw.rows - 1;
	else if (r < 0)
		r = 0;
	if (c >= ctw->ctw.columns)
		c = ctw->ctw.columns;
	else if (c < 0)
		c = 0;
	*row = r;
	*col = c;
}
/**********************************************************************/
/*   Allow user to set the timestamp/line number on output.	      */
/**********************************************************************/
void
ctw_label_lines(CtwWidget ctw, int flag)
{
	if (flag == 0) {
		ctw->ctw.flags[CTW_LABEL_LINES] ^= 1;
		}
	ctw->ctw.c_line_no = 0;
}
/**********************************************************************/
/*   Emulate mouse from the application.			      */
/**********************************************************************/
void
ctw_mouse(CtwWidget ctw, int reason, unsigned long time, int type, int state, int x, int y)
{	XEvent ev;
	Cardinal num = 0;
	String  str[1];

	UNUSED_PARAMETER(reason);

	str[1] = NULL;

	memset(&ev, 0, sizeof ev);
	ev.xbutton.type = type;
	ev.xbutton.time = time;
	ev.xbutton.state = state;
	ev.xbutton.x = X_PIXEL(ctw, x);
	ev.xbutton.y = Y_PIXEL(ctw, y);

	mouse_emulate++;
	switch (type) {
	  case ButtonPress:
	  	CtwSelectStart(ctw, &ev, str, &num);
	  	break;
	  case ButtonRelease:
	  	CtwSelectEnd(ctw, &ev, str, &num);
	  	break;
	  case MotionNotify:
	  	CtwSelectExtend(ctw, &ev, str, &num);
	  	break;
	  }
	mouse_emulate--;
}
/**********************************************************************/
/*   Force redraw of the screen.				      */
/**********************************************************************/
void
ctw_redraw(CtwWidget ctw)
{
	exposed_region(ctw, 0, 0, ctw->core.width, ctw->core.height);
}
/**********************************************************************/
/*   Function to set current attributes.			      */
/**********************************************************************/
void
ctw_set_attributes(CtwWidget ctw, int *ip)
{	int	old_reverse = ctw->ctw.flags[CTW_REVERSE_VIDEO];

	memcpy((char *) ctw->ctw.flags, (char *) ip, sizeof ctw->ctw.flags);
	ctw->ctw.sun_function_keys = ip[CTW_SUN_FUNCTION_KEYS];

	if (ctw->ctw.flags[CTW_RESET]) {
		reset_screen(ctw);
		}
	if (ctw->ctw.flags[CTW_LOGGING])
		do_logging(ctw, TRUE);
	else
		do_logging(ctw, FALSE);
	if (ctw->ctw.flags[CTW_REVERSE_VIDEO] != old_reverse) {
		set_reverse_video(ctw, ctw->ctw.flags[CTW_REVERSE_VIDEO]);
		}
	ctw->ctw.c_line_no = 0;
}
/**********************************************************************/
/*   Function to set current attributes.			      */
/**********************************************************************/
int
ctw_set_attribute(CtwWidget ctw, int i, int value)
{	int	old_reverse = ctw->ctw.flags[CTW_REVERSE_VIDEO];
	int	ret;

	if (i < 0 || i >= (int) CTWR_MAXIMUM)
		return -1;

	ret = ctw->ctw.flags[i];
	ctw->ctw.flags[i] = value;

	if (ctw->ctw.flags[CTW_RESET]) {
		reset_screen(ctw);
		}
	if (ctw->ctw.flags[CTW_LOGGING])
		do_logging(ctw, TRUE);
	else
		do_logging(ctw, FALSE);
	if (ctw->ctw.flags[CTW_REVERSE_VIDEO] != old_reverse) {
		set_reverse_video(ctw, ctw->ctw.flags[CTW_REVERSE_VIDEO]);
		}
	ctw->ctw.c_line_no = 0;
	return ret;
}
/**********************************************************************/
/*   Set the emulation mode.					      */
/**********************************************************************/
int
ctw_set_emulation(CtwWidget ctw, int type)
{
	ctw->ctw.emulation = type;
	switch (type) {
	  case CTW_EMULATE_ISC:
	  	ctw->ctw.flags[CTW_CSI_g] = TRUE;
	  	ctw->ctw.flags[CTW_CSI_EQUAL_C] = FALSE;
		ctw->ctw.flags[CTW_PC_CHARSET] = TRUE;
		ctw->ctw.flags[CTW_CSI_X_SUPPORTED] = FALSE;
		break;

	  case CTW_EMULATE_SCO:
	  	ctw->ctw.flags[CTW_CSI_g] = FALSE;
	  	ctw->ctw.flags[CTW_CSI_EQUAL_C] = FALSE;
		ctw->ctw.flags[CTW_PC_CHARSET] = TRUE;
		ctw->ctw.flags[CTW_CSI_X_SUPPORTED] = FALSE;
		break;

	  case CTW_EMULATE_LINUX:
	  	ctw->ctw.flags[CTW_COLOR_RESET] = TRUE;
	  	ctw->ctw.flags[CTW_CSI_g] = TRUE;
	  	ctw->ctw.flags[CTW_CSI_EQUAL_C] = FALSE;
		ctw->ctw.flags[CTW_PC_CHARSET] = TRUE;
		ctw->ctw.flags[CTW_CSI_X_SUPPORTED] = TRUE;
		ctw->ctw.flags[CTW_CSI_K_USES_BG_COLOR] = TRUE;
	  	break;

	  default:
		ctw->ctw.flags[CTW_PC_CHARSET] = TRUE;
		ctw->ctw.flags[CTW_CSI_X_SUPPORTED] = TRUE;
		ctw->ctw.flags[CTW_CSI_K_USES_BG_COLOR] = TRUE;
	  	break;
	  }
	return 0;
}
int
ctw_set_font(Widget w, char *font_name)
{	XFontStruct	*font;
	CtwWidget	ctw = (CtwWidget) w;
	Display	*dpy = XtDisplay(ctw);
		
	if ((font = XLoadQueryFont(dpy, font_name)) == NULL)
		return -1;
	ctw->ctw.fontp = font;
	XSetFont(dpy, ctw->ctw.gc, font->fid);

	reset_font((CtwWidget) ctw, TRUE);

# if 0
	n = 0;
	XtSetArg(args[n], XtNwidthInc, ctw->ctw.font_width); n++;
	XtSetArg(args[n], XtNheightInc, ctw->ctw.font_height); n++;
	XtSetValues(XtParent(ctw), args, n);
# endif
	return 0;
}
/**********************************************************************/
/*   Allow us to draw to a pixmap.				      */
/**********************************************************************/
void
ctw_set_pixmap_mode(CtwWidget ctw, int flag)
{
	ctw->ctw.pixmap_mode = flag;
}
/**********************************************************************/
/*   Set default spill size.					      */
/**********************************************************************/
void
ctw_set_spill_size(unsigned size)
{
	default_spill_size = size;
}
/**********************************************************************/
/*   Allow owner to force a specific size.			      */
/**********************************************************************/
void
ctw_resize(CtwWidget ctw, int rows, int cols)
{
	if (ctw->ctw.rows == rows && ctw->ctw.columns == cols)
		return;

	rows = rows * ctw->ctw.font_height + ctw->ctw.internal_height;
	cols = cols * ctw->ctw.font_width + ctw->ctw.internal_width;
	XtResizeWidget((Widget) ctw, cols, rows, 1);
	Resize(ctw);
}
/**********************************************************************/
/*   Restore state from section of a file.			      */
/**********************************************************************/
void
ctw_restore_state(CtwWidget ctw, FILE *fp)
{	char	buf[BUFSIZ];
	char	*name;
	char	*val;
	int	x = ctw->ctw.x;
	int	y = ctw->ctw.y;

	while (fgets(buf, sizeof buf, fp)) {
		if (*buf == '\n')
			break;
/*printf("loading:%s", buf);*/
		name = strtok(buf, "=");
		val = strtok(NULL, "\n");
		if (val == NULL)
			continue;
		if (strncmp(name, "attr", 4) == 0) {
			ctw->ctw.flags[atoi(name+4)] = atoi(val);
			continue;
			}
		if (strcmp(name, "x") == 0) {
			x = atoi(val);
			continue;
			}
		if (strcmp(name, "y") == 0) {
			y = atoi(val);
			continue;
			}
		if (strncmp(name, "ln", 2) == 0) {
# if 1
		  	int flag = ctw->ctw.flags[CTW_AUTOWRAP];
		  	ctw->ctw.flags[CTW_AUTOWRAP] = FALSE;
			ctw_add_string2(ctw, val, strlen(val));
			ctw_add_string2(ctw, "\r\n", 2);
		  	ctw->ctw.flags[CTW_AUTOWRAP] = flag;
# else
			int 	c;
			line_t *lp = dsp_get_row(ctw, dsp_get_top(ctw) + atoi(name + 2));
/*printf("loading line %d\n", atoi(name+2));*/
			for (c = 0; c < ctw->ctw.columns; c++) {
				lp->l_text[c].vb_byte = val[c];
				}
# endif
			continue;
			}
		}
	ctw->ctw.x = x;
	ctw->ctw.y = y;
}
/**********************************************************************/
/*   Serialise widget state so we can restore later on.		      */
/**********************************************************************/
void
ctw_save_state(CtwWidget ctw, dstr_t *dstrp)
{	char	buf[BUFSIZ];
	int	i, c;
	char	*cp;
	int	backsize = -200;
	unsigned char fg_color = 0;
	unsigned char bg_color = 0;

	sprintf(buf, "rows:%d\n", ctw->ctw.rows);
	sprintf(buf + strlen(buf), "columns=%d\n", ctw->ctw.columns);
	sprintf(buf + strlen(buf), "x=%d\n", ctw->ctw.x);
	sprintf(buf + strlen(buf), "y=%d\n", ctw->ctw.y);
	sprintf(buf + strlen(buf), "columns=%d\n", ctw->ctw.columns);
	sprintf(buf + strlen(buf), "internal_height=%d\n", ctw->ctw.internal_height);
	sprintf(buf + strlen(buf), "internal_width=%d\n", ctw->ctw.internal_width);
	for (i = 0; i < CTW_MAX_ATTR; i++) {
		sprintf(buf + strlen(buf), "attr%d=%d\n", i, ctw->ctw.flags[i]);
		}
	dstr_add_mem(dstrp, buf, strlen(buf));

	if ((cp = getenv("CTW_BACKSIZE")) != NULL)
		backsize = -atoi(cp);

	for (i = backsize; i < ctw->ctw.rows; i++) {
		int n = dsp_get_top(ctw) + i;
		line_t *lp;
		
		if ((lp = dsp_get_row(ctw, n)) == NULL)
			continue;

		snprintf(buf, sizeof buf, "ln%d=", i);
		dstr_add_mem(dstrp, buf, strlen(buf));
		for (c = 0; c < ctw->ctw.columns; c++) {
			if (c == 0 || fg_color != lp->l_text[c].vb_fcolor || bg_color != lp->l_text[c].vb_bcolor) {
				snprintf(buf, sizeof buf, "\033[%d;%dm",
					30 + lp->l_text[c].vb_fcolor,
					40 + lp->l_text[c].vb_bcolor);
				dstr_add_mem(dstrp, buf, strlen(buf));
				fg_color = lp->l_text[c].vb_fcolor;
				bg_color = lp->l_text[c].vb_bcolor;
				}
			dstr_add_char(dstrp, lp->l_text[c].vb_byte);
			}
		dstr_add_char(dstrp, '\n');
		}
	if (ctw->ctw.c_spill_fp)
		fclose(ctw->ctw.c_spill_fp);
}
/**********************************************************************/
/*   Function  to  set  current  setting  for the line at the top of  */
/*   the screen.						      */
/**********************************************************************/
void
ctw_set_top_line(CtwWidget ctw, int top_line)
{	ctw_callback_t	reason;
	int	old_top;
	int	amt_to_scroll;

/*
printf("%p set top %d (top=%d dsp_top=%d)\n", ctw, top_line, ctw->ctw.top_line, dsp_get_top(ctw));
*/

	/***********************************************/
	/*   Make sure value in range.		       */
	/***********************************************/
	if (top_line < 0)
		top_line = 0;
	else if (top_line > dsp_get_max_lines(ctw))
		top_line = dsp_get_max_lines(ctw);
	if (top_line + ctw->ctw.rows > ctw->ctw.bot_line) {
/*printf(" .. here0 %d + %d > %d\n", top_line, ctw->ctw.rows, ctw->ctw.bot_line);*/
		top_line = ctw->ctw.bot_line - ctw->ctw.rows;
		if (top_line < 0)
			top_line = 0;
		}
	/***********************************************/
	/*   Save  the  top  line so that when we get  */
	/*   some  more  data we put it where we left  */
	/*   off and not where the user scrolled.      */
	/***********************************************/
	if (ctw->ctw.old_top_line < 0)
		ctw->ctw.old_top_line = dsp_get_top(ctw);

	if (top_line == dsp_get_top(ctw))
		return;

	old_top = dsp_get_top(ctw);
	/***********************************************/
	/*   Try  and  update window intelligently if  */
	/*   we are scrolling a little up or down.     */
	/***********************************************/
	ctw->ctw.top_line = top_line;
	if (top_line < old_top && top_line + ctw->ctw.rows - 2 > old_top) {
		amt_to_scroll = old_top - top_line;
		insert_line(ctw, 0, ctw->ctw.rows, amt_to_scroll, FALSE);
		update_region(ctw, 0, 0, 
			amt_to_scroll, ctw->ctw.columns);
		}
	else if (top_line > old_top && top_line < old_top + ctw->ctw.rows) {
		amt_to_scroll = top_line - old_top;
		delete_line(ctw, 0, amt_to_scroll);
		update_region(ctw, ctw->ctw.rows - amt_to_scroll, 0, 
			ctw->ctw.rows, ctw->ctw.columns);
		}
	else {
/*printf(" .. here3 top_line=%d old_top=%d rows=%d\n", top_line, old_top, ctw->ctw.rows);*/
		update_region(ctw, 0, 0, ctw->ctw.rows, ctw->ctw.columns);
		}
	/***********************************************/
	/*   Let user affect a scrollbar.	       */
	/***********************************************/
	reason.reason = CTWR_TOP_LINE;
	reason.client_data = ctw->ctw.client_data;
	reason.top_line = dsp_get_top(ctw);
	XtCallCallbacks((Widget) ctw, XtNtopCallback, (caddr_t) &reason);
}
/**********************************************************************/
/*   Public function to toggle cursor.				      */
/**********************************************************************/
void
ctw_show_cursor(CtwWidget ctw, int flag)
{
	if (flag)
		turn_on_cursor(ctw);
	else
		turn_off_cursor(ctw);
}
void
ctw_set_cursor(CtwWidget ctw, int flag)
{
	ctw_show_cursor(ctw, flag);
	if (flag)
		ctw->ctw.cursor_state = CURSOR_HIDDEN;
	else
		ctw->ctw.cursor_state = CURSOR_OFF;
}
/**********************************************************************/
/*   Different visuals havent different order for RGB. So we need to  */
/*   use the visual to work out how to swap the bits around.	      */
/**********************************************************************/
static long
map_rgb_to_visual_rgb(CtwWidget ctw, long rgb)
{
	int	r = (rgb >> 16) & 0xff;
	int	g = (rgb >> 8) & 0xff;
	int	b = (rgb >> 0) & 0xff;
	Visual *visual = DefaultVisual(XtDisplay(ctw), 
		DefaultScreen(XtDisplay(ctw)));

# define swap_bits(r, shift, mask) \
(shift >= 8 ? (r << (shift - 8)) : (r >> (8 - shift))) & mask

/*printf("%d,%d,%d / %d,%d,%d :: %02x %02x %02x -> ", 
rshift, gshift, bshift, 
rshift1, gshift1, bshift1, 
r, g, b);*/
	r = swap_bits(r, rshift1, visual->red_mask);
	g = swap_bits(g, gshift1, visual->green_mask);
	b = swap_bits(b, bshift1, visual->blue_mask);
/*printf("%02x %02x %02x\n", r, g, b);*/
	return r | g | b;
}
/**********************************************************************/
/*   Reframe  the  screen  if  we  have just been played with by the  */
/*   scrollbar.                                                       */
/**********************************************************************/
static void
reframe_screen(CtwWidget ctw)
{
	if (ctw->ctw.old_top_line >= 0 && (ctw->ctw.c_flags & CTWF_FROZEN) == 0) {
		ctw_callback_t reason;
		if (ctw->ctw.old_top_line != dsp_get_top(ctw)) {
			ctw->ctw.top_line = ctw->ctw.old_top_line;
			update_region(ctw, 0, 0, 
				ctw->ctw.rows + 1, ctw->ctw.columns + 1);
			}
		ctw->ctw.old_top_line = -1;
		reason.reason = CTWR_TOP_LINE;
		reason.client_data = ctw->ctw.client_data;
		reason.top_line = dsp_get_top(ctw);
		XtCallCallbacks((Widget) ctw, XtNtopCallback, (caddr_t) &reason);
		}
}
/**********************************************************************/
/*   Function  to  handle  the ESC[?r;c;w;h;numS scrolling rectangle  */
/*   escape sequence.						      */
/**********************************************************************/
static void
scroll_rectangle(CtwWidget ctw, int r, int c, int w, int h, int num)
{	int	j, k;
	vbyte_t	space;
	line_t	*lp1, *lp2;

	vbyte_clear(&space);
	space.vb_fcolor = ctw->ctw.attr.vb_fcolor;
	space.vb_bcolor = 0;
	if (num > 0) {
		if (h != num) {
			wait_for_exposure(ctw);
			XCopyArea(XtDisplay(ctw), XtWindow(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c),
				ROW_TO_PIXEL(ctw, r + num),
				w * ctw->ctw.font_width,
				(h - num) * ctw->ctw.font_height,
				X_PIXEL(ctw, c),
				ROW_TO_PIXEL(ctw, r));
			}
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			X_PIXEL(ctw, c),
			ROW_TO_PIXEL(ctw, r + h - num),
			w * ctw->ctw.font_width,
			num * ctw->ctw.font_height, 
			FALSE);
		for (j = r; j < r + h - num; j++) {
			lp1 = dsp_get_row(ctw, dsp_get_top(ctw) + j);
			lp2 = dsp_get_row(ctw, dsp_get_top(ctw) + j + num);
			memcpy(&lp1->l_text[c],
				&lp2->l_text[c],
				w * sizeof(vbyte_t));
			}
		for ( ; j < r + h; j++) {
			lp1 = dsp_get_row(ctw, dsp_get_top(ctw) + j);
			for (k = c; k < c + w; k++) {
				lp1->l_text[k] = space;
				}
			}
		}
	else {
		num = -num;
		if (h != num) {
			wait_for_exposure(ctw);
			XCopyArea(XtDisplay(ctw), XtWindow(ctw), XtWindow(ctw), ctw->ctw.gc,
				X_PIXEL(ctw, c),
				ROW_TO_PIXEL(ctw, r),
				w * ctw->ctw.font_width,
				(h - num) * ctw->ctw.font_height,
				X_PIXEL(ctw, c),
				ROW_TO_PIXEL(ctw, r + num));
			}
		XClearArea(XtDisplay(ctw), XtWindow(ctw),
			X_PIXEL(ctw, c),
			ROW_TO_PIXEL(ctw, r),
			w * ctw->ctw.font_width, 
			num * ctw->ctw.font_height, FALSE);
		for (j = r + h - 1; j >= r + num; j--) {
			lp1 = dsp_get_row(ctw, dsp_get_top(ctw) + j);
			lp2 = dsp_get_row(ctw, dsp_get_top(ctw) + j - num);
			memcpy(&lp1->l_text[c],
				&lp2->l_text[c],
				w * sizeof(vbyte_t));
			}
		for ( ; j >= r; j--) {
			lp1 = dsp_get_row(ctw, dsp_get_top(ctw) + j);
			for (k = c; k < c + w; k++) {
				lp1->l_text[k] = space;
				}
			}
		}

}
/**********************************************************************/
/*   Function to toggle cursor.					      */
/**********************************************************************/
static void
toggle_cursor(CtwWidget ctw)
{
	if (ctw->ctw.c_spill_fp)
		fflush(ctw->ctw.c_spill_fp);

	if (ctw->ctw.cursor_state == CURSOR_OFF)
		turn_on_cursor(ctw);
	else
		turn_off_cursor(ctw);
}
/**********************************************************************/
/*   Function to turn off cursor.				      */
/**********************************************************************/
static void
turn_off_cursor(CtwWidget ctw)
{	int	y = ctw->ctw.y;
	int	x;

	if (ctw->ctw.cursor_state == CURSOR_OFF || ctw->ctw.cursor_state == CURSOR_HIDDEN)
		return;
	ctw->ctw.cursor_state = CURSOR_OFF;
	x = ctw->ctw.x >= ctw->ctw.columns ? ctw->ctw.columns - 1 : ctw->ctw.x;
	update_region(ctw, y, x, y+1, x+1);
}
/**********************************************************************/
/*   Function to turn on cursor.				      */
/**********************************************************************/
static void
turn_on_cursor(CtwWidget ctw)
{
	if (ctw->ctw.cursor_state == CURSOR_HIDDEN)
		return;
	if (ctw->ctw.cursor_state == CURSOR_ON && ctw->ctw.have_focus)
		return;
	ctw->ctw.cursor_state = CURSOR_ON;
	show_cursor(ctw);
}
/**********************************************************************/
/*   Function  to  read  the  state  of  the modifier keys so we can  */
/*   figure out when they are depressed or not.                       */
/**********************************************************************/
static void
read_modifier_keys(Display *dpy)
{	int	i, j, k;
	int	sk;
	KeySym	ks;
static int	keylist[MAX_SPECIAL_KEYS] = {
	XK_Num_Lock,
	XK_Mode_switch
	};

	keymap = XGetModifierMapping(dpy);
	/***********************************************/
	/*   Zap  out the buffer in case we dont find  */
	/*   the keys.				       */
	/***********************************************/
	memset(&key_specials[0], 0, sizeof key_specials);

	/***********************************************/
	/*   k  is  the current keysym we are looking  */
	/*   at.				       */
	/***********************************************/
	k = 0;
	/***********************************************/
	/*   For each bit in the mask_state....        */
	/***********************************************/
	for (i = 0; i < 8; i++) {
		/***********************************************/
		/*   Check each key to see the modifier map.   */
		/***********************************************/
		for (j = 0; j < keymap->max_keypermod; j++, k++) {
			/***********************************************/
			/*   If  no  modifiers  set  then  skip  loop  */
			/*   quickly.				       */
			/***********************************************/
			if (keymap->modifiermap[k] == 0)
				continue;

			/***********************************************/
			/*   Convert keycode to keysym.                */
			/***********************************************/
			ks = XKeycodeToKeysym(dpy, keymap->modifiermap[k], 0);
			for (sk = 0; sk < MAX_SPECIAL_KEYS; sk++) {
				if (ks == (KeySym) keylist[sk]) {
					key_specials[sk].sk_keysym = ks;
					key_specials[sk].sk_mask = 1 << i;
					break;
					}
				}
			}
		}
}
/**********************************************************************/
/*   Remove hilite without destroying string.			      */
/**********************************************************************/
static void
unhilite(CtwWidget ctw)
{
	int	sy = MIN(ctw->ctw.sel_start_y, ctw->ctw.sel_cur_y);
	int	ey = MAX(ctw->ctw.sel_start_y, ctw->ctw.sel_cur_y);
	char	*s = ctw->ctw.sel_string;

	if (s == NULL)
		return;

	ctw->ctw.sel_string = NULL;

	update_region(ctw, 
		sy - dsp_get_top(ctw),
		0,
		ey + 1 - dsp_get_top(ctw),
		ctw->ctw.columns);
	ctw->ctw.sel_start_y = 0;
	ctw->ctw.sel_start_x = 0;
	ctw->ctw.sel_cur_y = 0;
	ctw->ctw.sel_cur_x = 0;

	ctw->ctw.sel_string = s;
}
/**********************************************************************/
/*   Routine  to  update  a  region of the screen. We get passed the  */
/*   co-ordinates as character positions (not pixels).		      */
/**********************************************************************/
static void
update_region(CtwWidget ctw, int start_line, int start_col, int end_line, int end_col)
{	int	r;

/*printf("update_region: %d,%d -> %d,%d\n", start_line, start_col, end_line, end_col);*/
	if (end_line > ctw->ctw.rows)
		end_line = ctw->ctw.rows;
	if (end_col > ctw->ctw.columns)
		end_col = ctw->ctw.columns;
	if (start_line < 0)
		r = 0;
	else
		r = start_line;
	for (; r < end_line; r++) {
		line_t *lp = dsp_get_row(ctw, r + dsp_get_top(ctw));
		if (r == ctw->ctw.rows - 1 && ctw->ctw.c_flags & CTWF_CMD_MODE)
			command_draw(ctw);
		else
			print_string(ctw, lp, r, start_col, end_col - start_col);
		}

	/***********************************************/
	/*   Redraw any graphics chain.		       */
	/***********************************************/
	if (ctw->ctw.c_chain) {
		XRectangle	rect;
		rect.x = X_PIXEL(ctw, start_col);
		rect.y = ROW_TO_PIXEL(ctw, start_line);
		rect.width = (end_col - start_col + 1) * ctw->ctw.font_width;
		rect.height = (end_line - start_line + 1) * ctw->ctw.font_height;
		XSetClipRectangles(XtDisplay(ctw), gc,
				0, 0,
				&rect, 1,
				Unsorted);

		graph_redraw(ctw, NULL);

		XSetClipMask(XtDisplay(ctw), gc, None);
		}
}
static int
ctw_usleep(int usecs, CtwWidget ctw)
{
# if HAVE_NANOSLEEP
	struct timespec req, rem;

	UNUSED_PARAMETER(ctw);

	req.tv_sec = usecs / 1000000;
	req.tv_nsec = (usecs % 1000000) * 1000;
	nanosleep(&req, &rem);

# elif defined(POLL_PANIC) || CR_LINUX
	usecs /= 5;
	while (usecs-- > 0)
		getpid();
# else
	struct pollfd fds[1];

	fds[0].fd = ConnectionNumber(XtDisplay(ctw));
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	poll(fds, 1, usecs / 1000);
# endif
	return 0;
}
# if DO_VERIFY
void
verify(CtwWidget ctw)
{	int	i, j;

	for (i = 0; i < ctw->ctw.max_lines + ctw->ctw.rows; i++) {
		for (j = i + 1; j < ctw->ctw.max_lines + ctw->ctw.rows; j++) {
			if (ctw->ctw.c_lines[i].l_text == ctw->ctw.c_lines[j].l_text)
				abort();
			}
		}
}
# endif
/**********************************************************************/
/*   Wait  for  GraphicsExpose  or  NoExpose events to come in after  */
/*   scrolling screen.						      */
/**********************************************************************/
static void
wait_for_exposure(CtwWidget ctw)
{	XEvent	ev;

	/***********************************************/
	/*   Allow one outstanding exposure event.     */
	/***********************************************/
	if (ctw->ctw.num_exposures++ <= 0)
		return;

	XFlush(XtDisplay(ctw));
	ctw->ctw.doing_ge++;
	while (ctw->ctw.num_exposures > 1) {
		XWindowEvent(XtDisplay(ctw), XtWindow(ctw), ExposureMask, &ev);
		XtDispatchEvent(&ev);
		}
	ctw->ctw.doing_ge--;
}
/**********************************************************************/
/*   Ylib							      */
/**********************************************************************/
int
YAllocColor(Display *dpy, Colormap cmap, XColor *defp)
{
	return XAllocColor(dpy, cmap, defp);
}
int
YQueryColors(Display *dpy, Colormap cmap, XColor *colors, int num)
{
	return XQueryColors(dpy, cmap, colors, num);
}
int
xs_direct_color(Display *dpy)
{	Visual *visual = DefaultVisual(dpy, DefaultScreen(dpy));
	int	ret = 0;
static int xs_direct = -1;

	if (visual->red_mask == 0xff0000 &&
	    visual->green_mask == 0xff00 &&
	    visual->blue_mask == 0xff &&
	    visual->bits_per_rgb == 8)
	    	ret = 1;

	if (xs_direct < 0) {
		char *cp = getenv("CR_XS_DIRECT");
		xs_direct = 0;
		if (cp) {
			xs_direct = atoi(cp);
/*			printf("CR_XS_DIRECT: %x %x %x %p, ret=%d\n", visual->red_mask, visual->green_mask, visual->blue_mask, visual, ret);*/
			}
		}
	if (xs_direct == 1)
		return ret;

	return 0;
}

