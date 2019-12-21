/**********************************************************************/
/*   Include file common to the different terminal emulators.	      */
/**********************************************************************/
# include	"machine.h"
# include	<stdio.h>
# include	<fcntl.h>
# include	<string.h>
# include	<signal.h>
# include	<errno.h>
# include	<ctype.h>
#if !defined(COCOA)
# include	<X11/X.h>
# include	<X11/Intrinsic.h>
# include 	<X11/IntrinsicP.h>
# include	<X11/StringDefs.h>
# include	<X11/Shell.h>
#endif
# include	<dstr.h>
# include	"ctw.h"
# if defined(HAVE_TERMIOS)
#	include	<termios.h>
#	define	TERM_STRUCT	struct termios

#	if !defined(TIOCGWINSZ) && defined(HAVE_SYS_IOCTL_H)
#		include	<sys/ioctl.h>
#	endif

#	if defined(TCSANOW) && !defined(TCSETA)
#		define	TCSETA	TCSANOW
#	endif
# endif

# if defined(HAVE_TERMIO)
#	include	<termio.h>
#	define	TERM_STRUCT	struct termio
#	if defined(tcgetattr)
#		undef	tcgetattr
#		undef	tcsetattr
#	endif
#	define	tcgetattr(fd, ptr)	ioctl(fd, TCGETA, ptr)
#	define	tcsetattr(fd, opt, ptr)	ioctl(fd, TCSETA, ptr)
#	if !defined(TCSANOW)
#		define	TCSANOW
#	endif
# endif

# include	<unistd.h>
# if HAVE_STDLIB_H
# 	include	<stdlib.h>
# endif
# include	<memory.h>
# include	<chkalloc.h>

/*
# if defined(STREAMS_PTY)
#	include	<sys/stropts.h>
# endif
*/
# include	"ptysrv.h"

/*
# define	chk_zalloc(x)	calloc(x, 1)
# define	chk_free(x)	free(x)
# define	chk_strdup(x)	strdup(x)
# define	chk_free_ptr(x)	if (*(void **) (x)) free(x); *(void **) x = NULL
*/

# define	SCRBAR_WIDTH	16
# define	STATUS_HEIGHT	8

struct stats {
	long	tot_bytes;
	long	bytes;
	time_t	start_time;
	time_t	time;
	};
extern struct stats  stats, old_stats;

enum menu_items {
	TOGGLE_SCROLLBAR,
	RESET_CHARACTERS,
	DIALOG_BOX,
	ENABLE_AUTOSWITCH,
	MONITOR,
	SET_FONT,
	PASTE,
	SNAP_HISTORY,
	LINE_MENU_ITEM,
	EXIT_MENU_ITEM,
	LOWER_WINDOW,
	ZOOM_WINDOW,
	ABOUT_BOX,
	MENU_EOF,
	FORK_NEW_FCTERM,
	MENU_FORK_MWM,
	MENU_FORK_OLWM,
	MENU_PASTE_ALPHABET,
	MENU_NEW_SCREEN,
	MENU_SEARCH,
	MENU_TERMINAL_MAP,
	MARK_POSITION,
	LABEL_LINES,
	FONT_SMALLER,
	FONT_LARGER
	};

# if !defined(UNUSED_PARAMETER)
#	define	UNUSED_PARAMETER(x)	x = x
# endif

# if !defined(MIN)
#	define	MIN(a, b)	(a) < (b) ? (a) : (b)
# endif

# define	GHOST_ID	-100

/**********************************************************************/
/*   Color definitions.                                               */
/**********************************************************************/
# define	BACKGROUND_PIXEL	0
# define	GRAY_PIXEL		0
# define	BLACK_PIXEL		1
# define	TOP_SHADOW_PIXEL	2
# define	DIM_GRAY_PIXEL		2
# define	BOTTOM_SHADOW_PIXEL	3
# define	WHITE_PIXEL		3
# define	SELECTED_PIXEL		4
# define	GREEN_PIXEL		5
# define	TABLE_CELL_PIXEL	6
# define	WAVY_LINE_PIXEL		7
# define	TOOLTIP_BACKGROUND_PIXEL		8
# define	STATUS_CURRENT_PIXEL	9
# define	STATUS_FG_PIXEL		10
# define	STATUS_BG_PIXEL		11
# define	STATUS_ACTIVE_PIXEL	12

# define	MAX_OBJ_PIXELS	13
extern Pixel obj_pixels[MAX_OBJ_PIXELS];

/**********************************************************************/
/*   For setting window icon/title.				      */
/**********************************************************************/
# define	CHANGE_TITLE	0x01
# define	CHANGE_ICON	0x02

typedef struct fcterm_t {
	struct fcterm_t	*f_next;
	struct fcterm_t	*f_prev;
	int		f_pid;
	int		f_active;
	int		f_prompting;
	Widget		f_ctw;
	int		f_mark;
	int		f_rows;
	int		f_cols;
	int		f_pty_fd;
	int		f_pty_server;
	int		f_eof;
	char		f_ttyname[64];
# define	MAX_LABEL_SIZE	9
	char		f_label[MAX_LABEL_SIZE];
	XtInputId	f_input_id;
	XtInputId	f_output_id;
	int		f_id;		/* Ctrl-Shift-F */
	char		*f_title;
	/***********************************************/
	/*   PTY buffering.			       */
	/***********************************************/
	struct cbuf_t		f_pty_ibuf;
	char 		*f_pty_obuf;
	int		f_pty_oused;
	int		f_pty_osize;

	/***********************************************/
	/*   Co-ords for the minimap.		       */
	/***********************************************/
	int		mm_x, mm_y, mm_w, mm_h;
	} fcterm_t;

extern int	auto_switch;
extern char	*tmpdir;
extern char *application_name;
extern char *geometry;
extern char	*widget_name;
extern char	*prog_name;
extern int login_shell_flag;
extern int sun_function_keys;
extern fcterm_t	*hd_ctw;
extern int debug;

void	setup_signal_handlers PROTO((void));
void	input_callback();
void	appl_callback();
void	pty_input();
int	spawn_child(fcterm_t *, char *);
void	child_handler();
void	int_handler();
void	resize_callback();
void	set_window_size(fcterm_t *cur_ctw, CtwWidget w, int force);
int	steal_switches PROTO((int, char **));
int	do_switches PROTO((int, char **));
void	usage PROTO((void));
void	get_font_size PROTO((fcterm_t *, int *, int *));
void	create_icon_pixmap PROTO((void));
void	set_default_attributes PROTO((fcterm_t *));
void	inform_wm();
void	snap_history PROTO((fcterm_t *));
void	do_wm_hints PROTO((Widget, Widget, int));
void	set_appl_window_size PROTO((fcterm_t *, int, int));
fcterm_t *new_fcterm(int);
void close_screen(fcterm_t *ctwp);
void menu_new_screen(fcterm_t *old_ctw, int id);
GC create_gc(Display *dpy);
void set_autoswitch(int newval);
void	init_term(void);
void	v_write PROTO((fcterm_t *, char *, int));
void	pty_write PROTO((fcterm_t *, char *, int));
void set_title(fcterm_t *cur_ctw, int flags, char *title);
void cb_init(cbuf_t *cbp, int size);
void reset_terminal(fcterm_t *cur_ctw);
void map_click(Widget w, XtPointer client_data, XEvent *event);
void fork_new_terminal(fcterm_t *cur_ctw);
void fork_wmgr(fcterm_t *cur_ctw, int val);
void show_map(void *p, unsigned long *v);
void	do_utmp PROTO((fcterm_t *, int));
void set_font(char *font);
void restart_fcterm(void);
int	tcp_get_port_address(char *);
char	*group_status2(void);
void	group_init(void);
int	group_restore(char *);
void	group_write_config(long winid, int x, int y, int width, int height);
void handle_commands(int i, int argc, char **argv);
void status_expose_callback(Widget widget, XtPointer client_data, XtPointer call_data);
char	*time_str();
char	*time_str2();
int	mysscanf(char *, char *, ...);
char	*time_string(void);
void switch_screen(int id);
void switch_screen2(void);

extern char	*log_dir;
extern GC	gc;
extern Pixmap map_pixmap;
extern int	mwm_x_offset;
extern int	mwm_y_offset;

