# include	<stdio.h>
# include	<fcntl.h>
# include	<string.h>
# include	<signal.h>
# include	<errno.h>
# include	<ctype.h>
# include	<X11/X.h>
# include	<X11/Intrinsic.h>
# include	<X11/StringDefs.h>
# include	<X11/Shell.h>
# include	<Xm/MainW.h>
# include	<Xm/Separator.h>
# include	<Xm/CascadeB.h>
# include	<Xm/PushB.h>

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
/*   Structure used to build menus.				      */
/**********************************************************************/
# define	F_END	0x01
# define	F_SEP	0x02
# define	F_GLYPH	0x04
# define	GLYPH_MENU_ITEM(name, width, height, bits) \
	{F_GLYPH, name, NULL, NULL, NULL, NULL, width, height, bits}
struct menu {
	int		flags;
	char		*widget_name;
	struct menu	*sub_menu;
	char		*label;
	void		(*callback)();
	Widget		widget;
	int		width;
	int		height;
	char		*bits;
	Pixel		pixel;
	};

Widget	create_menu PROTO((Widget, struct menu *, void (*)()));
Widget	create_menu_button PROTO((char *, void (*)(), struct menu *, Widget));
int	find_menu_widget PROTO((struct menu *, Widget));
