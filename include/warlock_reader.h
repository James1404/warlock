#ifndef WARLOCK_READER_H
#define WARLOCK_READER_H

#include "warlock_atom.h"
#include "warlock_common.h"

typedef struct {
    char* src;
    u64 src_len;

    u64 start, position, line, line_pos;

    Sexp* lines;
    u64 linesLen, linesAllocated;
} Reader;

Reader Reader_make(char* src);
void Reader_free(Reader* reader);

void Reader_run(Reader* reader, Environment* env);

#endif//WARLOCK_READER_H
