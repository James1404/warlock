#include "warlock_error.h"

#include <stdarg.h>
#include <stdio.h>

static bool found = false;

void Warlock_info(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("Info :: ");

    vprintf(format, args);

    printf("\n");

    va_end(args);
}

void Warlock_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("Warning :: ");

    vprintf(format, args);

    printf("\n");

    va_end(args);
}

void Warlock_error(const char *format, ...) {
    found = true;
    
    va_list args;
    va_start(args, format);

    printf("Error :: ");

    vprintf(format, args);

    printf("\n");

    va_end(args);
}

bool Warlock_foundError(void) { return found; }

