#ifndef _ScrbarP_h
#define _ScrbarP_h

#include "scrbar.h"
/**********************************************************************/
/*   include superclass private header file			      */
/**********************************************************************/
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

typedef struct {
	int	dummy;
} ScrbarClassPart;

typedef struct _ScrbarClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ScrbarClassPart	scrbar_class;
} ScrbarClassRec;

# define	SB_VERTICAL	1
# define	SB_HORIZONTAL	2

extern ScrbarClassRec scrbarClassRec;
typedef struct {
	XtOrientation	orientation;	/* horizontal or vertical */
	float		shown;
	XtCallbackList	sbar_callback;
	XtCallbackList	jump_callback;
	XtCallbackList	scroll_callback;
	XtCallbackList	input_callback;
	XtCallbackList	resize_callback;
	int		page_repeat_delay;
	int		repeat_delay;

/* private: */
	char		uarrow_down;
	char		darrow_down;
	char		elev_down;

	GC		gc;
	int		sb_pos;
	int		reason;
	int		sb_min;
	int		sb_max;
	XtIntervalId	timer;
} ScrbarPart;

typedef struct _ScrbarRec {
    CorePart		core;
    ScrbarPart		scrbar;
} ScrbarRec;

#endif /* _ScrbarP_h */
