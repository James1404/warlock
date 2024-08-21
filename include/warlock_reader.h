#ifndef WARLOCK_READER_H
#define WARLOCK_READER_H

#include "warlock_atom.h"
#include "warlock_string.h"
#include "warlock_common.h"

typedef struct {
    String src;
    u64 start, position, line, line_pos;

    Sexp root;
} Reader;

Reader Reader_make(String src);
void Reader_free(Reader* reader);

void Reader_run(Reader* reader, SexpAllocator* alloc);

#endif//WARLOCK_READER_H
