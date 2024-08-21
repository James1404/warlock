#ifndef WARLOCK_ERROR_H
#define WARLOCK_ERROR_H

#include "warlock_common.h"

void Warlock_info(const char *fmt, ...);
void Warlock_warning(const char *fmt, ...);
void Warlock_error(const char *fmt, ...);

bool Warlock_errorFound(void);

#endif//WARLOCK_ERROR_H
