/*************** Identification ************************/

# define	CR_MACHINE_ID	"apple-osx-10.7-x86"
# define	CR_PLATFORM_ID	74
# define	CR_CPU_CLASS	"0"
# define	CR_OS_CLASS	CR_OS_MACOSX
# define	CR_FLAGS	"CR_APPLE_OSX_10_7_X86"
# define	UNIX_SYSTEM	1
# define	CRISP_MACHINE_TYPE	"UNIX"
# define	CPU_LITTLE_ENDIAN	1
# define	CPU_FLOAT_BYTE_ORDER	{0, 1, 2, 3, 4, 5, 6, 7}
# define	CPU_INT_ORDER	0x04030201
# define	SIZEOF_SHORT	2
# define	SIZEOF_INT	4
# define	SIZEOF_LONG	8
# define	SIZEOF_FLOAT    4
# define	SIZEOF_DOUBLE   8
# define	SIZEOF_PTR      8
# define	SIZEOF_stat_st_size      8
# define	STREAMS_PTY	1
# define	HAVE_PTY	1
# define	HAVE_DEV_RANDOM	1
# define	HAVE_DEV_URANDOM	1
# define	DONT_WANT_SYS_STREAM_H	1
# define	HAVE_ARPA_INET_H	1
# define	HAVE_DLFCN_H	1
# define	HAVE_DIRENT_H	1
# define	HAVE_EXECINFO_H	1
# define	HAVE_FNMATCH_H	1
# define	HAVE_GLOB_H	1
# define	HAVE_GRP_H	1
# define	HAVE_LIBGEN_H	1
# define	HAVE_LIMITS_H	1
# define	HAVE_LOCALE_H	1
# define	HAVE_MACH_O_DYLD_H	1
# define	HAVE_MEMORY_H	1
# define	HAVE_NLIST_H	1
# define	HAVE_NETDB_H	1
# define	HAVE_NETINET_IN_H	1
# define	HAVE_NETINET_TCP_H	1
# define	HAVE_PWD_H	1
# define	HAVE_PTHREAD_H	1
# define	HAVE_STDDEF_H	1
# define	HAVE_STDINT_H	1
# define	HAVE_STDLIB_H	1
# define	HAVE_STRING_H	1
# define	HAVE_SYSLOG_H	1
# define	HAVE_SYS_ACL_H	1
# define	HAVE_SYS_DIR_H	1
# define	HAVE_SYS_DIRENT_H	1
# define	HAVE_SYS_FILIO_H	1
# define	HAVE_SYS_FCNTL_H	1
# define	HAVE_SYS_IOCTL_H	1
# define	HAVE_SYS_PARAM_H	1
# define	HAVE_SYS_PROC_H	1
# define	HAVE_SYS_RESOURCE_H	1
# define	HAVE_SYS_SELECT_H	1
# define	HAVE_SYS_SOCKIO_H	1
# define	HAVE_SYS_SOCKET_H	1
# define	HAVE_SYS_SYSCALL_H	1
# define	HAVE_SYS_SYSCTL_H	1
# define	HAVE_SYS_SYSLIMITS_H	1
# define	HAVE_SYS_VCMD_H	1
# define	HAVE_SYS_TIME_H	1
# define	HAVE_SYS_TIMEB_H	1
# define	HAVE_SYS_UN_H	1
# define	HAVE_SYS_WAIT_H	1
# define	HAVE_TERMIOS	1
# define	HAVE_UCONTEXT_H	1
# define	HAVE_UTIME_H	1
# define	HAVE_UTMP_H	1
# define	HAVE_UTMPX_H	1
# define	HAVE_UNISTD_H	1
# define	HAVE_WCHAR_H	1
# define	HAVE_WORDEXP_H	1
# define	WORD_SIZE_64
# define	directory_entry_t struct dirent
# define	QSORT_IS_UNSTABLE	1

/* LIBS=" -ldl -lm -lpthread -lz" */

/*************** Libraries ************************/

# define LIBdl_AVAILABLE 1
# define LIBm_AVAILABLE 1
# define LIBpthread_AVAILABLE 1
# define LIBz_AVAILABLE 1

/*************** Functions ************************/

# define	HAVE__CLEANUP	1
/* UNIMPLEMENTED acl() */
# define	HAVE_ALARM	1
# define	HAVE_ASCTIME	1
# define	HAVE_ASCTIME_R	1
# define	HAVE_ATOL	1
# define	HAVE_ATOLL	1
# define	HAVE_ATEXIT	1
# define	HAVE_BACKTRACE	1
# define	HAVE_BACKTRACE_SYMBOLS	1
# define	HAVE_BCOPY	1
/* UNIMPLEMENTED cftime() */
# define	HAVE_CHOWN	1
# define	HAVE_CHROOT	1
/* UNIMPLEMENTED clone() */
/* UNIMPLEMENTED __creat64() */
/* UNIMPLEMENTED creat64() */
# define	HAVE_CRYPT	1
# define	HAVE_CTIME_R	1
# define	HAVE_DIRNAME	1
# define	HAVE_DLADDR	1
# define	HAVE_DLOPEN	1
# define	HAVE_DUP2	1
/* UNIMPLEMENTED epoll() */
/* UNIMPLEMENTED facl() */
# define	HAVE_FCHOWN	1
# define	HAVE_FDATASYNC	1
# define	HAVE_FGETPOS	1
/* UNIMPLEMENTED __fgetpos64() */
/* UNIMPLEMENTED fgetpos64() */
# define	HAVE_FLOCK	1
/* UNIMPLEMENTED fopen64() */
/* UNIMPLEMENTED __fopen64() */
/* UNIMPLEMENTED freopen64() */
/* UNIMPLEMENTED __freopen64() */
# define	HAVE_FORK	1
# define	HAVE_FSEEKO	1
/* UNIMPLEMENTED fseek64() */
# define	HAVE_FTELLO	1
/* UNIMPLEMENTED ftell64() */
# define	HAVE_FSETPOS	1
/* UNIMPLEMENTED fsetpos64() */
/* UNIMPLEMENTED __fsetpos64() */
# define	HAVE_FSTAT64	1
/* UNIMPLEMENTED __fstat64() */
# define	HAVE_FSTATVFS	1
/* UNIMPLEMENTED fstatvfs64() */
/* UNIMPLEMENTED __fstatvfs64() */
# define	HAVE_FSYNC	1
# define	HAVE_FTIME	1
# define	HAVE_FTRUNCATE	1
# define	HAVE_GETCWD	1
# define	HAVE_GETDOMAINNAME	1
/* UNIMPLEMENTED getdents() */
/* UNIMPLEMENTED getdents64() */
/* UNIMPLEMENTED __getdents64() */
# define	HAVE_GETEGID	1
# define	HAVE_GETEUID	1
# define	HAVE_GETGID	1
# define	HAVE_GETHOSTID	1
# define	HAVE_GETHOSTNAME	1
# define	HAVE_GETPAGESIZE	1
# define	HAVE_GETPROGNAME	1
# define	HAVE_GETPGRP	1
# define	HAVE_GETPID	1
# define	HAVE_GETRLIMIT	1
/* UNIMPLEMENTED getrlimit64() */
/* UNIMPLEMENTED __getrlimit64() */
/* UNIMPLEMENTED getruid() */
# define	HAVE_GETTIMEOFDAY	1
# define	HAVE_GETUID	1
# define	HAVE_GMTIME_R	1
# define	HAVE_INFLATE	1
# define	HAVE_KILL	1
# define	HAVE_LCHOWN	1
# define	HAVE_LINK	1
/* UNIMPLEMENTED lltostr() */
/* UNIMPLEMENTED lseek64() */
/* UNIMPLEMENTED llseek() */
/* UNIMPLEMENTED _lseek64() */
# define	HAVE_LOCALTIME_R	1
# define	HAVE_LOCKF	1
/* UNIMPLEMENTED lockf64() */
/* UNIMPLEMENTED __lockf64() */
# define	HAVE_LSTAT	1
# define	HAVE_LSTAT64	1
/* UNIMPLEMENTED __lstat64() */
# define	HAVE_MEMMOVE	1
# define	HAVE_MKDIR	1
# define	HAVE_MKFIFO	1
# define	HAVE_MKNOD	1
# define	HAVE_MKSTEMP	1
# define	HAVE_MKTEMP	1
# define	HAVE_MKTIME	1
# define	HAVE_MMAP	1
/* UNIMPLEMENTED mmap64() */
/* UNIMPLEMENTED __mmap64() */
# define	HAVE_MPROTECT	1
# define	HAVE_NANOSLEEP	1
/* UNIMPLEMENTED open64() */
/* UNIMPLEMENTED __open64() */
# define	HAVE_OPENPTY	1
# define	HAVE_POLL	1
# define	HAVE_POPEN	1
# define	HAVE___PROGNAME	1
/* UNIMPLEMENTED __progname_full() */
# define	HAVE_PTHREAD_CREATE	1
# define	HAVE_PUTENV	1
# define	HAVE_RAND	1
# define	HAVE_RANDOM	1
# define	HAVE_READDIR	1
/* UNIMPLEMENTED readdir64() */
/* UNIMPLEMENTED __readdir64() */
/* UNIMPLEMENTED readdir64_r() */
/* UNIMPLEMENTED __readdir64_r() */
# define	HAVE_READDIR_R	1
# define	HAVE_READLINK	1
# define	HAVE_REALPATH	1
# define	HAVE_REMOVE	1
# define	HAVE_RENAME	1
/* UNIMPLEMENTED resolvepath() */
# define	HAVE_SBRK	1
# define	HAVE_SCANDIR	1
/* UNIMPLEMENTED scandir64() */
/* UNIMPLEMENTED __scandir64() */
# define	HAVE_SELECT	1
# define	HAVE_SETEGID	1
# define	HAVE_SETEUID	1
# define	HAVE_SETGID	1
# define	HAVE_SETLOCALE	1
# define	HAVE_SETPGID	1
# define	HAVE_SETPGRP	1
/* UNIMPLEMENTED setpgrp_XX2() */
# define	HAVE_SETRGID	1
# define	HAVE_SETRUID	1
# define	HAVE_SETPROGNAME	1
# define	HAVE_SETRLIMIT	1
/* UNIMPLEMENTED setrlimit64() */
/* UNIMPLEMENTED __setrlimit64() */
# define	HAVE_SETSID	1
# define	HAVE_SETUID	1
# define	HAVE_SIGHOLD	1
# define	HAVE_SIGINTERRUPT	1
# define	HAVE_SIGPROCMASK	1
# define	HAVE_SIGRELSE	1
# define	HAVE_SIGSET	1
# define	HAVE_SIGSETMASK	1
# define	HAVE_SLEEP	1
# define	HAVE_SNPRINTF	1
# define	HAVE_STAT64	1
/* UNIMPLEMENTED __stat64() */
# define	HAVE_STATVFS	1
/* UNIMPLEMENTED statvfs64() */
/* UNIMPLEMENTED __statvfs64() */
/* UNIMPLEMENTED stpcpy() */
# define	HAVE_STRCASECMP	1
# define	HAVE_STRCOLL	1
# define	HAVE_STRDUP	1
# define	HAVE_STRERROR	1
# define	HAVE_STRERROR_R	1
# define	HAVE_STRFTIME	1
# define	HAVE_STRLCPY	1
# define	HAVE_STRLCAT	1
# define	HAVE_STRNCASECMP	1
# define	HAVE_STRSTR	1
# define	HAVE_STRTOK_R	1
# define	HAVE_STRTOUL	1
# define	HAVE_STRTOLL	1
# define	HAVE_STRTOULL	1
# define	HAVE_STRXFRM	1
# define	HAVE_SYMLINK	1
# define	HAVE_SYSCALL	1
# define	HAVE_SYSCTL	1
# define	HAVE_SYSLOG	1
# define	HAVE_TEMPNAM	1
/* UNIMPLEMENTED thr_create() */
# define	HAVE_TMPFILE	1
/* UNIMPLEMENTED tmpfile64() */
/* UNIMPLEMENTED __tmpfile64() */
# define	HAVE_TMPNAM	1
# define	HAVE_TRUNCATE	1
/* UNIMPLEMENTED truncate64() */
/* UNIMPLEMENTED __truncate64() */
/* UNIMPLEMENTED ulltostr() */
# define	HAVE_USLEEP	1
# define	HAVE_UNAME	1
# define	HAVE_UNLINK	1
# define	HAVE_UTIME	1
# define	HAVE_VFORK	1
# define	HAVE_VSPRINTF	1
# define	HAVE_VSNPRINTF	1
# define	HAVE_WAITPID	1

/*************** Miscellaneous ************************/

# define	COMMAND_STRIP	"strip -x"
# define	CR_CPU_TYPE	CR_CPU_X86_64
# define	SYS_SHLIB_SUPPORT	1
# define	MMU_PAGESIZE_DEFAULT	4096
# define	HAVE_PTY	1
# define	HAVE_struct_member_d_ino	1
# define	HAVE_struct_member_d_fileno	1
# define	HAVE_struct_member_d_reclen 1
# define	HAVE_struct_member_d_namlen 1
# define	HAVE_sys_errlist	1
# define	HAVE_STDDEF_H 1
# define	HAVE_OFFSETOF 1
# define	NEED_WORD_ALIGNMENT	0
# define	HAVE_struct_stat64	1
# define	HAVE_TYPE_UINT64_T 1
# define	SIZEOF_LONG_LONG	8
# define	SUPPORTS_LONG_LONG	1
# define	SUPPORTS_64_BIT		1
# define	TYPE_int64		long
# define	TYPE_uint64		unsigned long
# define	HAVE_PROTOTYPES	1
# define	SYSV_SETPGRP	1
# define	DONT_WANT_ANSIC_H	1
# define	HAVE_DIRNAME_PROTOTYPE	1
# define	HAVE_FOPEN64_PROTOTYPE	0
# define	HAVE_FSEEK64_PROTOTYPE	0
# define	HAVE_FTELL64_PROTOTYPE	0
# define	HAVE_STAT64_PROTOTYPE	1
# define	HAVE_OFF_T_TYPE 1
# define	PRINTF_64_BIT_BROKEN	0

/*** DO NOT EDIT BEFORE THIS POINT ***/

/**********************************************************************/
/*   Date April 2010						      */
/**********************************************************************/

/**********************************************************************/
/*   Generic define to avoid having to handle OSX variants.	      */
/**********************************************************************/
# define	OS_MACOSX	1

/**********************************************************************/
/*   Oh  dear  Mac OS X 10.1.4 has cache consistency problems. If we  */
/*   run crtags and need to extend the file then the seek/write will  */
/*   cause a coherency problem and we end up with not only a corrupt  */
/*   tags file but possible core dumps due to assertion failure.      */
/**********************************************************************/
# define	CR_MMAP_ENABLE	0
/*# undef HAVE_MMAP*/

/**********************************************************************/
/*   Dont include ucontext.h on 10.6				      */
/**********************************************************************/
# if defined(CR_APPLE_OSX_10_6_X86)
#	undef HAVE_UCONTEXT_H
# endif
