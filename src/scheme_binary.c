#include "scheme.h"
#include "scheme_error.h"

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
    Scheme_init();

    Scheme_set_error_callback(&error);
    Scheme_builtin();
    Scheme_run_file("test/test.mr");

    Scheme_free();
}
