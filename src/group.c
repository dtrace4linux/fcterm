/**********************************************************************/
/*                                                                    */
/*      CRiSP - Programmable editor                                   */
/*      ===========================                                   */
/*                                                                    */
/*  File:          group.c                                            */
/*  Author:        P. D. Fox                                          */
/*  Created:       24 Aug 2014                                        */
/*                                                                    */
/*  Copyright (c) 2014, Foxtrot Systems Ltd                           */
/*                All Rights Reserved.                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*  Description:  Add grouping semantics for terminals                */
/*--------------------------------------------------------------------*/
/*  $Header: Last edited: 24-Aug-2014 1.1 $ */
/**********************************************************************/

# include	"fcterm.h"
# include	<dstr.h>
# include	<sys/stat.h>
# include	<sys/mman.h>
# include	<time.h>
# include	<limits.h>

#define	DEBUG	1
#define MAX_PROCS	128
static int max_procs = MAX_PROCS;
/**********************************************************************/
/*   Save config for this fcterm.				      */
/**********************************************************************/
typedef struct config_t {
	int	c_pid;
	int	c_pid_shell;
	unsigned long c_ptime;
	unsigned long c_ttime;
	int	c_x;
	int	c_y;
	int	c_dx;
	int	c_dy;
	int	c_width;
	int	c_height;
	long	c_winid;

	int	t_msg;
	int	tx, ty, tw, th;	/* Target x,y w*h */
	char	c_pwd[1024];
	} config_t;

#define	G_VERSION	4
typedef struct sh_config_t {
	int	s_version;
	int	s_struct_size;
	int	s_size;
	int	s_lock;
	config_t s_array[MAX_PROCS];

	/***********************************************/
	/*   Following is the payload.		       */
	/***********************************************/
	unsigned long s_time;
	int	s_this;
	int	s_pid;
	int	s_x;
	int	s_y;
	int	s_grouping;
	} sh_config_t;
static int cx, cy;
sh_config_t	*shp;
char		*group_label;
config_t	*cur_cfg;
int	group_debug;

extern Widget top_level;
extern int do_group;
extern char *log_dir;

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
void group_update(void);
void group_init2(int alloc);

void
group_init()
{
	group_init2(TRUE);
}
void
group_init2(int alloc)
{	int	fd;
	int	i;
static	char	label_buf[2];
	struct stat sbuf;
	char	buf[4096];
	char	*str;

	if (shp)
		return;

	if (getenv("FCTERM_GROUP_DEBUG"))
		group_debug = atoi(getenv("FCTERM_GROUP_DEBUG"));
	if (getenv("FCTERM_GROUP_MAX_PROCS"))
		max_procs = atoi(getenv("FCTERM_GROUP_MAX_PROCS"));

	snprintf(buf, sizeof buf, "%s/fcterm.shm", log_dir);

	if (stat(buf, &sbuf) == -1 ||
	    sbuf.st_size != sizeof *shp ||
	    (fd = open(buf, O_RDWR, 0666)) < 0) {
	    	unlink(buf);
		if ((fd = open(buf, O_RDWR | O_CREAT, 0666)) < 0) {
			perror(buf);
			return;
			}
		for (i = 0; i < (int) sizeof *shp; i++)
			write(fd, "", 1);
		}
	shp = (sh_config_t *) mmap(NULL, sizeof *shp,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shp->s_version != G_VERSION ||
	    shp->s_size != max_procs) {
		memset(shp, 0, sizeof *shp);
		shp->s_version = G_VERSION;
		shp->s_struct_size = sizeof *shp;
		shp->s_size = max_procs;
		}
	if (!alloc)
		return;

	for (i = 0; i < max_procs; i++) {
		if (shp->s_array[i].c_pid == 0)
			break;
		snprintf(buf, sizeof buf, "/proc/%d", shp->s_array[i].c_pid);
		if (stat(buf, &sbuf) < 0)
			break;
		}
	if (i >= max_procs)
		return;

	cur_cfg = &shp->s_array[i];
	shp->s_array[i].c_pid = getpid();
	snprintf(buf, sizeof buf, "/proc/%d", shp->s_array[i].c_pid);
	stat(buf, &sbuf);
	shp->s_array[i].c_ptime = sbuf.st_mtime;

	if (group_label == NULL) {
		group_label = label_buf;
		strcpy(label_buf, "A");
		label_buf[0] = i + 'A';
		}

	str = chk_makestr("FCTERM_LABEL=", group_label, NULL);
	putenv(str);

	/***********************************************/
	/*   Only enable grouping if positively asked  */
	/*   for.				       */
	/***********************************************/
	if (do_group)
		shp->s_grouping = TRUE;
}

/**********************************************************************/
/*   See if someone told us to move.				      */
/**********************************************************************/
void
group_config_poll(XtIntervalId *cfg_timer_id)
{
	int	x, y;

	*cfg_timer_id = XtAppAddTimeOut(
		XtWidgetToApplicationContext(top_level), 
		1000L, group_config_poll, (void *) cfg_timer_id);

	/***********************************************/
	/*   See   if   we   have   been   asked   to  */
	/*   move/resize.			       */
	/***********************************************/
	if (cur_cfg->t_msg) {
		printf("fcterm: move %s: %d,%d %dx%d\n", group_label,
			cur_cfg->tx, cur_cfg->ty,
			cur_cfg->tw, cur_cfg->th);
		cur_cfg->t_msg = FALSE;
		XtConfigureWidget(top_level, 
			cur_cfg->tx - 0*mwm_x_offset,
			cur_cfg->ty - 0*mwm_y_offset,
			cur_cfg->tw,
			cur_cfg->th,
			1
			);
		}
	if (!shp->s_grouping)
		return;

	if (shp->s_version != G_VERSION)
		return;
	if (shp->s_this == cur_cfg - shp->s_array)
		return;

	x = shp->s_x;
	y = shp->s_y;

	if (group_debug)
		printf("move %d,%d %d,%d => %d,%d\n", cx, cy, x, y, 
		cx + x - mwm_x_offset, cy + y - mwm_y_offset);
	if (x == 0 && y == 0)
		return;


	XtMoveWidget(top_level, 
		cx + x - mwm_x_offset, cy + y - mwm_y_offset);
	XSync(XtDisplay(top_level), 1);
}

void
group_enable(int n)
{
	shp->s_grouping = n;
}
/**********************************************************************/
/*   Restore the saved state of windows.			      */
/**********************************************************************/
int
group_restore(char *fname)
{	char	buf[BUFSIZ];
	FILE	*fp;
/*	config_t	*cfg;
	Window	*children;
	Window root;
	Window parent;
*/
	Display *dpy = XtDisplay(top_level);

	group_init2(FALSE);
	if ((fp = fopen(fname, "r")) == NULL) {
		printf("Cannot open %s - %s\n", fname, strerror(errno));
		return -1;
		}
	while (fgets(buf, sizeof buf, fp) != NULL) {
		unsigned char	id;
		int	pid;
		long	win;
		int	n, x, y, w, h;

		if (buf[0] == '\0' || buf[1] != ':')
			continue;
		if ((n = mysscanf(buf, "%c: pid=%d win=0x%lx pos=%d,%d size=%dx%d\n",
			&id, &pid, &win, &x, &y, &w, &h)) != 7) {
				printf("scanf error - got %d\n", n);
				continue;
			}
		if (id < 'A' || id >= 'Z')
			continue;

		printf("Moving %c: pid=%d win=0x%lx pos=%d,%d size=%dx%d\n",
			id, pid, win, x, y, w, h);

/*
		XQueryTree(XtDisplay(top_level), win,
			&root,
			&parent,
			&children,
			&nchildren);
printf("parent=%p\n", parent);
*/

		id -= 'A';
		{XWindowChanges ch;
		memset(&ch, 0, sizeof ch);
		ch.x = x;
		ch.y = y;
		ch.width = w;
		ch.height = h;
		XMoveResizeWindow(dpy, win, x, y, w, h);

		XSync(dpy, 1);
		}
#if 0
		cfg = &shp->s_array[id];
		cfg->tx = x;
		cfg->ty = y;
		cfg->tw = w;
		cfg->th = h;
		cfg->t_msg = TRUE;
#endif
	}
	fclose(fp);
	return 0;
}

int
group_status(int cmd)
{
	if (cmd == 0)
		return shp->s_grouping;

	return 0;
}
char *
group_status2()
{	int	i;
	char	buf[BUFSIZ];
	char	id[BUFSIZ];
	dstr_t dstr;
	struct stat sbuf;

	group_update();

	dstr_init(&dstr, 256);
	for (i = 0; i < max_procs; i++) {
		if (shp->s_array[i].c_pid == 0)
			continue;
		snprintf(buf, sizeof buf, "/proc/%d",
			shp->s_array[i].c_pid);
		if (stat(buf, &sbuf) < 0) {
			continue;
			}

		if (i < 26)
			snprintf(id, sizeof id, "%c", i + 'A');
		else
			snprintf(id, sizeof id, "%d", i);
		snprintf(buf, sizeof buf, "%s: pid=%d win=0x%lx pos=%d,%d size=%dx%d\r\n", 
			id,
			shp->s_array[i].c_pid, 
			shp->s_array[i].c_winid,
			shp->s_array[i].c_x, 
			shp->s_array[i].c_y, 
			shp->s_array[i].c_width,
			shp->s_array[i].c_height
			);
		dstr_add_mem(&dstr, buf, strlen(buf));

		snprintf(buf, sizeof buf, "   pwd=%s\r\n",
			shp->s_array[i].c_pwd);
		dstr_add_mem(&dstr, buf, strlen(buf));

		}
	dstr_add_char(&dstr, '\0');
	return DSTR_STRING(&dstr);
}

void
group_update()
{	int	i;
	int	n;
	char	buf[BUFSIZ];
	char	path[PATH_MAX];
	struct stat sbuf;
	uid_t	uid = getuid();

	for (i = 0; i < max_procs; i++) {
		if (shp->s_array[i].c_pid == 0)
			continue;

		snprintf(buf, sizeof buf, "/proc/%d",
			shp->s_array[i].c_pid);
		if (stat(buf, &sbuf) < 0) {
			continue;
			}
		if (sbuf.st_uid != uid)
			continue;

		strcat(buf, "/cwd");
		n = readlink(buf, path, sizeof path);
		if (n < 0)
			continue;
		path[n] = '\0';
		strncpy(shp->s_array[i].c_pwd, path, sizeof shp->s_array[i].c_pwd);
		}
}

void
group_write_config(long winid, int x, int y, int width, int height)
{	int	dx = x - cx;
	int	dy = y - cy;

	/***********************************************/
	/*   If we ran out of letters.		       */
	/***********************************************/
	if (cur_cfg == NULL)
		return;

//	if (cur_cfg->c_x == x && cur_cfg->c_y == y)
//		return;

	if (group_debug)
		printf("write_config: pos=%d,%d dx,dy=%d,%d\n", x, y, x - cx, y - cy);

	shp->s_lock = 1;

	cur_cfg->c_winid = winid;
	cur_cfg->c_x = x;
	cur_cfg->c_y = y;
	cur_cfg->c_dx = dx;
	cur_cfg->c_dy = dy;
	cur_cfg->c_width = width;
	cur_cfg->c_height = height;

	shp->s_this = cur_cfg - shp->s_array;
	shp->s_x = dx;
	shp->s_y = dy;
	cur_cfg->c_ttime = time(NULL);

	shp->s_lock = 0;

	cx = x;
	cy = y;
}

