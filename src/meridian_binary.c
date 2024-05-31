#include "meridian.h"
#include "meridian_error.h"

#include <stdio.h>

void error(const char* msg, ErrorSeverity severity) {
    printf("%s\n", msg);
}

int main() {
    Meridian_set_error_callback(&error);
    Meridian_run_file("test/test.psg");
}
