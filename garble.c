#include "garble.h"

static char *password;
static char *passp;

void
set_password(char *p)
{
	password = p;
}

void
init_garble()
{
	passp = password;
}

uint8_t
ungarble(uint8_t byte)
{
	if (password) {
		byte ^= *passp++;
		if (!*passp) {
			passp = password;
		}
	}

	return byte;
}

uint8_t
garble(uint8_t byte)
{
	/* Garble and ungarble are the same thing */
	return ungarble(byte);
}
