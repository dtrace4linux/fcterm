/**********************************************************************/
/*                                                                    */
/*      CRiSP - Programmable editor                                   */
/*      ===========================                                   */
/*                                                                    */
/*  File:          ptysrv.c                                           */
/*  Author:        P. D. Fox                                          */
/*  Created:       31 Jul 2005                                        */
/*                                                                    */
/*  Copyright (c) 2005-2012, Foxtrot Systems Ltd                      */
/*                All Rights Reserved.                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*  Description:  PTY Server for persistent shared pty connections.   */
/*--------------------------------------------------------------------*/
/*  $Header: Last edited: 09-Jul-2012 1.4 $ 			      */
/**********************************************************************/

# include	"ptysrv.h"
# include	<pwd.h>
# include	<time.h>
# undef sscanf

Head_p	hd_ptys;
Head_p	hd_conns;

int	utmp_flags;
int	allow_flag = FALSE;
time_t	allow_time;
int	f_flag = FALSE;

/**********************************************************************/
/*   Make life easier for 'cr' inside a term window.		      */
/**********************************************************************/
char	*term_str = "TERM=xterm";
char	*bterm_str = "CRTERM=xcrisp-col";
char	*foxterm_str = "FOXTERM=xterm";

/**********************************************************************/
/*   Listen port for new connections.				      */
/**********************************************************************/
int	listen_port = PTY_SERVER_PORT;
int	listen_fd;

char	**main_argv;
int	main_argc;

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
int	unlockpt(int);
static void trace_conn(netconn_t *conp);
size_t strlcpy(char *, const char *, size_t);
static char *get_arg(char *name, int argc, char **argv, int *error);
static int get_iarg(char *name, int argc, char **argv, int *error);
char	*hostname(unsigned long);
static void parse_pty_env(void);
List_p find_net(netconn_t *conp);
static char *tcp_peer(struct sockaddr_in *sin);
void net_close(List_p lp);
pty_t *find_pty(int fd);
void trace_mem(unsigned char *cp, int len);
void	main_loop(void);
char	*time_str();
void	cb_init(cbuf_t *, int);
int	net_read(List_p lp);
int	pty_read(List_p lp);
char	*basename(char *);
int	spawn_child(pty_t *pty);
int	noinherit(int);
int pty_close(List_p lp);

/**********************************************************************/
/*   New connection coming in - accept it.			      */
/**********************************************************************/
static void
accept_new_connection()
{	struct sockaddr_in sinbuf;
	socklen_t	len;
	int	fd;
	netconn_t *conp;

	len = sizeof sinbuf;
	fd = accept(listen_fd, (struct sockaddr *) &sinbuf, &len);
	if (fd < 0) {
		printf("%s Accept connection error %s\n", time_str(), 
			strerror(errno));
		return;
		}
	printf("%s Accept succeeded from %s\n", time_str(), tcp_peer(&sinbuf));
	conp = (netconn_t *) chk_zalloc(sizeof *conp);
	conp->n_fd = fd;
	conp->n_pty_fd = -1;
	conp->n_ctime = time(NULL);
	conp->n_addr = sinbuf;
	cb_init(&conp->n_cbuf, MAX_CMD_SIZE);
	noinherit(fd);
	ll_append(hd_conns, ll_newatom((void *) conp));
}
/**********************************************************************/
/*   Broadcast a message to all in the party.			      */
/**********************************************************************/
static int
broadcast(pty_t *pty, unsigned char *str, int len)
{	List_p	lp1, lp2;
	int	ret;

	/***********************************************/
	/*   Broadcast the data to all clients.	       */
	/***********************************************/
	for (lp1 = ll_first(hd_conns); lp1; lp1 = lp2) {
		netconn_t *conp = ll_elem(lp1);

		lp2 = ll_next(lp1);
		if (conp->n_pty_fd != pty->p_pty_fd)
			continue;

		if ((ret = write(conp->n_fd, str, len)) != len) {
			printf("%s Write error - closing connection (wrote %d, got %d)\n", time_str(), len, ret);
			net_close(lp1);
			}
		}
	return 1;
}
static int
broadcast_net(netconn_t *conp, char *str, int len)
{	List_p	lp1, lp2;
	int	ret;
	int	share_all = getenv("SHARE_ALL") != NULL;

	/***********************************************/
	/*   Broadcast the data to all clients.	       */
	/***********************************************/
	for (lp1 = ll_first(hd_conns); lp1; lp1 = lp2) {
		netconn_t *conp1 = ll_elem(lp1);

		lp2 = ll_next(lp1);
		if (conp1 == conp)
			continue;
		if (conp1->n_pty_fd != conp->n_pty_fd && !share_all)
			continue;

		if ((ret = write(conp1->n_fd, str, len)) != len) {
			printf("%s Write error - closing connection (wrote %d, got %d)\n", time_str(), len, ret);
			net_close(lp1);
			}
		}
	return 1;
}

void
cb_init(cbuf_t *cbp, int size)
{
	cbp->c_size = size;
	cbp->c_in = 0;
	cbp->c_out = 0;
	cbp->c_buf = chk_alloc(size);
}
int
cb_read(int fd, cbuf_t *cbp, unsigned char **start_bp)
{	int	n = 0, len;
	unsigned char	*bp;
	int	in, ret;
	struct timeval tval;

	*start_bp = cbp->c_buf + cbp->c_in;
	bp = *start_bp;
	in = cbp->c_in;

	while (1) {
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

		FD_ZERO(&rbits);
		FD_SET(fd, &rbits);
		tval.tv_sec = 0;
		tval.tv_usec = 10000;
		ret = select(fd + 1, &rbits, NULL, NULL, &tval);
		if (ret <= 0)
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
int
child_handler()
{
	return 0;
}
/**********************************************************************/
/*   Let anyone connection/attach to someone elses connection.	      */
/**********************************************************************/
int
cmd_allow(netconn_t *conp, int argc, char **argv)
{
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	printf("%s ALLOW has been set\n", time_str());
	allow_flag = TRUE;
	allow_time = time(NULL) + 60 * 60;
	write(conp->n_fd, "", 1);
	return 0;
}
/**********************************************************************/
/*   Attach to an existing connection.				      */
/**********************************************************************/
int
cmd_attach(netconn_t *conp, int argc, char **argv)
{	List_p	lp;
	int	uid;
	char	*name;
	int	error = FALSE;

	uid = get_iarg("uid", argc, argv, &error);
	name = get_arg("name", argc, argv, &error);
	conp->n_uid = uid;
	if (error) {
		printf("%s Argument error for 'attach'\n", time_str());
		return -1;
		}

	for (lp = ll_first(hd_ptys); lp; lp = ll_next(lp)) {
		pty_t *pty = (pty_t *) ll_elem(lp);
		if (strcmp(name, pty->p_name) == 0) {
			/***********************************************/
			/*   See if we are permissioned.	       */
			/***********************************************/
			if (!allow_flag && time(NULL) < allow_time) {
				if (uid != (int) conp->n_uid) {
					if (strcmp(argv[0], "attach") == 0)
						printf("%s Permission denied to attach to '%s'\n", time_str(), name);
					break;
					}
				}

			printf("%s Attached to '%s'\n", time_str(), name);
			conp->n_pty_fd = pty->p_pty_fd;
			if (pty->p_ref++ == 0)
				conp->n_owner = TRUE;
			if (pty->p_hist.c_in) {
				write(conp->n_fd, 
					pty->p_hist.c_buf + pty->p_hist.c_in,
					pty->p_hist.c_used - pty->p_hist.c_in
					);
				}
			write(conp->n_fd, pty->p_hist.c_buf, pty->p_hist.c_in);
			return 0;
			}
		}
	if (strcmp(argv[0], "reopen") == 0)
		return 1;
	printf("%s Could not attach to '%s'\n", time_str(), name);
	net_close(find_net(conp));
	return -1;
}
/**********************************************************************/
/*   Send data to the pty.					      */
/**********************************************************************/
int
cmd_data(netconn_t *conp, unsigned char *cp, int len)
{	int	n;

	n = write(conp->n_pty_fd, cp, len);
	if (n != len) {
		printf("%s Write failed to pty\n", time_str());
		return -1;
		}
	return 0;
}
/**********************************************************************/
/*   Don't let foreigners into a connection.			      */
/**********************************************************************/
int
cmd_deny(netconn_t *conp, int argc, char **argv)
{
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	printf("%s DENY has been set\n", time_str());
	allow_flag = FALSE;
	write(conp->n_fd, "", 1);
	return 0;
}
/**********************************************************************/
/*   Forcibly detach a connection.				      */
/*   detach name=XXX						      */
/**********************************************************************/
int
cmd_detach(netconn_t *conp, int argc, char **argv)
{	List_p	lp;
	int	error = FALSE;
	char	*name = get_arg("name", argc, argv, &error);

	UNUSED_PARAMETER(conp);

	if (error) {
		printf("%s Argument error for 'detach'\n", time_str());
		return 1;
		}

	for (lp = ll_first(hd_ptys); lp; lp = ll_next(lp)) {
		pty_t *pty = (pty_t *) ll_elem(lp);
		if (strcmp(name, pty->p_name) == 0) {
			List_p	lp1;

			printf("%s Detaching pty '%s'\n", time_str(), argv[1]);
			for (lp = ll_first(hd_conns); lp; lp = lp1) {
				netconn_t *conp1 = (netconn_t *) ll_elem(lp);
				lp1 = ll_next(lp);
				if (conp1->n_pty_fd == pty->p_pty_fd) {
					printf("%s Detaching: %s\n",
						time_str(),
						tcp_peer(&conp1->n_addr));
					net_close(lp);
					}
				}
			return 0;
			}
		}
	printf("%s No-one attached to '%s'\n", time_str(), argv[1]);
	return 1;
}
/**********************************************************************/
/*   Give a name to the pty.					      */
/*   name name=XXX						      */
/**********************************************************************/
int
cmd_name(netconn_t *conp, int argc, char **argv)
{	pty_t	*pty;
	int	error = FALSE;
	char	*name = get_arg("name", argc, argv, &error);

	if (error) {
		printf("%s Argument error for 'name'\n", time_str());
		return 1;
		}

	pty = find_pty(conp->n_pty_fd);
	if (pty == NULL) {
		printf("%s name -- connection not connected to a pty\n", time_str());
		return 0;
		}
	strlcpy(pty->p_name, name, sizeof pty->p_name);
	return 0;
}
/**********************************************************************/
/*   Open up a new pty connection.				      */
/*   open uid=NNN name=XXX env=SSS				      */
/**********************************************************************/
int
cmd_open(netconn_t *conp, int argc, char **argv)
{	pty_t	*pty = (pty_t *) chk_zalloc(sizeof *pty);
	struct timeval tval;
	int	i;
	char	buf[BUFSIZ];
	char	*label = NULL;
	char	*screen = NULL;
	int	error = FALSE;
	int	uid;
	char	*name;
	struct passwd *pwd;

	uid = get_iarg("uid", argc, argv, &error);
	if (error) {
		printf("%s Invalid open request\n", time_str());
		return 1;
		}
	name = get_arg("name", argc, argv, &error);
	/***********************************************/
	/*   Allow client to supply env vars.	       */
	/***********************************************/
	pwd = getpwuid(uid);
	printf("%s Opening new pty for %s (%d)\n", time_str(), pwd ? pwd->pw_name : "???", uid);
	for (i = 0; i < argc; i++) {
		if (strncmp(argv[i], "env=", 4) == 0) {
			char	*env = argv[i] + 4;
			printf("           env %s\n", env);
			if (strncmp(env, "PTY_LABEL=", 10) == 0)
				label = argv[i]+10;
			if (strncmp(env, "PTY_SCREEN=", 11) == 0)
				screen = argv[i]+11;
			putenv(env);
			}
		}

	spawn_child(pty);
	/***********************************************/
	/*   Avoid  race  condition  reading from pty  */
	/*   before its ready.			       */
	/***********************************************/
	tval.tv_sec = 0;
	tval.tv_usec = 50000;
	select(FD_SETSIZE, NULL, NULL, NULL, &tval);

	conp->n_pty_fd = pty->p_pty_fd;
	conp->n_owner = TRUE;
	conp->n_uid = uid;
	pty->p_ref++;
	pty->p_uid = uid;
	pty->p_ctime = time(NULL);
	cb_init(&pty->p_hist, MAX_HIST_SIZE);
	cb_init(&pty->p_cbuf, MAX_HIST_SIZE);
	if (name) {
		strlcpy(pty->p_name, name, sizeof pty->p_name);
		}
	if (label && screen) {
		snprintf(buf, sizeof buf - 1, "%s-%s", label, screen);
		strlcpy(pty->p_name, buf, sizeof pty->p_name);
		}
	ll_append(hd_ptys, ll_newatom((void *) pty));
	return 0;
}
/**********************************************************************/
/*   Like attach or open - try and attach, else do an open.	      */
/*   reopen uid=NNN name=SSS env=SSS				      */
/**********************************************************************/
int
cmd_reopen(netconn_t *conp, int argc, char **argv)
{	int	ret;

	if ((ret = cmd_attach(conp, argc, argv)) <= 0)
		return ret;
	if (cmd_open(conp, argc, argv) == 0)
		return 0;
	return -1;
}
/**********************************************************************/
/*   Try and restart execution.					      */
/**********************************************************************/
int
cmd_restart(netconn_t *conp, int argc, char **argv)
{	char	buf[8192];
	char	*bp;
	List_p	lp;

	UNUSED_PARAMETER(conp);
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	strcpy(buf, "PTY_ENV=");
	bp = buf;
	for (lp = ll_first(hd_ptys); lp; lp = ll_next(lp)) {
		pty_t *pty = (pty_t *) ll_elem(lp);

		bp += strlen(buf);
		sprintf(bp, "%d %s;",
			pty->p_pty_fd,
			pty->p_name);
		ioctl(pty->p_pty_fd, F_SETFD, 0);
		}
	putenv(buf);

	printf("%s Restarting execution\n", time_str());
	execvp(main_argv[0], main_argv);
	return 0;
}
/**********************************************************************/
/*   We are asking for the status.				      */
/**********************************************************************/
static int
cmd_status_sort(pty_t **pp1, pty_t **pp2)
{
	return strcmp((*pp1)->p_name, (*pp2)->p_name);
}
int
cmd_status(netconn_t *conp, int argc, char **argv)
{	List_p	lp;
	char	buf[BUFSIZ];
	char	time_buf[BUFSIZ];
	struct tm *tm;
	int	i;
	pty_t	**ptys;
	struct passwd *pwd;

	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	sprintf(buf, "Allow: %s\n", allow_flag ? "YES" : "no");
	write(conp->n_fd, buf, strlen(buf));
	write(conp->n_fd, "\n", 1);

	/***********************************************/
	/*   Sort entries into a useful order.	       */
	/***********************************************/
	ptys = (pty_t *) chk_alloc(ll_length(hd_ptys) * sizeof(pty_t *) + 1);
	for (i = 0, lp = ll_first(hd_ptys); lp; lp = ll_next(lp), i++) {
		ptys[i] = (pty_t *) ll_elem(lp);
		}
	qsort(ptys, ll_length(hd_ptys), sizeof(pty_t *), (int (*)(void *, void *)) cmd_status_sort);

	for (i = 0; i < ll_length(hd_ptys); i++) {
		pty_t *pty = ptys[i];
		tm = localtime(&pty->p_ctime);
		strftime(time_buf, sizeof time_buf, 
			"%Y-%m-%d %H:%M:%S",
			tm);
		pwd = getpwuid(pty->p_uid);
		(void) snprintf(buf, sizeof buf, "PTY  %-8s %s ref=%d %2dx%2d %s %s\n", 
			pwd ? pwd->pw_name : "(anon)",
			time_buf, pty->p_ref, 
			pty->p_rows, pty->p_cols,
			pty->p_ttyname,
			pty->p_name);
		write(conp->n_fd, buf, strlen(buf));
		}
	chk_free(ptys);

	write(conp->n_fd, "\n", 1);
	for (lp = ll_first(hd_conns); lp; lp = ll_next(lp)) {

		netconn_t *conp1 = (netconn_t *) ll_elem(lp);
		if (conp == conp1)
			continue;
		tm = localtime(&conp1->n_ctime);
		strftime(time_buf, sizeof time_buf, 
			"%Y-%m-%d %H:%M:%S",
			tm);
		pwd = getpwuid(conp1->n_uid);
		sprintf(buf, "TCP%s %-8s %s %dx%d %s\n", 
			conp1->n_owner ? "*" : ":",
			pwd ? pwd->pw_name : "(anon)",
			time_buf, 
			conp->n_rows, conp->n_cols,
			tcp_peer(&conp1->n_addr));
		write(conp->n_fd, buf, strlen(buf));
		}
	write(conp->n_fd, "", 1);
	return 0;
}
/**********************************************************************/
/*   Set pty window size.					      */
/*   winsz rows=NNN cols=NNN					      */
/**********************************************************************/
int
cmd_winsz(netconn_t *conp, int argc, char **argv)
{	static struct winsize	winsize;
	pty_t	*pty;
	int	error = FALSE;
	int	rows, cols;

	rows = get_iarg("rows", argc, argv, &error);
	cols = get_iarg("cols", argc, argv, &error);
	if (error) {
		printf("invalid winsz argc=%d\n", argc);
		return 1;
		}

	winsize.ws_row = conp->n_rows = rows;
	winsize.ws_col = conp->n_cols = cols;

	if (ioctl(conp->n_pty_fd, TIOCSWINSZ, &winsize) < 0) {
		perror("ioctl");
		}
	else {
		pty = find_pty(conp->n_pty_fd);
		conp->n_rows = winsize.ws_row;
		conp->n_cols = winsize.ws_col;
		if (pty) {
			pty->p_rows = winsize.ws_row;
			pty->p_cols = winsize.ws_col;
			}
		}
	return 0;
}
int
do_switches(int argc, char **argv)
{	int	i;

	for (i = 1; i < argc; i++) {
		char	*cp = argv[i];
		if (*cp++ != '-')
			break;

		while (*cp) {
			switch (*cp++) {
			  case 'f':
				f_flag = TRUE;
				break;
			  }
			}
		}
	return i;
}
/**********************************************************************/
/*   Function to add or delete the utmp entry.			      */
/**********************************************************************/
void
do_utmp(pty_t *cur_ctw, int remove_flag)
{
# if defined(HAS_UTMP) || defined(HAS_UTMPX)
	char	buf[32];
	static char	*name;
	struct passwd *pwd;
	char	*tty_name;
	struct utmpx ut;
	char	*slavename;
	char	tty_name[64];

	slavename = tty_name;

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
# else /* defined(HAS_UTMP) || defined(HAS_UTMPX) */
	UNUSED_PARAMETER(cur_ctw);
	UNUSED_PARAMETER(remove_flag);
# endif /* defined(HAS_UTMP) || defined(HAS_UTMPX) */
}
List_p
find_net(netconn_t *conp)
{	List_p	lp;

	for (lp = ll_first(hd_conns); lp; lp = ll_next(lp)) {
		netconn_t *conp1 = (netconn_t *) ll_elem(lp);
		if (conp1 == conp)
			return lp;
		}
	return NULL;
}
/**********************************************************************/
/*   Map a pty fd to a ptr.					      */
/**********************************************************************/
pty_t *
find_pty(int fd)
{	List_p	lp;

	for (lp = ll_first(hd_ptys); lp; lp = ll_next(lp)) {
		pty_t *pty = (pty_t *) ll_elem(lp);
		if (pty->p_pty_fd == fd)
			return pty;
		}
	return NULL;
}
/**********************************************************************/
/*   Return string argument.					      */
/**********************************************************************/
static char *
get_arg(char *name, int argc, char **argv, int *error)
{	int	i;
	int	len = strlen(name);

	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], name, len) == 0 &&
		    argv[i][len] == '=') {
		    	return argv[i] + len + 1;
			}
		}
	*error = TRUE;
	return NULL;
}
/**********************************************************************/
/*   Look for an argument in the passed in command.		      */
/**********************************************************************/
static int
get_iarg(char *name, int argc, char **argv, int *error)
{	int	i;
	int	len = strlen(name);

	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], name, len) == 0 &&
		    argv[i][len] == '=') {
		    	return atoi(argv[i] + len + 1);
			}
		}
	*error = TRUE;
	return 0;
}
/**********************************************************************/
/*   Main entry point to the code.				      */
/**********************************************************************/
int
main(int argc, char **argv)
{	

	do_switches(argc, argv);

	main_argc = argc;
	main_argv = argv;

	hd_conns = ll_newlist();
	hd_ptys = ll_newlist();

	parse_pty_env();

	if (!f_flag) {
		switch (fork()) {
		  case -1:
		  	perror("fork");
			exit(1);
		  case 0:
		  	break;
		  default:
		  	exit(0);
		  }
		signal(SIGINT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		close(0);
		}

	main_loop();
	return 0;
}
void
main_loop()
{	fd_set	rbits;
	fd_set	wbits;
	List_p	lp, lp1;
	int	ret;

	signal(SIGPIPE, SIG_IGN);

	printf("%s PID#%d Listening on port %d\n", time_str(), getpid(), listen_port);
	listen_fd = tcp_connect(0, 0, listen_port, 0, 0);
	if (listen_fd < 0) {
		perror("Cannot create listen port");
		exit(1);
		}
	noinherit(listen_fd);

	while (1) {
		FD_ZERO(&wbits);

		/***********************************************/
		/*   Check  for  orphaned pty connections and  */
		/*   close them.			       */
		/***********************************************/
		for (lp = ll_first(hd_ptys); lp; lp = lp1) {
			pty_t *pty = (pty_t *) ll_elem(lp);
			lp1 = LL_NEXT(lp);
			if (pty->p_closed)
				pty_close(lp);
			}

		/***********************************************/
		/*   Set  up  the  rbits  so  we can wait for  */
		/*   something to happen.		       */
		/***********************************************/
		FD_ZERO(&rbits);
		FD_SET(listen_fd, &rbits);
		for (lp = ll_first(hd_ptys); lp; lp = ll_next(lp)) {
			pty_t *pty = (pty_t *) ll_elem(lp);
			FD_SET(pty->p_pty_fd, &rbits);
			}
		for (lp = ll_first(hd_conns); lp; lp = ll_next(lp)) {
			netconn_t *conp = (netconn_t *) ll_elem(lp);
			FD_SET(conp->n_fd, &rbits);
			}

		/***********************************************/
		/*   Wait for something to happen.	       */
		/***********************************************/
		ret = select(FD_SETSIZE, &rbits, &wbits, NULL, NULL);
		if (ret < 0) {
			continue;
			}

		/***********************************************/
		/*   Respond to stimuli			       */
		/***********************************************/
		if (FD_ISSET(listen_fd, &rbits)) {
			accept_new_connection();
			}
		for (lp = ll_first(hd_conns); lp; lp = lp1) {
			netconn_t *conp = (netconn_t *) ll_elem(lp);
			lp1 = ll_next(lp);
			if (FD_ISSET(conp->n_fd, &rbits)) {
				net_read(lp);
				}
			}
		for (lp = ll_first(hd_ptys); lp; lp = lp1) {
			pty_t *ptyp = (pty_t *) ll_elem(lp);
			lp1 = ll_next(lp);
			if (FD_ISSET(ptyp->p_pty_fd, &rbits)) {
				pty_read(lp);
				}
			}
		}
}
void
net_close(List_p lp)
{	netconn_t *conp = (netconn_t *) ll_elem(lp);
	pty_t	*pty = find_pty(conp->n_pty_fd);

	if (pty)
		pty->p_ref--;

	trace_conn(conp);
	printf("Connection closed\n");
	close(conp->n_fd);
	chk_free(conp->n_cbuf.c_buf);
	chk_free(conp);

	ll_remove(hd_conns, lp);
	ll_freeatom(lp);
}
int
net_read(List_p lp)
{	netconn_t *conp = (netconn_t *) ll_elem(lp);
	int	len, ret;
	char	*cmd;
# define	MAX_ARGV	8
	char	*argv[MAX_ARGV];
	int	argc;
	char	cmdbuf[BUFSIZ];
	unsigned char	*cp, *cp1, *cp2, *cpend;

	ret = cb_read(conp->n_fd, &conp->n_cbuf, &cp);
/*	printf("net: read %d bytes\n", ret);*/
	if (ret <= 0) {
		net_close(lp);
		return -1;
		}

	/***********************************************/
	/*   See if we have a newline terminator.      */
	/***********************************************/
	while (1) {
/*trace_mem(conp->n_cbuf.c_buf, conp->n_cbuf.c_in);*/
		cp = conp->n_cbuf.c_buf;
		cpend = cp + conp->n_cbuf.c_in;
		for (cp1 = cp; cp1 < cpend && *cp1 != '\0'; cp1++)
			;
		if (cp1 != cp) {
			cmd_data(conp, cp, cp1 - cp);
			if (cp1 >= cpend)
				break;
			len = cpend - cp1;
			memmove(cp, cp1, len);
			conp->n_cbuf.c_in = len;
			continue;
			}
		/***********************************************/
		/*   Gotta command. Look for end of command.   */
		/***********************************************/
		for (cp2 = cp1 + 1; cp2 < cpend && *cp2 ; cp2++)
			;
		if (cp2 >= cpend)
			return 0;

		cmd = (char *) ++cp1;
		trace_conn(conp);
		printf("Cmd='%*.*s'\n", (int) (cp2 - cp1), (int) (cp2 - cp1), cp1);
		memcpy(cmdbuf, cmd, cp2 - cp1 + 1);
		cmdbuf[cp2-cp1+1] = '\0';
		argv[0] = cmd;
/*printf("cmd=%s\n", cmd);*/
		for (argc = 1; argc < MAX_ARGV; argc++) {
			if ((argv[argc] = strtok((char *) cp1, " \r\n")) == NULL)
				break;
			cp1 = NULL;
			}

		if (strcmp(cmd, "allow") == 0) {
			ret = cmd_allow(conp, argc, argv);
			}
		else if (strcmp(cmd, "attach") == 0) {
			ret = cmd_attach(conp, argc, argv);
			}
		else if (strcmp(cmd, "deny") == 0) {
			ret = cmd_deny(conp, argc, argv);
			}
		else if (strcmp(cmd, "detach") == 0) {
			ret = cmd_detach(conp, argc, argv);
			}
		else if (strcmp(cmd, "mouse") == 0) {
			broadcast_net(conp, "", 1);
			broadcast_net(conp, cmdbuf, strlen(cmdbuf) + 1);
			ret = 0;
			}
		else if (strcmp(cmd, "open") == 0) {
			ret = cmd_open(conp, argc, argv);
			}
		else if (strcmp(cmd, "name") == 0) {
			ret = cmd_name(conp, argc, argv);
			}
		else if (strcmp(cmd, "open") == 0) {
			ret = cmd_reopen(conp, argc, argv);
			}
		else if (strcmp(cmd, "reopen") == 0) {
			ret = cmd_reopen(conp, argc, argv);
			}
		else if (strcmp(cmd, "restart") == 0) {
			ret = cmd_restart(conp, argc, argv);
			}
		else if (strcmp(cmd, "status") == 0) {
			ret = cmd_status(conp, argc, argv);
			}
		else if (strcmp(cmd, "winsz") == 0) {
			broadcast_net(conp, "", 1);
			broadcast_net(conp, cmdbuf, strlen(cmdbuf) + 1);
			ret = cmd_winsz(conp, argc, argv);
			}
		if (ret)
			return 0;
		memmove(cp, cp2 + 1, cpend - cp2 - 1);
		conp->n_cbuf.c_in = cpend - (cp2 + 1);
		}
	cb_reset(&conp->n_cbuf);
	return 0;
}
/**********************************************************************/
/*   On startup, see if we got restarted and set up the pty list.     */
/**********************************************************************/
static void
parse_pty_env()
{	char	*cp = getenv("PTY_ENV");
	pty_t	*pty;

	if (cp == NULL)
		return;

	for (cp = strtok(cp, ";"); cp; cp = NULL) {
		int	fd;
		char	buf[BUFSIZ];
		if (sscanf(cp, "%d %s", &fd, buf) != 2)
			continue;
		noinherit(fd);
		if (fcntl(fd, F_GETFL, 0) == -1) {
			printf("%s Ignoring %s\n", time_str(), cp);
			perror("ioctl");
			continue;
			}

		pty = (pty_t *) chk_zalloc(sizeof *pty);
		pty->p_pty_fd = fd;

		pty->p_slavename = "inherited";
		pty->p_ctime = time(NULL);
		strlcpy(pty->p_name, buf, sizeof pty->p_name);
		cb_init(&pty->p_hist, MAX_HIST_SIZE);
		cb_init(&pty->p_cbuf, MAX_HIST_SIZE);
		ll_append(hd_ptys, ll_newatom((void *) pty));
		}
}
/**********************************************************************/
/*   Process a command from the pty.				      */
/**********************************************************************/
static void
process_pty_command(pty_t *pty, char *str, int len)
{
	UNUSED_PARAMETER(len);

	if (strncmp(str, "label ", 6) == 0) {
		strncpy(pty->p_name, str + 6, sizeof pty->p_name);
		printf("Label \"%s\"\n", str + 6);
		return;
		}
}
/**********************************************************************/
/*   Close all connected net conns.				      */
/**********************************************************************/
int
pty_close(List_p lp)
{	List_p	lp1, lp2;
	pty_t	*pty = (pty_t *) ll_elem(lp);

	for (lp1 = ll_first(hd_conns); lp1; lp1 = lp2) {
		netconn_t *conp = ll_elem(lp1);
		lp2 = ll_next(lp1);
		if (conp->n_pty_fd == pty->p_pty_fd) {
			net_close(lp1);
			}
		}
/*	chk_free(pty->p_slavename);*/
	chk_free(pty->p_hist.c_buf);
	chk_free(pty->p_cbuf.c_buf);
	close(pty->p_pty_fd);
	chk_free(pty);
	ll_freeatom(ll_remove(hd_ptys, lp));
	return 0;
}
int
pty_read(List_p lp)
{	pty_t	*pty = (pty_t *) ll_elem(lp);
	int	ret;
	unsigned char	*start_bp;
	unsigned char	*cp, *cpend;
	int	partial = FALSE;
	int	len;

	ret = cb_read(pty->p_pty_fd, &pty->p_cbuf, &start_bp);
	if (ret < 0) {
		pty->p_closed = TRUE;
		return -1;
		}
	if (ret > 0) {
		cb_copy_in(&pty->p_hist, start_bp, ret);
		}
/*	printf("pty: read %d bytes\n", ret);*/
	/***********************************************/
	/*   Need  to  parse  the  output for our own  */
	/*   escape sequence.			       */
	/***********************************************/
	cpend = pty->p_cbuf.c_buf + pty->p_cbuf.c_in;

	for (cp = pty->p_cbuf.c_buf; cp < cpend; cp++) {
		if (*cp == '\0') {
			unsigned char *cp1 = cp + 1;
			while (cp1 < cpend && *cp1)
				cp1++;
			if (cp1 >= cpend) {
				partial = TRUE;
				break;
				}
			/***********************************************/
			/*   Write out prior to the command.	       */
			/***********************************************/
			if (cp != pty->p_cbuf.c_buf) {
				broadcast(pty, pty->p_cbuf.c_buf, cp - pty->p_cbuf.c_buf);
				}
			process_pty_command(pty, (char *) cp + 1, cp1 - cp);
			len = pty->p_cbuf.c_in - (cp1 + 1 - pty->p_cbuf.c_buf);
			memmove(pty->p_cbuf.c_buf,
				cp1 + 1,
				len);
			pty->p_cbuf.c_in = len;
			cp = pty->p_cbuf.c_buf - 1;
			cpend = pty->p_cbuf.c_buf + len;
			}
		}
	if (!partial) {
		broadcast(pty, pty->p_cbuf.c_buf, pty->p_cbuf.c_in);
		}
	cb_reset(&pty->p_cbuf);
	return 0;	
}
int
spawn_child(pty_t *pty)
{
	char	tty[BUFSIZ];
	char	tty_name[BUFSIZ];
	int	one = 1;
	char	*minus_name = (char *) NULL;
	int	tty_fd;

	pty->p_pty_fd = -1;

	putenv("PTY_INSIDE=1");

# if defined(STREAMS_PTY)
	{char *slavename;
	signal(SIGCHLD, SIG_DFL);

	pty->p_pty_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
	if (pty->p_pty_fd < 0) {
		perror("/dev/ptmx");
		exit(1);
		}
	if (grantpt(pty->p_pty_fd) < 0) {
		perror("grantpt");
		exit(1);
		}
	if (unlockpt(pty->p_pty_fd) < 0) {
		perror("unlockpt");
		exit(1);
		}
	if ((slavename = ptsname(pty->p_pty_fd)) == (char *) NULL) {
		perror("ptsname");
		exit(1);
		}
	strncpy(pty->p_ttyname, slavename, sizeof pty->p_ttyname);
# if 1
	if ((tty_fd = open(slavename, O_RDWR)) < 0) {
# else
	if ((tty_fd = open(slavename, O_RDWR | O_NOCTTY)) < 0 ||
	   ioctl(tty_fd, I_PUSH, "ptem") < 0 ||
	   ioctl(tty_fd, I_PUSH, "ldterm") < 0 ||
	   ioctl(tty_fd, I_PUSH, "ttcompat") < 0) {
# endif
	   	fprintf(stderr, "Error creating pty\n");
		perror(slavename);
		return -1;
		}
	strcpy(tty_name, slavename);
	}
# else
	{int letter1, letter2;
	char	pty_name[BUFSIZ];

	pty->p_slavename = "/dev/dont-know";
	for (letter1 = 'p'; letter1 < 'z'; letter1++) {
		for (letter2 = 0; letter2 < 16; letter2++) {
			sprintf(pty_name, "/dev/pty%c%x", letter1, letter2);
			if ((pty->p_pty_fd = open(pty_name, O_RDWR)) >= 0) {
				char	buf[BUFSIZ];

				sprintf(tty_name, "/dev/tty%c%x", letter1, letter2);
				pty->p_slavename = malloc(strlen(pty_name) + 1);
				strcpy(pty->p_slavename, pty_name);

				/***********************************************/
				/*   Check  we  can open the tty side. If not  */
				/*   scan  for  another  pty.  Sometimes  the  */
				/*   terminals are left in strange states.     */
				/***********************************************/
				tty_fd = open(tty_name, O_RDWR);
				if (tty_fd >= 0) {
					strncpy(pty->p_ttyname, tty_name, sizeof pty->p_ttyname);
					close(tty_fd);
					goto found_pty;
					}
				fprintf(stderr, "Warning: %s could not be opened - trying next tty\n", tty_name);
				sprintf(buf, "ls -l %s %s", pty_name, tty_name);
				system(buf);
				close(pty->p_pty_fd);
				}
			}
		}
	fprintf(stderr, "fcterm: Couldn't find a free pty\n");
	return -1;
	}
# endif
found_pty:
	strncpy(pty->p_name, basename(pty->p_ttyname), sizeof pty->p_name);
	signal(SIGCHLD, child_handler);
	/***********************************************/
	/*   Drop  any  privileges  we may have becos  */
	/*   we  might  have  been made setuid/setgid  */
	/*   on execution.			       */
	/***********************************************/
	setuid(getuid());
	setgid(getgid());
	do_utmp(pty, FALSE);
	noinherit(pty->p_pty_fd);
	switch (pty->p_pid = fork()) {
	  case -1:
	  	perror("fork");
		exit(1);
	  case 0: {
	  	TERM_STRUCT t, t1;
		char	*shell = getenv("SHELL");
		char	*shname;

		signal(SIGINT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGHUP, SIG_DFL);

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

		snprintf(tty, sizeof tty, "TTY=%s", tty_name);
		putenv(tty);
# if HAVE_SETSID
		setsid();
# endif
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
		
		tcgetattr(tty_fd, &t);
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

		tcsetattr(tty_fd, TCSANOW, &t);

# if defined(TIOCCONS)
		if (pty->p_console_flag) {
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
		if (pty->p_login_shell_flag) {
			minus_name = malloc(strlen(shname) + 2);
			minus_name[0] = '-';
			strcpy(minus_name + 1, shname);
			}
		execlp(shell, 
			pty->p_login_shell_flag ? minus_name : shell, 
			"-i", (char *) 0);
		perror("execlp");
		_exit(1);
		}
	  default:
	  	break;
	  }
	return 0;
}
static void 
trace_conn(netconn_t *conp)
{	struct passwd *pwd = getpwuid(conp->n_uid);

	printf("%s %s@%s ", 
		time_str(), 
		pwd ? pwd->pw_name : "anon",
		hostname(htonl(conp->n_addr.sin_addr.s_addr)));
}
void
trace_mem(unsigned char *cp, int len)
{	int	i;
	int	addr = 0;

	while (len > 0) {
		printf("%06x: ", addr);
		for (i = 0; i < 16 && i < len; i++)
			printf("%02x ", cp[i]);
		for (; i < 16; i++)
			printf("   ");
		printf("  ");
		for (i = 0; i < 16 && i < len; i++) {
			printf("%c", cp[i] >= ' ' && cp[i] < 0x7f ? cp[i] : '.');
			}
		printf("\n");
		len -= 16;
		cp += 16;
		addr += 16;
		}
}
static char *
tcp_peer(struct sockaddr_in *sin)
{	static char buf[128];

	tcp_ntoa(buf, htonl(sin->sin_addr.s_addr));
	snprintf(buf + strlen(buf), sizeof buf - strlen(buf) - 1,
		":%u (%s)", htons(sin->sin_port), hostname(htonl(sin->sin_addr.s_addr)));
	return buf;
}
