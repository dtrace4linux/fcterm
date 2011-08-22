/**********************************************************************/
/*   Color  terminal  emulator using the ctw (Color terminal widget)  */
/*   widget. This module is based on the Motif widgets.		      */
/*   								      */
/*   Author: Paul Fox						      */
/*   Date:   February 1992       				      */
/**********************************************************************/
# include	"fcterm.h"
# include	"motif.h"
# include	"scrbar.h"
# include	<Xm/ScrollBar.h>
# include	<Xm/RowColumn.h>
# include	<Xm/BulletinB.h>
# include	<Xm/SelectioB.h>
# include	<Xm/Frame.h>
# include	<Xm/Label.h>
# include	<Xm/Form.h>
# include	<Xm/ToggleB.h>

/**********************************************************************/
/*   Widgets created on startup.				      */
/**********************************************************************/
Widget		top_level;
Widget		main_window;
Widget		menu_bar;

/**********************************************************************/
/*   Menus for buttons on menu bar.				      */
/**********************************************************************/
struct menu file_tbl[] = {
	{0, "logging",	NULL,		" Log output "},
	{0, "snap",	NULL,		" Snap history "},
	{F_SEP, "",	NULL,		(char *) NULL},
	{0, "fork",	NULL,		" Fork new terminal "},
	{F_SEP, "",	NULL,		(char *) NULL},
	{0, "mwm",	NULL,		" Start mwm"},
	{0, "olwm",	NULL,		" Start olwm "},
	{F_SEP, "",	NULL,		(char *) NULL},
	{0, "about",	NULL,		" About FMCTERM "},
	{F_END}
	};

struct menu view_tbl[] = {
	{0, "scrollbar",	NULL,		" Scrollbar  "},
	{0, "attributes",	NULL,		" Attributes "},
	{0, "status",		NULL,		" Status "},
	{F_END}
	};

struct menu edit_tbl[] = {
	{0, "paste",		NULL,		" Paste"},
	{0, "alphabet",		NULL,		" Paste alphabet"},
	{F_END}
	};

struct menu options_tbl[] = {
	{0, "font",		NULL,		" Change font..."},
	{0, "reset",		NULL,		" Reset terminal "},
	{F_END}
	};


/* XXX */
static Widget		dialog;
static Widget	status_dialog;
static Widget	attr_dialog;
Widget		pshell;
Widget		pane;
Widget		scroll_bar;
Widget		ctw_widget;
Widget		w_attr[CTW_MAX_ATTR + 16];
Widget		w_status[4];
Widget		w_fonts[4];

XtIntervalId	timer_id;

char	*tmp_dir = "/tmp";
char	*font_names[6];

XtAppContext	app_con;
int	pid;
extern int fork_flag;

/**********************************************************************/
/*   Copy of attributes which user can change.			      */
/**********************************************************************/
int	attributes[64];
char	**attr_names;

char	*prog_name;

static String fallback_resources[] = {
	"*font:		7x13bold",
	"*fontList:		7x13bold",
	"*foreground:		yellow",
	"*background:		CadetBlue",

	"*menu*borderColor:	cyan",
	"*menu*borderWidth:	4",
	"*VT100.background: 	black",
	"*allowShellResize:	true",
	"*menuAccelerator: Ctrl Shift<Key>F10",
	(char *) NULL
	};

void	set_font PROTO((char *));
void	sbar_callback();
void	jump_callback();
void	scroll_callback();
void	attr_button_callback();
void	status_button_callback();
void	popup_attributes();
void	popup_status();
void	popup_font();
void	scrollbar_callback();
void	appl_callback();
void	input_callback();
void	top_callback();
void	menu_callback();
void	mouse_callback();
void	ok_dialog();
void	apply_dialog();
void	refresh_dialog();
void	toggle_callback();
void	timeout_proc();
void	font_ok_callback();

int
main(argc, argv)
int	argc;
char	**argv;
{	int	n;
	Arg	args[20];
	int	max_lines;
	char	*cp;

	if (getenv("TMP"))
		tmp_dir = getenv("TMP");

	setup_signal_handlers();

	argc = steal_switches(argc, argv);

	n = 0;
	XtSetArg(args[n], XtNinput, TRUE); n++;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
	cp = argv[0];
	if ((cp = strrchr(cp, '/')) != (char *) NULL)
		cp++;
	top_level = XtAppInitialize(&app_con, cp, NULL, 0,
		&argc, argv, fallback_resources, args, n);


	do_switches(argc, argv);

	/***********************************************/
	/*   Make  parent  detach  from  shell  so we  */
	/*   dont have lots of jobs hanging around.    */
	/***********************************************/
	if (fork_flag) {
		if (fork())
			exit(0);
# if defined(SYSV)
#	if defined(STREAMS_PTY)
		setsid();
#	else
		setpgrp();
#	endif
# else
		setpgrp(getpid(), 0);
# endif
		}

	main_window = XmCreateMainWindow(top_level, "main", NULL, 0);
	XtManageChild(main_window);
	menu_bar = XmCreateMenuBar(main_window, "menu_bar", NULL, 0);
	XtManageChild(menu_bar);

	/***********************************************/
	/*   Create scrollbar.			       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg(args[n], XmNwidth, 13); n++;
# if 0
	scroll_bar = XtCreateWidget("scrollbar", scrbarWidgetClass,
			main_window, args, n);
	XtAddCallback(scroll_bar, XtNsbarCallback, sbar_callback, cur_ctw);
	XtAddCallback(scroll_bar, XtNjumpProc, jump_callback, cur_ctw);
	XtAddCallback(scroll_bar, XtNscrollProc, scroll_callback, cur_ctw);
# else
	scroll_bar = XtCreateWidget("scrollbar", xmScrollBarWidgetClass,
		main_window, args, n);
	XtAddCallback(scroll_bar, XmNdecrementCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNincrementCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNpageDecrementCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNpageIncrementCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNtoTopCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNtoBottomCallback, scrollbar_callback, NULL);
	XtAddCallback(scroll_bar, XmNdragCallback, scrollbar_callback, NULL);
# endif
	/***********************************************/
	/*   Create VT100 widget.		       */
	/***********************************************/
	n = 0;
	ctw_widget = XtCreateManagedWidget("VT100",
		ctwWidgetClass,
		main_window, args, n);
	if (geometry) {
		int	w, h;
		int	fheight, fwidth;
		sscanf(geometry, "%dx%d", &w, &h);
		n = 0;
		XtSetArg(args[n], XtNcolumns, w); n++;
		XtSetArg(args[n], XtNrows, h); n++;
		get_font_size(cur_ctw, &fwidth, &fheight);
		XtSetArg(args[n], XtNwidth, fwidth * w); n++;
		XtSetArg(args[n], XtNheight, fheight * h); n++;
		XtSetValues(ctw_widget, args, n);

		n = 0;
		XtSetArg(args[n], XtNwidth, fwidth * w); n++;
		XtSetArg(args[n], XtNheight, fheight * h); n++;
		XtSetValues(scroll_bar, args, n);
		}
	XtAddCallback(ctw_widget, XtNapplCallback, appl_callback, 0);
	XtAddCallback(ctw_widget, XtNkbdCallback, input_callback, 0);
	XtAddCallback(ctw_widget, XtNresizeCallback, resize_callback, 0);
	XtAddCallback(ctw_widget, XtNtopCallback, top_callback, 0);
	XtAddCallback(ctw_widget, XtNmouseCallback, mouse_callback, 0);

	/***********************************************/
	/*   Find  out  what the maxLines resource in  */
	/*   the  VT100  widget  so  we  can  set the  */
	/*   scale of the scrollbar.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNsaveLines, &max_lines); n++;
	XtGetValues(ctw_widget, args, n);
	n = 0;
	XtSetArg(args[n], XmNmaximum, max_lines + 1); n++;
	XtSetValues(scroll_bar, args, n);

	create_menu_button("File", NULL, file_tbl, menu_bar);
	create_menu_button("View", NULL, view_tbl, menu_bar);
	create_menu_button("Edit", NULL, edit_tbl, menu_bar);
	create_menu_button("Options", NULL, options_tbl, menu_bar);

	/***********************************************/
	/*   Create sub-shell.			       */
	/***********************************************/
	spawn_child();
	XtAppAddInput(app_con, pty_fd, XtInputReadMask, pty_input, NULL);

	XtRealizeWidget(top_level);

	create_icon_pixmap();

	set_default_attributes(cur_ctw);

	do_wm_hints(top_level, ctw_widget, TRUE);

	stats.start_time = time((time_t *) NULL);

	XtSetKeyboardFocus(top_level, ctw_widget);

	XtAppMainLoop(app_con);
	exit(0);
	return 0;
}
/**********************************************************************/
/*   Function  to  handle  indication from VT100 widget that the top  */
/*   line  of  the window has moved so we can associate the thumb on  */
/*   the scrollbar with the position.				      */
/**********************************************************************/
void
top_callback(w, type, reason)
Widget	w;
int	type;
ctw_callback_t	*reason;
{
	int	n;
	Arg	args[20];

	n = 0;
	XtSetArg(args[0], XmNvalue, reason->top_line); n++;
	XtSetValues(scroll_bar, args, n);
}
/**********************************************************************/
/*   Handle a user moving the slider in the vertical scrollbar.	      */
/**********************************************************************/
void
scrollbar_callback(widget, type, call_data)
Widget	widget;
int	type;
XmScrollBarCallbackStruct *call_data;
{
	int	n;
	Arg	args[20];
	int	top_line;
	int	num_rows;

	n = 0;
	XtSetArg(args[n], XtNrows, &num_rows); n++;
	XtGetValues(ctw_widget, args, n);
	top_line = ctw_get_top_line((CtwWidget) ctw_widget);

	switch (call_data->reason) {
	  case XmCR_DECREMENT:
	  	top_line--;
	  	break;
	  case XmCR_INCREMENT:
	  	top_line++;
	  	break;
	  case XmCR_PAGE_DECREMENT:
	  	top_line -= num_rows;
	  	break;
	  case XmCR_PAGE_INCREMENT:
	  	top_line += num_rows;
	  	break;
	  case XmCR_TO_TOP:
	  	top_line = 0;
		break;
	  case XmCR_TO_BOTTOM:
	  	top_line = 32767;
		break;
	  case XmCR_DRAG:
	  	top_line = call_data->value;
	  	break;
	  }
	ctw_set_top_line((CtwWidget) ctw_widget, top_line);
}
/**********************************************************************/
/*   Callback when menu item selected.				      */
/**********************************************************************/
void
mouse_callback(widget, name, reason)
Widget	widget;
char	*name;
ctw_callback_t	*reason;
{	int	n;
	Arg	args[20];
	Dimension	x, y;

	if (reason->reason != CTW_BUTTON_DOWN)
		return;

	if (pshell == (Widget) NULL)
		return;

	n = 0;
	XtSetArg(args[n], XtNx, &x); n++;
	XtSetArg(args[n], XtNy, &y); n++;
	XtGetValues(top_level, args, n);

	n = 0;
	XtSetArg(args[n], XtNx, x + reason->event->xbutton.x - 5); n++;
	XtSetArg(args[n], XtNy, y + reason->event->xbutton.y - 5); n++;
	XtSetValues(pshell, args, n);
	if (!XtIsRealized(pshell))
		XtRealizeWidget(pshell);
	XtPopupSpringLoaded(pshell);
	/***********************************************/
	/*   Tell  Xt  that  we want all events to be  */
	/*   mapped  to  the  popup  so  it  looks  &  */
	/*   feels the way it should do.	       */
	/***********************************************/
  	XtGrabPointer(pshell, True,
		EnterWindowMask | LeaveWindowMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, None,
		reason->event->xbutton.time);
}
/**********************************************************************/
/*   Calback when menu item selected.				      */
/**********************************************************************/
void
menu_callback(widget, val, call_data)
Widget	widget;
enum menu_items val;
caddr_t	call_data;
{	int	ret;
	char	*name;
	char	*cp;

	if ((ret = find_menu_widget(file_tbl, widget)) >= 0) {
		name = file_tbl[ret].widget_name;
		if (strcmp(name, "snap") == 0) {
			snap_history(cur_ctw);
			return;
			}
		if (strcmp(name, "olwm") == 0) {
			fork_wmgr(cur_ctw, MENU_FORK_OLWM);
			return;
			}
		if (strcmp(name, "mwm") == 0) {
			fork_wmgr(cur_ctw, MENU_FORK_MWM);
			return;
			}
		if (strcmp(name, "fork") == 0) {
			fork_new_terminal(cur_ctw);
			return;
			}
	  	return;
		}
	if ((ret = find_menu_widget(view_tbl, widget)) >= 0) {
		name = view_tbl[ret].widget_name;
		if (strcmp(name, "scrollbar") == 0) {
			if (XtIsManaged(scroll_bar))
				XtUnmanageChild(scroll_bar);
			else
				XtManageChild(scroll_bar);
			return;
			}
		if (strcmp(name, "attributes") == 0) {
			popup_attributes();
			return;
			}
		if (strcmp(name, "status") == 0) {
			popup_status();
			return;
			}
		return;
		}
	if ((ret = find_menu_widget(options_tbl, widget)) >= 0) {
		name = options_tbl[ret].widget_name;
		if (strcmp(name, "font") == 0) {
			popup_font();
			return;
			}
		if (strcmp(name, "reset") == 0) {
			reset_terminal(cur_ctw);
			return;
			}
		if (strcmp(name, "snap") == 0) {
			snap_history(cur_ctw);
			return;
			}
		return;
		}
	if ((ret = find_menu_widget(edit_tbl, widget)) >= 0) {
		name = edit_tbl[ret].widget_name;
		if (strcmp(name, "paste") == 0) {
		  	ctw_get_selection((CtwWidget) ctw_widget);
			return;
			}
		if (strcmp(name, "alphabet") == 0) {
		  	char buf[256];
			int	i;
			for (i = ' '; i < 0x7f; i++)
				buf[i - ' '] = i;
			buf[i] = '\0';
			ctw_add_string((CtwWidget) ctw_widget, buf, strlen(buf));
		  	return;
			}
		return;
		}
}
/**********************************************************************/
/*   Function called when OK button selected in dialog box.	      */
/**********************************************************************/
void
ok_dialog(w, client_data, call_data)
Widget	w;
XtPointer	client_data, call_data;
{
	
	ctw_set_attributes((CtwWidget) ctw_widget, attributes);
	XtDestroyWidget(dialog);
	dialog = (Widget) NULL;
}
/**********************************************************************/
/*   Function called to apply current attribute settings.	      */
/**********************************************************************/
void
apply_dialog(w, client_data, call_data)
Widget	w;
XtPointer	client_data, call_data;
{
	
	ctw_set_attributes((CtwWidget) ctw_widget, attributes);
}
/**********************************************************************/
/*   Function  called  to  update  the  attributes  with the current  */
/*   settings.							      */
/**********************************************************************/
void
refresh_dialog()
{	XmString	str;
	char	buf[BUFSIZ];
	int	*flags;
	int	len;
	int	i, n;
	Arg	args[20];

	len = ctw_get_attributes((CtwWidget) ctw_widget, &flags, &attr_names);
	memcpy((char *) attributes, (char *) flags, len * sizeof (int));
	for (i = 0; i < len; i++) {
		n = 0;
		switch (i) {
		  case CTW_SIZE:
		  case CTW_FONT_SIZE:
			sprintf(buf, "%s: %dx%d", attr_names[i], 
				flags[i] >> 16, flags[i] & 0xffff);
			break;
		  default:
			sprintf(buf, "%s", attr_names[i]);
			XtSetArg(args[n], XmNset, flags[i]); n++;
			break;
		  }
		str = XmStringCreateSimple(buf);
		XtSetArg(args[n], XmNlabelString, str); n++;
		XtSetValues(w_attr[i], args, n);
		XtFree(str);
		}
}
/**********************************************************************/
/*   Function  called  when  user toggles an attribute in the dialog  */
/*   box.							      */
/**********************************************************************/
void
toggle_callback(w, client_data, call_data)
Widget	w;
XtPointer	client_data, call_data;
{
	int	i = (int) client_data;

	if (i != CTW_SIZE && i != CTW_FONT_SIZE) {
		attributes[i] = !attributes[i];
		}
}
/**********************************************************************/
/*   Timer function to update status display.			      */
/**********************************************************************/
void
timeout_proc(client_data, timer)
XtPointer	client_data;
XtIntervalId	*timer;
{	int	n;
	Arg	args[20];
	char	buf[BUFSIZ];
	time_t	t = time((time_t *) NULL);
	time_t	t1;
	XmString	str;

	if (stats.tot_bytes == old_stats.tot_bytes &&
	    stats.bytes == old_stats.bytes)
	    	goto restart_timer;

	if (stats.tot_bytes != old_stats.tot_bytes) {
		n = 0;
		sprintf(buf, "Total bytes: %ld", stats.tot_bytes);
		str = XmStringCreateSimple(buf);
		XtSetArg(args[n], XmNlabelString, str); n++;
		XtSetValues(w_status[0], args, n);
		XtFree(str);
		}

	if (stats.bytes != old_stats.bytes) {
		n = 0;
		sprintf(buf, "Cum bytes: %ld", stats.bytes);
		str = XmStringCreateSimple(buf);
		XtSetArg(args[n], XmNlabelString, str); n++;
		XtSetValues(w_status[1], args, n);
		XtFree(str);
		}

	n = 0;
	t1 = t - stats.start_time;
	sprintf(buf, "Total/sec: %ld", stats.tot_bytes / (t1 ? t1 : 1));
	str = XmStringCreateSimple(buf);
	XtSetArg(args[n], XmNlabelString, str); n++;
	XtSetValues(w_status[2], args, n);
	XtFree(str);

	n = 0;
	t1 = t - stats.time;
	sprintf(buf, "Cum/sec: %ld", stats.tot_bytes / (t1 ? t1 : 1));
	str = XmStringCreateSimple(buf);
	XtSetArg(args[n], XmNlabelString, str); n++;
	XtSetValues(w_status[3], args, n);
	XtFree(str);
	/***********************************************/
	/*   Start timer again.			       */
	/***********************************************/
restart_timer:
	timer_id = XtAppAddTimeOut(app_con, 1000L, timeout_proc, NULL);

	old_stats = stats;
}
/**********************************************************************/
/*   Function  called  when  user  wants to popup the font selection  */
/*   dialog box.						      */
/**********************************************************************/
void
popup_font()
{	int	n, i;
	Arg	args[20];
	static XmString table[6];
	static Widget	w;
	static int first_time = TRUE;

	if (first_time) {
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
		XtGetValues(ctw_widget, args, n);

		for (i = 0; i < 6; i++)
			table[i] = XmStringCreateSimple(font_names[i]);
		n = 0;
		XtSetArg(args[n], XmNlistItemCount, 6); n++;
		XtSetArg(args[n], XmNlistItems, table); n++;
		w = XmCreateSelectionDialog(top_level, "font_dialog", args, n);
		XtAddCallback(w, XmNokCallback, font_ok_callback, (XtPointer) 0);
		XtAddCallback(w, XmNapplyCallback, font_ok_callback, (XtPointer) 1);

		first_time = FALSE;
		}
	XtManageChild(w);
	XRaiseWindow(XtDisplay(w), XtWindow(XtParent(w)));
}
/**********************************************************************/
/*   Callback when font dialog box OK or APPLY button selected.	      */
/**********************************************************************/
void
font_ok_callback(widget, type, ptr)
Widget	widget;
int	type;
XmSelectionBoxCallbackStruct	*ptr;
{	int	n;
	Arg	args[20];
	char	*font;

	do_wm_hints(top_level, ctw_widget, FALSE);

	n = 0;
	XmStringGetLtoR(ptr->value, XmSTRING_DEFAULT_CHARSET, &font);
	XtSetArg(args[n], XtNfont, font); n++;
	XtSetValues(ctw_widget, args, n);
	XtFree(font);

	set_font(font);
}
/**********************************************************************/
/*   Routine to popup the status dialog box.			      */
/**********************************************************************/
void
popup_status()
{	int	n;
	int	y;
	Widget	w;
	Arg	args[20];
	static int first_time = TRUE;

	if (first_time) {
		n = 0;
		XtSetArg(args[n], XmNnoResize, FALSE); n++;
		XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
		XtSetArg(args[n], XmNmarginHeight, 0); n++;
		XtSetArg(args[n], XmNmarginWidth, 0); n++;
		status_dialog = XmCreateBulletinBoardDialog(top_level,
			"status", args, n);
		/***********************************************/
		/*   Create a title for the dialog box.	       */
		/***********************************************/
		y = 4;
		n = 0;
		XtSetArg(args[n], XmNx, 100); n++;
		XtSetArg(args[n], XmNy, y); n++;
		XtCreateManagedWidget("Status Monitor",
			xmLabelWidgetClass, status_dialog, args, n);
		n = 0;
		y += 24;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNwidth, 300); n++;
		XtCreateManagedWidget("sep1",
			xmSeparatorWidgetClass, status_dialog, args, n);
		y += 10;

		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 5); n++;
		w_status[0] = XtCreateManagedWidget("Total",
			xmLabelWidgetClass, status_dialog, args, n);
		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 160); n++;
		w_status[1] = XtCreateManagedWidget("Total",
			xmLabelWidgetClass, status_dialog, args, n);

		y += 15;
		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 5); n++;
		w_status[2] = XtCreateManagedWidget("Total",
			xmLabelWidgetClass, status_dialog, args, n);
		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 160); n++;
		w_status[3] = XtCreateManagedWidget("Total",
			xmLabelWidgetClass, status_dialog, args, n);
		y += 20;
		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNwidth, 300); n++;
		XtCreateManagedWidget("sep2",
			xmSeparatorWidgetClass, status_dialog, args, n);

		y += 10;
		/***********************************************/
		/*   Create the OK/Clear buttons	       */
		/***********************************************/
		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 50); n++;
		XtSetArg(args[n], XmNwidth, 75); n++;
		w = XtCreateManagedWidget("OK", 
			xmPushButtonWidgetClass, status_dialog, args, n);
		XtAddCallback(w, XmNactivateCallback, status_button_callback, (XtPointer) TRUE);

		n = 0;
		XtSetArg(args[n], XmNy, y); n++;
		XtSetArg(args[n], XmNx, 150); n++;
		XtSetArg(args[n], XmNwidth, 75); n++;
		w = XtCreateManagedWidget("CLEAR", 
			xmPushButtonWidgetClass, status_dialog, args, n);
		XtAddCallback(w, XmNactivateCallback, status_button_callback, (XtPointer) FALSE);
		first_time = FALSE;
		}
	if (!XtIsManaged(status_dialog)) {
		timeout_proc();
		XtManageChild(status_dialog);
		}
	stats.time = time((time_t *) NULL);
	stats.bytes = 0;
	old_stats = stats;
	XRaiseWindow(XtDisplay(status_dialog), XtWindow(XtParent(status_dialog)));
}
/**********************************************************************/
/*   Callback for the Status Monitor dialog box.		      */
/**********************************************************************/
void
status_button_callback(widget, type, ptr)
Widget	widget;
int	type;
caddr_t	ptr;
{
	if (type) {
		XtUnmanageChild(status_dialog);
		XtRemoveTimeOut(timer_id);
		return;
		}
	stats.time = time((time_t *) NULL);
	stats.bytes = 0;
}
/**********************************************************************/
/*   Routine to popup the attributes dialog box.		      */
/**********************************************************************/
void
popup_attributes()
{	int	n, i, len;
	Arg	args[20];
	char	buf[BUFSIZ];
	Widget	frame, w, rc, form, title;
	int	*flags;
	static int first_time = TRUE;
	Dimension	y;
	XmString	str;
static char *names[] = {" Ok ", " Cancel ", " Apply ", " Refresh "};

	if (first_time) {
		n = 0;
		XtSetArg(args[n], XmNnoResize, FALSE); n++;
		XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
		XtSetArg(args[n], XmNmarginHeight, 0); n++;
		XtSetArg(args[n], XmNmarginWidth, 0); n++;
		attr_dialog = XmCreateBulletinBoardDialog(top_level,
			"attr", args, n);
		n = 0;
		form = XtCreateManagedWidget("Form",
			xmFormWidgetClass, attr_dialog, args, n);
		/***********************************************/
		/*   Create a title for the dialog box.	       */
		/***********************************************/
		y = 4;
		n = 0;
		XtSetArg(args[n], XmNx, 150); n++;
		XtSetArg(args[n], XmNy, y); n++;
		title = XtCreateManagedWidget("Terminal Attributes",
			xmLabelWidgetClass, form, args, n);

		n = 0;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, title); n++;
		frame = XtCreateWidget("frame", xmFrameWidgetClass, 
			form, args, n);

		n = 0;
		XtSetArg(args[n], XmNnumColumns, 3); n++;
		XtSetArg(args[n], XmNadjustLast, FALSE); n++;
		XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
		rc = XtCreateWidget("rc", 
			xmRowColumnWidgetClass, frame, args, n);
		/***********************************************/
		/*   Put  the  attributes  into  a row column  */
		/*   widget.				       */
		/***********************************************/
	  	len = ctw_get_attributes(ctw_widget, &flags, &attr_names);
		memcpy((char *) attributes, (char *) flags, 
			len * sizeof (int));
		for (i = 0; i < len; i++) {
			n = 0;
			switch (i) {
			  case CTW_SIZE:
			  case CTW_FONT_SIZE:
				sprintf(buf, "%s: %dx%d", attr_names[i], 
					flags[i] >> 16, flags[i] & 0xffff);
				break;
			  default:
				sprintf(buf, "%s", attr_names[i]);
				XtSetArg(args[n], XmNset, flags[i]); n++;
				break;
				}
			str = XmStringCreateSimple(buf);
			XtSetArg(args[n], XmNlabelString, str); n++;
			w_attr[i] = XmCreateToggleButton(rc, "toggle", args, n);
			XtFree(str);
			XtAddCallback(w_attr[i], XmNvalueChangedCallback,
				toggle_callback, (XtPointer) i);
			XtManageChild(w_attr[i]);
			}


		XtManageChild(rc);
		n = 0;
		XtSetArg(args[n], XmNnumColumns, 4); n++;
		XtSetArg(args[n], XmNadjustLast, FALSE); n++;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, rc); n++;
		XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
		rc = XtCreateWidget("rc", 
			xmRowColumnWidgetClass, form, args, n);
		/***********************************************/
		/*   Create the buttons.		       */
		/***********************************************/
		for (i = 0; i < 4; i++) {
			n = 0;
			XtSetArg(args[n], XmNy, 260); n++;
			XtSetArg(args[n], XmNx, 50 + 100 * i); n++;
			XtSetArg(args[n], XmNwidth, 75); n++;
			w = XtCreateManagedWidget(names[i],
				xmPushButtonWidgetClass, rc, args, n);
			XtAddCallback(w, XmNactivateCallback, attr_button_callback, i);
			}
		XtManageChild(rc);
		XtManageChild(frame);
		first_time = FALSE;
		}
	if (!XtIsManaged(attr_dialog)) {
		XtManageChild(attr_dialog);
		}
	XRaiseWindow(XtDisplay(attr_dialog), XtWindow(XtParent(attr_dialog)));
}
/**********************************************************************/
/*   Callback for the Attributes dialog box.			      */
/**********************************************************************/
void
attr_button_callback(widget, type, ptr)
Widget	widget;
int	type;
caddr_t	ptr;
{
	switch (type) {
	  case 0:
	  	/***********************************************/
	  	/*   OK button				       */
	  	/***********************************************/
		ctw_set_attributes((CtwWidget) ctw_widget, attributes);
		XtUnmanageChild(attr_dialog);
		return;
	  case 1:
	  	/***********************************************/
	  	/*   Cancel button			       */
	  	/***********************************************/
		XtUnmanageChild(attr_dialog);
		return;
	  case 2:
	  	/***********************************************/
	  	/*   Apply button			       */
	  	/***********************************************/
		ctw_set_attributes((CtwWidget) ctw_widget, attributes);
		return;
	  case 3:
	  	/***********************************************/
	  	/*   Refresh button.			       */
	  	/***********************************************/
		refresh_dialog();
	  	return;
	  }
}
void
set_font(font)
char	*font;
{
	int	n;
	Arg	args[20];
	int	rows, columns;
	int	fwidth, fheight;

	do_wm_hints(top_level, ctw_widget, FALSE);

	n = 0;
	XtSetArg(args[n], XtNfont, font); n++;
	XtSetValues(ctw_widget, args, n);

	get_font_size(cur_ctw, &fwidth, &fheight);

	n = 0;
	XtSetArg(args[n], XtNrows, &rows); n++;
	XtSetArg(args[n], XtNcolumns, &columns); n++;
	XtGetValues(ctw_widget, args, n);
	/***********************************************/
	/*   Re-use the row column size.	       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNrows, rows); n++;
	XtSetArg(args[n], XtNcolumns, columns); n++;
	XtSetArg(args[n], XtNwidth, fwidth * columns); n++;
	XtSetArg(args[n], XtNheight, fheight * rows); n++;
	XtSetValues(ctw_widget, args, n);

	n = 0;
	XtSetArg(args[n], XtNheight, fheight * rows); n++;
	XtSetValues(pane, args, n);
	XtSetValues(scroll_bar, args, n);
	/***********************************************/
	/*   Tell    window    manager   about   size  */
	/*   increments.			       */
	/***********************************************/
	do_wm_hints(top_level, ctw_widget, TRUE);
}
void
usleep()
{
}
