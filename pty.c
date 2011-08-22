/**********************************************************************/
/*                                                                    */
/*      CRiSP - Programmable editor                                   */
/*      ===========================                                   */
/*                                                                    */
/*  File:          pty.c                                              */
/*  Author:        P. D. Fox                                          */
/*  Created:       1 Sep 2005                                         */
/*                                                                    */
/*  Copyright (c) 2005-2006, Foxtrot Systems Ltd                      */
/*                All Rights Reserved.                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*  Description:  Pty client utility                                  */
/*--------------------------------------------------------------------*/
/*  $Header: Last edited: 01-Sep-2005 1.1 $ */
/**********************************************************************/

# include	"ptysrv.h"

char	*host = "localhost";
int	port = PTY_SERVER_PORT;

int	force;

/**********************************************************************/
/*   TERM_STRUCT  is  set  in  tty.h depending on which tty style we  */
/*   are using.							      */
/**********************************************************************/
TERM_STRUCT	ot, nt;
int kbdflgs = 0;                    	/* saved keyboard fd flags      */

volatile int sigwinch;

/**********************************************************************/
/*   Prototypes.						      */
/**********************************************************************/
static void send_cmd(int fd, char *cmd);
static void sigwinch_handler(void);
static void set_window_size(int fd);
static void main_loop(char *cmd, int fd);
static int set_blocking(int fd, int flag);
int	do_switches(int argc, char **argv);
void tty_open(void);
void tty_close(void);
void	usage(void);
int	tcp_get_port_address(char *);

int
main(int argc, char **argv)
{	int	arg_index;
	unsigned long faddr;
	unsigned fport;
	char	buf[BUFSIZ];
	int	fd;
	char	*cmd = NULL;
	char	*name = NULL;
	char	*port_name;

	arg_index = do_switches(argc, argv);
	if (arg_index < argc && 
	    (
	     strcmp(argv[arg_index], "allow") == 0 ||
	     strcmp(argv[arg_index], "deny") == 0 ||
	     strcmp(argv[arg_index], "restart") == 0 ||
	     strcmp(argv[arg_index], "status") == 0)) {
		cmd = argv[arg_index++];
		}
	else if (arg_index < argc && 
	        (strcmp(argv[arg_index], "attach") == 0 ||
	         strcmp(argv[arg_index], "detach") == 0 ||
	         strcmp(argv[arg_index], "reopen") == 0 ||
	         strcmp(argv[arg_index], "label") == 0)) {
		cmd = argv[arg_index++];
		if (arg_index >= argc)
			usage();
		name = argv[arg_index++];
		}

	if (arg_index < argc)
		host = argv[arg_index++];
	if (arg_index < argc) {
		port_name = argv[arg_index++];
		port = tcp_get_port_address(port_name);
		}

	faddr = tcp_get_ipaddr(host);
	fport = port;
	tcp_ntoa(buf, faddr);
	printf("Connecting to %s:%u\n", buf, fport);
	fd = tcp_connect(0, 0, 0, faddr, fport);
	if (fd < 0) {
		perror("connect");
		exit(1);
		}

	if (cmd && 
	    (
	     strcmp(cmd, "allow") == 0 ||
	     strcmp(cmd, "deny") == 0 ||
	     strcmp(cmd, "status") == 0 ||
	     strcmp(cmd, "restart") == 0)) {
		send_cmd(fd, cmd);
		return 0;
		}
	if (cmd && strcmp(cmd, "label") == 0) {
		write(1, "\0label ", 7);
		write(1, name, strlen(name) + 1);
		return 0;
		}
	if (getenv("PTY_INSIDE") && !force) {
		printf("Error: already inside a pty session. Use -f to force a nested session\n");
		exit(1);
		}
	if (cmd && (strcmp(cmd, "attach") == 0 || 
	           strcmp(cmd, "detach") == 0 ||
	           strcmp(cmd, "reopen") == 0)) {
		set_blocking(fd, FALSE);
		write(fd, "\0", 1);
		sprintf(buf, "%s uid=%d name=%s", cmd, getuid(), name);
		write(fd, buf, strlen(buf) + 1);
		main_loop(cmd, fd);
		return 0;
		}

	set_blocking(fd, FALSE);
	/***********************************************/
	/*   Log us in to the server.		       */
	/***********************************************/
	if (cmd == NULL) {
		usage();
		}
	sprintf(buf, "open uid=%d", getuid());
	write(fd, "", 1);
	write(fd, buf, strlen(buf) + 1);
	main_loop(cmd, fd);
	return 0;
}
static void
main_loop(char *cmd, int fd)
{	fd_set	rbits;
	char	buf[BUFSIZ];
	char	buf1[BUFSIZ];
	int	i, n;

	if (strcmp(cmd, "reopen") == 0 ||
	    strcmp(cmd, "open") == 0) {
	    	printf("\033[47;34m...Now connected to the PTY server...\033[37;40m\n");
		fflush(stdout);
	    	}
	signal(SIGWINCH, sigwinch_handler);

	tty_open();
	set_window_size(fd);
	while (1) {
		FD_ZERO(&rbits);
		FD_SET(fd, &rbits);
		FD_SET(0, &rbits);

		n = select(FD_SETSIZE, &rbits, NULL, NULL, NULL);
		if (sigwinch) {
			set_window_size(fd);
			}
		if (n <= 0)
			continue;

		if (FD_ISSET(0, &rbits)) {
			n = read(0, buf, sizeof buf);
			for (i = 0; i < n; i++) {
				if (buf[i] == (']' & 0x1f)) {
					tty_close();
					printf("\nSuspending pty session\n");
					exit(0);
					}
				if (buf[i] == '\0') {
					sprintf(buf1, "%%%02x", buf[i]);
					write(fd, "\0", 2);
					}
				else {
					if (write(fd, &buf[i], 1) != 1) {
						printf("write error - closing down\n");
						tty_close();
						exit(0);
						}
					}
				}
			}
		if (FD_ISSET(fd, &rbits)) {
			n = read(fd, buf, sizeof buf);
			if (n <= 0) {
				tty_close();
				printf("Connection closed\n");
				exit(0);
				}
			write(1, buf, n);
			}
		}
}
/**********************************************************************/
/*   Command line switch decoder.				      */
/**********************************************************************/
int
do_switches(int argc, char **argv)
{	int	i;
	char	*cp;

	for (i = 1; i < argc; i++) {
		cp = argv[i];
		if (*cp++ != '-')
			return i;

		while (*cp) {
			switch (*cp++) {
			  case 'f':
			  	force = TRUE;
			  	break;
			  default:
			  	usage();
				break;
			  }
			}
		}
	return i;
}
static void
send_cmd(int fd, char *cmd)
{	char	buf[BUFSIZ];
	int	i, n;

	write(fd, "", 1);
	write(fd, cmd, strlen(cmd) + 1);
	while((n = read(fd, buf, sizeof buf)) > 0) {
		for (i = 0; i < n; i++) {
			if (buf[i]) {
				putchar(buf[i]);
				}
			}
		if (memchr(buf, '\0', n))
			break;
		}
}
static int
set_blocking(int fd, int flag)
{
	int fl = fcntl(fd, F_GETFL, 0);
	if (flag)
		fl &= ~O_NDELAY;
	else
		fl |= O_NDELAY;
	fcntl(fd, F_SETFL, fl);
	return 0;
}

static void
set_window_size(int fd)
{	char	buf[BUFSIZ];
	static struct winsize	winsize;

	ioctl(0, TIOCGWINSZ, &winsize);
	sprintf(buf, "winsz rows=%d cols=%d", winsize.ws_row, winsize.ws_col);
	write(fd, "\0", 1);
	write(fd, buf, strlen(buf)+1);
}
static void
sigwinch_handler()
{
	sigwinch++;
}

void
tty_close()
{
	tcsetattr(0, TCSANOW, &ot);
}
void
tty_open()
{
	/***********************************************/
	/*   Get current settings and save them.       */
	/***********************************************/
	tcgetattr(0, &ot);	
	nt = ot;

	/***********************************************/
	/*   Set the modes we're interested in.	       */
	/***********************************************/
# if	defined(HAVE_TERMIO) || defined(HAVE_TERMIOS)
		/***********************************************/
		/*   Note  that  ^S/^Q and ^Z will be handled  */
		/*   by  system  and  not  seen  by CRISP. If  */
		/*   you   want   to   see  these  characters  */
		/*   you'll  have  to  do  input_mode()  flow  */
		/*   control.				       */
		/***********************************************/
		nt.c_cc[VSUSP] = 0xff;
# if 0
		nt.c_cc[VINTR] = 'Y' & 0x1f;
# else
		nt.c_cc[VINTR] = 0xff;
# endif
		nt.c_cc[VQUIT] = 0xff;
		nt.c_cc[VMIN] = 1;      /* one character read is OK     */
		nt.c_cc[VTIME] = 0;     /* Never time out.              */
		nt.c_iflag |= IGNBRK;
		nt.c_iflag &= ~(ICRNL | INLCR | ISTRIP | IXOFF);
		nt.c_oflag &= ~(OPOST | ONLCR);
		nt.c_cflag |= CS8;      /* allow 8th bit on input       */
		nt.c_cflag &= ~PARENB;  /* Don't check parity           */
		nt.c_lflag &= ~(ECHO | ICANON);
		
# 	if defined(IEXTEN)
		nt.c_lflag &= ~IEXTEN;
# 	endif

# else
		nt.sg_flags |= CBREAK | PASS8;
		nt.sg_flags &= ~(ECHO|CRMOD);
# endif

	kbdflgs = fcntl(0, F_GETFL, 0);

# if defined(TIOCLGET) && defined(LPASS8)
	{int flags;
	ioctl(0, TIOCLGET, &flags);
	flags |= LPASS8;
	ioctl(0, TIOCLSET, &flags);
	}
# endif

	tcsetattr(0, TCSANOW, &nt);
}
/**********************************************************************/
/*   Print out program usage.					      */
/**********************************************************************/
void
usage()
{
	fprintf(stderr, "pty -- connect to the ptysrv daemon\n");
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty\n");
	fprintf(stderr, "            Start a new session.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty allow\n");
	fprintf(stderr, "            Tell ptysrv to enable all connections.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty attach <name> [host] [port]\n");
	fprintf(stderr, "            Attach to an existing open connection. [host] is the\n");
	fprintf(stderr, "            host where the ptysrv daemon is running.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty deny\n");
	fprintf(stderr, "            Tell ptysrv to only let owner to connect.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty detach <name> [host] [port]\n");
	fprintf(stderr, "            Attach to an existing open connection. [host] is the\n");
	fprintf(stderr, "            host where the ptysrv daemon is running.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty label <name>\n");
	fprintf(stderr, "            Label this connection with a friendly name\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty reopen <name> [host] [port]\n");
	fprintf(stderr, "            Like 'attach' but if the session isn't there,\n");
	fprintf(stderr, "            do an 'open' instead.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty restart\n");
	fprintf(stderr, "            Restart the ptysrv daemon\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   pty status\n");
	fprintf(stderr, "            List status of connections on the pty server\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Switches:\n");
	fprintf(stderr, "    -X		X switch\n");
	exit(1);
}
