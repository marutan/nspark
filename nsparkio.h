/*
 * file input/output function prototypes
 *
 * BB changed name to nsparkio.h because of conflict with
 * Borland C++'s io.h
 * $Header: nsparkio.h 1.4 95/06/23 $
 * $Log:    nsparkio.h,v $
 *
 * Revision 1.4  95/06/23  xx:xx:xx  BB
 * Changed name to nsparkio.h for use with Borland C++
 *
 * Revision 1.3  92/12/09  11:41:20  duplain
 * #ifdef'd out write_halfword() and write_word().
 *
 * Revision 1.2  92/10/07  10:37:46  duplain
 * Changed order of function prototypes in line with io.c r1.5 .
 *
 * Revision 1.1  92/09/29  18:02:35  duplain
 * Initial revision
 *
 */

/* BB changed next two lines */
/* #ifndef __IO_H */
/* #define __IO_H */
#ifndef __NSPARKIO_H
#define __NSPARKIO_H

Ferror check_stream(FILE *fp);
uint8_t read_byte(FILE *ifp);
uint16_t read_halfword(FILE *ifp);
uint32_t read_word(FILE *ifp);
void write_byte(FILE *ofp, uint8_t byte);
#ifdef notyet
void write_halfword(FILE *ofp, uint16_t halfword);
void write_word(FILE *ofp, uint32_t word);
#endif /* notyet */
Header *read_header(FILE *ifp);
Status read_sqsh_header(FILE *ifp, SqshHeader *sqsh_header);
void sqsh_header_to_header(SqshHeader *sqsh_header, Header *Header);

/* BB changed next line */
/* #endif */ /* __IO_H */
#endif /* __NSPARKIO_H */
