/**********************************************************************/
/*                                                                    */
/*  File:          scrbar.c                                           */
/*  Author:        P. D. Fox                                          */
/*  Created:       15 Feb 1994                     		      */
/*                                                                    */
/*  Copyright (c) 1994-2008, Paul Fox                                 */
/*                All Rights Reserved.                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*  Description:  Scrollbar widget                                    */
/**********************************************************************/

/**********************************************************************/
/*   TODO:							      */
/*   1. We dont have the horizontal XPM bitmaps.		      */
/*   3. We dont handle mouse clicking for horiz scrollbar	      */
/*   4. We dont implement the Motif callbacks			      */
/**********************************************************************/
# include	"machine.h"
# include	"fcterm.h"
# include 	<X11/IntrinsicP.h>
# include 	<X11/StringDefs.h>
# include 	<X11/Shell.h>
# include	<X11/Xatom.h>
# include	<X11/cursorfont.h>
# include 	"scrbarP.h"
# include	<xpm.h>
# include	<stdio.h>

# include	"xpm/uarrow1.xpm"
# include	"xpm/darrow1.xpm"
# include	"xpm/uarrow2.xpm"
# include	"xpm/darrow2.xpm"

# define	ARROW_HEIGHT	16
# define	ARROW_WIDTH	16

# define	DEBUG	0

static void SbarInput(/* Widget, XEvent*, String*, Cardinal* */);
static void SbarButtonUp(/* Widget, XEvent*, String*, Cardinal* */);
static void SbarButtonDown(/* Widget, XEvent*, String*, Cardinal* */);
static void SbarButtonMotion(/* Widget, XEvent*, String*, Cardinal* */);

static XtActionsRec actions[] =
{
	{"input",		SbarInput},
	{"button_motion",	SbarButtonMotion},
	{"button_down",		SbarButtonDown},
	{"button_up",		SbarButtonUp},
};
static char translations[] =
"\
<KeyPress>:		input()\n\
<Btn1Up>:		button_up()\n\
<Btn1Down>:		button_down()\n\
<Btn2Up>:		button_up()\n\
<Btn2Down>:		button_down()\n\
<Btn3Up>:		button_up()\n\
<Btn3Down>:		button_down()\n\
<BtnMotion>:		button_motion()\n";

static float floatZero = 0.0;

static XtResource resources[] = {
#define offset(field) XtOffset(ScrbarWidget, scrbar.field)
    { XtNpageRepeatDelay, XtCPageRepeatDelay, XtRInt, sizeof(int),
	  offset(page_repeat_delay), XtRImmediate, (caddr_t) 250},
    { XtNrepeatDelay, XtCRepeatDelay, XtRInt, sizeof(int),
	  offset(repeat_delay), XtRImmediate, (caddr_t) 60},
    {XtNshown, XtCShown, XtRFloat, sizeof(float),
       offset(shown), XtRFloat, (XtPointer)&floatZero},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
    	offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    { XtNresizeCallback, XtCResizeCallback, XtRCallback, sizeof(caddr_t),
	  offset(resize_callback), XtRCallback, (caddr_t) NULL},
    { XtNinputCallback, XtCInputCallback, XtRCallback, sizeof(caddr_t),
	  offset(input_callback), XtRCallback, (caddr_t) NULL},
    { XtNjumpProc, XtCCallback, XtRCallback, sizeof(caddr_t),
	  offset(jump_callback), XtRCallback, (caddr_t) NULL},
    { XtNscrollProc, XtCCallback, XtRCallback, sizeof(caddr_t),
	  offset(scroll_callback), XtRCallback, (caddr_t) NULL},
    { XtNsbarCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	  offset(sbar_callback), XtRCallback, (caddr_t) NULL},
#undef offset
};

static Pixmap	uarrow1;
static Pixmap	uarrow2;

static Pixmap	darrow1;
static Pixmap	darrow2;

static Pixel	bg_color;
# define	UP_ARROW	0
# define	DOWN_ARROW	1

int	xs_XParseColor();
static void	sbar_timer_proc(ScrbarWidget, XtIntervalId *);
static void	paint_arrow PROTO((ScrbarWidget, int));
static void	paint_thumb PROTO((ScrbarWidget));
static void	display_xpm_error PROTO((int, char *));
static void	Class_initialize();
static void	once_initialize();
static void	Destroy();
static void	initialize();
static void	realize();
static void	Resize();
static void	ScrbarExpose();
static void ExtractPosition PROTO((XEvent *, Position *, Position *));

ScrbarClassRec scrbarClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Scrbar",
    /* widget_size		*/	sizeof(ScrbarRec),
    /* class_initialize		*/	Class_initialize,
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
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	ScrbarExpose,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	XtInheritAcceptFocus,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* scrbar fields */
    /* empty			*/	0
  }
};

WidgetClass scrbarWidgetClass = (WidgetClass)&scrbarClassRec;
/**********************************************************************/
/*   Put in class record to override the default Motif one.	      */
/**********************************************************************/
WidgetClass xmScrollBarWidgetClass = (WidgetClass)&scrbarClassRec;

char	*getenv();
int	atoi();

# if !CRISP_WIDGET
Pixel		obj_pixels[MAX_OBJ_PIXELS];
char	*icon_colors[] = {
	"gray76", 	/* BACKGROUND_PIXEL & GRAY_PIXEL */
	"black", 	/* BLACK_PIXEL */
	"DimGray", 	/* DIM_GRAY_PIXEL & TOP_SHADOW_PIXEL */
	"white",	/* WHITE_PIXEL & BOTTOM_SHADOW_PIXEL */
	"#003f7f", 	/* SELECTED_PIXEL */
	"#007f00",	/* GREEN_PIXEL for RTF underlined text */
	"#ff007f",      /* TABLE_CELL_PIXEL */
	"#ff0000",	/* WAVY_LINE_PIXEL */
	"#ffff80",	/* TOOLTIP_BACKGROUND */
	"#ffff00",	/* STATUS_CURRENT_PIXEL */
	"#0000ff",	/* STATUS_FG_PIXEL */
	"#8090ff",	/* STATUS_BG_PIXEL */
	"#70d080",	/* STATUS_ACTIVE_PIXEL */
	};
GC create_gc(Display *dpy);

int
xs_XParseColor(dpy, cmap, name, def)
Display		*dpy;
Colormap	cmap;
char		*name;
XColor		*def;
{
	if (XParseColor(dpy, cmap, name, def) == FALSE)
		return -1;
	if (XAllocColor(dpy, cmap, def) == FALSE)
		return -1;
	return TRUE;
}
int
obj_color_init(Display *dpy)
{	int	i;
	XColor	cdef;

	for (i = 0; i < MAX_OBJ_PIXELS; i++) {
		xs_XParseColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)),
			icon_colors[i], &cdef);
		obj_pixels[i] = cdef.pixel;
		}

	return 0;
}
# endif
static void
Class_initialize()
{
	XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
		    NULL, (Cardinal)0 );
}

static void
once_initialize(Display *dpy)
{	char	*cp;
	XpmAttributes	xpm_attr;
	int	xpm_closeness = 100000;
	int	err;
	Pixmap	pixmap_mask;
	static int first_time = TRUE;
	static struct map {
		Pixmap	*pixmapp;
		char	*name;
		char	**xpm;
		} tbl[] = {
		{&uarrow1, "uarrow1", uarrow1_xpm},
		{&uarrow2, "uarrow2", uarrow2_xpm},
		{&darrow1, "darrow1", darrow1_xpm},
		{&darrow2, "darrow2", darrow2_xpm}
		};
	struct map *mp;
	XColor	cdef;

	if (!first_time)
		return;
	first_time = FALSE;

	if ((cp = getenv("CR_XPM_CLOSENESS")) != (char *) NULL ||
	    (cp = getenv("XPM_CLOSENESS")) != (char *) NULL)
		xpm_closeness = atoi(cp);

# if !CRISP_WIDGET
	obj_color_init(dpy);
# endif
	/***********************************************/
	/*   Allocate gray background color.	       */
	/***********************************************/
	if (xs_XParseColor(dpy, 
		DefaultColormap(dpy, DefaultScreen(dpy)), "#e2e2e2", &cdef) > 0)
		bg_color = cdef.pixel;
	else
		bg_color = BlackPixel(dpy, DefaultScreen(dpy)); 

	for (mp = tbl; mp < &tbl[sizeof tbl / sizeof tbl[0]]; mp++) {
		memset((char *) &xpm_attr, 0, sizeof xpm_attr);
		xpm_attr.valuemask = XpmCloseness;
		xpm_attr.closeness = xpm_closeness;
		err = XpmCreatePixmapFromData(dpy, 
			DefaultRootWindow(dpy),
			mp->xpm,
			mp->pixmapp, &pixmap_mask, &xpm_attr);
		if (err)
			display_xpm_error(err, mp->name);
		}

}

/* ARGSUSED */
static void
initialize(ScrbarWidget treq, ScrbarWidget tnew)
{
	once_initialize(XtDisplay(treq));

	tnew->scrbar.uarrow_down = FALSE;
	tnew->scrbar.darrow_down = FALSE;
	tnew->scrbar.elev_down = FALSE;
	tnew->scrbar.timer = 0;
	tnew->scrbar.sb_pos = 0;

	tnew->core.background_pixel = bg_color;
}
# define	superclass	(&widgetClassRec)
static void
realize(w, valueMask, attributes)
ScrbarWidget	w;
XtValueMask	*valueMask;
XSetWindowAttributes	*attributes;
{
	Display	*dpy;
	static Cursor cursor;

	/***********************************************/
	/*   Make sure widget isnt too small.	       */
	/***********************************************/
	if (w->core.width == 0)
		w->core.width = ARROW_WIDTH;

	if (w->core.height == 0)
		w->core.height = ARROW_HEIGHT;


	(*superclass->core_class.realize)((Widget) w, valueMask, attributes);

	w->scrbar.gc = create_gc(XtDisplay(w));
	dpy = XtDisplay(w);

	/***********************************************/
	/*   Use  a  right  pointing  mouse  for  the  */
	/*   scrollbar   to   avoid   problems   with  */
	/*   scrolling causing frantic mouse redraws.  */
	/***********************************************/
	if (cursor == (Cursor) 0)
		cursor = XCreateFontCursor(dpy, XC_right_ptr);
	XDefineCursor(dpy,  XtWindow(w), cursor);
}
/**********************************************************************/
/*   Free up private resources when widget is destroyed.	      */
/**********************************************************************/
static void
Destroy(ScrbarWidget w)
{
	XFreeGC(XtDisplay((Widget) w), w->scrbar.gc);
	if (w->scrbar.timer)
		XtRemoveTimeOut(w->scrbar.timer);
}
/* ARGSUSED */
static void
Resize(Widget w)
{
	if (!XtIsRealized(w))
		return;

	XtCallCallbacks(w, XtNresizeCallback, (caddr_t) NULL);
}
/**********************************************************************/
/*   Action routine to handle rescrbaring of window.		      */
/**********************************************************************/
static void 
ScrbarExpose(Widget w, XExposeEvent *event)
{	ScrbarWidget	sw = (ScrbarWidget) w;
	XSegment	segs[10];
	int	i;
	Display	*dpy = XtDisplay(sw);

	if (event->y < ARROW_HEIGHT)
		paint_arrow(sw, UP_ARROW);

	if (event->y + event->height >= sw->core.height - ARROW_HEIGHT)
		paint_arrow(sw, DOWN_ARROW);

	/***********************************************/
	/*   Draw the elevator.                        */
	/***********************************************/
	paint_thumb(sw);

	i = 0;
	if (event->y == 0) {
		segs[i].x1 = 0; segs[i].x2 = sw->core.width - 1;
		segs[i].y1 = 0; segs[i].y2 = 0;
		i++;
		}
	if (event->x == 0) {
		segs[i].x1 = 0; segs[i].x2 = 0;
		segs[i].y1 = event->y; segs[i].y2 = event->y + event->height;
		i++;
		}
	if (event->x + event->width >= sw->core.width) {
		segs[i].x1 = sw->core.width - 1; segs[i].x2 = sw->core.width - 1;
		segs[i].y1 = event->y; segs[i].y2 = event->y + event->height;
		i++;
		}
	if (i) {
		XDrawSegments(dpy, XtWindow(sw), sw->scrbar.gc,
			segs, i);
		}
}
static void 
SbarButtonDown(ScrbarWidget w, XEvent *event, String *x, Cardinal *y)
{	Position	px, py;
	struct scrbar_callback buf;
	int	do_up = 0;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (w->scrbar.timer) {
		XtRemoveTimeOut(w->scrbar.timer);
		w->scrbar.timer = 0;
		}
	ExtractPosition(event, &px, &py);
	/***********************************************/
	/*   Check  for which button is being pressed  */
	/*   on the top/bottom arrows. This allows us  */
	/*   to   change   scroll  direction  without  */
	/*   having  to move mouse all the way to the  */
	/*   other end of the scrollbar.	       */
	/***********************************************/
	if (py < ARROW_HEIGHT) {
		switch (event->xbutton.button) {
		  case Button1:
			do_up = 1;
			break;
		  case Button2:
		  	buf.sb_reason = SBAR_START;
			goto do_callback;
		  case Button3:
			do_up = -1;
			break;
		  }
		}
	else if (py >= w->core.height - ARROW_HEIGHT) {
		switch (event->xbutton.button) {
		  case Button1:
			do_up = -1;
			break;
		  case Button2:
		  	buf.sb_reason = SBAR_END;
			goto do_callback;
		  case Button3:
			do_up = 1;
			break;
		  }
		}

	if (do_up > 0) {
		w->scrbar.reason = SBAR_LINE_UP;
	  	buf.sb_reason = SBAR_LINE_UP;
		w->scrbar.uarrow_down = TRUE;
		paint_arrow(w, UP_ARROW);
		XtCallCallbacks(w, XtNsbarCallback, (XtPointer)&buf);
		w->scrbar.timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) w),
			(long) 3 * w->scrbar.repeat_delay, sbar_timer_proc, w);
		return;
		}
	if (do_up < 0) {
		w->scrbar.reason = SBAR_LINE_DOWN;
	  	buf.sb_reason = SBAR_LINE_DOWN;
		w->scrbar.darrow_down = TRUE;
		paint_arrow(w, DOWN_ARROW);
		XtCallCallbacks(w, XtNsbarCallback, (XtPointer)&buf);
		w->scrbar.timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) w),
			(long) 3 * w->scrbar.repeat_delay, sbar_timer_proc, w);
		return;
		}
	switch (event->xbutton.button) {
	  case Button1:
	  	buf.sb_reason = SBAR_PAGE_DOWN;
		break;
	  case Button2:
	  	w->scrbar.elev_down = TRUE;
		paint_thumb(w);
		buf.sb_reason = SBAR_ABSOLUTE;
		if (w->scrbar.orientation == SB_VERTICAL) {
			buf.sb_pos = (float) (py - ARROW_HEIGHT) /
				(float) (w->core.height - 3 * ARROW_HEIGHT);
			}
		else {
			buf.sb_pos = (float) (px - ARROW_HEIGHT) /
				(float) (w->core.width - 3 * ARROW_HEIGHT);
			}
		break;
	  case Button3:
	  	buf.sb_reason = SBAR_PAGE_UP;
		break;
	  default:
	  	return;
	  }

do_callback:
	w->scrbar.reason = buf.sb_reason;
	XtCallCallbacks(w, XtNsbarCallback, (XtPointer)&buf);
	if (buf.sb_reason != SBAR_ABSOLUTE && 
	    buf.sb_reason != SBAR_START &&
	    buf.sb_reason != SBAR_END) {
		w->scrbar.timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) w),
			(long) w->scrbar.page_repeat_delay, sbar_timer_proc, w);
			}
}
static void 
SbarButtonMotion(ScrbarWidget w, XEvent *event, String *x, Cardinal *y)
{
	struct scrbar_callback buf;
	Position	px, py;

	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if ((event->xmotion.state & Button2Mask) == 0)
		return;

	if (w->scrbar.timer) {
		XtRemoveTimeOut(w->scrbar.timer);
		w->scrbar.timer = 0;
		}
	ExtractPosition(event, &px, &py);

  	buf.sb_reason = SBAR_ABSOLUTE;
	buf.sb_pos = (float) (py - ARROW_HEIGHT) /
		(float) (w->core.height - 3 * ARROW_HEIGHT);

	XtCallCallbacks(w, XtNsbarCallback, (XtPointer)&buf);
}
static void 
SbarButtonUp(ScrbarWidget w, XEvent *event, String *x, Cardinal *y)
{

	UNUSED_PARAMETER(event);
	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);

	if (w->scrbar.timer) {
		XtRemoveTimeOut(w->scrbar.timer);
		w->scrbar.timer = 0;
		}

	if (w->scrbar.uarrow_down) {
		w->scrbar.uarrow_down = FALSE;
		paint_arrow(w, UP_ARROW);
		}
	if (w->scrbar.darrow_down) {
		w->scrbar.darrow_down = FALSE;
		paint_arrow(w, DOWN_ARROW);
		}
	if (w->scrbar.elev_down) {
	  	w->scrbar.elev_down = FALSE;
		paint_thumb(w);
		}
}
static void 
SbarInput(ScrbarWidget w, XEvent *event, String *x, Cardinal *y)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(event);
	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(y);
}

GC
create_gc(Display *dpy)
{
	XGCValues	gcValues;

	gcValues.background = BlackPixel(dpy, DefaultScreen(dpy));
	gcValues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
	gcValues.graphics_exposures = TRUE;
	return XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		GCGraphicsExposures | GCBackground | GCForeground, &gcValues);
}
static void
display_xpm_error(err, filename)
int	err;
char	*filename;
{	char	*msg;

	switch (err) {
	  case XpmSuccess:
	  	return;
	  case XpmColorError:
	  	msg = "Could not parse or allocate requested color";
		break;
	  case XpmOpenFailed:
	  	msg = "Couldnt open file";
		break;
	  case XpmFileInvalid:
	  	msg = "Invalid XPM file";
		break;
	  case XpmNoMemory:
	  	msg = "Not enough memory";
		break;
	  case XpmColorFailed:
		msg = "Failed to parse or alloc some color";
		break;
	  default:
	  	msg = "Unknown XPM error";
		break;
	  }
	fprintf(stderr, "Scrbar: XPM Error: %s (%s)\n", msg, filename);

}
static void 
ExtractPosition(XEvent *event, Position *x, Position *y)
{
    switch( event->type ) {
      case MotionNotify:
		*x = event->xmotion.x;	 
		*y = event->xmotion.y;	  
		break;
      case ButtonPress:
      case ButtonRelease:
		*x = event->xbutton.x;   
		*y = event->xbutton.y;   
		break;
      case KeyPress:
      case KeyRelease:
		*x = event->xkey.x;      
		*y = event->xkey.y;	  
		break;
      case EnterNotify:
      case LeaveNotify:
		*x = event->xcrossing.x; 
		*y = event->xcrossing.y; 
		break;
      default:
		*x = 0; 
		*y = 0;
    }
}
/**********************************************************************/
/*   Redraw the elevator.					      */
/**********************************************************************/
static void
paint_thumb(ScrbarWidget sw)
{	int	x, y;
	XSegment	segs[10];
	XSegment	*segp;

	/***********************************************/
	/*   Calculate  where  bottom of thumb goes -  */
	/*   if its below the lower drawing area move  */
	/*   it back into view.                        */
	/***********************************************/
	if (sw->scrbar.orientation == SB_VERTICAL) {
		x = 0;
		y = sw->scrbar.sb_pos + ARROW_HEIGHT;
		if (y + ARROW_HEIGHT >= sw->core.height - ARROW_HEIGHT)
			y = sw->core.height - 2 * ARROW_HEIGHT;
		}
	else {
		y = 0;
		x = sw->scrbar.sb_pos + ARROW_HEIGHT;
		if (x + ARROW_HEIGHT >= sw->core.width - ARROW_HEIGHT)
			x = sw->core.width - 2 * ARROW_HEIGHT;
		}

	XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[BACKGROUND_PIXEL]);
	XFillRectangle(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
		x, y,
		ARROW_HEIGHT, ARROW_WIDTH);

	if (sw->scrbar.elev_down) {
		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[DIM_GRAY_PIXEL]);
		XDrawRectangle(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			x, y, 
			ARROW_WIDTH - 1, ARROW_HEIGHT - 1);

		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[WHITE_PIXEL]);
		segp = segs;
		segp->x1 = x + ARROW_WIDTH - 2;
		segp->y1 = y + 1;
		segp->x2 = x + ARROW_WIDTH - 2;
		segp->y2 = y + ARROW_HEIGHT - 2;
		segp++;

		segp->x1 = x + 2;
		segp->y1 = y + ARROW_HEIGHT - 2;
		segp->x2 = x + ARROW_WIDTH - 2;
		segp->y2 = y + ARROW_HEIGHT - 2;
		segp++;

		XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			segs, segp - segs);
		}
	else {
		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[WHITE_PIXEL]);
		segp = segs;
		segp->x1 = x + 1;
		segp->y1 = y + 1;
		segp->x2 = x + ARROW_WIDTH - 3;
		segp->y2 = y + 1;
		segp++;

		segp->x1 = x + 1;
		segp->y1 = y + 1;
		segp->x2 = x + 1;
		segp->y2 = y + ARROW_HEIGHT - 3;
		segp++;

		XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			segs, segp - segs);

		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[DIM_GRAY_PIXEL]);
		segp = segs;
		segp->x1 = x + ARROW_WIDTH - 2;
		segp->y1 = y + 1;
		segp->x2 = x + ARROW_WIDTH - 2;
		segp->y2 = y + ARROW_HEIGHT - 2;
		segp++;

		segp->x1 = x + 1;
		segp->y1 = y + ARROW_HEIGHT - 2;
		segp->x2 = x + ARROW_WIDTH - 2;
		segp->y2 = y + ARROW_HEIGHT - 2;
		segp++;

		XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			segs, segp - segs);

		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[BLACK_PIXEL]);
		segp = segs;
		segp->x1 = x + ARROW_WIDTH - 1;
		segp->y1 = y;
		segp->x2 = x + ARROW_WIDTH - 1;
		segp->y2 = y + ARROW_HEIGHT - 1;
		segp++;

		segp->x1 = x;
		segp->y1 = y + ARROW_HEIGHT - 1;
		segp->x2 = x + ARROW_WIDTH - 1;
		segp->y2 = y + ARROW_HEIGHT - 1;
		segp++;

		XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			segs, segp - segs);
		}

	if (sw->scrbar.elev_down) {
		int	x1 = x + ARROW_WIDTH / 2 - 1;
		int	y1 = y + ARROW_HEIGHT / 2 - 3;

		XSetForeground(XtDisplay(sw), sw->scrbar.gc, obj_pixels[BLACK_PIXEL]);

		segp = segs;
		segp->x1 = x1;
		segp->y1 = y1;
		segp->x2 = x1;
		segp->y2 = y1;
		segp++;

		segp->x1 = x1 - 1;
		segp->y1 = y1 + 1;
		segp->x2 = x1 + 1;
		segp->y2 = y1 + 1;
		segp++;

		segp->x1 = x1 - 2;
		segp->y1 = y1 + 2;
		segp->x2 = x1 + 2;
		segp->y2 = y1 + 2;
		segp++;

		/***********************************************/
		/*   Line.				       */
		/***********************************************/
		segp->x1 = x1;
		segp->y1 = y1 + 3;
		segp->x2 = x1;
		segp->y2 = y1 + 3;
		segp++;

		segp->x1 = x1;
		segp->y1 = y1 + 4;
		segp->x2 = x1;
		segp->y2 = y1 + 4;
		segp++;

		segp->x1 = x1 - 2;
		segp->y1 = y1 + 5;
		segp->x2 = x1 + 2;
		segp->y2 = y1 + 5;
		segp++;

		segp->x1 = x1 - 1;
		segp->y1 = y1 + 6;
		segp->x2 = x1 + 1;
		segp->y2 = y1 + 6;
		segp++;

		segp->x1 = x1;
		segp->y1 = y1 + 7;
		segp->x2 = x1;
		segp->y2 = y1 + 7;
		segp++;

		XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
			segs, segp - segs);
		}
}
/**********************************************************************/
/*   Redraw an arrow.						      */
/**********************************************************************/
static void
paint_arrow(ScrbarWidget sw, int type)
{	Pixmap	pixmap;
	int	x, y;

	if (type == UP_ARROW)
		pixmap = sw->scrbar.uarrow_down ? uarrow2 : uarrow1;
	else
		pixmap = sw->scrbar.darrow_down ? darrow2 : darrow1;

	if (sw->scrbar.orientation == SB_VERTICAL) {
		x = 0;
		y = type == UP_ARROW ? 0 : sw->core.height - ARROW_HEIGHT;
		}
	else {
		x = type == UP_ARROW ? 0 : sw->core.width - ARROW_HEIGHT;
		y = 0;
		}
	XCopyArea(XtDisplay(sw), pixmap,
		XtWindow(sw), sw->scrbar.gc,
		0, 0, ARROW_WIDTH, ARROW_HEIGHT,
		x, y);
}
/**********************************************************************/
/*   Handle autorepeat on the scrollbar.			      */
/**********************************************************************/
static void
sbar_timer_proc(ScrbarWidget sw, XtIntervalId *timer)
{
	struct scrbar_callback buf;

	UNUSED_PARAMETER(timer);
	
	buf.sb_reason = sw->scrbar.reason;

	XtCallCallbacks(sw, XtNsbarCallback, (XtPointer)&buf);

	sw->scrbar.timer = XtAppAddTimeOut(
		XtWidgetToApplicationContext((Widget) sw),
		(long) sw->scrbar.repeat_delay, sbar_timer_proc, sw);
	
	
}
int
scrbarSetThumb(Widget w, float pos, float size)
{
	ScrbarWidget	sw = (ScrbarWidget) w;
	XSegment	segs[10];
	int	i;

	if (w == NULL || XtWindow(sw) == 0)
		return 0;

	UNUSED_PARAMETER(size);

	XClearArea(XtDisplay(sw), XtWindow(sw),
		1, ARROW_HEIGHT + sw->scrbar.sb_pos,
		ARROW_WIDTH - 2, ARROW_HEIGHT,
		FALSE);
	i = 0;
	segs[i].x1 = 0; segs[i].x2 = 0;
	segs[i].y1 = ARROW_HEIGHT + sw->scrbar.sb_pos; 
	segs[i].y2 = segs[i].y1 + ARROW_HEIGHT;
	i++;
	segs[i].x1 = sw->core.width - 1; segs[i].x2 = sw->core.width - 1;
	segs[i].y1 = ARROW_HEIGHT + sw->scrbar.sb_pos; 
	segs[i].y2 = segs[i].y1 + ARROW_HEIGHT;
	i++;

	XDrawSegments(XtDisplay(sw), XtWindow(sw), sw->scrbar.gc,
		segs, i);

	sw->scrbar.sb_pos = pos * (sw->core.height - 3 * ARROW_HEIGHT);
	paint_thumb(sw);
	return 0;
}
Widget
XmCreateScrollBar(Widget parent, char *name, Arg *args, int n)
{
	return XtCreateManagedWidget(name, scrbarWidgetClass, parent, args, n);
}
