#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"

#include <stdio.h>

int main(int argc, char** argv) {
    SexpAllocator allocator = SexpAllocator_make();
    
    Warlock_init();

    Warlock_builtin(&allocator);
    Warlock_run_file(&allocator, STR("test/test.mr"));

    Warlock_free();
    
    SexpAllocator_free(&allocator);
}
