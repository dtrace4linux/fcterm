/**********************************************************************/
/*   Color  terminal  emulator using the ctw (Color terminal widget)  */
/*   widget. This module is based on the athena widgets.	      */
/*   								      */
/*   Author: Paul Fox						      */
/*   Date:   January 1992-2019       				      */
/**********************************************************************/

# include	<fcterm.h>
# include	"include/build.h"
# include	<X11/Xaw/Paned.h>
# include	<X11/Xaw/Scrollbar.h>
# include	<X11/IntrinsicP.h>
# include	"scrbar.h"
# include	<w_draw.h>
# include	<w_string.h>
# include	<time.h>
# include	<limits.h>
# include	<sys/stat.h>
# include	<X11/Xaw/Command.h>
# include	<X11/Xaw/MenuButton.h>
# include	<X11/Xaw/SimpleMenu.h>
# include	<X11/Xaw/SmeBSB.h>
# include	<X11/Xaw/SmeLine.h>
# include	<X11/Xaw/AsciiText.h>
# include	<X11/Xaw/Toggle.h>
# include	<X11/Xaw/Command.h>
# undef sscanf

# define	MAX_SCREENS	12
/**********************************************************************/
/*   Allow us to enable/disable window manager offsets.		      */
/**********************************************************************/
int	cr_enable_wm_offset = TRUE;

/**********************************************************************/
/*   Offsets for window manager frame.				      */
/**********************************************************************/
int	mwm_x_offset;
int	mwm_y_offset;

static char *bitmap_fonts[] = {
	"5x7", "5x8", "6x10", "6x12", "6x13",
	"6x13bold",
	"6x9",
	"7x13",
	"* 7x13bold",
	"7x14",
	"7x14bold",
	"8x13",
	"8x13bold",
	"8x16",
	"9x15",
	"9x15bold",
	"10x20",
	"12x24",
	NULL
	};
static char *ft_fonts[] = {
	"DejaVu Sans Mono-5",
	"DejaVu Sans Mono-6",
	"DejaVu Sans Mono-7",
	"DejaVu Sans Mono-8",
	"DejaVu Sans Mono-9",
	"* DejaVu Sans Mono-10",
	"DejaVu Sans Mono-11",
	"DejaVu Sans Mono-12",
	"DejaVu Sans Mono-13",
	"DejaVu Sans Mono-14",
	"DejaVu Sans Mono-15",
	"DejaVu Sans Mono-16",
	"DejaVu Sans Mono-17",
	"DejaVu Sans Mono-18",
	NULL
	};
static char **fonts = bitmap_fonts;

int 	cur_font;
extern int	freetype_flag;
extern Atom WM_DELETE_WINDOW;

/**********************************************************************/
/*   Description for the popup menu.				      */
/**********************************************************************/
struct menu_commands {
	char	*name;
	int	user_data;
	Widget	widget;
	};
	
struct menu_commands menu_commands[] = {
		{0,			LINE_MENU_ITEM, 0},
		{" Paste",		PASTE, 0},
		{" Paste+NL",		PASTE_NL, 0},
		{" Mark position",	MARK_POSITION, 0},
		{" Auto label",		LABEL_LINES, 0},
		{" Font smaller",	FONT_SMALLER, 0},
		{" Font larger",	FONT_LARGER, 0},
		{" Reset Terminal",	RESET_CHARACTERS, 0},
		{" Snap history",	SNAP_HISTORY, 0},
		{" Lower window",	LOWER_WINDOW, 0},
		{" Zoom window",	ZOOM_WINDOW, 0},
		{" Zoom screen",	ZOOM_SCREEN, 0},
		{" Search",	        MENU_SEARCH, 0},
		{" Show minimap",	MENU_TERMINAL_MAP, 0},

		{0,			LINE_MENU_ITEM, 0},

		{" Enable autoswitch",	ENABLE_AUTOSWITCH, 0},
		{" Terminal attributes..",
					DIALOG_BOX, 0},
		{" Status Monitor..",	MONITOR, 0},
		{0,			LINE_MENU_ITEM, 0},
		{" New screen",		MENU_NEW_SCREEN, 0},
		{" Fork new fcterm",	FORK_NEW_FCTERM, 0},
		{0,			LINE_MENU_ITEM, 0},
		{" Paste alphabet",	MENU_PASTE_ALPHABET, 0},
		{" Toggle scrollbar",	TOGGLE_SCROLLBAR, 0},
		{" About...",		ABOUT_BOX, 0},
		{0, MENU_EOF, 0}
		};

struct menu_commands menu_extra[] = {
		{0,			LINE_MENU_ITEM, 0},
		{" Fork mwm",		MENU_FORK_MWM, 0},
		{" Form olwm",		MENU_FORK_OLWM, 0},
		{" Paste alphabet",	MENU_PASTE_ALPHABET, 0},
		{0,			LINE_MENU_ITEM, 0},
		{" Exit",		EXIT_MENU_ITEM, 0},
		{0, MENU_EOF, 0}
		};

/**********************************************************************/
/*   Pointer to the current display.				      */
/**********************************************************************/
fcterm_t	*cur_ctw;

/**********************************************************************/
/*   Pointer to the ghost display.				      */
/**********************************************************************/
fcterm_t	*ghost_ctw;

int	prog_argc;
char	**prog_argv;

extern char	*group_label;
extern int	label_arg_specified;
extern int	fork_flag;
extern int	main_x;
extern int	main_y;
extern char	*font_file;
static Widget	font_dialog;
static Widget	dialog;
static Widget	monitor;
Widget		status;
Widget		pane;
Widget		pshell;
Widget		scrollbar;
Widget		extra_pshell;
Widget		top_level;
Widget		w_attr[CTW_MAX_ATTR + 16];
Widget		w_status[4];
Widget		w_fonts[16];

XtIntervalId	timer_id;
XtIntervalId	spanel_timer_id;

char	*font_names[8];
XFontStruct	*fs_5x8;

XtAppContext	app_con;
extern int	pid;

/**********************************************************************/
/*   Copy of attributes which user can change.			      */
/**********************************************************************/
int	attributes[72];
char	**attr_names;

char	*prog_name;

static String fallback_resources[] = {
	"*foreground:		yellow",
	"*background:		black",
	"*status*background:		gray50",
	"XTerm*menu*borderColor:	#b7e5e5",
	"XTerm*menu*borderWidth:	4",
	 
	"*menu*font:		7x13bold",

	"*menu*background:	#b7e5e5",
	"*menu*foreground:	black",
	"*pane0*background:	#b7e5e5",
	"*pane0*foreground:	black",
	"XTerm*pane0*borderWidth:	0",
	"*internalBorderWidth:	0",
	"*VT100.background: 	black",
	"*scrollbar*background:	black",
	"*scrollbar*foreground:	#b7e5e5",
	"*allowShellResize:	true",
	(char *) NULL
	};

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
void	cmd_font_larger(void);
void	cmd_font_smaller(void);
void	setup_cur_font(void);
void handle_commands(int i, int argc, char **argv);
fcterm_t *restore_label(void);
fcterm_t *restore_state(void);
void set_font(char *font);
void say_hello(fcterm_t *cur_ctw);
void map_expose_callback(Widget widget, XtPointer client_data, XEvent *event);
void map_input_callback(Widget widget, XtPointer client_data, DrawingAreaCallbackStruct *event);
char	*basename(char *);
void	set_autoswitch(int);
static int x11_error_handler(Display *dpy, XErrorEvent *event);
static int x11_io_error_handler(Display *dpy);
void layout_frame(void);
int	create_screen(fcterm_t *, int, int, char *pwd);
static void structure_notify_callback(Widget w, XtPointer ptr, XEvent *event);
Widget	create_menu PROTO((Widget, char *, struct menu_commands *));
void	about_box();
Widget	create_monitor_box();
void	toggle_scrollbar(fcterm_t *);
void	status_expose_callback(Widget widget, XtPointer client_data, XtPointer call_data);
void	status_input_callback(Widget widget, XtPointer client_data, DrawingAreaCallbackStruct *cbs);
void	pane_focus_callback();
void	pane_input_callback();
void	sbar_callback();
void	jump_callback();
void	scroll_callback();
void	appl_callback();
void	input_callback();
void	top_callback(Widget w, int type, ctw_callback_t *reason);
void	menu_callback();
void	mouse_callback();
void	ok_dialog();
void	about_ok();
void	apply_dialog(Widget w, XtPointer client_data, XtPointer call_data);
void	cancel_dialog(Widget w, XtPointer client_data, XtPointer call_data);
void	refresh_dialog(Widget w, XtPointer client_data, XtPointer call_data);
void	read_font_file(void);
void	toggle_callback(Widget w, XtPointer client_data, XtPointer call_data);
void	ok_dialog(Widget w, XtPointer client_data, XtPointer call_data);
void	clear_status(Widget w, XtPointer client_data, XtPointer call_data);
void	status_timer_proc(XtPointer client_data, XtIntervalId *timer);
void	timeout_proc(XtPointer client_data, XtIntervalId *timer);
void	ok_font(Widget w, XtPointer client_data, XtPointer call_data);
void	ok_status(Widget w, XtPointer client_data, XtPointer call_data);
void	cancel_font(Widget w, XtPointer client_data, XtPointer call_data);
void	zoom_screen();
void	zoom_window();
char	*getenv();
void	terminal_attributes();
void menu_new_screen(fcterm_t *, int);

static void
ProtocolHandler( Widget w, XtPointer closure, XEvent *event, Boolean *cont )
{	Atom WM_PROTOCOLS;

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(closure);
	UNUSED_PARAMETER(cont);

	switch (event->type) {
	  case ClientMessage: {
		XClientMessageEvent *mp = (XClientMessageEvent *) event;

		WM_PROTOCOLS = XInternAtom(XtDisplay(top_level), "WM_PROTOCOLS", False);
		if (mp->message_type != WM_PROTOCOLS)
			break;

#if 0
	  	printf("got a client message: %ld %ld win=%ld top=%ld closure=%p\n", 
			mp->message_type,
			mp->data.l[0],
			mp->window,
			XtWindow(top_level), closure);
#endif

		if (closure == (XtPointer) 1) {
			XtPopdown(w);
			break;
		}

		if ((Atom) mp->data.l[0] == WM_DELETE_WINDOW &&
		    mp->window == XtWindow(top_level)) {
			printf("Received WM_DELETE_WINDOW - terminating\n");
			exit(1);
			}

		break;
		}
	  }
}
/**********************************************************************/
/*   Main entry point.						      */
/**********************************************************************/
int
main(int argc, char **argv)
{	int	n;
	int	restarted = FALSE;
	int	arg_index;
	Arg	args[20];
	fcterm_t	*cur_ctw;
	char	*cp;

	if ((cp = getenv("DISPLAY")) == NULL) {
		printf("$DISPLAY is not set - so we cannot connect to the X server.\n");
		exit(1);
		}
	if (*cp == '\0') {
		printf("$DISPLAY is blank - so we cannot connect to the X server.\n");
		exit(1);
		}

	argc = steal_switches(argc, argv);
	prog_argc = argc;
	prog_argv = argv;

	n = 0;
	XtSetArg(args[n], XtNinput, TRUE); n++;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
	XtSetArg(args[n], XtNwidth, 400); n++;
	XtSetArg(args[n], XtNheight, 400); n++;
	XtSetArg(args[n], XtNborderWidth, 0); n++;
	top_level = XtAppInitialize(&app_con, argv[0], NULL, 0,
		&argc, argv, fallback_resources, args, n);

	setup_signal_handlers();

	fcntl(ConnectionNumber(XtDisplay(top_level)), F_SETFD, FD_CLOEXEC);

	setup_cur_font();

	init_term();

	/***********************************************/
	/*   In  case  X server crashes, need to free  */
	/*   the ptys.				       */
	/***********************************************/
	XSetErrorHandler(x11_error_handler);
	XSetIOErrorHandler(x11_io_error_handler);

	fs_5x8 = XLoadQueryFont(XtDisplay(top_level), "5x8");

	/***********************************************/
	/*   Intercept  structure  redirect so we can  */
	/*   figure out wm size.		       */
	/***********************************************/
	XtAddEventHandler(top_level, 
		StructureNotifyMask, FALSE,
		structure_notify_callback, (XtPointer) 0);

	XtAddEventHandler(top_level,
		FocusChangeMask, FALSE,
		pane_focus_callback, (XtPointer) NULL);
	/***********************************************/
	/*   Now do command line switches.	       */
	/***********************************************/
	arg_index = do_switches(argc, argv);

	read_font_file();

	handle_commands(arg_index, argc, argv);

	/***********************************************/
	/*   Make  parent  detach  from  shell  so we  */
	/*   dont have lots of jobs hanging around.    */
	/***********************************************/
	if (fork_flag) {
		if (fork())
			exit(0);
		}

	group_init();
	
	/***********************************************/
	/*   Protect  us from ^C killing ourselves in  */
	/*   a child.				       */
	/***********************************************/
# if SYSV_SETPGRP
#	if defined(STREAMS_PTY)
		setsid();
#	else
		setpgrp();
#	endif
# else
		setpgrp(getpid(), 0);
# endif

	n = 0;
	XtSetArg(args[n], XtNwidth, 400); n++;
	XtSetArg(args[n], XtNheight, 400); n++;
	XtSetArg(args[n], XtNborderWidth, 0); n++;
	pane = XtCreateWidget("pane", drawWidgetClass,
			top_level, args, n);
	XtAddEventHandler(pane,
		FocusChangeMask, FALSE,
		pane_focus_callback, (XtPointer) NULL);
	XtAddCallback(pane, XtNinputCallback, pane_input_callback, NULL);
	XtManageChild(pane);

	n = 0;
	XtSetArg(args[n], XtNwidth, 400); n++;
	XtSetArg(args[n], XtNheight, STATUS_HEIGHT); n++;
	XtSetArg(args[n], XtNborderWidth, 0); n++;
	status = XtCreateWidget("status", drawWidgetClass,
			pane, args, n);
	XtManageChild(status);
	XtAddEventHandler(pane,
		FocusChangeMask, FALSE,
		pane_focus_callback, (XtPointer) NULL);
	XtAddCallback(status, XtNexposeCallback, status_expose_callback, NULL);
	XtAddCallback(status, XtNinputCallback, status_input_callback, NULL);

	/***********************************************/
	/*   Create a ghost display - where we can do  */
	/*   some interesting graphics.		       */
	/***********************************************/
	ghost_ctw = new_fcterm(GHOST_ID);
	create_screen(ghost_ctw, FALSE, FALSE, NULL);

	/***********************************************/
	/*   Now create the first screen.	       */
	/***********************************************/
	if (getenv("FCTERM_STATE")) {
		cur_ctw = restore_state();
		restarted = TRUE;
		}
	else if (label_arg_specified) {
		if ((cur_ctw = restore_label()) != NULL)
			restarted = TRUE;
		}

	if (!restarted) {
		cur_ctw = new_fcterm(0);
		if (create_screen(cur_ctw, TRUE, TRUE, NULL) < 0)
			exit(1);
		}

	/***********************************************/
	/*   Create scrollbar.			       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNwidth, SCRBAR_WIDTH); n++;
	XtSetArg(args[n], XtNborderWidth, 0); n++;
	scrollbar = XtCreateWidget("scrollbar", scrbarWidgetClass,
			pane, args, n);
	XtAddCallback(scrollbar, XtNsbarCallback, sbar_callback, cur_ctw);
	XtAddCallback(scrollbar, XtNjumpProc, jump_callback, cur_ctw);
	XtAddCallback(scrollbar, XtNscrollProc, scroll_callback, cur_ctw);

	/***********************************************/
	/*   Use the widget size to control the frame  */
	/*   size.				       */
	/***********************************************/
	{Dimension w, h;
	XtVaGetValues(cur_ctw->f_ctw, XtNheight, &h, XtNwidth, &w, NULL);
	XtVaSetValues(top_level, XtNwidth, w + SCRBAR_WIDTH, XtNheight, h + STATUS_HEIGHT, NULL);
	}

	/***********************************************/
	/*   Layout the whole frame before putting on  */
	/*   screen.				       */
	/***********************************************/
	layout_frame();

	XtRealizeWidget(top_level);
	XSetWMProtocols(XtDisplay(top_level), XtWindow(top_level), &WM_DELETE_WINDOW, 1);
	XtAddRawEventHandler(top_level, (EventMask)0, TRUE,
                             ProtocolHandler, (XtPointer) NULL);

	set_title(cur_ctw, CHANGE_TITLE | CHANGE_ICON, NULL);
	
	create_icon_pixmap();

	if (!restarted)
		say_hello(cur_ctw);
	
	do_wm_hints(top_level, cur_ctw->f_ctw, TRUE);

	stats.start_time = time((time_t *) NULL);

	XtSetKeyboardFocus(top_level, cur_ctw->f_ctw);

	XtManageChild(scrollbar);
	layout_frame();

	if (restarted) {
		/*printf("setting x, y = %d, %d\n", main_x, main_y);*/
		/*XtMoveWidget(top_level, main_x, main_y);*/
		}

	/***********************************************/
	/*   Timer to handle status panel updates.     */
	/***********************************************/
	spanel_timer_id = XtAppAddTimeOut(app_con, 1000L, status_timer_proc, NULL);

	XtAppMainLoop(app_con);
	exit(0);
	return 0;
}
/**********************************************************************/
/*   Display copyright info.					      */
/**********************************************************************/
void
about_box(int x, int y)
{	static char	*about_text[] = {
		"FCTERM - Color terminal emulator",
		"",
		"(C) 1990-2021 Paul Fox, Foxtrot Systems Ltd",
		"",
		"E-mail: crisp.editor:gmail.com, paul.d.fox@gmail.com",
		"",
		"<version>",
		(char *) NULL
		};
	char	verbuf[BUFSIZ];
	Widget	shell, pane0, w1;
	int	i;
	int	n;
	Arg	args[20];
	Display	*dpy = XtDisplay(top_level);

	for (i = 0; about_text[i]; i++) {
		if (strcmp(about_text[i], "<version>") == 0) {
			snprintf(verbuf, sizeof verbuf,
				"Fcterm version v%d.%03d-b%d PID:%d",
				MAJ_VERSION, MIN_VERSION, VERSION_BUILD_NO, 
				getpid());
			about_text[i] = verbuf;
			break;
			}
		}
	n = 0;
	XtSetArg(args[n], XtNx, x); n++;
	XtSetArg(args[n], XtNy, y); n++;
	shell = XtCreatePopupShell("shell",
		transientShellWidgetClass, top_level, args, n);
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane0 = XtCreateManagedWidget("pane0", panedWidgetClass,
		shell, args, n);
	/***********************************************/
	/*   Create title of dialog box.	       */
	/***********************************************/
	for (i = 0; about_text[i]; i++) {
		n = 0;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		XtCreateManagedWidget(about_text[i],
			labelWidgetClass, pane0, args, n);
		}
	/***********************************************/
	/*   Create push buttons.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("  Ok  ",
		commandWidgetClass, pane0, args, n);
	XtPopup(shell, XtGrabNone);
	XtAddCallback(w1, XtNcallback, about_ok, shell);

	XSetWMProtocols(dpy, XtWindow(shell), &WM_DELETE_WINDOW, 1);
	XtAddRawEventHandler(shell, (EventMask)0, TRUE,
                             ProtocolHandler, (XtPointer) 1);
//	printf("added protocol\n");
}
/**********************************************************************/
/*   Pop down about dialog box.                                       */
/**********************************************************************/
void
about_ok(Widget w, XtPointer shell_widget, XtPointer call_data)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(call_data);
	XtDestroyWidget(shell_widget);
}
int
my_sscanf(char *str, int *x, int *y)
{
	*x = atoi(str);
	while (isdigit(*str))
		str++;
	while (*str && !isdigit(*str))
		str++;
	*y = atoi(str);
	return 2;
}

void
cmd_asciitext_record(CtwWidget ctw, int cmd)
{	char	*fn = NULL;
	char	buf[BUFSIZ];
	char	buf2[BUFSIZ];
	time_t t;
	if (fn == NULL) {
		t = time(NULL);
		strftime(buf2, sizeof buf2, "%Y%m%d-%H%M%S", localtime(&t));
		snprintf(buf, sizeof buf, 
			"/var/tmp/%s/record-%s.cast",
			getenv("USER"), buf2);
		fn = buf;
		}

	ctw_asciitext_record(ctw, cmd, fn);
}

void
cmd_asciitext_stop(CtwWidget ctw)
{
	ctw_asciitext_record(ctw, RECORD_STOP, NULL);
	/* Auto create svg file. */
}

void
cmd_font_larger()
{
  	if (fonts[cur_font + 1] == NULL)
		return;
	set_font(fonts[++cur_font]);
}
void
cmd_font_smaller()
{
  	if (cur_font == 0)
		return;
	set_font(fonts[--cur_font]);
}

/**********************************************************************/
/*   Create a new popup menu.					      */
/**********************************************************************/
Widget
create_menu(Widget parent, char *title, struct menu_commands *cmds)
{	int	i;
	Widget	w;
	int	n;
	Widget	menu_item;
	Arg	args[20];

	UNUSED_PARAMETER(parent);

	/***********************************************/
	/*   Create popup menu.			       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNlabel, title); n++;
	w = XtCreatePopupShell("menu",
		simpleMenuWidgetClass,
		top_level,
		args, n);

	/***********************************************/
	/*   Create the popup menu.		       */
	/***********************************************/
	for (i = 0; ; i++) {
		if (cmds[i].user_data == MENU_EOF)
			break;
		if (cmds[i].user_data == LINE_MENU_ITEM) {
			cmds[i].widget = XtCreateManagedWidget(
				"line", smeLineObjectClass, w, NULL, 0);
			continue;
			}
		menu_item = XtCreateManagedWidget(cmds[i].name,
			smeBSBObjectClass, w, NULL, 0);
		XtAddCallback(menu_item, XtNcallback, menu_callback, 
			(XtPointer) (long) cmds[i].user_data);
		cmds[i].widget = menu_item;
		}

	return w;
}
/**********************************************************************/
/*   Create a new screen widget, and allocate a pty for it.	      */
/**********************************************************************/
int
create_screen(fcterm_t *ctwp, int proper, int do_spawn, char *pwd)
{	int	n;
	Arg	args[20];

	/***********************************************/
	/*   Create VT100 widget.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNallowResize, TRUE); n++;
	XtSetArg(args[n], XtNclientData, ctwp); n++;
	XtSetArg(args[n], XtNborderWidth, 0); n++;
	if (sun_function_keys >= 0) {
		XtSetArg(args[n], XtNsunFunctionKeys, sun_function_keys); n++;
		}
	if (proper) {
		ctwp->f_ctw = XtCreateManagedWidget("VT100",
			ctwWidgetClass,
			pane, args, n);
		if (geometry) {
			int	w, h;
			int	fheight, fwidth;
			my_sscanf(geometry, &w, &h);
			n = 0;
			XtSetArg(args[n], XtNcolumns, w); n++;
			XtSetArg(args[n], XtNrows, h); n++;
			get_font_size(ctwp, &fwidth, &fheight);
			XtSetArg(args[n], XtNwidth, fwidth * w); n++;
			XtSetArg(args[n], XtNheight, fheight * h); n++;
			XtSetValues(ctwp->f_ctw, args, n);

	/*		n = 0;
			XtSetArg(args[n], XtNwidth, fwidth * w); n++;
			XtSetArg(args[n], XtNheight, fheight * h); n++;
			XtSetValues(ctwp->f_scrollbar, args, n);*/
			}
		XtAddCallback(ctwp->f_ctw, XtNapplCallback, appl_callback, ctwp);
		XtAddCallback(ctwp->f_ctw, XtNkbdCallback, input_callback, ctwp);
		XtAddCallback(ctwp->f_ctw, XtNresizeCallback, resize_callback, ctwp);
		XtAddCallback(ctwp->f_ctw, XtNtopCallback, top_callback, ctwp);
		XtAddCallback(ctwp->f_ctw, XtNmouseCallback, mouse_callback, ctwp);
		XtVaSetValues(ctwp->f_ctw, XtNttyName, ctwp->f_ttyname, NULL);

		if (!do_spawn)
			return 0;

		if (spawn_child(ctwp, pwd) < 0)
			return -1;
		/***********************************************/
		/*   Dont let anyone else inherit the fd.      */
		/***********************************************/
		fcntl(ctwp->f_pty_fd, F_SETFD, 1);
		ctwp->f_input_id = XtAppAddInput(app_con, ctwp->f_pty_fd, (XtPointer) XtInputReadMask, pty_input, ctwp);
		if (debug) {
			printf("XtAppAddInput(%s - fd=%d)=%d\n", ctwp->f_ttyname, ctwp->f_pty_fd, (int) ctwp->f_input_id);
			}

		set_default_attributes(ctwp);

		}
	else {
		ctwp->f_ctw = XtCreateManagedWidget("ghost",
			drawWidgetClass,
			pane, args, n);
		XtUnmanageChild(ctwp->f_ctw);
		XtAddCallback(ctwp->f_ctw, XtNexposeCallback, (XtCallbackProc) map_expose_callback, NULL);
		XtAddCallback(ctwp->f_ctw, XtNinputCallback, (XtCallbackProc) map_input_callback, NULL);
		}
	cur_ctw = ctwp;
	return 0;
}
/**********************************************************************/
/*   return widget associated with a menu item.			      */
/**********************************************************************/
Widget
find_menu_item(struct menu_commands *tbl, int val)
{	int	i;

	for (i = 0; ; i++) {
		if (tbl[i].user_data == MENU_EOF)
			return (Widget) NULL;
		if (tbl[i].user_data == val)
			return tbl[i].widget;
		}
}
/**********************************************************************/
/*   Handle commands - shortcuts to printing escape sequences.	      */
/**********************************************************************/
void
handle_commands(int i, int argc, char **argv)
{	char	*help = 
"You can use 'fcterm status' and save it to a file, and then restore\n"
"from the saved file with 'fcterm restore <filename>'.\n"
"\n"
"colormap            Show ESC color map\n"
"create-screen <n>   Create or switch to the screen specified [0..11]\n"
"escseq              Show escape sequences\n"
"group / ungroup     Enable grouping of fcterm's. Grouping will keep the\n"
"                    relative positions fixed. Useful when screen size\n"
"                    changes.\n"
"larger              Make font larger.\n"
"minimap             Show minimap.\n"
"record              Record asciitext v2 and generate SVG file\n"
"record-silent       Same as \"record\" but dont add any output\n"
"restore <filename>  Restore position from saved state.\n"
"search              Display search prompt.\n"
"smaller             Make font smaller.\n"
"status              Show status.\n"
"stop                Stop recording file and generate SVG playback\n"
"zoom                Make window as tall as screen height\n"
		;

	if (i >= argc)
		return;

	if (argv[i][0] == '-')
		return;

	for ( ; i < argc; i++) {
		char	*cp = argv[i];
		if (strcmp(cp, "colormap") == 0) {
			printf("\033[1949m");
			continue;
			}
		if (strcmp(cp, "escseq") == 0) {
			extern char sequences[];
			printf("%s", sequences);
			exit(0);
			}
		if (strcmp(cp, "group") == 0) {
			printf("\033[1938;1mGrouping enabled.\n");
			continue;
			}
		if (strcmp(cp, "help") == 0) {
			printf("\n%s", help);
			continue;
			}
		if (strcmp(cp, "minimap") == 0) {
			printf("\033[1942m");
			continue;
			}
		if (strcmp(cp, "ungroup") == 0) {
			printf("\033[1938;0mGrouping disabled.\n");
			continue;
			}
		if (strcmp(cp, "record") == 0) {
			printf("\033[1946m");
			continue;
			}
		if (strcmp(cp, "record-silent") == 0) {
			printf("\033[1948m");
			continue;
			}
		if (strcmp(cp, "restore") == 0) {
			if (i + 1 >= argc) {
				printf("usage: restore <filename>\n");
				printf("File is the output from 'fcterm status >filename'\n");
				exit(1);
				}
			group_restore(argv[++i]);
			continue;
			}
		if (strcmp(cp, "search") == 0) {
			printf("\033[1940mSearch mode enabled.\n");
			continue;
			}
		if (strcmp(cp, "smaller") == 0) {
			printf("\033[1944m");
			continue;
			}
		if (strcmp(cp, "larger") == 0) {
			printf("\033[1945m");
			continue;
			}
		if (strcmp(cp, "stop") == 0) {
			printf("\033[1947m");
			continue;
			}
		if (strcmp(cp, "status") == 0) {
			printf("\033[1939m");
			continue;
			}
		if (strcmp(cp, "create-screen") == 0) {
			if (i + 1 >= argc) {
				printf("usage: create-screen <n>\n");
				exit(1);
				}

			int id = atoi(argv[++i]);
			printf("\033[1943;%dm", id);
			continue;
			}
		if (strcmp(cp, "zoom") == 0) {
			printf("\033[1941m");
			continue;
			}
		printf("Command: '%s' not recognized.\n", cp);
		printf("%s", help);
		exit(1);
		}
	exit(0);
}
/**********************************************************************/
/*   Re-organise everything after a size change.		      */
/**********************************************************************/
void
layout_frame()
{	Dimension fw, fh;
	fcterm_t	*ctwp;
	Dimension sw = 0, sh = 0;

	XtVaGetValues(top_level, XtNheight, &fh, XtNwidth, &fw, NULL);
	if (fw < STATUS_HEIGHT + 10 || fh < STATUS_HEIGHT + 10)
		return;

	if (XtIsManaged(scrollbar)) {
		XtVaGetValues(scrollbar, XtNheight, &sh, XtNwidth, &sw, NULL);
		sw = MIN(sw, SCRBAR_WIDTH);
		}

//printf("frame=%dx%d\n", fw, fh);
	XtVaSetValues(pane, 
		XtNx, 0,
		XtNy, 0,
		XtNheight, fh,
		XtNwidth, fw, 
		NULL);
	/***********************************************/
	/*   Status bar.			       */
	/***********************************************/
	XtVaSetValues(status, 
		XtNx, 0,
		XtNy, fh - STATUS_HEIGHT,
		XtNheight, STATUS_HEIGHT,
		XtNwidth, fw, 
		NULL);
	/***********************************************/
	/*   Scrollbar.				       */
	/***********************************************/
	if (sw) {
		XtVaSetValues(scrollbar, 
			XtNx, 0,
			XtNy, 0,
			XtNheight, fh - STATUS_HEIGHT, 
			XtNwidth, sw, 
			NULL);
		}

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		XtVaSetValues(ctwp->f_ctw, 
			XtNx, sw + 1,
			XtNy, 0,
			XtNheight, fh - STATUS_HEIGHT,
			XtNwidth, fw - sw,
			NULL);
		}
	if (status && XtWindow(status))
		status_expose_callback(status, 0, 0);
}
/**********************************************************************/
/*   Function  to  handle  indication from VT100 widget that the top  */
/*   line  of  the window has moved so we can associate the thumb on  */
/*   the scrollbar with the position.				      */
/**********************************************************************/
void
top_callback(Widget w, int type, ctw_callback_t *reason)
{	float	pos;
	int	save_lines = ctw_get_total_rows((CtwWidget) w);

	UNUSED_PARAMETER(type);

	/***********************************************/
	/*   Dont  touch  the  scrollbar if this isnt  */
	/*   the current widget.		       */
	/***********************************************/
	if (w != cur_ctw->f_ctw)
		return;

//	n = 0;
//	XtSetArg(args[n], XtNsaveLines, &save_lines); n++;
//	XtGetValues(w, args, n);
	pos = (float) reason->top_line / (float) save_lines; 
	if (pos >= (float) 1.0)
		pos = 1.0;
	XawScrollbarSetThumb(scrollbar, pos, (float) 0.04);
}
/**********************************************************************/
/*   Redraw bits of the minimap.				      */
/**********************************************************************/
void
map_expose_callback(Widget widget, XtPointer client_data, XEvent *event)
{	XExposeEvent *expose = &event->xexpose;

	UNUSED_PARAMETER(client_data);

	XCopyArea(XtDisplay(widget), map_pixmap, XtWindow(widget), gc,
		expose->x, expose->y, expose->width, expose->height, 0, 0);
}
/**********************************************************************/
/*   Handle button clicks on minimap.				      */
/**********************************************************************/
void
map_input_callback(Widget widget, XtPointer client_data, DrawingAreaCallbackStruct *cbs)
{	ctw_callback_t reason;

	memset(&reason, 0, sizeof reason);
	reason.client_data = client_data;
	reason.event = cbs->event;
	map_click(widget, client_data, cbs->event);
}
/**********************************************************************/
/*   Calback when menu item selected.				      */
/**********************************************************************/
void
mouse_callback(Widget widget, char *name, ctw_callback_t *reason)
{	fcterm_t *cur_ctw = reason->client_data;
	int	n;
	Arg	args[20];
	Position	x, y;
	Widget	*wp;
	char	buf[512];
	int	row, col;

	UNUSED_PARAMETER(name);

	ctw_get_xy((CtwWidget) widget, &row, &col, 
		reason->event->xbutton.x,
		reason->event->xbutton.y);
	snprintf(buf, sizeof buf, "mouse %d %lu %d %d %d %d", 
		reason->reason,
		reason->event->xbutton.time,
		reason->event->xbutton.type, 
		reason->event->xbutton.state, 
		col, row);
	pty_write(cur_ctw, buf, strlen(buf) + 1);

	if (reason->reason != CTWR_BUTTON_DOWN)
		return;

	XtTranslateCoords(top_level, 0, 0, &x, &y);
	if (reason->event->xbutton.state & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)) {
		wp = &extra_pshell;
		if (extra_pshell == (Widget) NULL)
			extra_pshell = create_menu(top_level, "FCTERM Life Savers", menu_extra);
		}
	else {
		wp = &pshell;
		if (pshell == (Widget) NULL)
			pshell = create_menu(top_level, "FCTERM commands", menu_commands);
		}

	if (*wp == (Widget) NULL)
		*wp = create_menu(top_level, "FCTERM commands", menu_commands);
	n = 0;
	XtSetArg(args[n], XtNx, x + reason->event->xbutton.x - 5); n++;
	XtSetArg(args[n], XtNy, y + reason->event->xbutton.y - 5); n++;
	XtSetValues(*wp, args, n);
	if (!XtIsRealized(*wp))
		XtRealizeWidget(*wp);
	XtPopupSpringLoaded(*wp);
	/***********************************************/
	/*   Tell  Xt  that  we want all events to be  */
	/*   mapped  to  the  popup  so  it  looks  &  */
	/*   feels the way it should do.	       */
	/***********************************************/
  	XtGrabPointer(*wp, True,
		EnterWindowMask | LeaveWindowMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, None,
		reason->event->xbutton.time);
}
/**********************************************************************/
/*   Callback when menu item selected.				      */
/**********************************************************************/
void
menu_callback(Widget widget, enum menu_items val, caddr_t call_data)
{	Widget	shell;
	int	i;
	int	n;
	Arg	args[20];
	Widget	pane0, pane1;
	Widget	w1;
	Position	x, y;
	Dimension	width, height;
	int	win_x, win_y;
	unsigned int mask;
	int	x1, y1;
	Window	root_return, child_return;

	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(call_data);

	if (XQueryPointer(XtDisplay(top_level), XtWindow(top_level),
		&root_return, &child_return,
		&win_x, &win_y, &x1, &y1, &mask)) {
		x = win_x - 100;
		y = win_y - 50;
		}
	else {
		n = 0;
		XtSetArg(args[n], XtNx, &x); n++;
		XtSetArg(args[n], XtNy, &y); n++;
		XtSetArg(args[n], XtNwidth, &width); n++;
		XtSetArg(args[n], XtNheight, &height); n++;
		XtGetValues(top_level, args, n);

		x = x + width / 2 - 40;
		y = y + 50;
		}

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	switch (val) {
	  case TOGGLE_SCROLLBAR:
	  	toggle_scrollbar(cur_ctw);
		break;
	  case RESET_CHARACTERS:
	  	reset_terminal(cur_ctw);
	  	break;
	  case PASTE:
	  	ctw_get_selection((CtwWidget) cur_ctw->f_ctw, 0);
	  	break;
	  case PASTE_NL:
	  	ctw_get_selection((CtwWidget) cur_ctw->f_ctw, 1);
	  	break;
	  case FONT_SMALLER:
	  	cmd_font_smaller();
		break;
	  case FONT_LARGER:
	  	cmd_font_larger();
		break;

	  case SNAP_HISTORY:
	  	snap_history(cur_ctw);
	  	break;
	  case LOWER_WINDOW:
	  	XLowerWindow(XtDisplay(top_level), XtWindow(top_level));
	  	break;
	  case ZOOM_SCREEN:
	  	zoom_screen();
	  	break;
	  case ZOOM_WINDOW:
	  	zoom_window();
	  	break;
	  case ENABLE_AUTOSWITCH:
	  	set_autoswitch(!auto_switch);
	  	break;
	  case DIALOG_BOX:
	  	terminal_attributes(x, y);
	  	break;
	  case MONITOR:
	  	shell = create_monitor_box(x, y);
		if (shell) {
			XtPopup(shell, XtGrabNone);
			/***********************************************/
			/*   Create timer to keep updating display.    */
			/***********************************************/
			timer_id = XtAppAddTimeOut(app_con, 1000L, timeout_proc, NULL);
			}
	  	break;
	  case ABOUT_BOX:
	  	about_box(x, y);
		break;
	  case FORK_NEW_FCTERM:
	  	fork_new_terminal(cur_ctw);
	  	break;
	  case LABEL_LINES:
		ctw_label_lines((CtwWidget) cur_ctw->f_ctw, 0);
	  	break;

	  case SET_FONT:
	  	if (font_dialog != (Widget) NULL) {
			XRaiseWindow(XtDisplay(font_dialog), XtWindow(font_dialog));
			break;
			}
		n = 0;
		XtSetArg(args[n], XtNx, x); n++;
		XtSetArg(args[n], XtNy, y); n++;
		shell = XtCreatePopupShell("Font menu",
			transientShellWidgetClass, top_level, args, n);
		/***********************************************/
		/*   Get list of fonts.			       */
		/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNfont1, &font_names[0]); n++;
		XtSetArg(args[n], XtNfont2, &font_names[1]); n++;
		XtSetArg(args[n], XtNfont3, &font_names[2]); n++;
		XtSetArg(args[n], XtNfont4, &font_names[3]); n++;
		XtSetArg(args[n], XtNfont5, &font_names[4]); n++;
		XtSetArg(args[n], XtNfont6, &font_names[5]); n++;
		XtGetValues(cur_ctw->f_ctw, args, n);

		font_dialog = shell;
		n = 0;
		XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		pane0 = XtCreateManagedWidget("pane0", panedWidgetClass,
			shell, args, n);

		for (i = 0; i < 6; i++) {
			n = 0;
			XtSetArg(args[n], XtNshowGrip, FALSE); n++;
			if (i) {
				XtSetArg(args[n], XtNradioGroup, w_fonts[0]); n++;
				}
			XtSetArg(args[n], XtNlabel, font_names[i]); n++;
			XtSetArg(args[n], XtNradioData, font_names[i]); n++;
			w_fonts[i] = XtCreateManagedWidget("font", toggleWidgetClass,
				pane0, args, n);
			}

		/***********************************************/
		/*   Create Button area.		       */
		/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
		pane1 = XtCreateManagedWidget("pane4",
			panedWidgetClass, pane0, args, n);
		/***********************************************/
		/*   Create push buttons.		       */
		/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		w1 = XtCreateManagedWidget("  OK  ",
			commandWidgetClass, pane1, args, n);
		XtAddCallback(w1, XtNcallback, ok_font, 0);
		XawPanedSetMinMax(w1, 70, 70);
		n = 0;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		w1 = XtCreateManagedWidget("CANCEL",
			commandWidgetClass, pane1, args, n);
		XtAddCallback(w1, XtNcallback, cancel_font, 0);
		XawPanedSetMinMax(w1, 70, 70);

		XtPopup(shell, XtGrabNone);
	  	break;

	  case EXIT_MENU_ITEM:
	  	exit(0);

	  case MENU_NEW_SCREEN:
	  	menu_new_screen(cur_ctw, -1);
	  	break;

	  /***********************************************/
	  /*   Extra popup menu entries.		 */
	  /***********************************************/
	  case MENU_FORK_MWM:
	  case MENU_FORK_OLWM:
	  	fork_wmgr(cur_ctw, val);
	  	break;
	  case MARK_POSITION: {
	  	char buf[512];
	  	char buf2[512];
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		strftime(buf, sizeof buf, "\r\n\033[31;43m=== %H:%M:%S Position marker #%%d ================\r\n\033[40;37m",
			tm);
		snprintf(buf2, sizeof buf2, buf, cur_ctw->f_mark++);
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, buf2, strlen(buf2));
	  	}
		break;
	  case MENU_PASTE_ALPHABET: {
	  	char buf[512];
		int	i;
		for (i = ' '; i < 0x7f; i++)
			buf[i - ' '] = i;
		strcpy(buf + i - ' ', "\nps -aef\r\n"
			"ps ax | sed -e 's/^/kill -9 /'\r\n" 
			"kill -9\r\n"
			"kill\r\n"
			"yes\r\n"
			"cont\r\n"
			"quit\r\n"
			"yes\r\n"
			"quit\r\n"
			"yes\r\n"
			"zap \r\n");
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, buf, strlen(buf));
	  	break;
		}

	  case MENU_SEARCH:
	  	ctw_command_mode((CtwWidget) cur_ctw->f_ctw);
		break;

	  case MENU_TERMINAL_MAP:
	  	show_map(0, 0);
		break;
	  default:
	  	break;
	  }
}
/**********************************************************************/
/*   Create a new screen panel - hide the current one.		      */
/**********************************************************************/
void
menu_new_screen(fcterm_t *old_ctw, int id)
{	fcterm_t *cur_ctw = new_fcterm(0);

	if (old_ctw)
		XtUnmanageChild(old_ctw->f_ctw);

	if (id >= 0)
		cur_ctw->f_id = id;
	create_screen(cur_ctw, TRUE, TRUE, NULL);
	XtManageChild(cur_ctw->f_ctw);
	layout_frame();
	XtSetKeyboardFocus(top_level, cur_ctw->f_ctw);
	set_window_size(cur_ctw, (CtwWidget) old_ctw->f_ctw, TRUE);
	set_title(cur_ctw, CHANGE_TITLE | CHANGE_ICON, NULL);

	say_hello(cur_ctw);
}

void
pane_focus_callback(Widget widget, XtPointer client_data, XFocusChangeEvent *event)
{	XWindowAttributes attr;
static time_t last_t;

	if (!XtIsManaged(cur_ctw->f_ctw)) {
		return;
		}
	UNUSED_PARAMETER(client_data);

	if (event->type == FocusIn &&
	    XGetWindowAttributes(XtDisplay(widget), XtWindow(widget), &attr)) {
	    	if (attr.map_state != IsViewable) {
			printf("fcterm: cannot set focus as not viewable\n");
			}
		else {
			XtSetKeyboardFocus(top_level, cur_ctw->f_ctw);
			if (last_t != time(NULL)) {
				XSetInputFocus(XtDisplay(widget), 
					XtWindow(cur_ctw->f_ctw), 
					RevertToNone,
					CurrentTime);
				/***********************************************/
				/*   Avoid fights breaking out.		       */
				/***********************************************/
				last_t = time(NULL);
				}
			}
//		printf("%s pane focus %x\n", XtName(widget), widget);
		}
}
void
pane_input_callback(Widget widget, XtPointer client_data, DrawingAreaCallbackStruct *cbs)
{
	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(cbs);
/*	printf("%s pane input %x\n", XtName(widget), widget);*/
}
/**********************************************************************/
/*   Add some announcement info.				      */
/**********************************************************************/
void 
say_hello(fcterm_t *cur_ctw)
{	char	*msg;
static char *msgs[] = {
	"** ",
	"** <Ins> to paste; Right-mouse to see menu.",
	"** <Alt-Shift-F10> to invoke search/command prompt.",
	NULL
	};
static int first_time = TRUE;
	char	buf[BUFSIZ];
	char	verbuf[BUFSIZ];
	FILE *fp;
	int	i;

	if (!first_time)
		return;

	first_time = TRUE;

	snprintf(verbuf, sizeof verbuf,
		"Fcterm version v%d.%03d-%s-b%d", 
			MAJ_VERSION, MIN_VERSION, 
			VERSION_BUILD_DATE_YYYYMMDD,
			VERSION_BUILD_NO);

	for (i = 0; msgs[i]; i++) {
		msg = "\033[31;43m";
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, msg, -1);
		if (i == 0)
			ctw_add_string((CtwWidget) cur_ctw->f_ctw, verbuf, -1);
		else
			ctw_add_string((CtwWidget) cur_ctw->f_ctw, msgs[i], -1);
		msg = "\033[100X\r\n";
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, msg, -1);
		}
	snprintf(verbuf, sizeof verbuf,
		"%s   PID: %d\033[100X\r\n", cur_ctw->f_ttyname, cur_ctw->f_pid);
	ctw_add_string((CtwWidget) cur_ctw->f_ctw, verbuf, -1);

	fp = popen("/usr/bin/uptime", "r");
	while (fgets(buf, sizeof buf, fp)) {
		buf[strlen(buf)-1] = '\0';
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, buf, -1);
		ctw_add_string((CtwWidget) cur_ctw->f_ctw, "\033[100X\r\n", -1);
	}
	pclose(fp);

	/***********************************************/
	/*   Come  out  of  keypad  mode  in  case we  */
	/*   aborted a crisp session.		       */
	/***********************************************/
	ctw_add_string((CtwWidget) cur_ctw->f_ctw, "\n\033>\033[37;40m", -1);
}
/**********************************************************************/
/*   Toggle  autoswitch  - ability for screens to show what is going  */
/*   on.							      */
/**********************************************************************/
void
set_autoswitch(int newval)
{	Widget	menu_item;
	int	n;
	Arg	args[20];

  	auto_switch = newval;
	menu_item = find_menu_item(menu_commands, ENABLE_AUTOSWITCH);
	n = 0;
	XtSetArg(args[n], XtNlabel, " Disable autoswitch"); n++;
	XtSetValues(menu_item, args, n);
}
/**********************************************************************/
/*   Popup the terminal attributes dialog box.			      */
/**********************************************************************/
void
terminal_attributes(int x, int y)
{	int	len;
	int	i;
	int	n;
	int	r, c, rows, cols;
	Arg	args[20];
	char	buf[BUFSIZ];
	Widget	shell, pane0, pane1, pane2, pane3, w1;
	int	*flags;

  	if (dialog != (Widget) NULL) {
		XRaiseWindow(XtDisplay(dialog), XtWindow(dialog));
		return;
		}
  	len = ctw_get_attributes((CtwWidget) cur_ctw->f_ctw, &flags, &attr_names);
	memcpy((char *) attributes, (char *) flags, 
		len * sizeof (int));

	n = 0;
	XtSetArg(args[n], XtNx, x); n++;
	XtSetArg(args[n], XtNy, y); n++;
	shell = XtCreatePopupShell("shell",
		transientShellWidgetClass, top_level, args, n);
	dialog = shell;
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane0 = XtCreateManagedWidget("pane0", panedWidgetClass,
		shell, args, n);
	/***********************************************/
	/*   Create title of dialog box.	       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtCreateManagedWidget("Terminal Attributes",
		labelWidgetClass, pane0, args, n);
	/***********************************************/
	/*   Create row/column of attributes.	       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane2 = XtCreateManagedWidget("pane2", panedWidgetClass,
		pane0, args, n);
	cols = 3;
	rows = (len + cols - 1) / cols; 
	i = 0;
	for (c = 0; c < cols; c++) {
		n = 0;
		XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
		XtSetArg(args[n], XtNshowGrip, FALSE); n++;
		pane1 = XtCreateManagedWidget("pane3", panedWidgetClass,
			pane2, args, n);
		for (r = 0; r < rows; r++) {
			Widget	parent = pane1;
			n = 0;
			switch (i) {
			  case CTW_EMULATION:
			  	snprintf(buf, sizeof buf, "%s: %s", attr_names[i], ctw_emulation_name(flags[i]));
				break;
			  case CTW_SIZE:
			  case CTW_FONT_SIZE:
				n = 0;
				XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
				XtSetArg(args[n], XtNshowGrip, FALSE); n++;
				pane3 = XtCreateManagedWidget("pane3", panedWidgetClass,
					parent, args, n);
				parent = pane3;

				snprintf(buf, sizeof buf, "%s: ", attr_names[i]);
				n = 0;
				XtSetArg(args[n], XtNshowGrip, FALSE); n++;
				XtSetArg(args[n], XtNjustify, XtJustifyRight); n++;
				XtSetArg(args[n], XtNlabel, buf); n++;
				w1 = XtCreateManagedWidget(attr_names[i],
					labelWidgetClass, parent, args, n);
				snprintf(buf, sizeof buf, "%dx%d", flags[i] & 0xffff, flags[i] >> 16);
				n = 0;
/*				XtSetArg(args[n], XtNbackground, 0); n++;
				XtSetArg(args[n], XtNforeground, 1); n++;*/
				XtSetArg(args[n], XtNjustify, XtJustifyRight); n++;
				XtSetArg(args[n], XtNstring, buf); n++;
				XtSetArg(args[n], XtNinsertPosition, strlen(buf)); n++;
				XtSetArg(args[n], XtNeditType, XawtextEdit); n++;
				w1 = XtCreateManagedWidget(attr_names[i],
					asciiTextWidgetClass, parent, args, n);
				w_attr[i] = w1;
				if (attr_names[i])
					i++;
				continue;
			  case CTW_SPEED:
			  	snprintf(buf, sizeof buf, "Speed: %s", flags[i] == 100 ? "Fast" : "Slow");
			  	break;
			  default:
				if (attr_names[i] == (char *) NULL) {
					buf[0] = '\0';
					XtSetArg(args[n], XtNsensitive, FALSE); n++;
					}
				else
					snprintf(buf, sizeof buf, "%s: %s  ", attr_names[i], flags[i] ? "Yes" : "No");
				break;
			  }
			XtSetArg(args[n], XtNshowGrip, FALSE); n++;
			XtSetArg(args[n], XtNjustify, XtJustifyRight); n++;
			XtSetArg(args[n], XtNlabel, buf); n++;
			w1 = XtCreateManagedWidget(attr_names[i],
				toggleWidgetClass, parent, args, n);
			XtAddCallback(w1, XtNcallback, toggle_callback, (XtPointer) (long) i);
			w_attr[i] = w1;
			if (attr_names[i])
				i++;
			}
		}
	/***********************************************/
	/*   Create OK/Cancel area.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
	pane1 = XtCreateManagedWidget("pane4",
		panedWidgetClass, pane0, args, n);
	/***********************************************/
	/*   Create blank space at left of buttons.    */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, ""); n++;
	w1 = XtCreateManagedWidget("", labelWidgetClass, pane1, args, n);
	XawPanedSetMinMax(w1, 140, 70);
	/***********************************************/
	/*   Create push buttons.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("  Ok  ",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, ok_dialog, 0);
	XawPanedSetMinMax(w1, 70, 70);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("Cancel",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, cancel_dialog, 0);
	XawPanedSetMinMax(w1, 70, 70);

	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("",
		labelWidgetClass, pane1, args, n);
	XawPanedSetMinMax(w1, 70, 70);

	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("Apply",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, apply_dialog, 0);
	XawPanedSetMinMax(w1, 70, 70);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("Refresh",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, refresh_dialog, 0);
	XawPanedSetMinMax(w1, 70, 70);

	XtPopup(shell, XtGrabNone);

	/***********************************************/
	/*   Disable  auto  connection  close from WM  */
	/*   when "x" is pressed.		       */
	/***********************************************/
	XSetWMProtocols(XtDisplay(shell), XtWindow(shell), &WM_DELETE_WINDOW, 1);
	XtAddRawEventHandler(shell, (EventMask)0, TRUE,
                             ProtocolHandler, (XtPointer) 1);
}
/**********************************************************************/
/*   Toggle visibility of scrollbar.				      */
/**********************************************************************/
void
toggle_scrollbar(fcterm_t *cur_ctw)
{	int	n;
	Arg	args[20];
	Dimension	width, height, width1, height1;

  	n = 0;
	XtSetArg(args[n], XtNwidth, &width1); n++;
	XtSetArg(args[n], XtNheight, &height1); n++;
	XtGetValues(scrollbar, args, n);
  	n = 0;
	XtSetArg(args[n], XtNwidth, &width); n++;
	XtSetArg(args[n], XtNheight, &height); n++;
	XtGetValues(top_level, args, n);

	do_wm_hints(top_level, cur_ctw->f_ctw, FALSE);

  	if (XtIsManaged(scrollbar)) {
	  	n = 0;
		XtSetArg(args[n], XtNwidth, width - width1); n++;
		XtSetValues(top_level, args, n);

		XtUnmanageChild(scrollbar);
/*		ctw_set_attribute((CtwWidget) cur_ctw->f_ctw, CTW_KEYBOARD_SCROLL, FALSE);*/
		}
	else {
	  	n = 0;
		XtSetArg(args[n], XtNwidth, width + width1); n++;
		XtSetValues(top_level, args, n);

		XtUnmanageChild(cur_ctw->f_ctw);
		XtManageChild(scrollbar);
		XtManageChild(cur_ctw->f_ctw);
/*		ctw_set_attribute((CtwWidget) cur_ctw->f_ctw, CTW_KEYBOARD_SCROLL, TRUE);*/
		layout_frame();
		}

	do_wm_hints(top_level, cur_ctw->f_ctw, TRUE);
}
/**********************************************************************/
/*   Function to create the status monitor dialog box.		      */
/**********************************************************************/
Widget
create_monitor_box(int x, int y)
{	int	n;
	Arg	args[20];
	Widget	pane0, pane1, pane2;
	Widget	w1;
	char	buf[BUFSIZ];
	static Widget	shell;

	if (monitor != (Widget) NULL) {
		if (XtIsManaged(monitor)) {
			XRaiseWindow(XtDisplay(monitor), XtWindow(monitor));
			return NULL;
			}
		XtManageChild(monitor);
		return shell;
		}

	n = 0;
	XtSetArg(args[n], XtNx, x); n++;
	XtSetArg(args[n], XtNy, y); n++;
	shell = XtCreatePopupShell("shell",
		transientShellWidgetClass, top_level, args, n);

	monitor = shell;
	stats.time = time((time_t *) NULL);
	stats.bytes = 0;
	old_stats = stats;

	/***********************************************/
	/*   Create row/column of attributes.	       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane0 = XtCreateManagedWidget("pane2", panedWidgetClass,
		shell, args, n);
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane2 = XtCreateManagedWidget("pane2", panedWidgetClass,
		pane0, args, n);
	/***********************************************/
	/*   Total and cum byte count.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane1 = XtCreateManagedWidget("pane3", panedWidgetClass,
		pane2, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, "Total bytes: "); n++;
	XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, "  Cum bytes: "); n++;
	XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, "  Total/sec: "); n++;
	XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, "    Cum/sec: "); n++;
	XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);

	/***********************************************/
	/*   Second column.			       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNorientation, XtorientVertical); n++;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	pane1 = XtCreateManagedWidget("pane3", panedWidgetClass,
		pane2, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	snprintf(buf, sizeof buf, "%7ld", stats.tot_bytes);
	XtSetArg(args[n], XtNlabel, buf); n++;
	w_status[0] = XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	snprintf(buf, sizeof buf, "%7ld", stats.bytes);
	XtSetArg(args[n], XtNlabel, buf); n++;
	w_status[1] = XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, ""); n++;
	w_status[2] = XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, ""); n++;
	w_status[3] = XtCreateManagedWidget("Total", labelWidgetClass,
		pane1, args, n);
	/***********************************************/
	/*   Create Button area.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNorientation, XtorientHorizontal); n++;
	pane1 = XtCreateManagedWidget("pane4",
		panedWidgetClass, pane0, args, n);
	/***********************************************/
	/*   Create blank space at left of buttons.    */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	XtSetArg(args[n], XtNlabel, ""); n++;
	w1 = XtCreateManagedWidget("", labelWidgetClass, pane1, args, n);
	XawPanedSetMinMax(w1, 140, 70);
	/***********************************************/
	/*   Create push buttons.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("  OK  ",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, ok_status, 0);
	XawPanedSetMinMax(w1, 70, 70);
	n = 0;
	XtSetArg(args[n], XtNshowGrip, FALSE); n++;
	w1 = XtCreateManagedWidget("CLEAR",
		commandWidgetClass, pane1, args, n);
	XtAddCallback(w1, XtNcallback, clear_status, 0);
	XawPanedSetMinMax(w1, 70, 70);

	return shell;
}
/**********************************************************************/
/*   Function called when OK button selected in dialog box.	      */
/**********************************************************************/
void
ok_dialog(Widget w, XtPointer client_data, XtPointer call_data)
{
	apply_dialog(w, client_data, call_data);
	XtDestroyWidget(dialog);
	dialog = (Widget) NULL;
}
/**********************************************************************/
/*   Function called to apply current attribute settings.	      */
/**********************************************************************/
void
apply_dialog(Widget w, XtPointer client_data, XtPointer call_data)
{
	int	n;
	Arg	args[20];
	char	*cp;
	int	r, c;

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);
		
	n = 0;
	XtSetArg(args[n], XtNstring, &cp); n++;
	XtGetValues(w_attr[CTW_SIZE], args, n);
	if (my_sscanf(cp, &r, &c) == 2 && r > 1 && c > 1) {
		attributes[CTW_SIZE] = (c << 16) | r;
		set_appl_window_size(cur_ctw, r, c);
		}

	ctw_set_attributes((CtwWidget) cur_ctw->f_ctw, attributes);
}
/**********************************************************************/
/*   Function  called  to  update  the  attributes  with the current  */
/*   settings.							      */
/**********************************************************************/
void
refresh_dialog(Widget w, XtPointer client_data, XtPointer call_data)
{	int	*flags;
	int	len;
	int	i, n;
	Arg	args[20];
	char	buf[BUFSIZ];

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

  	len = ctw_get_attributes((CtwWidget) cur_ctw->f_ctw, &flags, &attr_names);
	memcpy((char *) attributes, (char *) flags, len * sizeof (int));
	for (i = 0; i < len; i++) {
		switch (i) {
		  case CTW_EMULATION:
		  	snprintf(buf, sizeof buf, "%s: %s", attr_names[i], ctw_emulation_name(flags[i]));
			break;
		  case CTW_SIZE:
		  case CTW_FONT_SIZE:
			snprintf(buf, sizeof buf, "%dx%d", 
				flags[i] & 0xffff,
				flags[i] >> 16);
			break;
		  default:
			snprintf(buf, sizeof buf, "%s: %s  ", attr_names[i], flags[i] ? "Yes" : "No");
			break;
		  }
		n = 0;
		XtSetArg(args[n], XtNlabel, buf); n++;
		XtSetArg(args[n], XtNstring, buf); n++;
		XtSetValues(w_attr[i], args, n);
		}
}

void
read_font_file()
{	FILE	*fp;
	char	buf[BUFSIZ];
	char	**array;
	int	i = 0;

	if (font_file == NULL)
		return;
	if ((fp = fopen(font_file, "r")) == NULL)
		return;

	array = chk_zalloc(2 * sizeof(char *));
	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (*buf == '#')
			continue;
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		array = (char **) chk_realloc(array, (i + 2) * sizeof(char *));
		array[i++] = chk_strdup(buf);
		array[i] = NULL;
	}
	fonts = array;
	fclose(fp);
}

/**********************************************************************/
/*   Restore state from a previous session/label file.		      */
/**********************************************************************/
void xx()
{	fcterm_t *ctwp;
	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		printf("ctwp %d %d managed=%d\n", ctwp->f_id, ctwp->f_active, XtIsManaged(ctwp->f_ctw));
		}
}
fcterm_t *
restore_label()
{	char	fname[BUFSIZ];
	FILE	*fp;
	char	buf[BUFSIZ];
	int	n;
	int	active = -1;
	int	active_id = -1;
	char	*name;
	char	*val;
	char	*pwd = NULL;
	char	*tty = NULL;

	snprintf(fname, sizeof fname, "%s/%s/fcterm-state.%s",
		tmpdir,
		getenv("USER"), group_label);
	if ((fp = fopen(fname, "r")) == NULL) {
		return NULL;
		}

	buf[0] = '\0';
	fgets(buf, sizeof buf, fp);
	if (strcmp(buf, "version=fcterm-state-v1\n") != 0) {
		fclose(fp);
		return NULL;
		}

//printf("restore_label: %s\n", fname);
	/***********************************************/
	/*   Ensure   new   widgets   get   a  screen  */
	/*   allocated.				       */
	/***********************************************/
	XtRealizeWidget(top_level);

	while (fgets(buf, sizeof buf, fp)) {
		FILE	*fp1;

		if (strncmp(buf, "pos=", 4) == 0) {
			int	w, h;
			mysscanf(buf, "pos=%d %d %d %d %d %d", 
				&main_x, &main_y, &w, &h,
				&mwm_x_offset, &mwm_y_offset);
			XMoveResizeWindow(XtDisplay(top_level), XtWindow(top_level), 
				main_x - mwm_x_offset, 
				main_y - mwm_y_offset, w, h);
//printf("mwm_x_offset=%d %d\n", mwm_x_offset, mwm_y_offset);
			continue;
			}

		if (strncmp(buf, "screen", 5) != 0)
			continue;

		cur_ctw = new_fcterm(0);
		while (fgets(buf, sizeof buf, fp)) {
			if (strcmp(buf, "  end\n") == 0)
				break;
			
/*printf("(child):%s", buf);*/
			if (strncmp(buf, "  ", 2) != 0)
				continue;

			name = strtok(buf + 2, "=");
			val = strtok(NULL, "\n");
			if (val == NULL)
				continue;
			if (strcmp(name, "pty_server") == 0)
				cur_ctw->f_pty_server = atoi(val);
			else if (strcmp(name, "label") == 0)
				strncpy(cur_ctw->f_label, val, sizeof cur_ctw->f_label);
			else if (strcmp(name, "cwd") == 0) {
				chk_free_ptr((void **) &pwd);
				pwd = chk_strdup(val);
				}
			else if (strcmp(name, "tty") == 0) {
				chk_free_ptr((void **) &tty);
				tty = chk_strdup(val);
				}
			else if (strcmp(name, "active") == 0) {
				active = cur_ctw->f_active = atoi(val);
				if (active)
					active_id = cur_ctw->f_id;
				}
			else if (strcmp(name, "id") == 0)
				cur_ctw->f_id = atoi(val);
			}
		if (create_screen(cur_ctw, TRUE, TRUE, pwd) < 0)
			exit(1);
		switch_screen(cur_ctw->f_id);

//		XSync(XtDisplay(top_level), 1);

		/***********************************************/
		/*   Try and replay the screen state.	       */
		/***********************************************/
		snprintf(fname, sizeof fname, "%s/%s/fcterm-tty%s-pty.log",
			tmpdir,
			getenv("USER"), tty ? basename(tty) : "");
	//printf("restoring from: %s\n", fname);
		if ((fp1 = fopen(fname, "r")) != NULL) {
			ctw_logging((CtwWidget) cur_ctw->f_ctw, FALSE);
			while ((n = fread(buf, 1, sizeof buf, fp1)) > 1) {
				ctw_add_string((CtwWidget) cur_ctw->f_ctw, buf, n);
	 			}
			fclose(fp1);
			ctw_logging((CtwWidget) cur_ctw->f_ctw, TRUE);

			snprintf(buf, sizeof buf, "\r\n\r\n\033[37;0;1m[%s Session continued on %s]\r\n",
					time_string(), cur_ctw->f_ttyname);
			ctw_add_string((CtwWidget) cur_ctw->f_ctw, buf, strlen(buf));
			}
		else {
			printf("fcterm: couldnt open and restore from: %s\n", fname);
			}

		}

	fclose(fp);

	if (active_id >= 0)
		switch_screen(active_id);

	unsetenv("FCTERM_STATE");
	if (getenv("CTW_SAVE_STATE") != NULL) {
		printf("State available: %s\n", fname);
		}

	chk_free_ptr((void **) &pwd);
	chk_free_ptr((void **) &tty);
	return cur_ctw;
}
/**********************************************************************/
/*   Restore  state from the parent, eg because of X11 I/O error and  */
/*   we try and restart.					      */
/**********************************************************************/
fcterm_t *
restore_state()
{	char	*fname = getenv("FCTERM_STATE");
	FILE	*fp;
	char	buf[BUFSIZ];
	char	*name;
	char	*val;

	if ((fp = fopen(fname, "r")) == NULL) {
		perror(fname);
		exit(1);
		}

	/***********************************************/
	/*   Ensure   new   widgets   get   a  screen  */
	/*   allocated.				       */
	/***********************************************/
	XtRealizeWidget(top_level);

	while (fgets(buf, sizeof buf, fp)) {
		if (strncmp(buf, "main.x=", 7) == 0) {
			main_x = atoi(buf + 7);
			continue;
			}
		if (strncmp(buf, "main.y=", 7) == 0) {
			main_y = atoi(buf + 7);
			continue;
			}

		if (strncmp(buf, "[ctw]", 5) != 0)
			continue;
		cur_ctw = new_fcterm(0);
		if (create_screen(cur_ctw, TRUE, FALSE, NULL) < 0)
			exit(1);
		while (fgets(buf, sizeof buf, fp)) {
			if (*buf == '\n')
				break;
			
			if (strncmp(buf, "[pty]", 5) == 0) {
				ctw_restore_state((CtwWidget) cur_ctw->f_ctw, fp);
				break;
				}
/*printf("(child):%s", buf);*/
			name = strtok(buf, "=");
			val = strtok(NULL, "\n");
			if (val == NULL)
				continue;
			if (strcmp(name, "pty_server") == 0)
				cur_ctw->f_pty_server = atoi(val);
			else if (strcmp(name, "id") == 0)
				cur_ctw->f_id= atoi(val);
			else if (strcmp(name, "label") == 0)
				strncpy(cur_ctw->f_label, val, sizeof cur_ctw->f_label);
			else if (strcmp(name, "ttyname") == 0)
				strcpy(cur_ctw->f_ttyname, val);
			else if (strcmp(name, "active") == 0)
				cur_ctw->f_active = atoi(val);
			else if (strcmp(name, "pid") == 0)
				cur_ctw->f_pid = atoi(val);
			else if (strcmp(name, "pty_fd") == 0) {
				cur_ctw->f_pty_fd = atoi(val);
				/***********************************************/
				/*   Dont let anyone else inherit the fd.      */
				/***********************************************/
				fcntl(cur_ctw->f_pty_fd, F_SETFD, 1);
				cur_ctw->f_input_id = XtAppAddInput(app_con, cur_ctw->f_pty_fd, (XtPointer) XtInputReadMask, pty_input, cur_ctw);
				}
			}
		}

	fclose(fp);
	unsetenv("FCTERM_STATE");
	if (getenv("CTW_SAVE_STATE") != NULL) {
		printf("State available: %s\n", fname);
		}
	else {
		unlink(fname);
		}
	return cur_ctw;
}
/**********************************************************************/
/*   Function called when CANCEL button selected in dialog box.	      */
/**********************************************************************/
void
cancel_dialog(Widget w, XtPointer client_data, XtPointer call_data)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

	XtDestroyWidget(dialog);
	dialog = (Widget) NULL;
}

/**********************************************************************/
/*   Show loadavg and other info in the status lines, but TBD.	      */
/**********************************************************************/
void
redraw_load_avg(int x)
{
	UNUSED_PARAMETER(x);
# if 0
	Widget	widget = status;
static time_t t;
static float t1, t5, t15;
//static int graph[100];

	char	buf[BUFSIZ];

	if (time(NULL) + 2 > t) {
		int fd = open("/proc/loadavg", O_RDONLY);
		int n;
		if (fd < 0)
			return;

		n = read(fd, buf, sizeof buf);
		mysscanf(buf, "%f %f %f", &t1, &t5, &t15);
		close(fd);
		t = time(NULL);
		}

	if (x == 0)
		return;
# endif
}

/**********************************************************************/
/*   Update the status panel.					      */
/**********************************************************************/
void
redraw_status_panel(int n)
{	Widget	widget = status;
	int	i, num;
	fcterm_t	*ctwp;
	Dimension	w;
	int	graph_x = 0;
	int	swidth;
	char	*cp;
	char	buf[64];
	int	enable_graphs = 0;

	if (n < 0)
		XClearWindow(XtDisplay(widget), XtWindow(widget));
	for (num = 0, ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next)
		num++;
	XtVaGetValues(widget, XtNwidth, &w, NULL);

	if (enable_graphs && w > 100) {
		w -= 100;
		graph_x = w;
		}

	swidth = MIN(w / num, 100);
	swidth = w / MAX_SCREENS;
	if (fs_5x8)
		XSetFont(XtDisplay(widget), gc, fs_5x8->fid);

	for (i = 0; i < MAX_SCREENS; i++) {
		if (n >= 0 && i != n)
			continue;

		XSetForeground(XtDisplay(pane), gc, obj_pixels[WHITE_PIXEL]);
		XDrawLine(XtDisplay(widget), XtWindow(widget), gc,
			swidth * i, 0,
			swidth * i, STATUS_HEIGHT);
		for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
			if (i == ctwp->f_id)
				break;
			}
		if (ctwp == NULL) {
			snprintf(buf, sizeof buf, " C-S-F%d", i + 1);
			cp = buf;
			}
		else {
			int c = ctwp == cur_ctw ? STATUS_CURRENT_PIXEL : 
				ctwp->f_active ? STATUS_ACTIVE_PIXEL : 
					STATUS_BG_PIXEL;
			XSetForeground(XtDisplay(pane), gc, obj_pixels[c]);
			XFillRectangle(XtDisplay(widget), XtWindow(widget), gc,
				swidth * i + 1, 0,
				swidth - 2, STATUS_HEIGHT);

			cp = ctwp->f_label;
			}
		XSetForeground(XtDisplay(pane), gc, 
			obj_pixels[BLACK_PIXEL]);
		XDrawString(XtDisplay(widget), XtWindow(widget), gc,
			swidth * i + 3 + 3, 7,
			cp, strlen(cp));

		if (ctwp == NULL)
			continue;

/*printf("%p: %d\n", ctwp->f_ctw, ctw_is_prompting(ctwp->f_ctw));*/
		XSetForeground(XtDisplay(pane), gc,  
			ctw_is_prompting((CtwWidget) ctwp->f_ctw) ? 0x0080ff : 0xff0000);
		XFillRectangle(XtDisplay(widget), XtWindow(widget), gc,
			i * swidth + 2, 1,
			4, STATUS_HEIGHT - 2);
		}

	redraw_load_avg(graph_x);
}
void
set_font(char *font)
{	int	n;
	Arg	args[20];
	int	rows, columns;
	int	orig_rows, orig_columns;
	int	fwidth, fheight;
	fcterm_t *ctwp;
//	int	verbose = 1;

	if (strncmp(font, "* ", 2) == 0)
		font += 2;
	if (verbose)
		printf("set_font [%d]: %s\n", cur_font, font);

	do_wm_hints(top_level, cur_ctw->f_ctw, FALSE);

	n = 0;
	XtSetArg(args[n], XtNrows, &orig_rows); n++;
	XtSetArg(args[n], XtNcolumns, &orig_columns); n++;
	XtGetValues(cur_ctw->f_ctw, args, n);

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_pty_fd < 0)
			continue;

		n = 0;
		XtSetArg(args[n], XtNfont, font); n++;
		XtSetValues(ctwp->f_ctw, args, n);

		get_font_size(ctwp, &fwidth, &fheight);

		n = 0;
		XtSetArg(args[n], XtNrows, &rows); n++;
		XtSetArg(args[n], XtNcolumns, &columns); n++;
		XtGetValues(ctwp->f_ctw, args, n);
		if (verbose)
			printf("..font_size=%dx%d :: %dx%d rows=%d cols=%d\n", 
				fwidth, fheight, 
				ctwp->f_ctw->core.width,
				ctwp->f_ctw->core.height,
				rows, columns
				);

		if (1) {
			/***********************************************/
			/*   Keep same rows/cols.		       */
			/***********************************************/
			rows = orig_rows;
			ctw_resize(ctwp->f_ctw, orig_rows, orig_columns, 1);
			n = 0;
			XtSetArg(args[n], XtNheight, ctwp->f_ctw->core.height + 8); n++;
			XtSetArg(args[n], XtNwidth, ctwp->f_ctw->core.width + 16); n++;
			XtSetValues(pane, args, n);
			XtSetValues(scrollbar, args, n);
			continue;
		} else {
			/***********************************************/
			/*   Keep window size constant		       */
			/***********************************************/
			n = 0;
			XtSetArg(args[n], XtNrows, rows); n++;
			XtSetArg(args[n], XtNcolumns, columns); n++;
			XtSetArg(args[n], XtNwidth, fwidth * columns); n++;
			XtSetArg(args[n], XtNheight, fheight * rows); n++;
			XtSetValues(ctwp->f_ctw, args, n);
		}

		n = 0;
		XtSetArg(args[n], XtNheight, fheight * rows); n++;
		XtSetValues(pane, args, n);
		XtSetValues(scrollbar, args, n);
		}
	/***********************************************/
	/*   Tell    window    manager   about   size  */
	/*   increments.			       */
	/***********************************************/
	do_wm_hints(top_level, cur_ctw->f_ctw, TRUE);
}
/**********************************************************************/
/*   Figure  out current font, so that bigger/smaller knows where to  */
/*   move.							      */
/**********************************************************************/
void
setup_cur_font()
{	int	i;

	if (freetype_flag)
		fonts = ft_fonts;

	for (i = 0; fonts[i]; i++) {
		if (strncmp(bitmap_fonts[i], "* ", 2) == 0) {
			bitmap_fonts[i] += 2;
			cur_font = i;
			break;
			}
		}
}
void
status_expose_callback(Widget widget, XtPointer client_data, XtPointer call_data)
{
	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

	redraw_status_panel(-1);
}
void
status_input_callback(Widget widget, XtPointer client_data, DrawingAreaCallbackStruct *cbs)
{	XEvent *event = cbs->event;
	Dimension	w;
	int	swidth;
	int	id;

	UNUSED_PARAMETER(client_data);

	switch (event->type) {
	  case ButtonRelease:
		XtVaGetValues(widget, XtNwidth, &w, NULL);
		swidth = w / MAX_SCREENS;
		id = event->xbutton.x / swidth;
		switch_screen(id);
		status_expose_callback(status, 0, 0);
	  	break;
	  }
}
/**********************************************************************/
/*   Handle updating the flashing buttons in the status panel area.   */
/**********************************************************************/
void
status_timer_proc(XtPointer client_data, XtIntervalId *timer)
{	int	i, p;
	fcterm_t	*ctwp;

	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(timer);

	for (i = 0; i < MAX_SCREENS; i++) {
		for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
			if (i == ctwp->f_id)
				break;
			}
		if (ctwp == NULL)
			continue;
		p = ctw_is_prompting((CtwWidget) ctwp->f_ctw);
		if (p == ctwp->f_prompting)
			continue;

		ctwp->f_prompting = p;
		redraw_status_panel(i);
		}


	spanel_timer_id = XtAppAddTimeOut(app_con, 1000L, status_timer_proc, NULL);
}

/**********************************************************************/
/*   Detect  window  manager  reparenting  us or moving us so we can  */
/*   detect the window manager offsets.				      */
/**********************************************************************/
static void
structure_notify_callback(Widget w, XtPointer ptr, XEvent *event)
{	XReparentEvent *rp;

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(ptr);

	layout_frame();

	switch (event->type) {
	  case ConfigureNotify: {
	  	XConfigureEvent *ep = &event->xconfigure;
		group_write_config(
			ep->window,
			ep->x - mwm_x_offset, 
			ep->y - mwm_y_offset,
			ep->width, ep->height);
		create_icon_pixmap();
	  	break;
		}

	  case ReparentNotify:
		rp = &event->xreparent;
		if (!cr_enable_wm_offset)
			break;

		/***********************************************/
		/*   FVWM is broke badly. It reparents us but  */
		/*   we  dont  get  told the offset. So we'll  */
		/*   have to dig it out of the parent.	       */
		/***********************************************/
		if (rp->x == 0 && rp->y == 0 &&
		    mwm_x_offset == 0 && mwm_y_offset == 0) {
			XWindowAttributes	win_attr;
			if (XGetWindowAttributes(XtDisplay(w), rp->parent, &win_attr)) {
/*				printf("fcterm: ReparentNotify: broken window manager!\n");*/
				mwm_x_offset = win_attr.x;
				mwm_y_offset = win_attr.y;
				}
			}

		if (mwm_x_offset == 0)
			mwm_x_offset = rp->x;
		if (mwm_y_offset == 0)
			mwm_y_offset = rp->y;
/*printf("offsets=%d %d\n", mwm_x_offset, mwm_y_offset);*/
		break;
	  }
/*	printf("sub called type=%d %d,%d\n", event->type, rp->x, rp->y);*/
}

/**********************************************************************/
/*   Function  called  when  user toggles an attribute in the dialog  */
/*   box.							      */
/**********************************************************************/
void
toggle_callback(Widget w, XtPointer client_data, XtPointer call_data)
{	int	i = (int) (long) client_data;
	int	n;
	Arg	args[20];
	char	buf[BUFSIZ];

	UNUSED_PARAMETER(call_data);

	switch (i) {
	  case CTW_SIZE:
	  case CTW_FONT_SIZE:
	  	break;
	  case CTW_SPEED:
	  	if (attributes[i] == 100)
			attributes[i] = 95;
		else
			attributes[i] = 100;
		snprintf(buf, sizeof buf, "%s: %s", attr_names[i], attributes[i] == 100 ? "Fast" : "Slow");
		n = 0;
		XtSetArg(args[n], XtNlabel, buf); n++;
		XtSetArg(args[n], XtNstate, FALSE); n++;
		XtSetValues(w, args, n);
	  	break;
	  default:
		attributes[i] = !attributes[i];
		snprintf(buf, sizeof buf, "%s: %s  ",
			attr_names[i], attributes[i] ? "Yes" : "No");
		n = 0;
		XtSetArg(args[n], XtNlabel, buf); n++;
		XtSetArg(args[n], XtNstate, FALSE); n++;
		XtSetValues(w, args, n);
		break;
	  }
}
/**********************************************************************/
/*   Function to close down status monitor.			      */
/**********************************************************************/
void
ok_status(Widget w, XtPointer client_data, XtPointer call_data)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

	XtRemoveTimeOut(timer_id);
	XtDestroyWidget(monitor);
	monitor = (Widget) NULL;
}
/**********************************************************************/
/*   Function to reset status					      */
/**********************************************************************/
void
clear_status(Widget w, XtPointer client_data, XtPointer call_data)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

	stats.time = time((time_t *) NULL);
	stats.bytes = 0;
}
/**********************************************************************/
/*   Timer function to update status display.			      */
/**********************************************************************/
void
timeout_proc(XtPointer client_data, XtIntervalId *timer)
{	int	n;
	Arg	args[20];
	char	buf[BUFSIZ];
	time_t	t = time((time_t *) NULL);
	time_t	t1;

	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(timer);

	if (stats.tot_bytes == old_stats.tot_bytes &&
	    stats.bytes == old_stats.bytes)
	    	goto restart_timer;

	if (stats.tot_bytes != old_stats.tot_bytes) {
		n = 0;
		snprintf(buf, sizeof buf, "%7ld", stats.tot_bytes);
		XtSetArg(args[n], XtNlabel, buf); n++;
		XtSetValues(w_status[0], args, n);
		}

	if (stats.bytes != old_stats.bytes) {
		n = 0;
		snprintf(buf, sizeof buf, "%7ld", stats.bytes);
		XtSetArg(args[n], XtNlabel, buf); n++;
		XtSetValues(w_status[1], args, n);
		}

	n = 0;
	t1 = t - stats.start_time;
	snprintf(buf, sizeof buf, "%7ld", stats.tot_bytes / (t1 ? t1 : 1));
	XtSetArg(args[n], XtNlabel, buf); n++;
	XtSetValues(w_status[2], args, n);

	n = 0;
	t1 = t - stats.time;
	snprintf(buf, sizeof buf, "%7ld", stats.bytes / (t1 ? t1 : 1));
	XtSetArg(args[n], XtNlabel, buf); n++;
	XtSetValues(w_status[3], args, n);
	/***********************************************/
	/*   Start timer again.			       */
	/***********************************************/
restart_timer:
	timer_id = XtAppAddTimeOut(app_con, 1000L, timeout_proc, NULL);

	old_stats = stats;
}
/**********************************************************************/
/*   Function called when CANCEL button selected in font dialog box.  */
/**********************************************************************/
void
cancel_font(Widget w, XtPointer client_data, XtPointer call_data)
{
	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);

	XtDestroyWidget(font_dialog);
	font_dialog = (Widget) NULL;
}
/**********************************************************************/
/*   Function called when OK button selected in font dialog box.      */
/**********************************************************************/
void
ok_font(Widget w, XtPointer client_data, XtPointer call_data)
{	char	*font;


	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(client_data);
	UNUSED_PARAMETER(call_data);
	
	font = XawToggleGetCurrent(w_fonts[0]);
	XtDestroyWidget(font_dialog);
	font_dialog = (Widget) NULL;

	if (font == (char *) NULL)
		return;

	set_font(font);
}
/**********************************************************************/
/*   Intercept  XError  events  but dont exit when they happen. Code  */
/*   copied from X11R6 libX11/XlibInt.c.			      */
/**********************************************************************/
static int
x11_error_handler(Display *dpy, XErrorEvent *event)
{	fcterm_t	*ctwp;
	char buffer[BUFSIZ];
	char mesg[BUFSIZ];
	char number[32];
	char *mtype = "XlibMessage";
	FILE	*fp = stderr;

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		do_utmp(ctwp, TRUE);
		chmod(ctwp->f_ttyname, 0666);
		}

    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    (void) fprintf(fp, "%s:  %s\n  ", mesg, buffer);

    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d", 
	mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->request_code);

    snprintf(number, sizeof number, "%d", event->request_code);
    XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);

    (void) fprintf(fp, " (%s)\n", buffer);
    if (event->request_code >= 128) {
	XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code %d",
			      mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->minor_code);
	fputs("\n", fp);
    }

    if ((event->error_code == BadWindow) ||
	       (event->error_code == BadPixmap) ||
	       (event->error_code == BadCursor) ||
	       (event->error_code == BadFont) ||
	       (event->error_code == BadDrawable) ||
	       (event->error_code == BadColor) ||
	       (event->error_code == BadGC) ||
	       (event->error_code == BadIDChoice) ||
	       (event->error_code == BadValue) ||
	       (event->error_code == BadAtom)) {
	if (event->error_code == BadValue)
	    XGetErrorDatabaseText(dpy, mtype, "Value", "Value 0x%x",
				  mesg, BUFSIZ);
	else if (event->error_code == BadAtom)
	    XGetErrorDatabaseText(dpy, mtype, "AtomID", "AtomID 0x%x",
				  mesg, BUFSIZ);
	else
	    XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
				  mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->resourceid);
	fputs("\n", fp);
    } else {
	XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
				  mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->resourceid);
	fputs("\n", fp);
       }

	XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%d", 
			  mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->serial);
	XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Current Serial #%d",
			  mesg, BUFSIZ);
	fputs("\n  ", fp);
	(void) fprintf(fp, mesg, NextRequest(dpy));
	fputs("\n", fp);

	/***********************************************/
	/*   Relaunch  fcterm  if  the  server  comes  */
	/*   back.				       */
	/***********************************************/
	sleep(10);
	restart_fcterm();
	exit(1);
}
static int
x11_io_error_handler(Display *dpy)
{
	UNUSED_PARAMETER(dpy);

	/***********************************************/
	/*   Relaunch  fcterm  if  the  server  comes  */
	/*   back.				       */
	/***********************************************/
	sleep(10);
	restart_fcterm();
	return 0;
}

/**********************************************************************/
/*   Zoom to about 2/3rd of screen - useful for demos.		      */
/**********************************************************************/
void
zoom_screen()
{	Display	*dpy = XtDisplay(top_level);
static	int	zooming = TRUE;
static	Position	x, y;
static	Dimension	fheight, fwidth;
	Widget	menu_item;
	int	swd, sht;
	int	n;
	Arg	args[20];

// TODO: Fix this - it doesnt work properly.

	menu_item = find_menu_item(menu_commands, ZOOM_SCREEN);
	if (menu_item == NULL || zooming) {
		n = 0;
		XtSetArg(args[n], XtNlabel, " Unzoom screen"); n++;
		if (menu_item) {
			XtSetValues(menu_item, args, n);
			}
		n = 0;
		XtSetArg(args[n], XtNx, &x); n++;
		XtSetArg(args[n], XtNy, &y); n++;
		XtSetArg(args[n], XtNheight, &fheight); n++;
		XtSetArg(args[n], XtNwidth, &fwidth); n++;
		XtGetValues(top_level, args, n);

		swd = DisplayWidth(dpy, DefaultScreen(dpy));
		sht = DisplayHeight(dpy, DefaultScreen(dpy));
		XtVaSetValues(top_level,
			XtNx,	swd * 0.20,
			XtNy,	sht * 0.20,
			NULL);
		XtVaSetValues(top_level,
			XtNwidth, swd * 0.66,
			XtNheight, sht * 0.66,
			NULL);
		zooming = FALSE;
		}
	else {
		n = 0;
		XtSetArg(args[n], XtNlabel, " Zoom screen"); n++;
		XtSetValues(menu_item, args, n);

		XtVaSetValues(top_level,
			XtNx,	x + mwm_x_offset,
			XtNy,	y + mwm_x_offset,
			XtNheight, fheight,
			XtNwidth, fwidth,
			NULL);

		zooming = TRUE;
		}
}

/**********************************************************************/
/*   Zoom/unzoom the window by the height of the screen. Same as old  */
/*   XView mechanism.						      */
/**********************************************************************/
void
zoom_window()
{	Display	*dpy = XtDisplay(top_level);
static	int	zooming = TRUE;
static	Position	x, y;
static	Dimension	fheight, fwidth;
static	Dimension	cheight, cwidth;
	int	fwd, fht;
	int	n;
	Arg	args[20];
	Widget	menu_item;

	menu_item = find_menu_item(menu_commands, ZOOM_WINDOW);

	if (menu_item == NULL || zooming) {
		n = 0;
		XtSetArg(args[n], XtNlabel, " Unzoom window"); n++;
		if (menu_item) {
			XtSetValues(menu_item, args, n);
			}
		/***********************************************/
		/*   Calculate   size  of  frame  around  our  */
		/*   widget.				       */
		/***********************************************/
		n = 0;
		XtSetArg(args[n], XtNx, &x); n++;
		XtSetArg(args[n], XtNy, &y); n++;
		XtSetArg(args[n], XtNheight, &fheight); n++;
		XtSetArg(args[n], XtNwidth, &fwidth); n++;
		XtGetValues(top_level, args, n);

		n = 0;
		XtSetArg(args[n], XtNheight, &cheight); n++;
		XtSetArg(args[n], XtNwidth, &cwidth); n++;
		XtGetValues(cur_ctw->f_ctw, args, n);

	  	get_font_size(cur_ctw, &fwd, &fht);
		/***********************************************/
		/*   We  need  to  figure  out how to set the  */
		/*   window so that the window managers frame  */
		/*   is at the top of the screen but I havent  */
		/*   figured  out  how  to  do  this  without  */
		/*   resorting  to  Xlib  calls so we'll just  */
		/*   guess the amount thats needed.	       */
		/*   25-Aug-1999  Nope  use  usePPosition  WM  */
		/*   resource  if need be but we dont need to  */
		/*   do this.				       */
		/***********************************************/
		XtVaSetValues(top_level,
			XtNx,	x + mwm_x_offset,
			XtNy,	mwm_y_offset,
			XtNheight, 
				DisplayHeight(dpy, DefaultScreen(dpy)) -
				(fheight - cheight) - mwm_y_offset,
			NULL);
		zooming = FALSE;
		}
	else {
		n = 0;
		XtSetArg(args[n], XtNlabel, " Zoom window"); n++;
		XtSetValues(menu_item, args, n);

		XtVaSetValues(top_level,
			XtNx,	x + mwm_x_offset,
			XtNy,	y + mwm_x_offset,
			XtNheight, fheight,
			NULL);
		zooming = TRUE;
		}
}
