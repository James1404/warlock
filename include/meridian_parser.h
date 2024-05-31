#ifndef MERIDIAN_PARSER_H
#define MERIDIAN_PARSER_H

#include "meridian_atom.h"

typedef struct {
    String src;
    u64 start, position, line;

    ListNode out;
} Parser;

Parser Parser_make(String src);
void Parser_free(Parser* parser);

void Parser_run(Parser* parser);

#endif//MERIDIAN_PARSER_H
