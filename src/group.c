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

/**********************************************************************/
/*   Save config for this fcterm.				      */
/**********************************************************************/
typedef struct config_t {
	int	c_pid;
	unsigned long c_time;
	int	c_x;
	int	c_y;
	} config_t;
typedef struct sh_config_t {
	int	s_x;
	int	s_y;
	int	s_pid;
	int	s_grouping;
	} sh_config_t;
static int cx, cy;
static int sx, sy;
static int ignore_move;

extern int do_group;
extern Widget top_level;
extern char *log_dir;

void
group_config_poll(XtIntervalId *cfg_timer_id)
{
	FILE	*fp;
	char	buf[BUFSIZ];

	if (!do_group)
		return;

	*cfg_timer_id = XtAppAddTimeOut(
		XtWidgetToApplicationContext(top_level), 
		1000L, group_config_poll, (void *) cfg_timer_id);

	snprintf(buf, sizeof buf, "%s/fcterm.pub", log_dir);
	if ((fp = fopen(buf, "r")) == NULL)
		return;
	while (fgets(buf, sizeof buf, fp) != NULL) {
		int	x, y;
		int	pid;

		if (strncmp(buf, "delta=", 6) != 0)
			continue;
		if (sscanf(buf, "delta=%d,%d,%d", &x, &y, &pid) != 3)
			continue;

		if (pid == getpid())
			break;

		if (cx + x == sx && cy + y == sy)
			break;
printf("\npid=%d buf=%s", getpid(), buf);
printf("move %d,%d %d,%d %d,%d\n", cx, cy, x, y, sx, sy);
		ignore_move = TRUE;
		XtMoveWidget(top_level, 
			cx + x, cy + y);
		XSync(XtDisplay(top_level), 1);
		ignore_move = FALSE;
		break;
	}

	fclose(fp);
}

int
group_read_config(char *fname, config_t *cfg)
{	FILE	*fp;
	char	buf[4096];
	struct stat sbuf;

	memset(cfg, 0, sizeof *cfg);
	if ((fp = fopen(fname, "r")) == NULL)
		return TRUE;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		strtok(buf, "\n");
		if (strncmp(buf, "pid=", 4) == 0) {
			int p = atoi(buf + 4);
			snprintf(buf, sizeof buf, "/proc/%d", p);
			if (stat(buf, &sbuf) == 0) {
				cfg->c_pid = p;
				continue;
				}
			continue;
			}
		if (strncmp(buf, "start=", 6) == 0) {
			cfg->c_time = strtoul(buf + 6, NULL, 0);
			continue;
			}
		if (strncmp(buf, "pos=", 4) == 0) {
			sscanf(buf + 4, "%d,%d", &cfg->c_x, &cfg->c_y);
			continue;
			}
		}
	fclose(fp);
	if (cfg->c_pid == 0)
		return TRUE;
	return TRUE;
}

void
group_write_config(int x, int y, int pub)
{	char	buf[4096];
	char	buf1[4096];
	char	label_buf[2];
static	char *label;
	FILE	*fp;
	int	i;
	config_t cfg;
	int	debug = 1;

	if (!do_group)
		return;

	cx = x;
	cy = y;

	if (debug)
		printf("write_config: pos=%d,%d\n", x, y);

	if (pub) {
		snprintf(buf, sizeof buf, "%s/fcterm.pub", log_dir);
		snprintf(buf1, sizeof buf1, "%s/fcterm.pub.tmp", log_dir);
		if ((fp = fopen(buf, "w")) != NULL) {
			fprintf(fp, "delta=%d,%d,%d\n",
				x - sx, y - sy, getpid());
			fclose(fp);
			rename(buf1, buf);
			}
		}
	sx = x;
	sy = y;

	if (label == NULL)
		label = getenv("PTY_LABEL");

	/***********************************************/
	/*   Try and find a label for us.	       */
	/***********************************************/
	if (label == NULL) {
		for (i = 'A'; i <= 'Z'; i++) {
			snprintf(buf, sizeof buf, "%s/fcterm.%c.pos", 
				log_dir,
				i);
			label_buf[0] = i;
			label_buf[1] = '\0';

			if (group_read_config(buf, &cfg) == 0) {
				if (debug)
					printf("trying %s - failed\n", buf);
				label = label_buf;
				continue;
				}
			if (cfg.c_pid)
				continue;
			if (debug)
				printf("trying %s - good\n", buf);
			/***********************************************/
			/*   Found free slot. Restore position.	       */
			/***********************************************/
			label = label_buf;
			printf("fcterm: pid=%d label=%s\n", (int) getpid(), label);
			XtMoveWidget(top_level, cfg.c_x, cfg.c_y);
			break;
			}
		}

	snprintf(buf, sizeof buf, "%s/fcterm.%s.pos", 
		log_dir,
		label ? label : "anon"
		);
	if ((fp = fopen(buf, "w")) != NULL) {
		struct stat sbuf;

		fprintf(fp, "pid=%d\n", (int) getpid());
		fprintf(fp, "pos=%d,%d\n", x, y);
		stat("/proc/self", &sbuf);
		fprintf(fp, "start=%lu\n", sbuf.st_mtime);
		fclose(fp);
		}
}

