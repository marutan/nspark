#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spark.h"
#include "arcfs.h"
#include "unarc.h"
#include "error.h"

char *ourname;
bool apptype     = false;
bool force       = false;
bool inffiles    = false;
bool listing     = false;
bool testing     = false;
bool quiet       = true;
bool retry       = false;
bool singlecase  = false;
bool stamp       = true;
bool to_stdout   = false;
bool verbose     = false;
bool moddbformat = false;
char *archive = NULL;
char **files = NULL;
char *logfile = NULL;

void
usage(void)
{
	fprintf(stderr, "sqsh: un-squashes RISC OS squashed files\n");
	fprintf(stderr, "Usage: sqsh [options] <filename>\n");
	fprintf(stderr, "	where options are:\n");
	fprintf(stderr, "       -c output to stdout\n");
	fprintf(stderr, "       -f force file overwrite\n");
	fprintf(stderr, "       -s do not set timestamp of decompressed file\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	int r;
	int i;

	if (argc < 2)
	{
		usage();
	}

	ourname = argv[0];
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'c':
					to_stdout = true;
					break;
				case 'f':
					force = true;
					break;
				case 's':
					stamp = false;
					break;
			}
		}
		else
		{
			break;
		}
	}

	if (i == argc)
	{
		usage();
	}
	archive = argv[i];

	r = do_unsquash();
	if (r != 0 || writesize > 0)
	{
		error("Error unsquashing file");
	}

	return r;
}
