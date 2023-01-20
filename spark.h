/*
 * defines types used in nspark
 *
 * $Header: spark.h 1.8 95/08/01 $
 * $Log:	spark.h,v $
 * Revision 1.8  95/08/01  xx:xx:xx  BB
 * Added MSDOS6 and MSDOS7 macros
 *
 * Revision 1.7  94/11/09  10:36:00  auj
 * Added Windows NT support.
 *
 * Revision 1.6  93/04/05  12:34:56  arb
 * Added RISC OS support.
 *
 * Revision 1.5  92/11/04  16:57:32  duplain
 * Completed CT_xxx types.
 * 
 * Revision 1.4  92/10/23  14:06:57  duplain
 * Added test for Sys V.4 when defining Sys V file and path name lengths.
 * 
 * Revision 1.3  92/10/06  12:22:11  duplain
 * Removed "days_since_1900" from struct date.
 * 
 * Revision 1.2  92/09/30  10:28:13  duplain
 * Added W_OPENMODE and R_OPENMODE.
 * 
 * Revision 1.1  92/09/29  18:02:41  duplain
 * Initial revision
 * 
 */

#ifndef __SPARK_H
#define __SPARK_H

/*
 * globalise system-type defines...
 */
#if defined(RISCOS2) || defined(RISCOS3)
#define RISCOS
#endif
/* BB changed next line */
/* #if defined(MSDOS2) || defined(MSDOS3) || defined(MSDOS4) || defined(MSDOS5) || defined(WINNT) */
#if defined(MSDOS2) || defined(MSDOS3) || defined(MSDOS4) || \
    defined(MSDOS5) || defined(MSDOS6) || defined(MSDOS7) || \
    defined(MSDOS32) || defined(WINNT)
#define MSDOS
#endif

#include <stdint.h>

/*
 * define the path seperator character, and file open mode string.
 */
#if defined(POSIX)
#define PATHSEP	'/'
#define PATHSEPSTR	"/"
#define W_OPENMODE	"w"
#define R_OPENMODE	"r"
#endif
#if defined(RISCOS)
#define PATHSEP	'.'
#define PATHSEPSTR	"."
#define W_OPENMODE	"w"
#define R_OPENMODE	"r"
#endif
#if defined(MSDOS)
#if defined(WINNT)
#define PATHSEP   '/'       /* Win NT uses \ as the seperator, but MSC
                               allows /, so use that as it makes life
                               easier in filename translation. */
#define PATHSEPSTR    "/"
#else
#define PATHSEP	'\\'
#define PATHSEPSTR	"\\"
#endif
#define W_OPENMODE	"wb"
#define R_OPENMODE	"rb"
#endif

/*
 * define maximum filename and pathname length
 */
#if defined(POSIX)
#define FILENAMELEN	256
#define PATHNAMELEN	1024
#endif
#if defined (RISCOS)
#define FILENAMELEN	10
#define PATHNAMELEN	256
#endif
#if defined (MSDOS)
#if defined (WINNT)
#define FILENAMELEN 256
#define PATHNAMELEN 1024
#else
#define FILENAMELEN	12		/* including dot */
#define PATHNAMELEN	256
#endif
#endif

/*
 * stream error enumeration
 */
typedef enum {FNOERR, FEND, FRWERR} Ferror;

/*
 * file type
 */
typedef enum {NOEXIST, ISFILE, ISDIR } Ftype;

/*
 * un-archiving status
 */
typedef enum {NOERR, RERR, WERR, CRCERR } Status;

/*
 * compressed file header
 */
typedef struct {
	uint8_t comptype;		/* compression type */
	char name[14];		/* name of file */
	uint32_t complen;	/* compressed length */
	uint16_t date;		/* file creation date */
	uint16_t time;		/* file creation time */
	uint16_t crc;		/* Cyclic Redundancy Check */
	uint32_t origlen;	/* original length */
	uint32_t load;		/* load address */
	uint32_t exec;		/* exec address */
	uint32_t attr;		/* file attributes */
} Header;

typedef struct {
	char magic[4];		/* Should be SQSH */
	uint32_t origlen;	/* Original length */
	uint32_t load;		/* load address */
	uint32_t exec;		/* exec address */
	uint32_t reserved;	/* For future use */
} SqshHeader;

/*
 * universal date structure
 */
typedef struct {
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
} Date;
		
/*
 * compress type (passed to uncompress())
 */
typedef enum { COMPRESS, SQUASH, CRUNCH, UNIX_COMPRESS } CompType;

/*
 * compression types
 */
#define CT_NOTCOMP	(uint8_t)0x01	/* not compressed (old) */
#define CT_NOTCOMP2	(uint8_t)0x02	/* not compressed (new) */
#define CT_PACK		(uint8_t)0x03	/* run length */
#define CT_PACKSQUEEZE	(uint8_t)0x04	/* run length + Huffman squeezing */
#define CT_LZOLD	(uint8_t)0x05	/* Lempel-Ziv (old) */
#define CT_LZNEW	(uint8_t)0x06	/* Lempel-Ziv (new) */
#define CT_LZW		(uint8_t)0x07	/* Lempel-Ziv Welch */
#define CT_CRUNCH	(uint8_t)0x08	/* Dynamic LZW with adaptive reset */
#define CT_SQUASH	(uint8_t)0x09	/* PKARC squashing */
#define CT_COMP		(uint8_t)0x7f	/* UNIX compress */

/*
 * other spark defines
 */
#define STARTBYTE	(uint8_t)0x1a	/* start of archive marker */
#define RUNMARK		(uint8_t)0x90	/* start of run (pack/unpack) */
#define ARCHPACK	(uint8_t)0x80	/* bit-7 set in comptype if Archie */

#endif /* __SPARK_H */
