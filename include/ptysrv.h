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
# include	<list.h>
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
#include	<netinet/in.h>
#include	<tcplib.h>

# define	PTY_SERVER_PORT 13456

# define 	MAX_HIST_SIZE	32768
# define	MAX_NAME_SIZE	64

typedef struct cbuf_t {
	int		c_size;
	int		c_in;
	int		c_out;
	int		c_used;
	unsigned char	*c_buf;
	} cbuf_t;

typedef struct pty_t {
	int		p_pty_fd;
	int		p_ref;
	int		p_pid;
	char		p_name[MAX_NAME_SIZE];
	cbuf_t		p_hist;
	time_t		p_ctime;
	unsigned long	p_uid;
	unsigned	p_rows, p_cols;
	char		*p_slavename;
	char		p_ttyname[64];
	int		p_console_flag;
	int		p_login_shell_flag;
	cbuf_t		p_cbuf;
	int		p_closed;
	} pty_t;

# define 	MAX_CMD_SIZE	256
typedef struct netconn_t {
	int		n_fd;
	int		n_owner;
	int		n_pty_fd; /* Link to the pty. */
	time_t		n_ctime;
	unsigned long	n_uid;
	cbuf_t		n_cbuf;
	int		n_rows;
	int		n_cols;
	struct sockaddr_in n_addr;
	} netconn_t;

# define	TRUE	1
# define	FALSE	0

# if !defined(UNUSED_PARAMETER)
# define	UNUSED_PARAMETER(x)	(x) = (x)
# endif
