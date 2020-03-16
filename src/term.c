/**********************************************************************/
/*   Color  terminal  emulator using the ctw (Color terminal widget)  */
/*   widget.							      */
/*   								      */
/*   This  file  contains  routines  which  are common to the athena  */
/*   and motif versions of fcterm.				      */
/*   								      */
/*   Author: Paul Fox						      */
/*   Date:   February 1992       				      */
/*   $Header: Last edited: 13-Dec-2019 1.12 $ 			      */
/**********************************************************************/

//#define __STRICT_ANSI__
# include	"fcterm.h"
# include	"pwd.h"
# include	"scrbar.h"
# if defined(HAS_UTMP)
# 	include	<utmp.h>
#	define	utmpx	utmp
#       if defined(UTMP_FILENAME)
#	        define	endutxent()
#       else
#	        define	endutxent endutent
#       endif
# endif
# include	<sys/stat.h>
# if defined(HAVE_SYS_WAIT_H)
#	include	<sys/wait.h>
# endif
# if defined(HAS_UTMPX)
# 	include	<utmpx.h>
# endif
# if CR_LINUX
#	define	wchar_t	wchar_t_redefined
# 	include	<malloc.h>
# endif
# include	<limits.h>

# include	<xpm.h>
# include	"fcterm.bitmap"
# include	"fcterm.xpm"
# include	<X11/Xatom.h>

# undef sscanf

#if !defined(TAB3)
# define   TAB3	0014000
#endif
#if !defined(PATH_MAX)
# define	PATH_MAX 1024
#endif

static char **command_to_exec;
extern char *group_label;
extern int version_major;
extern int version_minor;
extern int version_build_no;
int	direct;
int	dump_flag;
extern int ctw_history;
int	console_flag = FALSE;
int	utmp_flag = TRUE;
int	login_shell_flag = FALSE;
char	*slavename;
char	*geometry;
int	do_group = FALSE;
int	label_arg_specified = FALSE;
int	geom_x = -1, geom_y = -1;
int	geom_width = -1, geom_height = -1;
char	*term_str = "TERM=xterm";
char	*bterm_str = "CRTERM=xcrisp-col";
char	*foxterm_str = "FOXTERM=xterm";
char	*application_name = "XTerm";
int	fork_flag = TRUE;
int	sun_function_keys = -1;
int	debug;
int	auto_switch;
char	*tmpdir = "/var/tmp";
int	verbose;
volatile XtSignalId xchild_id;
volatile int	child_flag;

enum ctw_emulation emulator = CTW_EMULATE_LINUX;

Widget		top_level;

int	main_x;
int	main_y;

struct stats  stats, old_stats;
XtIntervalId	snap_id;
XtIntervalId	save_state_id;
XtIntervalId	timer_id;
XtIntervalId	cfg_timer_id;
XtIntervalId	map_timer_id;

XtAppContext	app_con;

/**********************************************************************/
/*   Name  of  main  widget  can  be changed depending on whether we  */
/*   are  in  ISC  or  SCO  mode  so  that user can set up different  */
/*   Xdefaults.							      */
/**********************************************************************/
char	*widget_name = "pane";
/**********************************************************************/
/*   Copy of attributes which user can change.			      */
/**********************************************************************/
char	**attr_names;

char	*log_dir;

/**********************************************************************/
/*   GC in case we need it for the minimap.			      */
/**********************************************************************/
GC	gc;

fcterm_t	*hd_ctw;

/**********************************************************************/
/*   Minimap pixmap.						      */
/**********************************************************************/
Pixmap	map_pixmap;

extern int	prog_argc;
extern char	**prog_argv;
extern Widget pane;
extern fcterm_t	*cur_ctw;

/**********************************************************************/
/*   Default clipboard handling.				      */
/**********************************************************************/
extern int	enable_primary;
extern int	enable_secondary;
extern int	enable_clipboard;
extern int	enable_cut_buffer0;

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
int	grantpt(int);
int	unlockpt(int);
char	*find_exec_path PROTO((char *, char *));
void	dump_screens(void);
void	save_state(void);
void	top_callback();
char	*basename(char *);
void set_status_label(fcterm_t *cur_ctw, char *title);
void pty_output();
void	group_config_poll(void *);
char	*ptsname();

void
init_term()
{	XGCValues	values;
	Display	*dpy = XtDisplay(top_level);
	char	buf[BUFSIZ];

	if (getenv("CTW_LOGDIR"))
		log_dir = chk_strdup(getenv("CTW_LOGDIR"));
	else {
		snprintf(buf, sizeof buf, "%s/%s", 
			tmpdir,
			getenv("USER") ? getenv("USER") : "fcterm");
		log_dir = chk_strdup(buf);
		}
	if (mkdir(log_dir, 0700) == -1 && errno != EEXIST) {
		printf("fcterm: failed to mkdir(%s) - %s\n", log_dir, strerror(errno));
		}

	values.graphics_exposures = TRUE;
	values.background = BlackPixel(dpy, DefaultScreen(dpy));
	values.foreground = WhitePixel(dpy, DefaultScreen(dpy));
	gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		GCForeground | GCBackground | GCGraphicsExposures, &values);

	snap_id = XtAppAddTimeOut(app_con, 500L, 
		(XtTimerCallbackProc) dump_screens, NULL);

	save_state_id = XtAppAddTimeOut(app_con, 4000L, 
		(XtTimerCallbackProc) save_state, NULL);

	cfg_timer_id = XtAppAddTimeOut(app_con, 1000L, 
		(XtTimerCallbackProc) group_config_poll, &cfg_timer_id);
}
/**********************************************************************/
/*   Create a new screen structure.				      */
/**********************************************************************/
fcterm_t *
new_fcterm(int new_id)
{	fcterm_t *ctwp, *cur_ctw;
static int	id;

	cur_ctw = chk_zalloc(sizeof *cur_ctw);
	cur_ctw->f_pty_fd = -1;
	if (hd_ctw == NULL) {
		hd_ctw = cur_ctw;
		}
	else {
		for (ctwp = hd_ctw; ctwp->f_next; ctwp = ctwp->f_next)
			;
		ctwp->f_next = cur_ctw;
		cur_ctw->f_prev = ctwp;
		}

	cb_init(&cur_ctw->f_pty_ibuf, 4 * 8192);
	if (new_id == 0)
		cur_ctw->f_id = id++;
	else
		cur_ctw->f_id = new_id;
	return cur_ctw;
}
void
cb_init(cbuf_t *cbp, int size)
{
	cbp->c_size = size;
	cbp->c_in = 0;
	cbp->c_out = 0;
	cbp->c_buf = chk_alloc(size);
}
void
cb_free(cbuf_t *cbp)
{
	chk_free(cbp->c_buf);
}
int
cb_read(int fd, cbuf_t *cbp, unsigned char **start_bp)
{	int	n = 0, len;
	unsigned char	*bp;
	int	in, ret;
	int	loop;
	struct timeval tval;
	Display *dpy = XtDisplay(top_level);

	*start_bp = cbp->c_buf + cbp->c_in;
	bp = *start_bp;
	in = cbp->c_in;

	for (loop = 0; loop < 2; loop++) {
		fd_set	rbits;

		if (in >= cbp->c_out) {
			len = cbp->c_size - in;
			}
		else {
			len = cbp->c_out - in;
			}
		if (len == 0)
			break;

		if ((n = read(fd, bp, len)) <= 0)
			break;

		cbp->c_in += n;
		in += n;
		bp += n;

		/***********************************************/
		/*   Dont hang around if we got a tiny amount  */
		/*   of data.				       */
		/***********************************************/
		if (bp - *start_bp < 512)
			break;

		FD_ZERO(&rbits);
		FD_SET(fd, &rbits);
		FD_SET(ConnectionNumber(dpy), &rbits);
		tval.tv_sec = 0;
		tval.tv_usec = 10000;
		ret = select(fd + 1, &rbits, NULL, NULL, &tval);
		if (ret <= 0 || FD_ISSET(ConnectionNumber(dpy), &rbits))
			break;
		}
	if (bp == *start_bp)
		return n;
	return bp - *start_bp;
}
int
cb_copy_in(cbuf_t *cbp, unsigned char *buf, int len)
{	int	n = 0, clen;

	if ((cbp->c_used += len) > cbp->c_size)
		cbp->c_used = cbp->c_size;
	while (len > 0) {
		if (cbp->c_in >= cbp->c_out) {
			clen = cbp->c_size - cbp->c_in;
			}
		else {
			clen = cbp->c_out - cbp->c_in;
			}
		clen = clen > len ? len : clen;
		memcpy(cbp->c_buf + cbp->c_in, buf, clen);
		if ((cbp->c_in += clen) == cbp->c_size)
			cbp->c_in = 0;
		buf += clen;
		len -= clen;
		n += clen;
		}
	return n;
}
void
cb_reset(cbuf_t *cbp)
{
	cbp->c_in = cbp->c_out = 0;
}
/**********************************************************************/
/*   Child handler - a pty died on us.				      */
/**********************************************************************/
void
child_handler()
{
	child_flag = TRUE;
	XtNoticeSignal(xchild_id);
}
void
close_screen(fcterm_t *ctwp)
{
	if (debug) {
		printf("Closing: fd=%d id=%d tty=%s\n", ctwp->f_pty_fd, ctwp->f_id, ctwp->f_ttyname);
		}
	cb_free(&ctwp->f_pty_ibuf);
	do_utmp(ctwp, TRUE);
	chmod(ctwp->f_ttyname, 0666);
	/***********************************************/
	/*   Remove child.			       */
	/***********************************************/
	if (ctwp->f_input_id) {
		XtRemoveInput(ctwp->f_input_id);
		if (debug)
			printf("XtRemoveInput(input): %d %s fd=%d\n", (int) ctwp->f_input_id, ctwp->f_ttyname, ctwp->f_pty_fd);
		}
	if (ctwp->f_output_id) {
		XtRemoveInput(ctwp->f_output_id);
		if (debug)
			printf("XtRemoveInput(output): %d %s fd=%d\n", (int) ctwp->f_output_id, ctwp->f_ttyname, ctwp->f_pty_fd);
		}
	if (ctwp->f_pty_fd >= 0 && close(ctwp->f_pty_fd) < 0) {
		fprintf(stderr, "fcterm: close pty(%d)\n", ctwp->f_pty_fd);
		perror("close pty");
		}

	if (hd_ctw == ctwp) {
		hd_ctw = ctwp->f_next;
		if (hd_ctw)
			hd_ctw->f_prev = NULL;
		}
	else {
		ctwp->f_prev->f_next = ctwp->f_next;
		if (ctwp->f_next)
			ctwp->f_next->f_prev = ctwp->f_prev;
		}
//printf("Not XtDestroyWidget\n");
	XtUnmanageChild(ctwp->f_ctw);
	XtDestroyWidget(ctwp->f_ctw);
	chk_free_ptr((void *) &cur_ctw->f_title);
	chk_free(ctwp);

	if (debug) {
		for (cur_ctw = hd_ctw; cur_ctw; cur_ctw = cur_ctw->f_next) {
			printf("child open: %p %d\n", cur_ctw, cur_ctw->f_id);
			}
		}

	cur_ctw = hd_ctw;
	if (cur_ctw->f_id == GHOST_ID)
		cur_ctw = cur_ctw->f_next;
}
void
create_icon_pixmap()
{	Pixmap	pixmap;
	Pixmap	pixmap_mask;
	XpmAttributes xpm_attr;
	Arg	args[20];
	int	err;
	Display	*dpy = XtDisplay(top_level);
	Window	win;

	pixmap = XCreateBitmapFromData(dpy, XtWindow(top_level),
		fcterm_bits, fcterm_width, fcterm_height);
	XtSetArg(args[0], XtNiconPixmap, pixmap);
	XtSetValues(top_level, args, 1);

	/***********************************************/
	/*   Do color bitmap.			       */
	/***********************************************/
	memset((char *) &xpm_attr, 0, sizeof xpm_attr);
	err = XpmCreatePixmapFromData(dpy,
			RootWindow(dpy, DefaultScreen(dpy)),
			fcterm_xpm,
			&pixmap,
			&pixmap_mask,
			&xpm_attr
			);
	win = XCreateSimpleWindow(dpy,
		RootWindow(dpy, DefaultScreen(dpy)),
		0, 0, 64, 55, 0, 0, 0);
	XSetWindowBackgroundPixmap(dpy, win, pixmap);

	XtSetArg(args[0], XtNiconWindow, win);
	/***********************************************/
	/*   iconPixmap can be used by eXceed.	       */
	/***********************************************/
	XtSetArg(args[1], XtNiconPixmap, pixmap);
	XtSetValues(top_level, args, 2);

	/***********************************************/
	/*   Create the _NET_WM_ICON.		       */
	/***********************************************/
	{int	sz;
	int	i, x, y;
	Atom	_net_wm_icon;
	XImage	*xim = NULL;
typedef unsigned long CARD32;
static CARD32 *arr;

	err = XpmCreateImageFromData(dpy, fcterm_xpm, &xim, NULL, NULL);
	UNUSED_PARAMETER(err);
//printf("xim=%p %dx%d\n", xim, xim->width, xim->height);

	_net_wm_icon = XInternAtom(dpy, "_NET_WM_ICON", FALSE);
	sz = (xim->width * xim->height) + 2;
	arr = chk_alloc(sz * sizeof(CARD32));
	i = 0;
	arr[i++] = xim->width;
	arr[i++] = xim->height;
	for (y = 0; y < xim->height; y++) {
		for (x = 0; x < xim->width; x++) {
			Pixel p = XGetPixel(xim, x, y);
//printf("p=%lx ", p);
			arr[i++] = 0xff000000 | p;
			}
		}
	XChangeProperty(dpy, XtWindow(top_level), _net_wm_icon, XA_CARDINAL,
		32, PropModeReplace, 
		(unsigned char *) arr, sz);
	chk_free(arr);
	XDestroyImage(xim);
	}
}
int
do_switches(int argc, char **argv)
{	int	i;
	char	*cp;
	int	flag;

	for (i = 1; i < argc; i++) {
		int	no = FALSE;

		cp = argv[i];
		flag = TRUE;
		if (*cp == '+') {
			flag = FALSE;
			cp++;
			}
		else if (*cp++ != '-')
			return i;

		if (strncmp(cp, "no", 2) == 0) {
			no = TRUE;
			cp += 2;
			}

		if (strcmp(cp, "debug") == 0) {
			debug = TRUE;
			continue;
			}
		if (strcmp(cp, "direct") == 0) {
			direct = TRUE;
			continue;
			}
		if (strcmp(cp, "dump") == 0) {
			dump_flag = TRUE;
			continue;
			}

		if (strcmp(cp, "label") == 0) {
			if (++i >= argc)
				usage();
			group_label = argv[i];
			label_arg_specified = TRUE;
			continue;
			}

		if (strcmp(cp, "spill") == 0) {
			ctw_history = TRUE;
			continue;
			}
		if (strcmp(cp, "ut") == 0) {
			utmp_flag = !flag;
			continue;
			}
		if (strcmp(cp, "sf") == 0) {
			sun_function_keys = flag;
			continue;
			}
		if (strcmp(cp, "tmpdir") == 0) {
			if (++i >= argc)
				usage();
			tmpdir = argv[i];
			continue;
			}
		if (strcmp(cp, "ls") == 0) {
			login_shell_flag = flag;
			break;
			}
		if (strncmp(cp, "geom", 4) == 0) {
			if (++i >= argc)
				usage();
			cp = argv[i];
			if (*cp == '+')
				mysscanf(cp, "+%d+%d", &geom_x, &geom_y);
			else
				mysscanf(cp, "%dx%d+%d+%d", 
					&geom_width, &geom_height,
					&geom_x, &geom_y);
			continue;
			}
		if (strcmp(cp, "group") == 0) {
			do_group = !no;
			continue;
			}
		if (strcmp(cp, "verbose") == 0) {
			verbose = TRUE;
			break;
			}
		if (strcmp(cp, "version") == 0) {
			/***********************************************/
			/*   Dont  add  anything  else here, else the  */
			/*   "make install" will fail.		       */
			/***********************************************/
			printf("b%d\n", version_build_no);
			exit(0);
			}

		while (*cp) {
			switch (*cp++) {
			  case 'C':
			  	console_flag = TRUE;
				break;
			  case 'e':
			  	if (++i >= argc)
					usage();
			  	command_to_exec = &argv[i];
				return argc;
			  case 'f':
			  	fork_flag = FALSE;
				break;
			  default:
			  	usage();
			  }
			}
		}
	return i;
}
/**********************************************************************/
/*   Function to handle application oriented escape sequences.	      */
/**********************************************************************/
void
appl_callback(Widget w, int type, ctw_callback_t *reason)
{	fcterm_t *cur_ctw = reason->client_data;
	int	top_line, wheel_incr = 2;

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(type);

	switch (reason->reason) {
	  case CTWR_FUNCTION_KEY:
	  	switch_screen(reason->key);
	  	break;
	  case CTWR_SET_AUTOSWITCH:
	  	set_autoswitch(reason->flags);
	  	break;
	  case CTWR_SET_STATUS_LABEL:
	  	set_status_label(cur_ctw, reason->ptr);
	  	break;
	  case CTWR_SET_TITLE:
	  	set_title(cur_ctw, reason->flags, reason->ptr);
	  	break;
	  case CTWR_OPEN_WINDOW:
	  	XtVaSetValues(top_level, XtNiconic, FALSE, NULL);
	  	break;
	  case CTWR_CLOSE_WINDOW:
	  	XtVaSetValues(top_level, XtNiconic, TRUE, NULL);
	  	break;
	  case CTWR_MOVE_WINDOW:
	  	XtVaSetValues(top_level,
			XtNx,	reason->x,
			XtNy,	reason->y,
			NULL);
		break;
	  case CTWR_SIZE_WINDOW_PIXELS:
	  	XtVaSetValues(top_level,
			XtNwidth,	reason->width,
			XtNheight,	reason->height,
			NULL);
		break;
	  case CTWR_FRONT_WINDOW:
	  	XRaiseWindow(XtDisplay(top_level), XtWindow(top_level));
		break;
	  case CTWR_BACK_WINDOW:
	  	XLowerWindow(XtDisplay(top_level), XtWindow(top_level));
		break;
	  case CTWR_SIZE_WINDOW_CHARS:
	  	set_appl_window_size(cur_ctw, reason->height, reason->width);
		break;
	  case CTWR_FONT_CHANGE:
	  	set_font(reason->font);
	  	break;
	  case CTWR_WHEEL_DOWN:
	  	if (getenv("WHEEL_INCR"))
			wheel_incr = atoi(getenv("WHEEL_INCR"));
		top_line = ctw_get_top_line((CtwWidget) cur_ctw->f_ctw);
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line - wheel_incr);
	  	break;
	  case CTWR_WHEEL_UP:
	  	if (getenv("WHEEL_INCR"))
			wheel_incr = atoi(getenv("WHEEL_INCR"));
		top_line = ctw_get_top_line((CtwWidget) cur_ctw->f_ctw);
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line + wheel_incr);
	  	break;
	  case CTWR_SAVE_STATE:
	  	restart_fcterm();
		break;
	  case CTWR_RESTORE_STATE:
	  	break;
	  default:
	  	break;
	  }
}
void
fork_new_terminal(fcterm_t *cur_ctw)
{	char	*cp;
  	int pid;

	if ((pid = fork()) == 0) {
		close(cur_ctw->f_pty_fd);
		execlp(prog_name, prog_name, "-name", "xcrisp", NULL);
		printf("[FCTERM#%d] Failed to exec new fcterm\n", getpid());
		fflush(stdout);
		_exit(1);
		}
	if (pid < 0) {
		cp = "\033[47;30m   Failed to fork process    \033[37;40m";
	  	ctw_add_string((CtwWidget) cur_ctw->f_ctw, cp, strlen(cp));
		}
}
void
fork_wmgr(fcterm_t *cur_ctw, int val)
{	int pid;
	char *name = val == MENU_FORK_MWM ? "mwm" : "olwm";

	if ((pid = fork()) == 0) {
		close(cur_ctw->f_pty_fd);
		execlp(name, name, NULL);
		printf("[FCTERM#%d] Failed to exec: %s\n", getpid(), name);
		fflush(stdout);
		_exit(1);
		}
}
/**********************************************************************/
/*   Function  to  advise  the window manager about our width/height  */
/*   increments.  Thanks  to Jim Wight for suggesting and helping me  */
/*   with this.							      */
/**********************************************************************/
void
inform_wm(fcterm_t *cur_ctw)
{	XSizeHints	hints;
	long		supplied;
	Display		*dpy = XtDisplay(top_level);
	Window		w = XtWindow(top_level);
	int		fwidth, fheight;

	get_font_size(cur_ctw, &fwidth, &fheight);

	if (!XGetWMNormalHints(dpy, w, &hints, &supplied))
		hints.flags = 0;

	hints.flags |= PResizeInc /*| PBaseSize*/;
	hints.width_inc = fwidth;
	hints.height_inc = fheight;
/*	hints.base_width = lm + rm;
	hints.base_height = pheight - (height - tm - bm);*/

	XSetWMNormalHints(dpy, w, &hints);
}
/**********************************************************************/
/*   Function to handle keyboard input. Just send it to the pty.      */
/**********************************************************************/
void
input_callback(Widget w, int type, ctw_callback_t *reason)
{	int	len = reason->len;
	char	*cp;
	fcterm_t *cur_ctw = reason->client_data;

	UNUSED_PARAMETER(w);
	UNUSED_PARAMETER(type);

	cp = reason->ptr;
//printf("input_callback: cur_ctw=%x\n", cur_ctw);
	if (reason->reason == CTWR_PASTE_PROTECTED)
		v_write(cur_ctw, "\033[200~", 6);
	v_write(cur_ctw, cp, len);
	if (reason->reason == CTWR_PASTE_PROTECTED)
		v_write(cur_ctw, "\033[201~", 6);
}
void
int_handler()
{	fcterm_t	*ctwp;

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_pid) {
			kill(ctwp->f_pid, SIGTERM);
			kill(ctwp->f_pid, SIGHUP);
			kill(ctwp->f_pid, SIGKILL);
			chmod(ctwp->f_ttyname, 0666);
			}
		}
	exit(2);
}
/**********************************************************************/
/*   User is clicking on the minimap.				      */
/**********************************************************************/
void
map_click(Widget widget, XtPointer client_data, XEvent *event)
{	fcterm_t	*ctwp;
	int	i, x, y;
static int id = 0;

	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(client_data);

	switch (event->type) {
	  case KeyPress:
//	  case KeyRelease:
//printf("event=%d\n", event->type);
		ctwp = hd_ctw;
		for (i = 0; i < id; i++) {
			if (ctwp->f_next == NULL) {
				id = i;
				break;
				}
			ctwp = ctwp->f_next;
			}
		if (ctwp == NULL)
			ctwp = hd_ctw->f_next;
		ctw_send_input((CtwWidget) ctwp->f_ctw, event, NULL, NULL);
		/***********************************************/
		/*   Force a redraw.			       */
		/***********************************************/
		XtRemoveTimeOut(map_timer_id);
		show_map(0, 0);
	  	return;

	  case ButtonPress:
	  	break;

	  default:
	  	return;
	  }

	/***********************************************/
	/*   Need  to  figure  out  which  window  we  */
	/*   clicked in.			       */
	/***********************************************/
	x = event->xbutton.x;
	y = event->xbutton.y;
	id = 0;
	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next, id++) {
		if (x >= ctwp->mm_x && y >= ctwp->mm_y &&
		    x < ctwp->mm_x + ctwp->mm_w &&
		    y < ctwp->mm_y + ctwp->mm_h) {
		    	/***********************************************/
		    	/*   Let  user  select  which  is  the active  */
		    	/*   window.				       */
		    	/***********************************************/
		  	if (event->xbutton.button != 1) {
				XtRemoveTimeOut(map_timer_id);
				switch_screen(ctwp->f_id);
				}
		    	break;
		    	}
		}
}
/**********************************************************************/
/*   Handle one of the screens dying on us.			      */
/*   We  are horrible - we do all the things we shouldn't do because  */
/*   this  is  an  async  signal. But its rare so live with it. Just  */
/*   move all the code to a new subroutine, and then figure out when  */
/*   to call it?!						      */
/**********************************************************************/
void
process_sigchld(XtPointer ptr, XtSignalId *id)
{	int	status;
	int	pid;
	fcterm_t *ctwp;

	UNUSED_PARAMETER(ptr);
	UNUSED_PARAMETER(id);

	child_flag = FALSE;

	status = 0;
# if defined(HAVE_WAITPID)
	pid = waitpid(-1, &status, WNOHANG);
# else
	pid = wait(&status);
# endif
	if (pid && pid != -1) {
		if (debug) {
			printf("SIGCHLD: wait returns %d\n", pid);
			}
		for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
			if (pid == ctwp->f_pid)
				break;
			}
		if (ctwp == NULL) {
			return;
			}
		if (status || debug)
			printf("Child process %d died exit code: %d\n", pid, status);
		}
	else {
		for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
			if (ctwp->f_eof)
				break;
			}
		if (ctwp == NULL) {
			return;
			}
		}

	close_screen(ctwp);

	/***********************************************/
	/*   Exit  if  last screen (and not the ghost  */
	/*   screen).				       */
	/***********************************************/
	if (hd_ctw == NULL || (hd_ctw->f_next == NULL && hd_ctw->f_id == GHOST_ID)) {
		save_state();
		exit(1);
		}

	switch_screen2();
}
static void
pty_command(fcterm_t *cur_ctw, char *str, int len)
{	char	*cmd;
	char	*arg;

	if (getenv("FCTERM_DEBUG"))
		printf("pty cmd: len=%d '%*.*s'\n", len, len, len, str);
	cmd = strtok(str, " ");
	if (cmd == NULL)
		return;
	arg = strtok(NULL, "\n");
	if (strcmp(cmd, "winsz") == 0) {
		int	rows, cols;
		if (arg == NULL || mysscanf(arg, "rows=%d cols=%d", &rows, &cols) != 2)
			return;
		if (rows == cur_ctw->f_rows &&
		    cols == cur_ctw->f_cols)
		    	return;
		set_appl_window_size(cur_ctw, rows, cols);
		return;
		}
	if (strcmp(cmd, "mouse") == 0) {
		int	reason, type, state, x, y;
		unsigned long time;

		if (arg == NULL || 
		    mysscanf(arg, "%d %lu %d %d %d %d", &reason, &time, &type, &state, &x, &y) != 6)
			return;
		ctw_mouse((CtwWidget) cur_ctw->f_ctw, reason, time, type, state, x, y);
		return;
		}
}
/**********************************************************************/
/*   Function to handle input from pty.				      */
/**********************************************************************/
void
pty_input(fcterm_t *cur_ctw, int src, XtInputId id)
{	int	n;
	int	len;
	int	eio_count = 0;
	unsigned char *start_bp;
	unsigned char *cp, *cpend;

	UNUSED_PARAMETER(src);
	UNUSED_PARAMETER(id);

	if (cur_ctw->f_eof)
		return;

//printf("fcterm(pid=%d): reading %x %d\n", getpid(), cur_ctw, cur_ctw->f_pty_fd);
reread:
	n = cb_read(cur_ctw->f_pty_fd, &cur_ctw->f_pty_ibuf, &start_bp);
//printf("pty_input: read %d\n", n);
	if (n == -2) {
		printf("fcterm: buffer full - clearing buffer\n");
		cb_reset(&cur_ctw->f_pty_ibuf);
		return;
		}
	if (n < 0) {
		/***********************************************/
		/*   Strange Linux startup race condition.     */
		/***********************************************/
		if (errno == EIO) {
			/***********************************************/
			/*   In case child handler wants to fire.      */
			/***********************************************/
			if (child_flag)
				return;
			if (++eio_count < 10) {
//printf("EIO error - sleeping\n");
				usleep(100 * 1000);
				goto reread;
				}
			}
		if (errno != EIO) {
			fprintf(stderr, "fcterm(pid=%d): ", getpid());
			perror("read");
			}
		if (errno == EAGAIN) {
//printf("EAGAIN error - sleeping\n");
			usleep(100 * 1000);
			goto reread;
			}
		fprintf(stderr, "fcterm[%d]: error reading from pty(fd=%d), errno=%d %s\n", getpid(), cur_ctw->f_pty_fd, errno,
			strerror(errno));
		usleep(100 * 1000);
		/***********************************************/
		/*   This shouldnt happen!		       */
		/***********************************************/
		if (errno == EBADF)
			return;

		if (errno == EIO) {
			cur_ctw->f_eof = TRUE;
			child_handler();
			return;
			}
		goto reread;
		}
	if (n == 0) {
		fprintf(stderr, "EOF detected on pty(fd=%d)\n", cur_ctw->f_pty_fd);
		cur_ctw->f_eof = TRUE;
		child_handler();
		usleep(100 * 1000);
		return;
		}

	/***********************************************/
	/*   If  this  is  a  ptysrv connection, then  */
	/*   need   to  break  up  into  in-line  and  */
	/*   out-of-line messages.		       */
	/***********************************************/
	cpend = cur_ctw->f_pty_ibuf.c_buf + cur_ctw->f_pty_ibuf.c_in;
	if (cur_ctw->f_pty_server) {
		for (cp = cur_ctw->f_pty_ibuf.c_buf; cp < cpend; cp++) {
			if (*cp == '\0') {
				unsigned char *cp1 = cp + 1;
				while (cp1 < cpend && *cp1)
					cp1++;
				if (cp1 >= cpend) {
					/***********************************************/
					/*   Give up if this is too large.	       */
					/***********************************************/
					if (cp1 - cp >= 128)
						break;
					return;
					}
				/***********************************************/
				/*   Write out prior to the command.	       */
				/***********************************************/
				pty_command(cur_ctw, (char *) cp + 1, cp1 - cp - 1);
				len = cur_ctw->f_pty_ibuf.c_in - (cp1 + 1 - cur_ctw->f_pty_ibuf.c_buf);
				memmove(cur_ctw->f_pty_ibuf.c_buf,
					cp1 + 1,
					len);
				cur_ctw->f_pty_ibuf.c_in = len;
				cp = cur_ctw->f_pty_ibuf.c_buf - 1;
				cpend = cur_ctw->f_pty_ibuf.c_buf + len;
				}
			}
		}

	n = cur_ctw->f_pty_ibuf.c_in;
	stats.bytes += n;
	stats.tot_bytes += n;
	ctw_add_string((CtwWidget) cur_ctw->f_ctw, (char *) cur_ctw->f_pty_ibuf.c_buf, n);
	cb_reset(&cur_ctw->f_pty_ibuf);

	/***********************************************/
	/*   Allow us to auto-switch on input.	       */
	/***********************************************/
if(0) {
	if (!cur_ctw->f_active) {
		cur_ctw->f_active = TRUE;
		status_expose_callback(0, 0, 0);
		}
}
	if (auto_switch)
		switch_screen(cur_ctw->f_id);
}
/**********************************************************************/
/*   We can try and write some of our blocked up stuff to the PTY.    */
/**********************************************************************/
void
pty_output(fcterm_t *cur_ctw, int src, XtInputId *id)
{	int	ret;

	UNUSED_PARAMETER(src);
	UNUSED_PARAMETER(id);

	ret = write(cur_ctw->f_pty_fd, cur_ctw->f_pty_obuf, cur_ctw->f_pty_oused);
	if (ret == cur_ctw->f_pty_oused) {
		/***********************************************/
		/*   Managed to write the whole lot.	       */
		/***********************************************/
		cur_ctw->f_pty_oused = 0;
		free((void *) cur_ctw->f_pty_obuf);
		cur_ctw->f_pty_obuf = NULL;
		cur_ctw->f_input_id = 0;
		XtRemoveInput(*id);
		if (debug) {
			printf("fcterm(pid=%d): XtRemoveInput(%d)\n", (int) getpid(), (int) *id);
			}
		return;
		}
	/***********************************************/
	/*   Did we write any of it?		       */
	/***********************************************/
	if (ret > 0) {
		memcpy(cur_ctw->f_pty_obuf, 
			cur_ctw->f_pty_obuf + ret, 
			cur_ctw->f_pty_oused - ret);
		cur_ctw->f_pty_oused -= ret;
		}
}
/**********************************************************************/
/*   Send  a  command  to the virtual pty. Dont bother if its a real  */
/*   one.							      */
/**********************************************************************/
void
pty_write(fcterm_t *cur_ctw, char *buf, int len)
{
	if (cur_ctw->f_pty_server == FALSE)
		return;
	v_write(cur_ctw, "", 1);
	v_write(cur_ctw, buf, len);
}
void
reset_terminal(fcterm_t *cur_ctw)
{
  	ctw_add_string((CtwWidget) cur_ctw->f_ctw, "\033c\033(B\033[2J", 9);
}
/**********************************************************************/
/*   Function to handle notification of window being resized.	      */
/**********************************************************************/
void
resize_callback(Widget w, int type, ctw_callback_t *reason)
{	fcterm_t *cur_ctw = reason->client_data;
	Position	x, y;

	UNUSED_PARAMETER(type);

	XtTranslateCoords(top_level, 0, 0, &x, &y);
	main_x = x;
	main_y = y;

	set_window_size(cur_ctw, (CtwWidget) w, FALSE);
}
/**********************************************************************/
/*   Restart fcterm after saving state.				      */
/**********************************************************************/
void
restart_fcterm()
{	fcterm_t *ctwp;
	dstr_t dstr;
	char	buf[BUFSIZ];
	char	buf1[BUFSIZ];
	FILE	*fp;
	int	i, rows, cols;
	char	**new_argv;

	/***********************************************/
	/*   Get  the -geom switch at the front so we  */
	/*   can restore the size.		       */
	/***********************************************/
	new_argv = (char **) chk_alloc(sizeof(char *) * (prog_argc + 4));
	new_argv[0] = prog_argv[0];
	new_argv[1] = "-geom";
	ctw_get_geometry((CtwWidget) cur_ctw->f_ctw, &rows, &cols);
	sprintf(buf, "%dx%d+%d+%d", 
		cols, rows, main_x, main_y);
	new_argv[2] = chk_strdup(buf);
	for (i = 1; i < prog_argc; i++)
		new_argv[i+2] = prog_argv[i];
	new_argv[i+2] = NULL;

	dstr_init(&dstr, 1024);
	sprintf(buf, "main.x=%d\nmain.y=%d\n", main_x, main_y);
	dstr_add_mem(&dstr, buf, strlen(buf));

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_id == GHOST_ID)
			continue;
		sprintf(buf, "[ctw]\n");
		sprintf(buf + strlen(buf), "pty_fd=%d\n", ctwp->f_pty_fd);
		sprintf(buf + strlen(buf), "pty_server=%d\n", ctwp->f_pty_server);
		sprintf(buf + strlen(buf), "ttyname=%s\n", ctwp->f_ttyname);
		sprintf(buf + strlen(buf), "label=%s\n", ctwp->f_label);
		sprintf(buf + strlen(buf), "active=%d\n", ctwp->f_active);
		sprintf(buf + strlen(buf), "pid=%d\n", ctwp->f_pid);
		sprintf(buf + strlen(buf), "[pty]\n");
		dstr_add_mem(&dstr, buf, strlen(buf));
		ctw_save_state((CtwWidget) ctwp->f_ctw, &dstr);
		dstr_add_char(&dstr, '\n');
		fcntl(ctwp->f_pty_fd, F_SETFD, 0);
		}
/*	fwrite(DSTR_STRING(&dstr), 1, DSTR_SIZE(&dstr), stdout);*/

	/***********************************************/
	/*   Save  state  to a file - we are about to  */
	/*   exec, so not a lot we can do if the file  */
	/*   isnt there.			       */
	/***********************************************/
	snprintf(buf, sizeof buf, "%s/%s/fcterm-state-%d", 
		tmpdir, getenv("USER") ? getenv("USER") : "fcterm", getpid());
	if ((fp = fopen(buf, "w")) == NULL) {
		perror(buf);
		exit(0);
		}
	fwrite(DSTR_STRING(&dstr), 1, DSTR_SIZE(&dstr), fp);
	fclose(fp);

	sprintf(buf1, "FCTERM_STATE=%s", buf);
	putenv(buf1);

	dstr_free(&dstr);
	printf("**Restarting[%d] %s %s %s...\n", getpid(), new_argv[0], new_argv[1], new_argv[2]);
	execvp(new_argv[0], new_argv);
	printf("fcterm: Should not get here -- aborting\n");
	exit(0);
}

/**********************************************************************/
/*   Save the state of our terminal session.			      */
/**********************************************************************/
void
save_state()
{	int	num;
	fcterm_t	*ctwp;
	int	rows, cols;
	char	buf[BUFSIZ];
	char	path[PATH_MAX];
	int	n;
	FILE	*fp;

	snprintf(buf, sizeof buf, "%s/%s/fcterm-state.%s", 
		tmpdir,
		getenv("USER"),
		group_label);
	if (cur_ctw == NULL) {
		remove(buf);
		return;
		}

	if ((fp = fopen(buf, "w")) == NULL) {
		return;
	}

	ctw_get_geometry((CtwWidget) cur_ctw->f_ctw, &rows, &cols);
	fprintf(fp, "version=fcterm-state-v1\n");
	fprintf(fp, "pid=%d\n", getpid());
	fprintf(fp, "label=%s\n", group_label);
	fprintf(fp, "rows=%d\n", rows);
	fprintf(fp, "cols=%d\n", cols);
	fprintf(fp, "win=0x%lx\n", XtWindow(top_level));
	fprintf(fp, "pos=%d %d %d %d %d %d\n", main_x, main_y, 
		top_level->core.width,
		top_level->core.height,
		mwm_x_offset, mwm_y_offset);
	for (num = 0, ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_id == GHOST_ID)
			continue;
		num++;
		fprintf(fp, "screen %d\n", num);
		fprintf(fp, "  id=%d\n", ctwp->f_id);
		fprintf(fp, "  pid=%d\n", ctwp->f_pid);
		fprintf(fp, "  active=%d\n", ctwp->f_active);
		fprintf(fp, "  label=%s\n", ctwp->f_label);

		snprintf(buf, sizeof buf, "/proc/%d/cwd", ctwp->f_pid);
		n = readlink(buf, path, sizeof path);
		if (n < 0)
			fprintf(fp, "  cwd=<unavailable>\n");
		else {
			path[n] = '\0';
			fprintf(fp, "  cwd=%s\n", path);
			}
		fprintf(fp, "  tty=%s\n", ctwp->f_ttyname);
		fprintf(fp, "  end\n");
	}

	fclose(fp);

	save_state_id = XtAppAddTimeOut(app_con, 1000L, 
		(XtTimerCallbackProc) save_state, NULL);
}
/**********************************************************************/
/*   Routine  to  set the default attributes of the widget depending  */
/*   on whether user want SCO/ISC functionality.		      */
/**********************************************************************/
void
set_default_attributes(fcterm_t *cur_ctw)
{	int	len;
	int	*flags;
	int	attributes[64];

	/***********************************************/
	/*   Set   attributes  depending  on  command  */
	/*   line terminal type.		       */
	/***********************************************/
	len = ctw_get_attributes((CtwWidget) cur_ctw->f_ctw, &flags, &attr_names);
	memcpy((char *) attributes, (char *) flags, len * sizeof (int));
	ctw_set_attributes((CtwWidget) cur_ctw->f_ctw, attributes);
	ctw_set_emulation((CtwWidget) cur_ctw->f_ctw, emulator);
}
/**********************************************************************/
/*   Set mini status label.					      */
/**********************************************************************/
void
set_status_label(fcterm_t *ctwp, char *title)
{
	strncpy(ctwp->f_label, title, sizeof ctwp->f_label);
	ctwp->f_label[MAX_LABEL_SIZE - 1] = '\0';
	status_expose_callback(0, 0, 0);
}
/**********************************************************************/
/*   Set window manager title.					      */
/**********************************************************************/
void
set_title(fcterm_t *ctwp, int flags, char *title)
{	int	i, n;
	Arg	args[10];
	Widget w = ctwp->f_ctw;
	char	*name;
	char	buf[BUFSIZ];
static char *names[] = {XtNtitle, XtNiconName};

	if (title == NULL) {
		title = ctwp->f_title;
		if (title == NULL) {
			snprintf(buf, sizeof buf - 1, "(%s%d) fcterm -- %s -- PID: %d", 
				group_label,
				ctwp->f_id + 1, 
				ctwp->f_ttyname, 
				ctwp->f_pid);
			title = chk_strdup(buf);
			}
		}
	else if (ctwp->f_id != 1) {
		snprintf(buf, sizeof buf - 1, "(%s%d) %s", 
			group_label, ctwp->f_id + 1, title);
		title = chk_strdup(buf);
		}

	while (XtParent(w))
		w = XtParent(w);

	for (i = 0; ctwp == cur_ctw && i < (int) (sizeof names / sizeof names[0]); i++) {
		switch (i) {
		  case 0:
		  	if ((flags & CHANGE_TITLE) == 0)
				continue;
		  	break;
		  case 1:
		  	if ((flags & CHANGE_ICON) == 0)
				continue;
		  	break;
		  }
		n = 0;
		XtSetArg(args[n], names[i], &name); n++;
		XtGetValues(w, args, n);
		if (strcmp(name, title) == 0)
			continue;
		n = 0;
		XtSetArg(args[n], names[i], chk_strdup(title)); n++;
		XtSetValues(w, args, n);
		}
	if (ctwp->f_title == NULL)
		ctwp->f_title = chk_strdup(title);
	if (ctwp->f_title != title) {
		chk_free_ptr((void **) &ctwp->f_title);
		ctwp->f_title = chk_strdup(title);
		}
}
/**********************************************************************/
/*   Trap certain types of signals.				      */
/**********************************************************************/
void
setup_signal_handlers()
{
	xchild_id = XtAppAddSignal(app_con, process_sigchld, NULL);

	signal(SIGCHLD, child_handler);
	signal(SIGINT, int_handler);
	signal(SIGTERM, int_handler);
	signal(SIGHUP, int_handler);
}
/**********************************************************************/
/*   Show the tty minimap.					      */
/**********************************************************************/
void
show_map(void *p, unsigned long *v)
{	Window	save_win;
	fcterm_t	*ctwp;
	int	w, h;
	int	x, y, ybot, r, c;
	XImage	*image, *new_image = NULL;
	Display		*dpy = XtDisplay(top_level);
	GC	gc = 0;
	float	y_frac, yf;
	int	num;

	UNUSED_PARAMETER(p);
	UNUSED_PARAMETER(v);

	if (map_pixmap) {
		XFreePixmap(dpy, map_pixmap);
		map_pixmap = 0;
		}

	for (num = 0, ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_id == GHOST_ID)
			continue;
		num++;
		/***********************************************/
		/*   Use any GC - they are all the same.       */
		/***********************************************/
		gc = ctw_get_gc((CtwWidget) ctwp->f_ctw);
		}
	if (num <= 1)
		return;

	ctw_get_size((CtwWidget) cur_ctw->f_ctw, &w, &h);
	if (num & 0x01)
		y_frac = 2. / (num + 1);
	else
		y_frac = 2. / num;
	map_pixmap = XCreatePixmap(dpy, XtWindow(top_level),
		cur_ctw->f_ctw->core.width, cur_ctw->f_ctw->core.height,
		DefaultDepth(dpy, DefaultScreen(dpy)));

	/***********************************************/
	/*   Clean out the image.		       */
	/***********************************************/
	new_image = XGetImage(dpy, map_pixmap, 0, 0, w, h, AllPlanes, ZPixmap);
	for (x = 0; x < w; x++) {
		for (y = 0; y < h; y++) {
			XPutPixel(new_image, x, y, 0);
			}
		}
	XFillRectangle(dpy, map_pixmap, gc, 0, 0, 
		cur_ctw->f_ctw->core.width, cur_ctw->f_ctw->core.height);
	switch_screen(GHOST_ID);

	x = y = 0;
//printf("%s start\n", time_str2());
	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_id == GHOST_ID)
			continue;
		save_win = ctwp->f_ctw->core.window;
		ctwp->f_ctw->core.window = map_pixmap;
		/***********************************************/
		/*   Force redraw to the pixmap.	       */
		/***********************************************/
		ctw_set_pixmap_mode((CtwWidget) ctwp->f_ctw, TRUE);
		ctw_redraw((CtwWidget) ctwp->f_ctw);
		ctw_set_pixmap_mode((CtwWidget) ctwp->f_ctw, FALSE);

//printf("%s %d %s Getting image pixmap=%x %dx%d %dx%d (%d,%d)\n", time_str(), ctwp->f_id, ctwp->f_label, map_pixmap, w, h, cur_ctw->f_ctw->core.width, cur_ctw->f_ctw->core.height, x, y);
		image = XGetImage(dpy, map_pixmap, 0, 0, w, h, AllPlanes, ZPixmap);

		ybot = y + h * y_frac;
		ybot = MIN(h, ybot);
		ctwp->mm_x = x;
		ctwp->mm_y = y;
		ctwp->mm_w = w / 2;
		ctwp->mm_h = h * y_frac;
		for (r = y; r < ybot; r++) {
			int c1 = x + w/2;
			for (c = x; c < c1; c++) {
				int xs = (c - x) * 2;
				int ys = (r - y) / y_frac;
//xs = MIN(xs, w - 1);
//ys = 0;
//if (xs >= w || ys >= h) printf("(%d,%d) %d,%d -> %d,%d\n", w, h, r - y, c  - x, ys, xs);
				XPutPixel(new_image, c, r, 
					XGetPixel(image, xs, ys));
/*if (new_image->width < 10 || new_image->height < 10) {
printf("overflow: %dx%d %d,%d\n", w, h, c, r);
abort();
}*/
				}
			}

		XDestroyImage(image);
		ctwp->f_ctw->core.window = save_win;

		x += w / 2;
		if (x >= w) {
			x = 0;
			y += h * y_frac;
			}
		}
//printf("%s end\n", time_str2());

	XPutImage(dpy, map_pixmap, gc, new_image, 0, 0, 0, 0, w, h);
	XDrawLine(dpy, map_pixmap, gc, w / 2, 0, w / 2, h);
	for (yf = h * y_frac; yf < h; yf += h * y_frac) {
		XDrawLine(dpy, map_pixmap, gc, 0, yf, w , yf);
		}

	XCopyArea(dpy, map_pixmap, XtWindow(cur_ctw->f_ctw), gc,
		0, 0, w, h, 0, 0);
	if (new_image)
		XDestroyImage(new_image);

	map_timer_id = XtAppAddTimeOut(app_con, 500L, show_map, NULL);
}
/**********************************************************************/
/*   Grab  switches  before  X  sees  them. We need to grab geometry  */
/*   before  X  sees  them because we want geometry to be treated in  */
/*   width*cols  and  we want it to apply to the VT100 widget -- not  */
/*   necessarily the application shell.				      */
/**********************************************************************/
int
steal_switches(int argc, char **argv)
{	int	i, j;
	char	*cp;

	prog_name = find_exec_path(getenv("PATH"), argv[0]);

	for (i = 1; i < argc; ) {
		if (strcmp(argv[i], "-isc") == 0) {
			term_str = "TERM=AT386";
			foxterm_str = "FOXTERM=isc";
			bterm_str = "CRTERM=isc";
			emulator = CTW_EMULATE_ISC;
			widget_name = "iscterm";
			}
		else if (strcmp(argv[i], "-sco") == 0) {
			term_str = "TERM=AT386";
			foxterm_str = "FOXTERM=sco";
			bterm_str = "CRTERM=sco";
			emulator = CTW_EMULATE_SCO;
			widget_name = "scoterm";
			}
		else if (strncmp(argv[i], "-geo", 4) == 0) {
			/***********************************************/
			/*   Leave  the  (x,y)  co-ords  intact. Just  */
			/*   steal the window size.		       */
			/***********************************************/
			if (argv[++i][0] >= '0' && argv[i][0] <= '9') {
				geometry = argv[i];
				for (cp = argv[i]; *cp && *cp != '-' && *cp != '+'; )
					cp++;
				if (*cp == '\0') {
					/***********************************************/
					/*   If  we  only  have WxH then delete whole  */
					/*   argument.				       */
					/***********************************************/
					for (j = --i; j < argc; j++) {
						argv[j] = argv[j+1];
						}
					argc--;
					}
				else
					argv[i] = cp;
				}
			i++;
			continue;
			}
		else {
			i++;
			continue;
			}
		for (j = i; j < argc; j++) {
			argv[j] = argv[j+1];
			}
		argc--;
		}
	return argc;
	
}
/**********************************************************************/
/*   Function to create a child shell process.			      */
/**********************************************************************/
int
spawn_child(fcterm_t *cur_ctw, char *pwd)
{	char	buf[4096];
static char	ctw_pid[32];
	char	tty[BUFSIZ];
	char	tty_name[BUFSIZ];
	int	one = 1;
	char	*minus_name = (char *) NULL;
	int	n;
	Arg	args[20];
	int	tty_fd;
	char	*cp;
	int open_max = -1;
	
	if (open_max < 0)
		open_max = sysconf (_SC_OPEN_MAX);

	putenv("FCTERM_TERM=1");
	snprintf(ctw_pid, sizeof ctw_pid, "FCTERM_PID=%d", getpid());
	putenv(ctw_pid);

	cur_ctw->f_pty_fd = -1;

	{static char buf[64];
	snprintf(buf, sizeof buf, "FCTERM_LABEL=%s.%d", group_label, cur_ctw->f_id);
	putenv(buf);
	}

	{static char buf[32];
	snprintf(buf, sizeof buf, "PTY_SCREEN=%d", cur_ctw->f_id);
	putenv(buf);
	}
	/***********************************************/
	/*   Get  current  child  window size in case  */
	/*   we   need   to   set  the  LINES/COLUMNS  */
	/*   environment variables.		       */
	/***********************************************/
	n = 0;
	XtSetArg(args[n], XtNrows, &cur_ctw->f_rows); n++;
	XtSetArg(args[n], XtNcolumns, &cur_ctw->f_cols); n++;
	XtGetValues(cur_ctw->f_ctw, args, n);

	if (!direct && (cp = getenv("PTY_SERVER")) != NULL) {
		/***********************************************/
		/*   See if we can use the pty server.	       */
		/***********************************************/
		char host[BUFSIZ];
		char port[BUFSIZ];
		cur_ctw->f_pty_fd = -1;
		if (mysscanf(cp, "%[^:]:%s", host, port) == 2) {
			int p = tcp_get_port_address(port);
			if (p == 0) {
				printf("Invalid PTY_SERVER setting - port is zero\n");
				exit(1);
				}
			cur_ctw->f_pty_fd = tcp_connect(0, 0, 0, tcp_get_ipaddr(host), p);
			}
		else {
			cur_ctw->f_pty_fd = tcp_connect(0, 0, 0, tcp_get_ipaddr(cp), PTY_SERVER_PORT);
			}
		if (cur_ctw->f_pty_fd >= 0) {
			signal(SIGPIPE, SIG_IGN);
			cp = getenv("PTY_LABEL");
			snprintf(buf, sizeof buf - 1, 
				"%s uid=%d name=%s-%d env=PTY_SCREEN=%d", 
				cp ? "reopen" : "open",
				getuid(), 
				cp ? cp : "fcterm",
				cur_ctw->f_id + 1,
				cur_ctw->f_id + 1);
			if (cp) {
				sprintf(buf + strlen(buf), " PTY_LABEL=");
				while (*cp) {
					if (*cp == ' ')
						strcat(buf, "_");
					else {
						int len = strlen(buf);
						buf[len++] = *cp;
						buf[len] = '\0';
						}
					cp++;
					}
				}
			putenv("PTY_LABEL=");
			write(cur_ctw->f_pty_fd, "", 1);
			write(cur_ctw->f_pty_fd, buf, strlen(buf) + 1);
			sprintf(buf, "winsz rows=%d cols=%d", cur_ctw->f_rows, cur_ctw->f_cols);
			write(cur_ctw->f_pty_fd, "", 1);
			write(cur_ctw->f_pty_fd, buf, strlen(buf) + 1);
			cur_ctw->f_pty_server = TRUE;
			return 0;
			}
		else {
			if (verbose)
				printf("Failed to connect to pty server - using normal pty's\n");
			cur_ctw->f_pty_server = FALSE;
			}
		}

# if defined(STREAMS_PTY)

	signal(SIGCHLD, SIG_DFL);

	cur_ctw->f_pty_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
	if (cur_ctw->f_pty_fd < 0) {
		perror("/dev/ptmx");
		exit(1);
		}

	/***********************************************/
	/*   Dont  let  child inherit our controlling  */
	/*   pty.				       */
	/***********************************************/
	fcntl(cur_ctw->f_pty_fd, F_SETFD, 1);

	if (grantpt(cur_ctw->f_pty_fd) < 0) {
		perror("grantpt");
		exit(1);
		}
	if (unlockpt(cur_ctw->f_pty_fd) < 0) {
		perror("unlockpt");
		exit(1);
		}
	if ((slavename = ptsname(cur_ctw->f_pty_fd)) == (char *) NULL) {
		perror("ptsname");
		exit(1);
		}
	strncpy(cur_ctw->f_ttyname, slavename, sizeof cur_ctw->f_ttyname);
	strncpy(cur_ctw->f_label, basename(cur_ctw->f_ttyname), sizeof cur_ctw->f_label);
	signal(SIGCHLD, child_handler);
	/***********************************************/
	/*   Drop  any  privileges  we may have becos  */
	/*   we  might  have  been made setuid/setgid  */
	/*   on execution.			       */
	/***********************************************/
	setuid(getuid());
	setgid(getgid());
	fcntl(cur_ctw->f_pty_fd, F_SETFD, FD_CLOEXEC);

	do_utmp(cur_ctw, FALSE);

	switch (cur_ctw->f_pid = fork()) {
	  case -1:
	  	perror("fork");
		exit(1);
	  case 0: {
	  	TERM_STRUCT t, t1;
		char	*shell = getenv("SHELL");
		char	*shname;
		int	i;

		close(ConnectionNumber(XtDisplay(top_level)));
		if (pwd) {
			chdir(pwd);
			}

		/***********************************************/
		/*   Reset the signals.			       */
		/***********************************************/
		for (i = 1; i < 32; i++)
			signal(i, SIG_DFL);

# if 1
		if ((tty_fd = open(slavename, O_RDWR)) < 0)
# else
		if ((tty_fd = open(slavename, O_RDWR | O_NOCTTY)) < 0 ||
		   ioctl(tty_fd, I_PUSH, "ptem") < 0 ||
		   ioctl(tty_fd, I_PUSH, "ldterm") < 0 ||
		   ioctl(tty_fd, I_PUSH, "ttcompat") < 0)
# endif
			{
		   	fprintf(stderr, "Error creating pty\n");
			perror(slavename);
			return -1;
			}
		strcpy(tty_name, slavename);
# else
		{int letter1, letter2;
		char	pty_name[BUFSIZ];

		slavename = "/dev/dont-know";
		for (letter1 = 'p'; letter1 < 'z'; letter1++) {
			for (letter2 = 0; letter2 < 16; letter2++) {
				sprintf(pty_name, "/dev/pty%c%x", letter1, letter2);
				if ((cur_ctw->f_pty_fd = open(pty_name, O_RDWR)) >= 0) {
					char	buf[BUFSIZ];

					sprintf(tty_name, "/dev/tty%c%x", letter1, letter2);
					slavename = malloc(strlen(pty_name) + 1);
					strcpy(slavename, pty_name);

					/***********************************************/
					/*   Check  we  can open the tty side. If not  */
					/*   scan  for  another  pty.  Sometimes  the  */
					/*   terminals are left in strange states.     */
					/***********************************************/
					tty_fd = open(tty_name, O_RDWR);
					if (tty_fd >= 0) {
						strncpy(cur_ctw->f_ttyname, tty_name, sizeof cur_ctw->f_ttyname);
						close(tty_fd);
						goto found_pty;
						}
					fprintf(stderr, "Warning: %s could not be opened - trying next tty\n", tty_name);
					sprintf(buf, "ls -l %s %s", pty_name, tty_name);
					system(buf);
					close(cur_ctw->f_pty_fd);
					}
				}
			}
		fprintf(stderr, "fcterm: Couldn't find a free pty\n");
		return -1;
	found_pty: ;
		}
# endif
		if (shell == NULL)
			shell = "/bin/sh";

		if ((shname = strrchr(shell, '/')) == (char *) NULL)
			shname = shell;
		else
			shname++;

		putenv(term_str);
		putenv(foxterm_str);
		putenv(bterm_str);

# if !defined(TIOCSWINSZ)
		{
		char	lines_buf[64];
		char	columns_buf[64];
		/***********************************************/
		/*   Brain-dead  systems  which  dont support  */
		/*   window sizes.			       */
		/***********************************************/
		sprintf(lines_buf, "LINES=%d", rows);
		sprintf(columns_buf, "COLUMNS=%d", cols);
		putenv(lines_buf);
		putenv(columns_buf);
		}
# endif /* defined(TIOCSWINSZ) */
	  	close(0);
	  	close(1);
	  	close(2);

		sprintf(tty, "TTY=%s", tty_name);
		putenv(tty);
		if (setsid() < 0) {
			perror("setsid");
			}
# if SYSV_SETPGRP
#	if !defined(STREAMS_PTY)
		setpgrp();
#	endif
# else
		setpgrp(getpid(), 0);
# endif
		tty_fd = open(tty_name, O_RDWR);
		if (tty_fd < 0) {
			perror(tty_name);
			exit(1);
			}

		tcgetattr(0, &t1);

		if (tty_fd != 0)
			dup2(tty_fd, 0);
		if (tty_fd != 1)
			dup2(tty_fd, 1);
		if (tty_fd != 2)
			dup2(tty_fd, 2);
		tcgetattr(0, &t);
		t.c_cflag &= ~PARENB;
		t.c_cflag |= CS8;
		t.c_oflag &= ~TAB3;
		t.c_oflag |= OPOST | ONLCR;
		t.c_iflag |= IGNPAR | ICRNL;
		t.c_lflag = ISIG | ICANON;
# if defined(ECHOCTL)
		t.c_lflag |= ECHOCTL | ECHOKE;
# endif
# if defined(IEXTEN)
		t.c_lflag |= IEXTEN;
# endif
		t.c_lflag |= ECHO | ECHOE | ECHOK;

		t.c_cc[VINTR] = 'c' & 0x1f;
		t.c_cc[VINTR] = 'c' & 0x1f;
		t.c_cc[VQUIT] = t1.c_cc[VQUIT];
		t.c_cc[VERASE] = '\b';
		t.c_cc[VKILL] = 'u' & 0x1f;
# if defined(VSUSP)
		t.c_cc[VSUSP] = 'z' & 0x1f;
# endif

		n = tcsetattr(0, TCSANOW, &t);

# if defined(TIOCCONS)
		if (console_flag) {
			char	buf[64];
			if (ioctl(tty_fd, TIOCCONS, (char *) &one) >= 0) {
				sprintf(buf, "CTW_CONSOLE=(console)");
				putenv(strdup(buf));
				}
			else {
				perror("TIOCCONS");
				}
			}
# endif

		signal(SIGTERM, SIG_DFL);
		/***********************************************/
		/*   If  login  shell  requested then add the  */
		/*   '-' to the shell name.		       */
		/***********************************************/
		if (login_shell_flag) {
			minus_name = malloc(strlen(shname) + 2);
			minus_name[0] = '-';
			strcpy(minus_name + 1, shname);
			}
		/***********************************************/
		/*   Add in the virtual screen id.	       */
		/***********************************************/
	        for (i = 3; i < open_max; i++)
        	    close(i);

	    	if (command_to_exec) {
			execvp(*command_to_exec, command_to_exec);
			perror("execvp");
			}
		else {
			execlp(shell, 
				login_shell_flag ? minus_name : shell, 
				"-i", (char *) 0);
			perror("execlp");
			}
		_exit(1);
		}
	  default:
	  	/***********************************************/
	  	/*   Protect parent from possible death.       */
	  	/***********************************************/
		signal(SIGINT, SIG_IGN);
	  	break;
	  }
	return 0;
}
/**********************************************************************/
/*   Handle a new-style scrollbar.				      */
/**********************************************************************/
void
sbar_callback(Widget widget, fcterm_t *dummy_cur_ctw, struct scrbar_callback *sbarp)
{	int	n;
	Arg	args[20];
	int	rows;
	int	top_line;
	int	tot_rows;

	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(dummy_cur_ctw);

	n = 0;
	XtSetArg(args[n], XtNrows, &rows); n++;
	XtGetValues(cur_ctw->f_ctw, args, n);
	top_line = ctw_get_top_line((CtwWidget) cur_ctw->f_ctw);
	tot_rows = ctw_get_total_rows((CtwWidget) cur_ctw->f_ctw);

	switch (sbarp->sb_reason) {
	  case SBAR_PAGE_UP:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line - rows);
		break;
	  case SBAR_PAGE_DOWN:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line + rows);
		break;
	  case SBAR_LINE_UP:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line - 1);
		break;
	  case SBAR_LINE_DOWN:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			top_line + 1);
		break;
	  case SBAR_START:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 0);
	  	break;
	  case SBAR_END:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, tot_rows);
	  	break;
	  case SBAR_ABSOLUTE:
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, 
			(int) (sbarp->sb_pos * tot_rows));
	  	break;
	  }
}
/**********************************************************************/
/*   Handle a jump in the vertical scrollbar.			      */
/**********************************************************************/
void
jump_callback(Widget widget, fcterm_t *dummy_cur_ctw, void *call_data)
{
	float	top = *(float *) call_data;
	int	tot_rows;

	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(dummy_cur_ctw);
		
	tot_rows = ctw_get_total_rows((CtwWidget) cur_ctw->f_ctw);

	ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, (int) (tot_rows * top));
}
/**********************************************************************/
/*   Handle a user moving the slider in the vertical scrollbar.	      */
/**********************************************************************/
void
scroll_callback(Widget widget, fcterm_t *dummy_cur_ctw, void *call_data)
{	int	n;
	Arg	args[20];
	int	scroll_fwd = (int) (long) call_data > 0 ? TRUE : FALSE;
	int	num_rows;
	int	top_line;

	UNUSED_PARAMETER(widget);
	UNUSED_PARAMETER(dummy_cur_ctw);

	n = 0;
	XtSetArg(args[n], XtNrows, &num_rows); n++;
	XtGetValues(cur_ctw->f_ctw, args, n);
	top_line = ctw_get_top_line((CtwWidget) cur_ctw->f_ctw);

	/***********************************************/
	/*   Dont   scroll   the  screen  whilst  the  */
	/*   scrollbar is scrolling.		       */
	/***********************************************/
	ctw_freeze_display((CtwWidget) cur_ctw->f_ctw, TRUE);

	if (scroll_fwd)
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, top_line + num_rows);
	else
		ctw_set_top_line((CtwWidget) cur_ctw->f_ctw, top_line - num_rows);
}
/**********************************************************************/
/*   Set application window size.				      */
/**********************************************************************/
void
set_appl_window_size(fcterm_t *cur_ctw, int r, int c)
{
 	int	fht, fwd;

	do_wm_hints(top_level, cur_ctw->f_ctw, FALSE);
   	get_font_size(cur_ctw, &fwd, &fht);
   	XtVaSetValues(pane,
 		XtNheight,	fht * r + STATUS_HEIGHT,
 		XtNwidth,	fwd * c + SCRBAR_WIDTH + 1,
 		NULL);
	XFlush(XtDisplay(top_level));
	do_wm_hints(top_level, cur_ctw->f_ctw, TRUE);
	XFlush(XtDisplay(top_level));
}
/**********************************************************************/
/*   Tell PTY that the window size has changed.			      */
/**********************************************************************/
void
set_window_size(fcterm_t *cur_ctw, CtwWidget w, int force)
{	int	n;
	Arg	args[20];
	char	buf[BUFSIZ];
	static struct winsize	winsize;
	fcterm_t *ctwp;

	n = 0;
	XtSetArg(args[n], XtNrows, &cur_ctw->f_rows); n++;
	XtSetArg(args[n], XtNcolumns, &cur_ctw->f_cols); n++;
	XtGetValues((Widget) w, args, n);

	if (!force && winsize.ws_row == cur_ctw->f_rows && winsize.ws_col == cur_ctw->f_cols)
		return;

	/***********************************************/
	/*   Propagate this to all the windows.	       */
	/***********************************************/
	winsize.ws_row = cur_ctw->f_rows;
	winsize.ws_col = cur_ctw->f_cols;
	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_pty_fd < 0)
			continue;

		if (ctwp->f_pty_server) {
			snprintf(buf, sizeof buf, "winsz rows=%d cols=%d", cur_ctw->f_rows, cur_ctw->f_cols);
			if (getenv("FCTERM_DEBUG"))
				printf("TX: %s\n", buf);
			write(cur_ctw->f_pty_fd, "", 1);
			write(cur_ctw->f_pty_fd, buf, strlen(buf) + 1);
			}
		else {
			ioctl(ctwp->f_pty_fd, TIOCSWINSZ, &winsize);
			/***********************************************/
			/*   Make sure shell knows about this.	       */
			/***********************************************/
			kill(ctwp->f_pid, SIGWINCH);
			}
		}
}
/**********************************************************************/
/*   Function  to  grab  a  snapshot  of everything scrolled away in  */
/*   the  history  buffer.  Because  its  a pain to cut and paste if  */
/*   you didnt turn logging on.					      */
/**********************************************************************/
void
snap_history(fcterm_t *cur_ctw)
{	FILE	*fp;
	char	buf[BUFSIZ];
	int	line_no;
	vbyte_t	*vp;
	int	col, end_col;
	int	n;
	int	j;
	Arg	args[20];

	sprintf(buf, "%s/CtwSnap.%d", log_dir, getpid());
	fp = fopen(buf, "w+");
	if (fp == NULL) {
		perror(buf);
		return;
		}
	n = 0;
	XtSetArg(args[n], XtNcolumns, &col); n++;
	XtGetValues(cur_ctw->f_ctw, args, n);

	for (line_no = 0; ; ) {
		if ((vp = ctw_get_line((CtwWidget) cur_ctw->f_ctw, line_no++)) == NULL)
			break;
		/***********************************************/
		/*   Start  from  right hand side of line and  */
		/*   delete trailing spaces.		       */
		/***********************************************/
		for (end_col = col - 1; end_col >= 0; end_col--)
			if (vp[end_col].vb_byte != ' ') {
				end_col++;
				break;
				}
		for (j = 0; j < end_col; j++) {
			fputc(vp[j].vb_byte, fp);
			}
		fputc('\n', fp);
		}
	fclose(fp);
	printf("Appended %d lines of history buffer appended to %s\n", 
		line_no - 1, buf);
}
/**********************************************************************/
/*   Dump the screens to a visible file.			      */
/**********************************************************************/
void
dump_screens()
{	FILE	*fp;
	int	fd;
	char	buf[BUFSIZ];
	int	line_no;
	vbyte_t	*vp;
	int	col, end_col, rows;
	int	n;
	int	j;
	Arg	args[20];
	fcterm_t *ctwp;
	dstr_t dstr;

	if (dump_flag == FALSE)
		return;

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (ctwp->f_ttyname[0] == '\0')
			continue;

		dstr_init(&dstr, 4096);

		n = 0;
		XtSetArg(args[n], XtNcolumns, &col); n++;
		XtSetArg(args[n], XtNrows, &rows); n++;
		XtGetValues(cur_ctw->f_ctw, args, n);

		for (line_no = 0; ; ) {
			if ((vp = ctw_get_line((CtwWidget) cur_ctw->f_ctw, line_no++)) == NULL)
				break;
			/***********************************************/
			/*   Start  from  right hand side of line and  */
			/*   delete trailing spaces.		       */
			/***********************************************/
			for (end_col = col - 1; end_col >= 0; end_col--)
				if (vp[end_col].vb_byte != ' ') {
					end_col++;
					break;
					}
			for (j = 0; j < end_col; j++) {
				if (j == 0 || 
				    vp[j-1].vb_attr != vp[j].vb_attr) {
				    	dstr_add_mem(&dstr, vp[j].vb_attr & VB_LINE ?
						"\033(0" : "\033(B", 3);
				    	}
				if (j == 0 || 
				    vp[j-1].vb_fcolor != vp[j].vb_fcolor ||
				    vp[j-1].vb_bcolor != vp[j].vb_bcolor) {
				    	snprintf(buf, sizeof buf, "\033[3%d;4%dm",
						vp[j].vb_fcolor,
						vp[j].vb_bcolor);
					dstr_add_mem(&dstr, buf, strlen(buf));
				    	}
				dstr_add_char(&dstr, vp[j].vb_byte);
				}
			dstr_add_char(&dstr, '\n');
			}

		/***********************************************/
		/*   Write the file only if it didnt change.   */
		/***********************************************/
		sprintf(buf, "%s/CtwDump.%s.%s", log_dir, getenv("USER"),
			basename(ctwp->f_ttyname));
		if ((fd = open(buf, O_RDONLY)) >= 0) {
			dstr_t dstr1;
			int	n;
			int	diff = 0;
			char	buf[8192];

			dstr_init(&dstr1, 4096);
			while ((n = read(fd, buf, sizeof buf)) > 0) {
				dstr_add_mem(&dstr1, buf, n);
				}
			close(fd);
/*printf("size=%d %d\n", DSTR_SIZE(&dstr), DSTR_SIZE(&dstr1));*/
			if (DSTR_SIZE(&dstr) != DSTR_SIZE(&dstr1) ||
			    memcmp(DSTR_STRING(&dstr), DSTR_STRING(&dstr1),
			       DSTR_SIZE(&dstr)) != 0)
			       	diff = 1;
			dstr_free(&dstr1);
			if (diff == 0) {
				dstr_free(&dstr);
				continue;
				}
			}

		fp = fopen(buf, "w");
		if (fp == NULL) {
			perror(buf);
			}
		else {
			fwrite(DSTR_STRING(&dstr), DSTR_SIZE(&dstr), 1, fp);
			fclose(fp);
			}
		dstr_free(&dstr);
		}

	snap_id = XtAppAddTimeOut(app_con, 500L, 
		(XtTimerCallbackProc) dump_screens, NULL);
}
/**********************************************************************/
/*   Switch to a screen due to a keystroke.			      */
/**********************************************************************/
void
switch_screen(int id)
{	fcterm_t *ctwp;
	fcterm_t *t = NULL;

	if (cur_ctw && cur_ctw->f_id == id)
		return;

	for (ctwp = hd_ctw; ctwp; ctwp = ctwp->f_next) {
		if (id == ctwp->f_id) {
			t = ctwp;
			continue;
			}

		ctwp->f_active = FALSE;
		XtUnmanageChild(ctwp->f_ctw);
		}

	if (t == NULL) {
		/***********************************************/
		/*   Not there - create it.		       */
		/***********************************************/
		menu_new_screen(cur_ctw, id);
		return;
		}

	cur_ctw = t;

	switch_screen2();
}
/**********************************************************************/
/*   On  exit,  we need to force the current screen to be displayed.  */
/*   (process_sigchld callback)					      */
/**********************************************************************/
void
switch_screen2()
{
	ctw_callback_t	reason;

	cur_ctw->f_active = TRUE;
	XtManageChild(cur_ctw->f_ctw);

	XtSetKeyboardFocus(top_level, cur_ctw->f_ctw);
	set_title(cur_ctw, CHANGE_TITLE | CHANGE_ICON, NULL);
	status_expose_callback(0, 0, 0);

	/***********************************************/
	/*   Reset the scrollbar.		       */
	/***********************************************/
	reason.reason = CTWR_TOP_LINE;
	reason.client_data = cur_ctw->f_ctw;
	reason.top_line = ctw_get_top_line((CtwWidget) cur_ctw->f_ctw);
	top_callback(cur_ctw->f_ctw, XtNtopCallback, (void *) &reason);
}
/**********************************************************************/
/*   Write  stuff to the pty. Be careful about overflowing pty input  */
/*   buffer.							      */
/**********************************************************************/
void
v_write(fcterm_t *cur_ctw, char *buf, int len)
{	int	n;
	int	fl;

	/***********************************************/
	/*   May be trying to write to the ghost ctw.  */
	/***********************************************/
	if (cur_ctw == NULL)
		return;

	/***********************************************/
	/*   If stuff already buffered to go out then  */
	/*   stick this lot on the output queue.       */
	/***********************************************/
	if (cur_ctw->f_pty_oused) {
		if (len + cur_ctw->f_pty_oused > cur_ctw->f_pty_osize) {
			cur_ctw->f_pty_osize = cur_ctw->f_pty_oused + len;
			cur_ctw->f_pty_obuf = (char *) realloc(cur_ctw->f_pty_obuf, cur_ctw->f_pty_osize);
			}
		memcpy(&cur_ctw->f_pty_obuf[cur_ctw->f_pty_oused], buf, len);
		cur_ctw->f_pty_oused += len;
		return;
		}

	/***********************************************/
	/*   See if we can write without blocking.     */
	/***********************************************/
	fl = fcntl(cur_ctw->f_pty_fd, F_GETFL, 0);
	if ((fl & O_NDELAY) == 0)
		fcntl(cur_ctw->f_pty_fd, F_SETFL, fl | O_NDELAY);
	n = write(cur_ctw->f_pty_fd, buf, len);
	if (n == len)
		return;
	if (n > 0) {
		buf += n;
		len -= n;
		}
	/***********************************************/
	/*   Didnt  manage  to  write. Lets buffer it  */
	/*   up.				       */
	/***********************************************/
	cur_ctw->f_pty_osize = len;
	cur_ctw->f_pty_obuf = (char *) malloc(cur_ctw->f_pty_osize);
	cur_ctw->f_pty_oused = len;
	memcpy(cur_ctw->f_pty_obuf, buf, len);

	/***********************************************/
	/*   Put us on the callback list.	       */
	/***********************************************/
	cur_ctw->f_output_id = XtAppAddInput(app_con, cur_ctw->f_pty_fd, 
		(XtPointer) XtInputWriteMask, 
		(XtInputCallbackProc) pty_output, 
		(XtPointer) cur_ctw);
}
/**********************************************************************/
/*   Function  called  to  set  the  window  manager hints about the  */
/*   character  size  and  base  window size. Need to do this and do  */
/*   it properly especially for 'mwm' which is very picky.	      */
/**********************************************************************/
void
do_wm_hints(Widget top_level, Widget widget, int flag)
{	int	n;
	Arg	args[20];
	Dimension wf, hf, wc, hc;
	int	bw, bh, wi, hi;
	int	internal_width;
	int	internal_height;
	static int debug_wm = -1;

	if (flag == FALSE) {
		n = 0;
		XtSetArg(args[n], XtNheightInc, 0); n++;
		XtSetArg(args[n], XtNwidthInc, 0); n++;
		XtSetArg(args[n], XtNbaseHeight, 0); n++;
		XtSetArg(args[n], XtNbaseWidth, 0); n++;
		XtSetValues(top_level, args, n);
		return;
		}
	/***********************************************/
	/*   Allow debugging of window manager stuff.  */
	/***********************************************/
	if (debug_wm < 0) {
		debug_wm = getenv("CR_DEBUG_WM") != (char *) NULL;
		}
	if (!XtIsRealized(top_level))
		return;

	wf = hf = wc = hc = 0;

	n = 0;
	XtSetArg(args[n], XtNwidth, &wf); n++;
	XtSetArg(args[n], XtNheight, &hf); n++;
	XtGetValues(top_level, args, n);

	n = 0;
	XtSetArg(args[n], XtNwidth, &wc); n++;
	XtSetArg(args[n], XtNheight, &hc); n++;
	XtSetArg(args[n], XtNinternalWidth, &internal_width); n++;
	XtSetArg(args[n], XtNinternalHeight, &internal_height); n++;
	XtGetValues(widget, args, n);

	bw = wf - wc + 2 * internal_width;
	bh = hf - hc + 2 * internal_height;
	ctw_get_font_info((CtwWidget) widget, &wi, &hi);

	if (debug_wm > 0) {
		printf("WM: top.w=%d top.h=%d cr.w=%d cr.h=%d\n",
			wf, hf, wc, hc);
		printf("WM: baseWidth=%d baseHeight=%d widthInc=%d heightInc=%d\n",
			bw, bh, wi, hi);
		}

	/***********************************************/
	/*   If either increment is valid try setting  */
	/*   both  to  zero  before  setting to their  */
	/*   final  values.  This avoids a bug in mwm  */
	/*   which  stops  us  resizing  a  top-level  */
	/*   shell  or  transient  shell  below  some  */
	/*   random size for peel off windows.	       */
	/***********************************************/
	if (bw >= 0 || bh >= 0) {
		n = 0;
		XtSetArg(args[n], XtNbaseWidth, 0); n++;
		XtSetArg(args[n], XtNbaseHeight, 0); n++;
		XtSetArg(args[n], XtNheightInc, 0); n++;
		XtSetArg(args[n], XtNwidthInc, 0); n++;
		XtSetValues(top_level, args, n);
		}

	n = 0;
	if (bw >= 0 && wi >= 0) {
		XtSetArg(args[n], XtNbaseWidth, bw); n++;
		XtSetArg(args[n], XtNwidthInc, wi); n++;
		}
	if (bh >= 0 && hi >= 0) {
		XtSetArg(args[n], XtNbaseHeight, bh); n++;
		XtSetArg(args[n], XtNheightInc, hi); n++;
		}
	if (n)
		XtSetValues(top_level, args, n);
}
/**********************************************************************/
/*   Function to add or delete the utmp entry.			      */
/**********************************************************************/
void
do_utmp(fcterm_t *cur_ctw, int remove_flag)
{
# if defined(HAS_UTMP) || defined(HAS_UTMPX)
	char	buf[32];
	static char	*name;
	struct passwd *pwd;
	char	*tty_name;
	struct utmpx ut;

	if (utmp_flag == FALSE)
		return;

	if (name == (char *) NULL) {
		pwd = getpwuid(getuid());
		if (pwd)
			name = pwd->pw_name;
		else if ((name = getenv("LOGNAME")) == (char *) NULL) {
			sprintf(buf, "UID#%ld", (long) getuid());
			name = buf;
			}
		}
	memset((char *) &ut, 0, sizeof ut);
	strncpy(ut.ut_line, slavename + sizeof("/dev"), sizeof(ut.ut_line));

# if defined(DEAD_PROCESS)
	strncpy(ut.ut_user, name, sizeof(ut.ut_user));
	ut.ut_id[0] = 'f';
	ut.ut_id[1] = 'c';
	ut.ut_id[2] = 't';
	ut.ut_id[3] = 'e';
	ut.ut_pid = getpid();
	ut.ut_type = remove_flag ? DEAD_PROCESS : USER_PROCESS;
# if defined(HAS_UTMPX)
	time(&ut.ut_tv.tv_sec);
# else
	time(&ut.ut_time);
# endif
# endif /* DEAD_PROCESS */
	pututxline(&ut);
	endutxent();
# endif /* defined(HAS_UTMP) || defined(HAS_UTMPX) */

	UNUSED_PARAMETER(cur_ctw);
	UNUSED_PARAMETER(remove_flag);
}
void
usage()
{	char	*argv[2] = {"help", NULL};
	extern const char *usage_text;

	printf("version: %d.%d b%d\n", version_major, version_minor, version_build_no);
	fputs(usage_text, stderr);

	handle_commands(0, 1, argv);

	exit(1);
}
# if 0
void
usleep(int milli)
{	struct timespec ts;

	ts.tv_sec = milli / 1000;
	ts.tv_nsec = (milli % 1000) * 1000 * 1000;
	nanosleep(&ts, NULL);

}
# endif
/**********************************************************************/
/*   Get the current font size for the Ctw widget.		      */
/**********************************************************************/
void
get_font_size(fcterm_t *cur_ctw, int *fwidth, int *fheight)
{
	int	*flags;

	ctw_get_attributes((CtwWidget) cur_ctw->f_ctw, &flags, &attr_names);
	*fwidth = flags[CTW_FONT_SIZE] >> 16;
	*fheight = flags[CTW_FONT_SIZE] & 0xffff;
}
# if defined(UTMP_FILENAME)
int
pututxline(struct utmp *utp)
{       int     slot;
        int     fd;

        return -1;

        slot = ttyslot();
        fd = open(UTMP_FILENAME, O_WRONLY);

        if (fd < 0)
                return -1;
        lseek(fd, (long) (slot * sizeof(struct utmp)), 0);
        write(fd, (char *) utp, sizeof *utp);
        close(fd);
        return 0;
}
# endif
# if defined(NEED_STRDUP)
char *
strdup(char *str)
{	char *cp = malloc(strlen(str) + 1);

	if (cp)
		strcpy(cp, str);
	return cp;
}
# endif
/**********************************************************************/
/*   Stub function for the x11/widgets w_button.c file.		      */
/**********************************************************************/
void
set_focus()
{
}

