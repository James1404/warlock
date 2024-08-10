#ifndef SCHEME_READER_H
#define SCHEME_READER_H

#include "scheme_atom.h"
#include "scheme_string.h"
#include "scheme_common.h"

typedef struct {
    String src;
    u64 start, position, line, line_pos;

    Atom global;
} Reader;

Reader Reader_make(String src);
void Reader_free(Reader* reader);

void Reader_run(Reader* reader);

#endif//SCHEME_READER_H
