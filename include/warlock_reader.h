#ifndef WARLOCK_READER_H
#define WARLOCK_READER_H

#include "warlock_atom.h"
#include "warlock_string.h"
#include "warlock_common.h"

typedef struct {
    String src;
    u64 start, position, line, line_pos;

    Sexp* lines;
    u64 linesLen, linesAllocated;
} Reader;

Reader Reader_make(String src);
void Reader_free(Reader* reader);

void Reader_run(Reader* reader, Environment* env);

#endif//WARLOCK_READER_H
