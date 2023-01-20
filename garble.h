#ifndef GARBLE_H
#define GARBLE_H
#include "spark.h"

void set_password(char *p);
void init_garble(void);
uint8_t ungarble(uint8_t byte);
uint8_t garble(uint8_t byte);

#endif
