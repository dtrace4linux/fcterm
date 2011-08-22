#ifndef _Scrbar_h
#define _Scrbar_h

# include	<X11/Xmu/Converters.h>

/****************************************************************
 *
 * Scrbar widget
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

scrbarWidget:
 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 maximum             Maximum      	Int             100
 minimum             Minimum      	Int             0
 orientation         Orientation  	Int             horizontal
 pageRepeatDelay     PageRepeatDelay  	Int             240  
 repeatDelay  	     RepeatDelay  	Int             60  
 shown        	     Shown        	Float           0.0 
 sbarCallback        SbarCallback	sbar_callback	0.0 
 inputCallback	     InputCallback	Callback	NULL
 resizeCallback	     ResizeCallback	Callback	NULL
*/
# if !defined(XtNmaximum)
#	define	XtNmaximum	"maximum"
#	define	XtCmaximum	"Maximum"
# endif
# if !defined(XtNminimum)
#	define	XtNminimum	"minimum"
#	define	XtCminimum	"Minimum"
# endif
# if !defined(XtNorientation)
#	define	XtNorientation	"orientation"
#	define	XtCorientation	"Orientation"
# endif

#define	XtNpageRepeatDelay "pageRepeatDelay"
#define	XtNrepeatDelay "repeatDelay"
# if !defined(XtNshown)
#	define	XtNshown "shown"
# endif
#define	XtNsbarCallback "sbarCallback"
#define	XtNinputCallback "inputCallback"
#define	XtNresizeCallback "resizeCallback"

#define	XtCPageRepeatDelay "PageRepeatDelay"
#define	XtCRepeatDelay "RepeatDelay"
# if !defined(XtCShown)
#	define	XtCShown "Shown"
# endif
#define	XtCSbarCallback "SbarCallback"
#define	XtCInputCallback "InputCallback"
#define	XtCResizeCallback "ResizeCallback"

/**********************************************************************/
/*   declare specific ScrbarWidget class and instance datatypes	      */
/**********************************************************************/

typedef struct _ScrbarClassRec*	ScrbarWidgetClass;
typedef struct _ScrbarRec*	ScrbarWidget;

/**********************************************************************/
/*   declare the class constant					      */
/**********************************************************************/

extern WidgetClass scrbarWidgetClass;

# define	SBAR_PAGE_UP	0
# define	SBAR_PAGE_DOWN	1
# define	SBAR_LINE_UP	2
# define	SBAR_LINE_DOWN	3
# define	SBAR_ABSOLUTE	4
# define	SBAR_START	5
# define	SBAR_END	6
# define	SBAR_INPUT	7

struct scrbar_callback {
	int	sb_reason;
	float	sb_pos;
	XEvent	*sb_event;
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

int	scrbarSetThumb(Widget w, float, float);
# define	XawScrollbarSetThumb(a, b, c)	scrbarSetThumb(a, b, c)
#endif /* _Scrbar_h */
