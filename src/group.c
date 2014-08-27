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
# include	<sys/stat.h>
# include	<sys/mman.h>
# include	<time.h>

#define	DEBUG	1
#define MAX_PROCS	26
/**********************************************************************/
/*   Save config for this fcterm.				      */
/**********************************************************************/
typedef struct config_t {
	int	c_pid;
	unsigned long c_ptime;
	unsigned long c_ttime;
	int	c_x;
	int	c_y;
	int	c_dx;
	int	c_dy;
	} config_t;

#define	G_VERSION	2
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
extern int mwm_x_offset;
extern int mwm_y_offset;

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/

void
group_init()
{	int	fd;
	int	i;
static	char	label_buf[2];
	struct stat sbuf;
	char	buf[4096];

	if (getenv("FCTERM_GROUP_DEBUG"))
		group_debug = atoi(getenv("FCTERM_GROUP_DEBUG"));

	group_label = label_buf;
	strcpy(label_buf, "A");
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
	if (shp->s_version != G_VERSION) {
		memset(shp, 0, sizeof *shp);
		shp->s_version = G_VERSION;
		shp->s_struct_size = sizeof *shp;
		shp->s_size = MAX_PROCS;
		}

	for (i = 0; i < MAX_PROCS; i++) {
		if (shp->s_array[i].c_pid == 0)
			break;
		snprintf(buf, sizeof buf, "/proc/%d", shp->s_array[i].c_pid);
		if (stat(buf, &sbuf) < 0)
			break;
		}
	if (i >= MAX_PROCS)
		return;

	cur_cfg = &shp->s_array[i];
	shp->s_array[i].c_pid = getpid();
	snprintf(buf, sizeof buf, "/proc/%d", shp->s_array[i].c_pid);
	stat(buf, &sbuf);
	shp->s_array[i].c_ptime = sbuf.st_mtime;

	label_buf[0] = i + 'A';

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
int
group_status()
{
	return shp->s_grouping;
}
void
group_write_config(int x, int y)
{	int	dx = x - cx;
	int	dy = y - cy;

//	if (cur_cfg->c_x == x && cur_cfg->c_y == y)
//		return;

	if (group_debug)
		printf("write_config: pos=%d,%d dx,dy=%d,%d\n", x, y, x - cx, y - cy);

	shp->s_lock = 1;

	cur_cfg->c_x = x;
	cur_cfg->c_y = y;
	cur_cfg->c_dx = dx;
	cur_cfg->c_dy = dy;
	shp->s_this = cur_cfg - shp->s_array;
	shp->s_x = dx;
	shp->s_y = dy;
	cur_cfg->c_ttime = time(NULL);

	shp->s_lock = 0;

	cx = x;
	cy = y;
}

