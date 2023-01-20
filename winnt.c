
/*
 * Operating System specific function (Windows NT)
 *
 * $Header: winnt.c 1.0 94/11/09 $
 * $Log:	winnt.c,v $
 * Revision 1.0  94/11/09  10:04:00  auj
 * Initial revision
 * 
 */

#include "spark.h"
#include "date.h"
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>
#include <direct.h>
#include <string.h>

/*
 * return the length of a file
 */
uint32_t
filesize(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return 0;
	return (uint32_t) statb.st_size;;
}

/*
 * test for the existance of a file or directory
 */
Ftype
exist(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return NOEXIST;

	if (statb.st_mode & S_IFDIR)
		return ISDIR;

	return (ISFILE);
}

/*
 * make a directory
 */
int
makedir(char *pathname)
{
	return _mkdir(pathname);
}

/*
 * stamp a file with date and time
 */
int
filestamp(Header *header, char *filename)
{
	Date *date;
	struct tm tm;
	struct utimbuf utimbuf;
	time_t filetime;

	if (exist(filename) == ISDIR)
		return (0);				/* Win NT appears not to allow stamping dirs. */

	if ((header->load & (uint32_t) 0xfff00000) != (uint32_t) 0xfff00000)
		return (0);				/* not a timestamp */

	memset((char *) &tm, '\0', sizeof(tm));

	if (!(date = makedate(header)))
		return (-1);

	tm.tm_sec = date->second;
	tm.tm_min = date->minute;
	tm.tm_hour = date->hour;
	tm.tm_mday = date->day;
	tm.tm_mon = date->month - 1;
	tm.tm_year = date->year;
	filetime = mktime(&tm);

	utimbuf.actime = filetime;
	utimbuf.modtime = filetime;
	return (utime(filename, &utimbuf));
}
