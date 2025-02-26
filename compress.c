
/*
 * compress/uncompress archive
 *
 * Authors:	Spencer W. Thomas	(decvax!utah-cs!thomas)
 *		Jim McKie		(decvax!mcvax!jim)
 *		Steve Davies		(decvax!vax135!petsd!peora!srd)
 *		Ken Turkowski		(decvax!decwrl!turtlevax!ken)
 *		James A. Woods		(decvax!ihnp4!ames!jaw)
 *		Joe Orost		(decvax!vax135!petsd!joe)
 *
 * NOTE: these functions also support "squash" (which is just a
 * 13-bit compress), and "crunch" (which is a 12-bit compress
 * with additional run-length encoding).  AJD
 *
 * $Header: compress.c 1.11 95/08/01 $
 * $Log:	compress.c,v $
 * Revision 1.11 95/08/01  xx:xx:xx  BB
 * Quite a few changes for Borland C/C++
 * Made htab and codetab arrays dynamic.
 * (Compile with -DBB_HUGE_STATIC_ARRAYS if you DO want these
 * huge static arrays in your executable.)
 * Changed pointers to normalized or huge pointers because
 * arrays span more than 64k.
 * Changed a few types from int to long because 32bits integers
 * are needed.
 *
 * Revision 1.10 95/01/25  12:49:43  arb
 * Bug fixes caused by 1.9
 *
 * Revision 1.9  95/01/06  12:00:06  arb
 * Fixes for Alpha.
 *
 * Revision 1.8  94/02/28  23:57:55  arb
 * Fixed number of compression bits for ArcFS format archives
 *
 * Revision 1.7  93/08/20  11:35:20  arb
 * Prevent printing of "uncompressed" etc. if quiet flag is set
 *
 * Revision 1.6  92/12/07  17:17:28  duplain
 * reformatted source.
 *
 * Revision 1.5  92/11/09  14:48:00  duplain
 * Initialised offset and size from getcode() each time uncompress() called.
 *
 * Revision 1.4  92/11/02  11:43:14  duplain
 * Correct comment about crunch/squash in header.
 *
 * Revision 1.3  92/10/23  14:08:13  duplain
 * Minor changes to printf's at end of uncompress.
 *
 * Revision 1.2  92/10/01  11:20:19  duplain
 * Added check for EOF.
 *
 * Revision 1.1  92/09/29  18:02:14  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "spark.h"
#include "pack.h"
#include "main.h"
#include "crc.h"
#include "garble.h"
#include "error.h"

/* BB changed next line because of conflict with Borland's io.h */

/* #include "io.h" */
#include "nsparkio.h"
#include "arcfs.h"

#if defined(__MSDOS__) && !defined(MSDOS32)
#ifdef __WATCOMC__
#include <malloc.h>
#define farcalloc halloc
#else
#include <alloc.h>				/* for farcalloc() */
#endif
#else
#define farcalloc calloc
#endif							/* __MSDOS__ */

#define PBITS 16
#define CRUNCHBITS 12
#define SQUASHBITS 13
#define COMPRESSBITS 16

/* BB changed constant in next line to long: 16bits 65536 == 0 ! */
#define HSIZE 65536L
#define INIT_BITS 9				/* initial number of bits/code */

/* BB changed next macros.
 * Arrays htab and codetab both exceed 64k. To prevent wraparound
   at the 64k boundary, normalized or huge pointers have to be used.
   Since subscripts are 16 bit ints under the Borland compiler,
   subscripts have to be made explicitely long.
   And finally COMPRESSBITS == 16, but 1 << 16 == 0 for 16 bits
   integers! */

/* #define MAXCODE(n_bits)	((1 << (n_bits)) - 1) */

/* #define htabof(i) htab[i] */

/* #define codetabof(i) codetab[i] */

/* #define tab_prefixof(i)	codetabof(i) */

/* #define tab_suffixof(i)	((uint8_t *)(htab))[i] */

/* #define de_stack ((uint8_t *)&tab_suffixof(1<<COMPRESSBITS)) */
#if defined(__MSDOS__) && !defined(MSDOS32)
#define MAXCODE(n_bits)	((long)(1L << (n_bits)) - 1L)
#define htabof(i) htab[(long)(i)]
#define codetabof(i) codetab[(long)(i)]
#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((uint8_t huge *)(htab))[(long)(i)]
#define de_stack \
          ((uint8_t huge *)&tab_suffixof(1L<<COMPRESSBITS))
#else
#define MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#define htabof(i) htab[i]
#define codetabof(i) codetab[i]
#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((uint8_t *)(htab))[i]
#define de_stack ((uint8_t *)&tab_suffixof(1<<COMPRESSBITS))
#endif							/* __MSDOS__ */
#define FIRST 257				/* first free entry */
#define	CLEAR 256				/* table clear output code */

#if defined(__MSDOS__) && !defined(MSDOS32)
#define NSHUGE huge
#else
#define NSHUGE
#endif							/* __MSDOS__ */

static int n_bits;				/* number of bits/code */
static int maxbits;				/* user settable max # bits/code */
static int32_t maxcode;		/* maximum code, given n_bits */
static int32_t maxmaxcode;		/* should NEVER generate this code */

/* BB changed next two lines.
   Under Borland C/C++ static arrays are REALLY static, i.e. they
   clog the executable with some 384k of `empty space'. So use
   dynamic arrays instead. */

/* static count_int htab[HSIZE]; */

/* static uint16_t codetab[HSIZE]; */
#if !defined(__MSDOS__) || defined(MSDOS32)
#define BB_HUGE_STATIC_ARRAYS
#else
/* For those that do want to use static arrays:
   define BB_HUGE_STATIC_ARRAYS. */
#endif

#ifdef BB_HUGE_STATIC_ARRAYS
static int32_t NSHUGE htab[HSIZE];
static uint16_t NSHUGE codetab[HSIZE];
#else							/* BB_HUGE_STATIC_ARRAYS */
static int32_t NSHUGE *htab = NULL;
static uint16_t NSHUGE *codetab = NULL;
#endif							/* BB_HUGE_STATIC_ARRAYS */

static uint8_t rmask[9] =
	{ 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
static int32_t free_ent;		/* first unused entry */
static int clear_flg;
static long readsize;			/* number of bytes left to read */

static int32_t offset;		/* from getcode() */
static size_t size;

static int32_t getcode(FILE * ifp);


Status
uncompress(Header *header, FILE *ifp, FILE *ofp, CompType type)
{
	/* BB changed next line. stackp points to huge pointers. */
	uint8_t NSHUGE *stackp;
	int32_t finchar;
	int32_t code, oldcode, incode;
	char *message;

	init_garble();

#if !defined(BB_HUGE_STATIC_ARRAYS)
	if (!htab)
		htab = (int32_t NSHUGE *) farcalloc(HSIZE, sizeof(int32_t));
	if (!codetab)
		codetab =
			(uint16_t NSHUGE *) farcalloc(HSIZE,
											  sizeof(uint16_t));
	if (!htab || !codetab)
	{
		error("%s: uncompress: out of memory", ourname);
		exit(1);
	}
#endif							/* ! BB_HUGE_STATIC_ARRAYS */

	crc = 0;
	clear_flg = 0;
	offset = 0;
	size = 0;
	readsize = header->complen;

	if (type == SQUASH)
		maxbits = SQUASHBITS;
	else if (type == UNIX_COMPRESS)
	{
		/* Read the unix compress header */
		read_byte(ifp);
		read_byte(ifp);
		maxbits = read_byte(ifp) & 0x1f;
		readsize -= 3;
	}
	else
	{
		if (arcfs)
		{
			maxbits = arcfs_maxbits;
			ungarble('\0'); // Need to consume one byte of password.
		}
		else
		{
			maxbits = read_byte(ifp);
			readsize--;
		}
	}
	maxmaxcode = MAXCODE(maxbits);

	/*
	 * As above, initialize the first 256 entries in the table.
	 */
	maxcode = MAXCODE(n_bits = INIT_BITS);
	for (code = 255; code >= 0; code--)
	{
		tab_prefixof(code) = 0;
		tab_suffixof(code) = (uint8_t) code;
	}
	free_ent = FIRST;

	finchar = oldcode = getcode(ifp);
	if (oldcode == -1)			/* EOF already? */
		goto compress_exit;		/* Get out of here */

	/* first code must be 8 bits = char */
	if (type == CRUNCH)
	{
		putc_init();
		/* BB changed next line for Borland C/C++ 4 */
		putc_ncr(ofp, (uint8_t) finchar);
	}
	else
	{
		/* BB changed next three lines for Borland C/C++ 4 */
		if (!testing)
			write_byte(ofp, (uint8_t) finchar);
		calccrc((uint8_t) finchar);
	}

	stackp = de_stack;

	while ((code = getcode(ifp)) != -1)
	{
		if (check_stream(ifp) != FNOERR)
			break;
		if (code == CLEAR)
		{
			for (code = 255; code >= 0; code--)
				tab_prefixof(code) = 0;
			clear_flg = 1;
			free_ent = FIRST - 1;
			if ((code = getcode(ifp)) == -1)	/* O, untimely death! */
				break;
		}
		incode = code;
		/*
		 * Special case for KwKwK string.
		 */
		if (code >= free_ent)
		{
			/* BB changed next line for Borland C/C++ 4 */
			*stackp++ = (uint8_t) finchar;
			code = oldcode;
		}
		/*
		 * Generate output characters in reverse order
		 */

		while (code >= 256)
		{
			if ((char NSHUGE *)(stackp+1) > (char NSHUGE *)(&htab[0] + HSIZE))
			{
				error("%s: uncompress: corrupt or garbled archive file", ourname);
				exit(1);
			}
			*stackp++ = tab_suffixof(code);
			code = tab_prefixof(code);
		}
		if ((char NSHUGE *)(stackp+1) > (char NSHUGE *)(&htab[0] + HSIZE))
		{
			error("%s: uncompress: corrupt or garbled archive file", ourname);
			exit(1);
		}
		/* BB changed next line for Borland C/C++ 4 */
		/* *stackp++ = finchar = tab_suffixof(code); */
		finchar = tab_suffixof(code);
		*stackp++ = (uint8_t) finchar;

		/*
		 * And put them out in forward order
		 */
		while (stackp > de_stack)
		{
			stackp--;
			if (type == CRUNCH)
				putc_ncr(ofp, *stackp);
			else
			{
				if (!testing)
					write_byte(ofp, *stackp);
				calccrc(*stackp);
			}
		}

		/*
		 * Generate the new entry.
		 */
		if ((code = free_ent) < maxmaxcode)
		{
			/* BB changed next two lines for Borland C/C++ 4 */
			tab_prefixof(code) = (uint16_t) oldcode;
			tab_suffixof(code) = (uint8_t) finchar;
			free_ent = code + 1;
		}
		/*
		 * Remember previous code.
		 */
		oldcode = incode;
	}
  compress_exit:
	if (check_stream(ifp) == FRWERR)
		return (RERR);
	if (!testing && check_stream(ofp) == FRWERR)
		return (WERR);
	if ((uint16_t) crc != header->crc)
		return (CRCERR);
	if (testing)
		switch (type)
		{
		case COMPRESS:
		case UNIX_COMPRESS:
			message = "OK (compressed)";
			break;
		case CRUNCH:
			message = "OK (crunched)";
			break;
		case SQUASH:
			message = "OK (squashed)";
			break;
		default:
			message = "internal error";
			break;
		}
	else
		switch (type)
		{
		case COMPRESS:
		case UNIX_COMPRESS:
			message = "uncompressed";
			break;
		case CRUNCH:
			message = "uncrunched";
			break;
		case SQUASH:
			message = "unsquashed";
			break;
		default:
			message = "internal error";
			break;
		}
	if (!quiet)
		msg("%s", message);
	return (NOERR);
}

/*
 * Read one code from the input.  If EOF, return -1.
 */
static int32_t
getcode(FILE *ifp)
{
	int32_t code;
	static uint8_t buf[COMPRESSBITS];
	int r_off, bits;
	size_t i;
	/* BB changed next line. We are doing pointer-artithmatics
	   and that can be dangerous if other than normalized (huge)
	   pointers are being used. */
	uint8_t NSHUGE *bp = buf;

	assert(offset >= 0);

	if (clear_flg > 0 || (size_t) offset >= size || free_ent > maxcode)
	{
		int tmp_size;
		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies
		 * reading a new buffer full, too.
		 */
		if (free_ent > maxcode)
		{
			n_bits++;
			maxcode = n_bits == maxbits ? maxmaxcode : MAXCODE(n_bits);
		}
		if (clear_flg > 0)
		{
			maxcode = MAXCODE(n_bits = INIT_BITS);
			clear_flg = 0;
		}
		if (readsize == 0)
			return (-1);
		tmp_size = readsize < n_bits ? readsize : n_bits;
		assert(tmp_size >= 0);
		size = fread(buf, 1, tmp_size, ifp);
		if (size == 0)
			return (-1);		/* end of file */
		for (i = 0; i < size; i++)
		{
			buf[i] = ungarble(buf[i]);
		}
		readsize -= size;
		offset = 0;
		/* Round size down to integral number of codes */
		size = (size << 3) - (n_bits - 1);
	}
	r_off = (int)offset;
	bits = n_bits;

	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/* Get first part (low order bits) */

	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;			/* now, offset into code word */
	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if (bits >= 8)
	{
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}
	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	offset += n_bits;

	return (code);
}
