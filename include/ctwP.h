#ifndef _CtwP_h
#define _CtwP_h

# include "ctw.h"
/* include superclass private header file */
#include <X11/CoreP.h>

# define	UNUSED_PARAMETER(x)	x = x

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRCtwResource "CtwResource"

typedef struct {
/* private: */
	int	dummy;
} CtwClassPart;

typedef struct _CtwClassRec {
    CoreClassPart	core_class;
    CtwClassPart	ctw_class;
} CtwClassRec;

extern CtwClassRec ctwClassRec;

# define	BEL		0x07
# define	ESC		0x1b
# define	XON		('q' & 0x1f)
# define	XOFF		('s' & 0x1f)

# define	MAX_ESCBUF	256
# define	MAX_UTFBUF	12

/**********************************************************************/
/*   Structure containing stats.				      */
/**********************************************************************/
# define COLLECT_STATS	1
# if COLLECT_STATS
struct stats {
	unsigned long	lines_scrolled;
	unsigned long	lines_jumped;
	unsigned long	scroll_ups;
	} ctw_stats;
# endif

/**********************************************************************/
/*   Definitions for the multi-click code.			      */
/**********************************************************************/
# define	CLICK_CHAR	0
# define	CLICK_WORD	1
# define	CLICK_LINE	2

/**********************************************************************/
/*   Number  of  colors  supported.  Be  VERY  CAREFUL changing this  */
/*   value  because  you  need to modify the widget default resource  */
/*   values as well.						      */
/**********************************************************************/
# define	MAX_COLORS	256

enum cursor_type {
	CURSOR_OFF,
	CURSOR_ON,
	CURSOR_SORT_OF,
	CURSOR_HIDDEN
	};

/**********************************************************************/
/*   Structure  used to describe a line. Need to be able to set line  */
/*   attributes.						      */
/**********************************************************************/
# define	LA_TOP_HALF	0x001
# define	LA_BOTTOM_HALF	0x002
# define	LA_DOUBLE_WIDTH	0x004
# define	LA_FLASHING	0x008	/* Something blinking on line */
# define	LA_CONTINUED	0x010	/* Line continues from prior line. */
# define	LA_SPILT	0x020	/* Line is a spilled line.	*/
# define	LA_MATCH	0x040	/* Highlight line in error color. */
# define	LA_MATCH2       0x080	/* Highlight line in warning color */
# define	LA_MATCH3       0x100	/* Highlight matched word.      */

typedef struct line_t {
	int	l_attr;
	vbyte_t	*l_text;
	} line_t;

struct font_stack {
	struct font_stack *fnt_next;
	char		*fnt_name;
	};

# define	MAX_GARGS	4

# define	DRAW_RECT		1920
# define	DRAW_FILL_RECT		1921
# define	DRAW_LINE		1922
# define	DRAW_PIXEL		1923
# define	DRAW_CLEAR		1924
# define	DRAW_CLEAR_REDRAW	1925
# define	DRAW_NAME		1926
# define	DRAW_STRING		1927
# define	DRAW_ARC		1928
# define	DRAW_FILL_ARC		1929
# define	DRAW_SET_BACKGROUND	1930
# define	DRAW_SET_FOREGROUND	1931
# define	DRAW_SET_FONT		1932
# define	DRAW_IMAGE_STRING	1933
# define	DRAW_QUERY_WINSIZE	1934
# define	DRAW_DUMP_STATUS  	1935
# define	DRAW_DRAW_BOXED		1936
# define	DRAW_DRAW_UNBOXED	1937
# define	DRAW_GROUP		1938

typedef struct graph_t {
	struct graph_t *g_next;
	unsigned short	g_code;
	XRectangle	g_rect;
	int	g_args[MAX_GARGS];
	char		*g_str;
	} graph_t;

/**********************************************************************/
/*   c_flags.							      */
/**********************************************************************/
# define	CTWF_FROZEN	0x0001
# define	CTWF_CMD_MODE   0x0002
# define	CTWF_CTRLO_MODE	0x0004

/**********************************************************************/
/*   Number  of  columns  we  allocate  in  addition to the resource  */
/*   columns  settings.  Used  to  avoid  doubling memory allocation  */
/*   when people add/subtract scrollbars.			      */
/**********************************************************************/
# define	SPARE_COLUMNS	2
typedef struct line_cache_t {
	unsigned	lc_number;
	unsigned long	lc_offset;
	} line_cache_t;

typedef struct ctw_part {
	/* resources */
	Pixel		window_background;
	String		table;
	Pixel		cursor_color;
	Pixel		save_cursor_color;
	Pixel		gridLine_color;
	int		rows;
	int		columns;
	XtCallbackList	kbd_callback;
	Boolean		sun_function_keys;
	int		max_lines;
	XtCallbackList	resize_callback;
	XtCallbackList	top_callback;
	XtCallbackList	mouse_callback;
	XtCallbackList	appl_callback;
	char		*geometry;
	int		flashrate;
	char		*font;
	char		*font1;
	char		*font2;
	char		*font3;
	char		*font4;
	char		*font5;
	char		*font6;
	Pixel		hilite_fg;
	Pixel		hilite_bg;
	Pixel		spill_fg;
	Pixel		spill_bg;
	Pixel		link_fg;
	Pixel		link_bg;
	Pixel		search_fg;
	Pixel		search_bg;
	Pixel		cont_fg;
	Pixel		cont_bg;
	int		multiClickTime;
	char		*color_names[MAX_COLORS];
	char		*log_dir;
	char		*log_file;
	int		internal_height;
	int		internal_width;
	void		*client_data;
	int		pixmap_mode;
	int		blink_state;	/* Used for blinking so we know whether */
					/* to draw or flash.			*/
	char		*ttyname;
	int		prompting;	/* For detecting shell prompts. */
	/***********************************************/
	/*   Graph list.			       */
	/***********************************************/
	unsigned	c_graph_fg;
	unsigned	c_graph_bg;
	graph_t		*c_chain;
	graph_t		*c_chain_mouse;
/* private: */
	struct font_stack	*font_stack;
	FILE		*log_fp;
# if HAVE_FREETYPE
	FT_Face		*xft_fontp;
# endif
# if HAVE_FREETYPE_XFT
	XftFont		*xft_fontp;
# endif
	XFontStruct	*fontp;
	XFontStruct	*line_fontp;	/* Font for line-drawing graphics. */
	XFontStruct	*prev_fontp;	/* Font for line-drawing graphics. */
	int		font_ascent;
	int		font_descent;
	Pixel		x11_colors[MAX_COLORS];
	/***********************************************/
	/*   Number  of  allocated  columns  is a bit  */
	/*   greater   than   number  of  columns  to  */
	/*   allow   resize   of  window  (e.g.  when  */
	/*   adding/subtracting  scrollbar)  to occur  */
	/*   without   doubling   memory   usage  and  */
	/*   causing a lot of paging.		       */
	/***********************************************/
	int		alloc_columns;
	/***********************************************/
	/*   Need  to  keep  track  of size of window  */
	/*   allocated  so  if  window  is resized to  */
	/*   be  larger  than what we have we need to  */
	/*   allocate a new window.		       */
	/***********************************************/
	int	win_width_allocated;
	int	win_height_allocated;

	/***********************************************/
	/*   Graphics contexts			       */
	/***********************************************/
	GC	gc;		/* Normal text.		*/
	GC	line_gc;	/* Line-drawing GC.	*/
	GC	cursor_gc;	/* For drawing cursor.	*/

	/***********************************************/
	/*   Array  of 256 pixmaps created on the fly  */
	/*   from  the  current  font  to  create the  */
	/*   double  width,  top-half  or bottom half  */
	/*   fonts for VT100 compatability.	       */
	/***********************************************/
	Pixmap	*c_dw_pixmaps;
	Pixmap	*c_th_pixmaps;
	Pixmap	*c_bh_pixmaps;

	int	font_height;
	int	font_width;

	int	x, y;
	int	saved_x, saved_y;
	int	cursor_visible;
	XtIntervalId	cursor_timer;
	int		cursor_style;	/* 1==no cursor	*/
					/* 2==underline */
					/* 3==lower third */
					/* 4==lower half */
					/* 5==two thirds */
					/* 6==block */
	short		c_cursor_fg;
	short		c_cursor_bg;
	enum cursor_type	cursor_state;
	int	have_focus;	/* Used for cursor flashing.	*/

	char	escbuf[MAX_ESCBUF];	/* Used to assemble escape sequences*/
	char	*escp;
	unsigned char	utfbuf[MAX_UTFBUF];	/* Used to assemble UTF-8 sequences. */
	unsigned char	*utfp;

	int	num_exposures;	/* Number of Exposure events to wait for. */

	int	bot_line;	/* Cannot scroll past this point.	*/
	int	old_top_line;
	int	top_line;	/* Current top visible line in scrollback buffer */

	int	scroll_top;	/* Top and bottom scrolling region lines. */
	int	scroll_bot;

	int	old_rows;
	int	old_columns;
	line_t	*c_lines;	/* Array of lines corresponding to screen */
	char	*tab_stops;	/* Tab settings.			  */

	Time	timestamp;	/* Time of last event.			  */
	char	*sel_string;	/* Copy of selected text.		  */
	int	sel_length;
	int	sel_start_x, sel_start_y; /* Starting co-ordinate of hilite*/
	int	sel_cur_x, sel_cur_y;	/* Current motion position.	  */
	int	num_clicks;	/* Number of button clicks for selecting text */
	int	old_color_avail;
	int	old_fg_color;
	int	old_bg_color;
	int	blinking;	/* != 0 if blinking in effect.		*/
	int	nodraw;
	int	curtop;		/* Index used to avoid memcpy()ing line array. */
	int	spill_cnt;	/* Number of lines spilled out of our lines[] array. */
	int	tot_rows;
	vbyte_t	saved_attr;	/* Saved attributes for ESC[8m		*/
	vbyte_t	attr;		/* Current display attributes.		*/
	vbyte_t	blank;		/* Current 'space' color.		*/

	int	nest_level;	/* Nest level inside the add_string functions. */
	FILE	*c_spill_fp;	/* Spill area.			*/
	char	*c_spill_name;	/* Name of spill file.		*/
	int	c_spill_line;	/* Last line we read from the file. */
	int	c_spill_append;	/* In append mode.		*/
	unsigned long c_spill_bytes;
	unsigned int c_spill_size;
	line_cache_t *c_lcache;	/* Fast index into spill file so we can find lines quickly. */
	int	c_lcache_size;
	/***********************************************/
	/*   Stuff to handle ESC(? and ESC)?	       */
	/***********************************************/
	char	char_sets[4];
	int	curr_cset;

	int	mouse_mask;	/* ESC[nY sets this			*/

	int	doing_ge;	/* Doing a Graphics Expose.		*/

	int	flags[CTW_MAX_ATTR];/* Current state of various escape sequences */
	int	emulation;
	int	c_flags;
	int	c_discards;
	int	c_line_no;
	unsigned long c_scr_line_cnt;

	/***********************************************/
	/*   Command line buffer.		       */
	/***********************************************/
# define MAX_CMD_BUF 80
	char	c_ibuf[MAX_CMD_BUF];
	int	c_idx;
	char	*c_search;
} CtwPart;

typedef struct _CtwRec {
    CorePart		core;
    CtwPart		ctw;
} CtwRec;

#endif /* _CtwP_h */
