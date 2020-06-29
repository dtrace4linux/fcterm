/**********************************************************************/
/*                                                                    */
/*        CRiSP - Custom Reduced Instruction Set Programmers Editor   */
/*                                                                    */
/*        (C) Paul Fox, 1989-2010                                     */
/*                                                                    */ 
/*  File:  machine.h                                                  */ 
/*                                                                    */ 
/*  This file contains the #defines necessary to configure different  */
/*  machine architectures.                                            */
/**********************************************************************/
/*   $Header: Last edited: 07-Jun-2012 1.9 $ 			      */
/**********************************************************************/

# if !defined(MACHINE_H_INCLUDED)
# define	MACHINE_H_INCLUDED
 
# if	CR_ACORN
#	include	"machos/acorn.h"
# endif 

# if	CR_AIX_3
#	include	"machos/aix3.h"
# endif

# if CR_AIX_4
#	include	"machos/aix4.h"
# endif

# if	CR_ALPHA
#	include	"machos/alpha.h"
# endif

# if	CR_AVIION_M88K
#	include	"machos/aviion_m88k.h"
# endif

# if	CR_AVIION_X86
#	include	"machos/aviion_x86.h"
# endif

# if	CR_BEOS
#	include	"machos/be.h"
# endif

# if	CR_BSDI
#	include	"machos/bsdi.h"
# endif

# if	CR_BSDI3
#	include	"machos/bsdi3.h"
# endif

# if	CR_BSDI4
#	include	"machos/bsdi4.h"
# endif

# if	CR_CETIA
#	include	"machos/cetia.h"
# endif

# if	CR_CRAY
#	include	"machos/cray.h"
# endif

# if	CR_CYGWIN
#	include	"machos/cygwin.h"
# endif

# if	CR_DOS
#	include	"machos/dos.h"
# endif

# if	CR_DRS6000
#	include	"machos/drs6000.h"
# endif

# if	CR_FREEBSD
#	include	"machos/freebsd.h"
# endif

# if	CR_FREEBSD6
#	include	"machos/freebsd6.h"
# endif

# if	CR_FREEBSD8_X86_64
#	include	"machos/freebsd8_x86_64.h"
# endif

# if	CR_HPUX
#	include	"machos/hpux.h"
# endif 

# if	CR_HPUX10
#	include	"machos/hpux10.h"
# endif 

# if	CR_HPUX11_IA64
#	include	"machos/hpux11-ia64.h"
# endif 

# if	CR_HPUX11_IA64_64
#	include	"machos/hpux11-ia64-64.h"
# endif 

/**********************************************************************/
/*   Silicon Graphics running SVR3.2    			      */
/**********************************************************************/
# if	CR_IRIX4
#	include	"machos/irix4.h"
# endif 
/**********************************************************************/
/*   Silicon Graphics running SVR4.				      */
/**********************************************************************/
# if	CR_IRIX5
#	include	"machos/irix5.h"
# endif 
/**********************************************************************/
/*   Silicon Graphics running SVR4. (64-bit)			      */
/**********************************************************************/
# if	CR_IRIX6
#	include	"machos/irix6.h"
# endif 
/**********************************************************************/
/*   Following  entry  works for Interactive Unix V.3.2 rel 3.0 with  */
/*   TCP + X11R4						      */
/**********************************************************************/
# if	CR_ISC_30
#	include	"machos/isc_30.h"
# endif

/**********************************************************************/
/*   Entry for Linux operating system				      */
/**********************************************************************/
# if	CR_LINUX_ARMV6L
#	include	"machos/linux-armv6l.h"
# endif
# if	CR_LINUX_ARMV7L
#	include	"machos/linux-armv7l.h"
# endif
# if	CR_LINUX_LIBC5
#	include	"machos/linux_libc5.h"
# endif
# if	CR_LINUX_GLIBC21
#	include	"machos/linux_glibc2.1.h"
# endif
# if	CR_LINUX_GLIBC22
#	include	"machos/linux_glibc2.2.h"
# endif
# if	CR_LINUX_GLIBC23
#	include	"machos/linux_glibc2.3.h"
# endif
# if	CR_LINUX_IA64
#	include	"machos/linux_ia64.h"
# endif
# if	CR_LINUX_PPC
#	include	"machos/linux_ppc.h"
# endif
# if	CR_LINUX_SPARC
#	include	"machos/linux_sparc.h"
# endif
# if	CR_LINUX_ZSERIES
#	include	"machos/linux_zseries.h"
# endif
# if	CR_LINUX_GLIBC25_X86_32
#	include	"machos/linux-glibc2.5-x86_32.h"
# endif
# if	CR_LINUX_GLIBC25_X86_64
#	include	"machos/linux-glibc2.5-x86_64.h"
# endif
# if	CR_LINUX_X86_32
#	include	"machos/linux-x86_32.h"
# endif
# if	CR_LINUX_X86_64
#	include	"machos/linux-x86_64.h"
# endif

/**********************************************************************/
/*   LynxOS real-time POSIX system.				      */
/**********************************************************************/
# if	CR_LYNX_x86
#	include	"machos/lynx86.h"
# endif
# if	CR_LYNX_68k
#	include	"machos/lynx_68k.h"
# endif
# if	CR_LYNX_ppc
#	include	"machos/lynx_ppc.h"
# endif
# if	CR_LYNX_sparc
#	include	"machos/lynx_sparc.h"
# endif

/**********************************************************************/
/*   Following entry for Motorola M88100 Unix V3.2 system.	      */
/**********************************************************************/
# if	CR_M88K_V3
#	include	"machos/m88k_v3.h"
# endif

# if	CR_M88K_V4
#	include	"machos/m88k_v4.h"
# endif

# if	CR_APPLE_OSX
#	include	"machos/apple_osx.h"
# endif

# if	CR_APPLE_OSX_10_4_PPC
#	include	"machos/apple-osx-10.4-ppc.h"
# endif

# if	CR_APPLE_OSX_10_4_X86
#	include	"machos/apple-osx-10.4-x86.h"
# endif

# if	CR_APPLE_OSX_10_5_X86
#	include	"machos/apple-osx-10.5-x86.h"
# endif

# if	CR_APPLE_OSX_10_6_X86
#	include	"machos/apple-osx-10.6-x86.h"
# endif

# if	CR_APPLE_OSX_10_7_X86
#	include	"machos/apple-osx-10.7-x86.h"
# endif

/**********************************************************************/
/*   Following  entry  works  for  Mips  4.52  with  2.0  compiler    */
/**********************************************************************/
# if    CR_MIPS
#	include	"machos/mips.h"
# endif

# if	CR_NETBSD
#	include	"machos/netbsd.h"
# endif

# if	CR_NT
#	include	"machos/nt.h"
# endif

# if	CR_OPENBSD
#	include	"machos/openbsd.h"
# endif

# if	CR_OS2
#	include	"machos/os2.h"
# endif

# if	CR_QNX
#	include	"machos/qnx.h"
# endif

/**********************************************************************/
/*   Following entry works for ESIX Unix V.3.2 with TCP support.      */
/**********************************************************************/
# if	CR_SYSV_ESIX
#	include	"machos/esix.h"
# endif

# if	CR_SOLARIS2_INTEL
#	include	"machos/sol2-intel.h"
# endif

# if	CR_SOLARIS2_PPC
#	include	"machos/sol2-ppc.h"
# endif

# if	CR_SOLARIS2_SPARC
#	include	"machos/sol2-sparc.h"
# endif

# if	CR_SOLARIS26_SPARC
#	include	"machos/sol26-sparc.h"
# endif

# if	CR_SOLARIS10_X86
#	include	"machos/solaris-10-x86.h"
# endif

# if	CR_SOLARIS10_X86_32
#	include	"machos/solaris-10-x86_32.h"
# endif

# if	CR_SOLARIS10_X86_64
#	include	"machos/solaris-10-x86_64.h"
# endif


# if	CR_SOLARIS10_SPARC32
#	include	"machos/solaris-10-sparc32.h"
# endif

# if	CR_SOLARIS10_SPARC64
#	include	"machos/solaris-10-sparc64.h"
# endif

/**********************************************************************/
/*   Following entry for Sequent running DYNIX. Tested under V2.      */
/**********************************************************************/
# if	CR_SEQUENT
#	include	"machos/sequent.h"
# endif

# if	CR_SUNOS41
#	include	"machos/sunos41.h"
# endif 

# if	CR_SVR4
#	include	"machos/svr4.h"
# endif

# if	CR_SYSV
#	include	"machos/sysv.h"
# endif 

# if	CR_SYSV_3_2
#	include	"machos/sysv32.h"
# endif 

# if	CR_SCO_UNIX
#	include	"machos/sco.h"
# endif 
 
# if	CR_SCO5_UNIX
#	include	"machos/sco5.h"
# endif 

# if	CR_TEST
#	include	"machos/test.h"
# endif

# if	CR_ULTRIX
#	include	"machos/ultrix.h"
# endif

# if	CR_UNIXWARE21
#	include	"machos/unixware21.h"
# endif

# if	CR_UNIXWARE7
#	include	"machos/unixware7.h"
# endif

# if	CR_VMS
#	include	"machos/vms.h"
# endif

# if	CR_WILLOWS
#	include	"machos/willows.h"
# endif

# if	CR_WIN32
#	include	"machos/win32.h"
# endif

# if	CR_WIN64
#	include	"machos/win64.h"
# endif
/**********************************************************************/
/*   CR_OS_CLASS enumerations.					      */
/**********************************************************************/
# define	CR_OS_UNKNOWN	0
# define	CR_OS_LINUX	1
# define	CR_OS_SOLARIS2	2
# define	CR_OS_MACOSX	3
# define	CR_OS_HPUX	4

# define	CR_CPU_UNKNOWN  0
# define	CR_CPU_X86	1
# define	CR_CPU_SPARC32  2
# define	CR_CPU_SPARC64  3
# define	CR_CPU_X86_64	4
# define	CR_CPU_M68K	5
# define	CR_CPU_CRAY	6
# define	CR_CPU_M88K	7
# define	CR_CPU_PPC 	8

/**********************************************************************/ 
/*   This section gives some default values to various definitions.   */
/**********************************************************************/
 
# if	!defined(OPEN_W_BINARY)
#	define	OPEN_W_BINARY	0
#	define	OPEN_R_BINARY	0
# endif
# if	!defined(FOPEN_W_BINARY)
#	define	FOPEN_A_BINARY	"a"
#	define	FOPEN_W_BINARY	"w"
#	define	FOPEN_R_BINARY	"r"
# endif

# if !defined(NULL_DEVICE)
#	define	NULL_DEVICE	"/dev/null"
# endif

# if !defined(DEFAULT_ROOT_DIRECTORY)
#	define	DEFAULT_ROOT_DIRECTORY	"/usr/local/crisp"
# endif

/**********************************************************************/
/*   Get rid of reserved words if not under MS_DOS.		      */
/**********************************************************************/
# if !defined(__MSDOS) && !defined(MSDOS)
#	define	_near
#	define	_far
# endif

# if !defined(EXPORT_FUNCTION)
#	define	EXPORT_FUNCTION
# endif

/**********************************************************************/
/*   Be careful of __declspec() when we are not under Windows.	      */
/**********************************************************************/
# if defined(DLL_EXPORT) && defined(__GNUC__)
#	undef DLL_EXPORT
# endif

# if !defined(DLL_EXPORT)
#	define	DLL_EXPORT
# endif

# if !defined(__STDC__) && !CR_NT && !CR_WIN32 && !CR_IRIS && !CR_OS2
#	define	const
#	define	volatile
# endif

# if !defined(SIGNAL_RET_TYPE)
#	define	SIGNAL_RET_TYPE	void
# endif
# if !defined(SIGNAL_ARG_TYPE)
#	define	SIGNAL_ARG_TYPE	PROTO(int)
# endif
# if !defined(SIGNAL_CAST_TYPE)
#	define	SIGNAL_CAST_TYPE SIGNAL_RET_TYPE (*)(SIGNAL_ARG_TYPE)
# endif

# if !defined(CALLBACK_ATTRIBUTE)
#	define	CALLBACK_ATTRIBUTE
# endif

# if defined(NEED_STRUCT_TIMEVAL)
struct timeval {
	unsigned long tv_sec;
	unsigned long tv_usec;
	};
# endif

/**********************************************************************/
/*   Handle  primitive  16/32  bit types. On the Cray or other wierd  */
/*   architectures  we  are  stuffed  -  we need to define a complex  */
/*   accessor function.                                               */
/**********************************************************************/
# if !defined(BASE_TYPES_DEFINED)
# 	if defined(WORD_SIZE_64)
		typedef	unsigned short 	u_int16;
		typedef	unsigned int	u_int32;
		typedef	int	int32;
# 		define	MASK_32bit(x)	((x) & 0xffffffffL)
# 	else
		typedef	unsigned short 	u_int16;
		typedef	unsigned long	u_int32;
		typedef	long	int32;
# 		define	MASK_32bit(x)	(x)
# 	endif
# 	define	GET_16bit(a)	a
# 	define	SET_16bit(a, b)	a = b
# 	define	GET_32bit(a)	a
# 	define	SET_32bit(a, b)	a = b
# endif

# if !defined(CPU_LITTLE_ENDIAN) && (defined(i386) || defined(__i386__) || defined(_i386))
#	define	CPU_LITTLE_ENDIAN	1
# endif

# if defined(GETTIMEOFDAY_1_ARG)
#	define	GETTIMEOFDAY(x)	gettimeofday(x);
# else
#	define	GETTIMEOFDAY(x)	gettimeofday(x, (struct timezone *) NULL);
# endif

/**********************************************************************/
/*   Decide on varargs and GNU-ness before declaring PROTO().	      */
/**********************************************************************/
# if defined(__GNUC__)
#	undef HAVE_VARARGS_H
#	if !defined(HAVE_STDARG_H)
#		define HAVE_STDARG_H
#	endif
#	if !defined(HAVE_PROTOTYPES)
#		define	HAVE_PROTOTYPES	1
# 	endif
# endif /* defined(__GNUC__) */

/**********************************************************************/
/*   Linux/GLIBC6  inlines  string  functions  causing references to  */
/*   __bzero  and  friends  which are not portable. Get rid of these  */
/*   forever.							      */
/**********************************************************************/
# define	__NO_STRING_INLINES

/**********************************************************************/
/*   Pthreads compatibility.					      */
/**********************************************************************/
# if !defined(HAVE_PTHREAD_H)
#	define	pthread_t	unsigned long
# endif

/**********************************************************************/
/*   Following  macro  used  to  allow  compilation  with ANSI C and  */
/*   non-ANSI C compilers automatically.			      */
/**********************************************************************/
# if !defined(PROTO)
# 	if	defined(__STDC__) || HAVE_PROTOTYPES
#		define	PROTO(x)	x
#	else
#		define	PROTO(x)	()
#	endif
# endif

# if defined(HAVE_STDARG_H)
#		define	STDARG_args	STDARG_arguments
#		define	STDARG_decl2	va_list	STDARG_arguments;
#		define	STDARG_end(x)	va_end(STDARG_arguments)
#		define	STDARG_start(x)	va_start(STDARG_arguments, x)
# 		if	defined(__STDC__) || HAVE_PROTOTYPES
#			define	STDARG_PROTO(x)	x
#		else
#			define	STDARG_PROTO(x)	()
#		endif
#		define	STDARG_vars	STDARG_arguments
# else
#	if defined(HAVE_VARARGS_H)
#		define	STDARG_args	va_alist
#		define	STDARG_decl	va_dcl
#		define	STDARG_decl2	va_list	STDARG_arglist;
#		define	STDARG_end(x)	va_end(STDARG_arglist)
#		define	STDARG_start(x)	va_start(STDARG_arglist)
#		define	STDARG_vars	STDARG_arglist
# 	else
#		define	STDARG_args VA_A, VA_B, VA_C, VA_D, VA_E, VA_F, VA_G, VA_H, VA_I, VA_J, VA_K, VA_L, VA_M, VA_N, VA_O, VA_P, VA_Q, VA_R, VA_S, VA_T, VA_U, VA_V, VA_W, VA_X, VA_Y, VA_Z 
#		define	STDARG_decl	long	STDARG_args;
#		define	STDARG_decl2
#		define	STDARG_end(x)	
#		define	STDARG_start(x)
#		define	STDARG_vars	&VA_A
#		define	va_list	void *
#	endif
#	define	STDARG_PROTO(x)	()
# endif

# if !defined(CR_WIN32) && !defined(__cdecl)
# 	define	__cdecl
# endif

/**********************************************************************/
/*   If we have STAT64 then we must have stat64.		      */
/**********************************************************************/
# if !defined(stat_type_t)
#	if HAVE_struct_stat64
#       	define	stat_type_t	struct stat64
#       	if !defined(_LARGEFILE64_SOURCE)
#       		define _LARGEFILE64_SOURCE 1
#       	endif
#        else
#       	define	stat_type_t	struct stat
#        endif
# endif

/**********************************************************************/
/*   Used to avoid warnings in GCC.				      */
/**********************************************************************/
# if !defined(UNUSED_PARAMETER)
#	define UNUSED_PARAMETER(x)	x = x
# endif

/**********************************************************************/
/*   Machine independent format specs. Win32 uses "%I64x" to achieve  */
/*   this. Defined in the win32.h file.				      */
/**********************************************************************/
# if !defined(SPRINTF_FMT_D)
#	if SUPPORTS_64_BIT
#		define	SPRINTF_FMT_D "%lld"
#		define	SPRINTF_FMT_O "%llo"
#		define	SPRINTF_FMT_U "%llu"
#		define	SPRINTF_FMT_X "%llx"
# 	else
#		define	SPRINTF_FMT_D "%ld"
#		define	SPRINTF_FMT_O "%lo"
#		define	SPRINTF_FMT_U "%lu"
#		define	SPRINTF_FMT_X "%lx"
# 	endif
# endif

/**********************************************************************/
/*   Handle missing types.					      */
/**********************************************************************/
# if !HAVE_TYPE_UINT64_T && !defined(uint64_t)
#	define	uint64_t unsigned long long
# endif

/**********************************************************************/
/*   Turn off gcc/glibc flipping annoying printf_chk type stuff.      */
/**********************************************************************/
# if !defined(_FORTIFY_SOURCE)
#	define _FORTIFY_SOURCE 0
# endif

# if !defined(CAST_TO_INT)
#	define	CAST_TO_INT(ptr)	((int) (long) (ptr))
#	define	CAST_TO_PTR(ptr)	((void *) (long) (ptr))
# endif

# endif /* defined(MACHINE_H_INCLUDED) */
