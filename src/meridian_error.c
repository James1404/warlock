#include "meridian_error.h"

#include <stdlib.h>

static ErrorCallback error_callback = NULL;

void Meridian_set_error_callback(ErrorCallback fn) {
    error_callback = fn;
}

void Meridian_error(const char* msg) {
    error_callback(msg, ErrorSeverity_Warning);
}
