#include "meridian.h"
#include "meridian_error.h"

#include <stdio.h>

void error(const char* msg, ErrorSeverity severity) {
    switch(severity) {
        case ErrorSeverity_Info: printf("info: "); break;
        case ErrorSeverity_Warning: printf("warning: "); break;
        case ErrorSeverity_Error: printf("error: "); break;
    }

    printf("%s\n", msg);
}

int main(int argc, char** argv) {
    Meridian_init();

    Meridian_set_error_callback(&error);
    Meridian_builtin();
    Meridian_run_file("test/test.mr");

    Meridian_free();
}
