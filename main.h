/*
 * global variable declarations
 *
 * $Header: main.h 1.4 92/12/08 $
 * $Log:	main.h,v $
 * Revision 1.5  93/08/20  11:35:10  arb
 * Added reference to singlecase variable.
 *
 * Revision 1.4  92/12/08  10:19:54  duplain
 * Added reference to apptype variable.
 * 
 * Revision 1.3  92/10/01  11:21:57  duplain
 * Added reference to retry variable.
 *
 * Revision 1.3 99/03/17   MU
 * Added inffiles
 * 
 * Revision 1.2  92/09/30  10:27:42  duplain
 * Added logfile.
 * 
 * Revision 1.1  92/09/29  18:02:36  duplain
 * Initial revision
 * 
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <stdbool.h>

extern char *ourname;
extern char *archive;
extern char *logfile;
extern char **files;
extern bool inffiles;
extern bool quiet;
extern bool verbose;
extern bool testing;
extern bool listing;
extern bool force;
extern bool stamp;
extern bool retry;
extern bool apptype;
extern bool singlecase;
extern bool to_stdout;
extern bool moddbformat;
#ifdef DEBUGGING
extern bool debugging;
#endif /* DEBUGGING */

#endif /* __MAIN_H */
