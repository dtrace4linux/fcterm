 /* LIBS=" -lm -lsocket -lnsl -lgen -ldl -lelf" */
# define	CPU_FLOAT_BYTE_ORDER	{0, 1, 2, 3, 4, 5, 6, 7}
# define	CPU_INT_ORDER	0x04030201
# define	CPU_LITTLE_ENDIAN
# define	CRISP_MACHINE_TYPE	"UNIX"
# define	CR_FLAGS	"CR_SVR4"
# define	CR_MACHINE_ID	"svr4"
# define	CR_PLATFORM_ID	37
# define	DONT_WANT_ANSIC_H	1
# define	HAVE_ALARM	1
# define	HAVE_ASCTIME	1
# define	HAVE_ATEXIT	1
# define	HAVE_A_OUT_H	1
# define	HAVE_CFTIME	1
# define	HAVE_CHOWN	1
# define	HAVE_CHROOT	1
# define	HAVE_CRYPT	1
# define	HAVE_DIRENT_H	1
# define	HAVE_DLFCN_H	1
# define	HAVE_DLOPEN	1
# define	HAVE_DUP2	1
# define	HAVE_FORK	1
# define	HAVE_GETCWD	1
# define	HAVE_GETDOMAINNAME	1
# define	HAVE_GETEGID	1
# define	HAVE_GETEUID	1
# define	HAVE_GETGID	1
# define	HAVE_GETHOSTNAME	1
# define	HAVE_GETPGRP	1
# define	HAVE_GETPID	1
# define	HAVE_GETRLIMIT	1
# define	HAVE_GETTIMEOFDAY	1
# define	HAVE_GETUID	1
# define	HAVE_GRP_H	1
# define	HAVE_KILL	1
# define	HAVE_LIBELF_H	1
# define	HAVE_LIMITS_H	1
# define	HAVE_LINK	1
# define	HAVE_LOCALE_H	1
# define	HAVE_LOCKF	1
# define	HAVE_MALLOC_H	1
# define	HAVE_MEMMOVE	1
# define	HAVE_MEMORY_H	1
# define	HAVE_MKDIR	1
# define	HAVE_MKFIFO	1
# define	HAVE_MKNOD	1
# define	HAVE_MKTEMP	1
# define	HAVE_MKTIME	1
# define	HAVE_MMAP	1
# define	HAVE_NETDB_H	1
# define	HAVE_NETINET_IN_H	1
# define	HAVE_NETINET_TCP_H	1
# define	HAVE_OFFSETOF 1
# define	HAVE_POLL	1
# define	HAVE_POPEN	1
# define	HAVE_PROTOTYPES	1
# define	HAVE_PTY	1
# define	HAVE_PWD_H	1
# define	HAVE_REMOVE	1
# define	HAVE_RENAME	1
# define	HAVE_SBRK	1
# define	HAVE_SELECT	1
# define	HAVE_SETEGID	1
# define	HAVE_SETEUID	1
# define	HAVE_SETGID	1
# define	HAVE_SETPGID	1
# define	HAVE_SETPGRP	1
# define	HAVE_SETRLIMIT	1
# define	HAVE_SETSID	1
# define	HAVE_SETUID	1
# define	HAVE_SIGSET	1
# define	HAVE_SLEEP	1
# define	HAVE_STDARG_H	1
# define	HAVE_STDDEF_H	1
# define	HAVE_STDLIB_H	1
# define	HAVE_STRDUP	1
# define	HAVE_STRERROR	1
# define	HAVE_STRFTIME	1
# define	HAVE_STRING_H	1
# define	HAVE_SYMLINKS	1
# define	HAVE_SYSLOG	1
# define	HAVE_SYSLOG_H	1
# define	HAVE_SYS_DIRENT_H	1
# define	HAVE_SYS_DIR_H	1
# define	HAVE_SYS_FAULT_H	1
# define	HAVE_SYS_FCNTL_H	1
# define	HAVE_SYS_FILIO_H	1
# define	HAVE_SYS_IOCTL_H	1
# define	HAVE_SYS_PROCFS_H	1
# define	HAVE_SYS_PTEM_H	1
# define	HAVE_SYS_REGSET_H	1
# define	HAVE_SYS_RESOURCE_H	1
# define	HAVE_SYS_SELECT_H	1
# define	HAVE_SYS_SOCKET_H	1
# define	HAVE_SYS_STREAM_H	1
# define	HAVE_SYS_SYSCONFIG_H	1
# define	HAVE_SYS_SYSTEMINFO_H	1
# define	HAVE_SYS_TIMEB_H	1
# define	HAVE_SYS_TIME_H	1
# define	HAVE_SYS_UN_H	1
# define	HAVE_SYS_WAIT_H	1
# define	HAVE_TEMPNAM	1
# define	HAVE_TERMIOS	1
# define	HAVE_TMPNAM	1
# define	HAVE_UNAME	1
# define	HAVE_UNISTD_H	1
# define	HAVE_UNLINK	1
# define	HAVE_UTIME	1
# define	HAVE_UTIME_H	1
# define	HAVE_VALUES_H	1
# define	HAVE_VFORK	1
# define	HAVE_VSPRINTF	1
# define	HAVE_WAITPID	1
# define	HAVE__CLEANUP	1
# define	HAVE_struct_member_d_ino	1
# define	HAVE_struct_member_d_reclen 1
# define	MMU_PAGESIZE_DEFAULT	4096
# define	PROC_HAVE_SWRSS	1
# define	SIZEOF_DOUBLE   8
# define	SIZEOF_FLOAT    4
# define	SIZEOF_INT	4
# define	SIZEOF_LONG	4
# define	SIZEOF_PTR      4
# define	SIZEOF_SHORT	2
# define	STREAMS_PTY	1
# define	SYSV_SETPGRP	1
# define	UNIX_NAME	"/unix"
# define	UNIX_SYSTEM	1
# define	directory_entry_t struct dirent

/*** DO NOT EDIT BEFORE THIS POINT ***/

/**********************************************************************/
/*   Motif  1.2.2 or so wont allow popup menu to appear if parent is  */
/*   not visible.						      */
/**********************************************************************/
# define	CR_MOTIF_MENU_SHELL_MAPPING_BUG	1

/**********************************************************************/
/*   UnixWare  2.01  has the bug where we die if we do: New->Window;  */
/*   File->Exit                                                       */
/**********************************************************************/
# define	CR_MOTIF_BUG	1

