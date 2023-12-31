#ifndef _Ctw_h
#define _Ctw_h

# if	defined(X11r3)
#	define	XtPointer	void *
# endif
/****************************************************************
 *
 * Ctw widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

ctwWidget:
 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 applCallback	     ApplCallback	Callback	NULL
 color1		     Color1	        String		""
 color2		     Color2	        String		""
 color3		     Color3	        String		""
 color4		     Color4	        String		""
 color5		     Color5	        String		""
 color6		     Color6	        String		""
 color7		     Color7	        String		""
 color8		     Color8	        String		""
 color9		     Color9	        String		""
 color10             Color10            String		""
 color11             Color11            String		""
 color12             Color12            String		""
 color13             Color13            String		""
 color14             Color14            String		""
 color15             Color15            String		""
 color16             Color16            String		""
 columns	     Columns		Integer		80
 cursorColor	     CursorColor	Pixel		"red"
 flashrate	     Flashrate		Integer		500
 font		     Font		String		"fixed"
 font1		     Font1		String		"5x8"
 font2		     Font2		String		"6x9"
 font3		     Font3		String		"6x10"
 font4		     Font4		String		"6x13"
 font5		     Font5		String		"7x13bold"
 font6		     Font6		String		"8x13bold"
 geometry	     Geometry		String		NULL
 hiliteBackground    HiliteBackground	String		"CadetBlue"
 hiliteForeground    HiliteForeground	String		"yellow"
 internalHeight	     InternalHeight	Integer		0
 internalWidth 	     InternalWith  	Integer		0
 logFile             LogFile            String          "/tmp/CtwLog.XXXXXX"
 logging             Logging            Integer         False
 kbdCallback	     KbdCallback	Callback	NULL
 mouseCallback	     MouseCallback	Callback	NULL
 multiClickTime	     MultiClickTime	Integer		250
 resizeCallback	     ResizeCallback	Callback	NULL
 rows		     Rows		Integer		24
 saveLines	     SaveLines		Integer		512
 sunFunctionKeys     SunFunctionKeys	Boolean		false
 topCallback	     TopCallback	Callback	NULL
*/

/**********************************************************************/
/*   define  any  special  resource  names  here  that  are  not  in  */
/*   <X11/StringDefs.h>						      */
/**********************************************************************/

#define	XtNapplCallback "applCallback"
#define	XtNcursorColor	"cursorColor"
#define	XtNrows "rows"
#define	XtNcolumns "columns"
#define	XtNkbdCallback "kbdCallback"
#define	XtNsunFunctionKeys "sunFunctionKeys"
#define	XtNsaveLines "saveLines"
#define	XtNresizeCallback "resizeCallback"
#define	XtNtopCallback "topCallback"
#define	XtNmouseCallback "mouseCallback"
#if !defined(XtNgeometry)
#	define	XtNgeometry "geometry"
# endif
#define	XtNflashrate "flashrate"
#define XtNlogDir "logDir"
#define XtNlogFile "logFile"
#define XtNlogging "logging"
#define XtNfont1 "font1"
#define XtNfont2 "font2"
#define XtNfont3 "font3"
#define XtNfont4 "font4"
#define XtNfont5 "font5"
#define XtNfont6 "font6"
#define	XtNhiliteBackground "hiliteBackground"
#define	XtNhiliteForeground "hiliteForeground"
#define	XtNlinkBackground "linkBackground"
#define	XtNlinkForeground "linkForeground"
#define	XtNspillBackground "spillBackground"
#define	XtNspillForeground "spillForeground"
#define	XtNsearchBackground "searchBackground"
#define	XtNsearchForeground "searchForeground"
#define	XtNcontBackground "contBackground"
#define	XtNcontForeground "contForeground"
#define	XtNspillSize "spillSize"
#define	XtNmultiClickTime "multiClickTime"
#define	XtNcolor1 "color1"
#define	XtNcolor2 "color2"
#define	XtNcolor3 "color3"
#define	XtNcolor4 "color4"
#define	XtNcolor5 "color5"
#define	XtNcolor6 "color6"
#define	XtNcolor7 "color7"
#define	XtNcolor8 "color8"
#define	XtNcolor9 "color9"
#define	XtNcolor10 "color10"
#define	XtNcolor11 "color11"
#define	XtNcolor12 "color12"
#define	XtNcolor13 "color13"
#define	XtNcolor14 "color14"
#define	XtNcolor15 "color15"
#define	XtNcolor16 "color16"
#define	XtNcolor17 "color17"
#define	XtNcolor18 "color18"
#define	XtNcolor19 "color19"
#define	XtNcolor20 "color20"
#define	XtNcolor21 "color21"
#define	XtNcolor22 "color22"
#define	XtNcolor23 "color23"
#define	XtNcolor24 "color24"
#define	XtNcolor25 "color25"
#define	XtNcolor26 "color26"
#define	XtNcolor27 "color27"
#define	XtNcolor28 "color28"
#define	XtNcolor29 "color29"
#define	XtNcolor30 "color30"
#define	XtNcolor31 "color31"
#define	XtNcolor32 "color32"
#define	XtNclientData "clientData"
# if !defined(XtNinternalHeight)
#	define	XtNinternalHeight "internalHeight"
# endif

# if !defined(XtNinternalWidth)
#	define	XtNinternalWidth "internalWidth"
# endif
#define	XtNttyName	"ttyName"

#define	XtCApplCallback "ApplCallback"
#define	XtCCursorColor "CursorColor"
#define	XtCRows "Rows"
#define	XtCColumns "Columns"
#define	XtCKbdCallback "KbdCallback"
#define	XtCSunFunctionKeys "SunFunctionKeys"
#define XtCSaveLines "SaveLines"
#define	XtCResizeCallback "ResizeCallback"
#define	XtCTopCallback "TopCallback"
#define	XtCMouseCallback "MouseCallback"
#if !defined(XtCGeometry)
#	define	XtCGeometry "Geometry"
# endif
#define	XtCFlashrate "Flashrate"
#define XtCLogDir "LogDir"
#define XtCLogDir  "LogDir"
#define XtCLogFile "LogFile"
#define XtCLogging "Logging"
#define XtCFont1 "Font1"
#define XtCFont2 "Font2"
#define XtCFont3 "Font3"
#define XtCFont4 "Font4"
#define XtCFont5 "Font5"
#define XtCFont6 "Font6"
#define	XtCHiliteBackground "HiliteBackground"
#define	XtCHiliteForeground "HiliteForeground"
#define	XtCLinkBackground "LinkBackground"
#define	XtCLinkForeground "LinkForeground"
#define	XtCSpillBackground "SpillBackground"
#define	XtCSpillForeground "SpillForeground"
#define	XtCSearchBackground "SearchBackground"
#define	XtCSearchForeground "SearchForeground"
#define	XtCContBackground "ContBackground"
#define	XtCContForeground "ContForeground"
#define	XtCSpillSize "SpillSize"
#define	XtCMultiClickTime "MultiClickTime"
#define	XtCColor1 "Color1"
#define	XtCColor2 "Color2"
#define	XtCColor3 "Color3"
#define	XtCColor4 "Color4"
#define	XtCColor5 "Color5"
#define	XtCColor6 "Color6"
#define	XtCColor7 "Color7"
#define	XtCColor8 "Color8"
#define	XtCColor9 "Color9"
#define	XtCColor10 "Color10"
#define	XtCColor11 "Color11"
#define	XtCColor12 "Color12"
#define	XtCColor13 "Color13"
#define	XtCColor14 "Color14"
#define	XtCColor15 "Color15"
#define	XtCColor16 "Color16"
#define	XtCColor17 "Color17"
#define	XtCColor18 "Color18"
#define	XtCColor19 "Color19"
#define	XtCColor20 "Color20"
#define	XtCColor21 "Color21"
#define	XtCColor22 "Color22"
#define	XtCColor23 "Color23"
#define	XtCColor24 "Color24"
#define	XtCColor25 "Color25"
#define	XtCColor26 "Color26"
#define	XtCColor27 "Color27"
#define	XtCColor28 "Color28"
#define	XtCColor29 "Color29"
#define	XtCColor30 "Color30"
#define	XtCColor31 "Color31"
#define	XtCColor32 "Color32"
#define	XtCClientData "ClientData"
# if !defined(XtCInternalHeight)
#	define	XtCInternalHeight "InternalHeight"
# endif
# if !defined(XtCInternalWidth)
#	define	XtCInternalWidth "InternalWidth"
# endif
#define	XtCTtyName	"TtyName"
#define XtNgridLineColor "gridLineColor"
#define XtCGridLineColor "GridLineColor"

/**********************************************************************/
/*   declare specific CtwWidget class and instance datatypes	      */
/**********************************************************************/

typedef struct _CtwClassRec*	CtwWidgetClass;
typedef struct _CtwRec*		CtwWidget;

# if COCOA
#	define WidgetClass id
typedef struct XEvent {
	int	x;
	} XEvent;
#define Cursor XCursor
#include <xwindows.h>
# endif

/**********************************************************************/
/*   declare the class constant					      */
/**********************************************************************/

extern WidgetClass ctwWidgetClass;

struct map {
	char *name;
	char *value;
	};

/**********************************************************************/
/*   Attributes of each character.				      */
/**********************************************************************/
# define	VB_UNDERLINE	0x01
# define	VB_REVERSE	0x02
# define	VB_BOLD		0x04
# define	VB_FLASHING	0x08
# define	VB_LINE		0x10
# define	VB_SELECT	0x20
# define	VB_STRIKETHRU	0x40
# define	VB_BOXED	0x80

# define	VB2_FG_24	0x01
# define	VB2_BG_24	0x02
/**********************************************************************/
/*   Define  a  'byte'. A byte represents a character on the screen.  */
/*   Its  4-bytes  wide containing an 8-bit character, plus a 'font'  */
/*   indicator.   This  is  used  to  indicate  whether  we  have  a  */
/*   line-drawing  character  at that position. Another byte is used  */
/*   to   encode  color  information.  One  more  byte  for  display  */
/*   attributes  (such  as  bold,  reverse  video). And one which is  */
/*   spare for now.						      */
/**********************************************************************/
typedef struct vbyte_t {
	unsigned short	vb_byte;
	unsigned char	vb_attr;
	unsigned char	vb_fcolor;
	unsigned char	vb_bcolor;
	unsigned char	vb_attr2;
	unsigned char	vb_fg[3];
	unsigned char	vb_bg[3];
	} vbyte_t;

# define vbyte_eq(vp1, vp2) ((vp1)->vb_byte == (vp2)->vb_byte && \
			     (vp1)->vb_attr ==  (vp2)->vb_attr && \
			     (vp1)->vb_attr2 ==  (vp2)->vb_attr2 && \
			     (vp1)->vb_fcolor == (vp2)->vb_fcolor && \
			     (vp1)->vb_bcolor == (vp2)->vb_bcolor)
# define vbyte_clear(vp1) ((vp1)->vb_attr = 0, \
		(vp1)->vb_attr2 = 0, \
		(vp1)->vb_byte = ' ', \
		(vp1)->vb_fcolor = 0, \
		(vp1)->vb_bcolor = 0 \
		)

/**********************************************************************/
/*   Declare a callback structure.				      */
/**********************************************************************/
enum ctw_reasons {
	CTWR_INPUT,		/* Key or string input.	*/
	CTWR_RESIZE,		/* Window was resized.  */
	CTWR_TOP_LINE,		/* Top line changed. Allow application to change */
				/* thumb on a scrollbar.			 */
	CTWR_BUTTON_DOWN,	/* Button down event.				 */
	CTWR_BUTTON_UP,		/* Button up event.				 */
	CTWR_BUTTON_MOTION,	/* Button motion event.			 */
	CTWR_FRONT_WINDOW,	/* Raise window.				*/
	CTWR_BACK_WINDOW,	/* Lower window.				*/
	CTWR_FONT_CHANGE,
	CTWR_FUNCTION_KEY,	/* Ctrl-Shift-Fn pressed.			*/
	CTWR_SELECTION,		/* User is selecting text -- tell app to not give us */
				/* any more data.				*/
	CTWR_PASTE,		/* Received selection from owner.		*/
	CTWR_PASTE_PROTECTED,	/* Paste with protected escape sequences. */
	CTWR_OPEN_WINDOW,	/* Uniconise window.				*/
	CTWR_CLOSE_WINDOW,	/* Iconise window.				*/
	CTWR_MOVE_WINDOW,	/* Change window position.			*/
	CTWR_SIZE_WINDOW_PIXELS, /* Change window size.			*/
	CTWR_SET_AUTOSWITCH,	/* Set autoswitch.			*/
	CTWR_SET_STATUS_LABEL,	/* Set label on bottom row.		*/
	CTWR_SET_TITLE,		/* Set title.					*/
	CTWR_SIZE_WINDOW_CHARS,	/* Change window size.			*/
	CTWR_WHEEL_UP,
	CTWR_WHEEL_DOWN,
	CTWR_SAVE_STATE,	/* For debugging, save the execution state. */
	CTWR_RESTORE_STATE,	/* For debugging, restore the saved execution state. */

	CTWR_MAXIMUM
	};

typedef struct ctw_callback_t {
	void			*client_data;
	enum ctw_reasons	reason;
	char			*ptr;
	int			len;
	int			key;
	int			flags;
	int			id;
	int			top_line;
	int			x, y;
	int			width, height;
	XEvent			*event;
	char		*font;
	} ctw_callback_t;
enum ctw_emulation {
	/***********************************************/
	/*   Profiles.				       */
	/***********************************************/
	CTW_EMULATE_DEFAULT,
	CTW_EMULATE_ISC,
	CTW_EMULATE_SCO,
	CTW_EMULATE_LINUX,
	};

/**********************************************************************/
/*   Index into array of flag values.				      */
/**********************************************************************/
enum ctw_flags { /* edit attr_names in ctw.c */
	CTW_ALLOW_CTRLO = 0,
	CTW_AUTOWRAP,
	CTW_AUTOLINEFEED,
	CTW_APPL_KEYPAD,
	CTW_APPL_MOUSE,
	CTW_BRACKETED_PASTE_MODE,
	CTW_CURSOR_KEYPAD,
	CTW_COLOR_RESET,
	CTW_COLOR,
	CTW_CUT_NEWLINES,
	CTW_DESTRUCTIVE_TABS,
	CTW_EMULATION,
	CTW_ERASE_BLACK,
	CTW_ESC_LITERAL,
	CTW_FONT_LOCK,
	CTW_FONT_SIZE,
	CTW_GRAPHICS_EXPOSE,
	CTW_KEYBOARD_SCROLL,
	CTW_INSERT_MODE,
	CTW_LABEL_LINES,
	CTW_LITERAL_MODE,
	CTW_LOGGING,
	CTW_NEWLINE_GLITCH,
	CTW_NUMBER_LINES,
	CTW_PC_CHARSET,
	CTW_RESET,
	CTW_REVERSE_VIDEO,
	CTW_SCROLLING_REGION_ENABLE,
	CTW_SCROLLING_REGION,
	CTW_SIZE,
	CTW_SMOOTH_SCROLL,
	CTW_SPEED,
	CTW_SUN_FUNCTION_KEYS,
	CTW_WATCH_DRAWING,
	CTW_CURRATTR,
	CTW_CSI_X_SUPPORTED,
	CTW_CSI_K_USES_BG_COLOR,
	CTW_CSI_g,		/* VT220 special */
	CTW_CSI_EQUAL_C,	/* ISC */
	CTW_UTF8,
	CTW_ESC_LOG_BAD,

	CTW_MAX_ATTR
	};

/**********************************************************************/
/*   Following  macro  used  to  allow  compilation  with ANSI C and  */
/*   non-ANSI C compilers automatically.			      */
/**********************************************************************/
# if !defined(PROTO)
# 	if	defined(__STDC__)
#		define	PROTO(x)	x
#	else
#		define	PROTO(x)	()
#	endif
# endif

/**********************************************************************/
/*   Add a string to be displayed including ANSI escape sequences.    */
/**********************************************************************/
void	ctw_add_string(CtwWidget, char *, int);

/**********************************************************************/
/*   Asciitext record/pause/stop				      */
/**********************************************************************/
# define RECORD_NORMAL	1
# define RECORD_SILENT	2
# define RECORD_STOP	3
# define RECORD_PAUSE	4
int	ctw_asciitext_record(CtwWidget ctw, int cmd, char *fn);

/**********************************************************************/
/*   Add string to display but dont interpret any control sequences.  */
/*   Just display raw glyphs.					      */
/**********************************************************************/
void	ctw_add_raw_string(CtwWidget, char *, int);

/**********************************************************************/
/*   Cycle through font changes.				      */
/**********************************************************************/
void ctw_font_change_size(CtwWidget ctwp, int sz);

/**********************************************************************/
/*   Stop scrolling whilst playing with scrollbars.		      */
/**********************************************************************/
int	ctw_freeze_display(CtwWidget, int);

/**********************************************************************/
/*   Return  line  number  at  top  of screen. Used for implementing  */
/*   scrollable area.						      */
/**********************************************************************/
int	ctw_get_top_line(CtwWidget);

/**********************************************************************/
/*   Get total number of scrollable rows.			      */
/**********************************************************************/
int	ctw_get_total_rows(CtwWidget ctw);

/**********************************************************************/
/*   Set  the  line at the top of the display. Used for implementing  */
/*   scrollable area.						      */
/**********************************************************************/
void	ctw_set_top_line(CtwWidget, int);

/**********************************************************************/
/*   Function to retrieve current state of the various attributes.     */
/**********************************************************************/
int	ctw_get_attributes(CtwWidget, int **, char ***);

/**********************************************************************/
/*   Guess if we are at an input prompt.			      */
/**********************************************************************/
int	ctw_is_prompting(CtwWidget ctw);

/**********************************************************************/
/*   Restore state from a file.					      */
/**********************************************************************/
void	ctw_restore_state(CtwWidget ctw, FILE *fp);
void	ctw_save_state(CtwWidget ctw, dstr_t *dstrp);

/**********************************************************************/
/*   Function  to  set  attributes  current  state  of  the  various  */
/*   attributes.						      */
/**********************************************************************/
void	ctw_set_attributes(CtwWidget, int *);

/**********************************************************************/
/*   Allow us to draw to a pixmap.				      */
/**********************************************************************/
void	ctw_set_pixmap_mode(CtwWidget ctw, int flag);

/**********************************************************************/
/*   Tell widget to grab the contents of the selection.		      */
/**********************************************************************/
void	ctw_get_selection(CtwWidget, int);

/**********************************************************************/
/*   Force mouse input.						      */
/**********************************************************************/
void ctw_mouse(CtwWidget ctw, int reason, unsigned long time, int type, int state, int x, int y);

/**********************************************************************/
/*   Flush the log file.					      */
/**********************************************************************/
void	ctw_flush_log(CtwWidget);

/**********************************************************************/
/*   Label output.						      */
/**********************************************************************/
void	ctw_label_lines(CtwWidget, int);

/**********************************************************************/
/*   Retrieve current font info.				      */
/**********************************************************************/
void	ctw_get_font_info(CtwWidget, int *, int *);

/**********************************************************************/
/*   Function to retrieve size of screen.			      */
/**********************************************************************/
void	ctw_get_geometry(CtwWidget, int *, int *);

/**********************************************************************/
/*   Enable/disable logging - eg during startup.		      */
/**********************************************************************/
void	ctw_logging(CtwWidget, int);

/**********************************************************************/
/*   Dump  a  copy  of  everything  in  the  history  buffer  to the  */
/*   specified file.						      */
/**********************************************************************/
vbyte_t	*ctw_get_line(CtwWidget, int);

/**********************************************************************/
/*   Function to set a single attribute.			      */
/**********************************************************************/
int	ctw_set_attribute(CtwWidget, int, int);

/**********************************************************************/
/*   Set the terminal attribute parsing.			      */
/**********************************************************************/
int	ctw_set_emulation(CtwWidget, int);
char	*ctw_emulation_name(int);

void	ctw_command_mode(CtwWidget ctw);

/**********************************************************************/
/*   Function to retrieve current percentage playback speed.	      */
/**********************************************************************/
int	ctw_get_speed(CtwWidget);

/**********************************************************************/
/*   Force screen redraw.					      */
/**********************************************************************/
void	ctw_redraw(CtwWidget);
void	ctw_get_size(CtwWidget, int *, int *);

/**********************************************************************/
/*   Retrieve the GC.						      */
/**********************************************************************/
GC	ctw_get_gc(CtwWidget ctw);
void	ctw_get_xy(CtwWidget, int *, int *, int, int);

/**********************************************************************/
/*   Set default rollover filesize.				      */
/**********************************************************************/
void	ctw_set_spill_size(unsigned size);

void	ctw_send_input(CtwWidget ctw, XEvent *event, String *x, Cardinal *y);

void show_color_map(CtwWidget ctw);
#endif /* _Ctw_h */
