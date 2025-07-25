#include "warlock_reader.h"

#include "warlock_atom.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool Reader_eof(Reader* reader) {
    return !(reader->position < reader->src.len);
}

static char Reader_current(Reader* reader) {
    if(Reader_eof(reader)) {
        return '\0';
    }

    return STR_IDX(reader->src, reader->position);
}

static void Reader_advance(Reader* reader) {
    if(Reader_eof(reader)) return;

    reader->position++;
    reader->line_pos++;
}

static bool Reader_match(Reader* reader, char expected) {
    if(Reader_current(reader) == expected) {
        Reader_advance(reader);
        return true;
    }

    return false;
}


static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

static bool isSymbolCharStart(char c) {
    switch(c) {
        case '+':
        case '-':
        case '/':
        case '*':
        case '_':
        case '#':
        case '@':
        case '?':
        case '>':
        case '<':
        case '!':
        case '=':
        case '$':
        case '%':
        case '^':
        case '&':
        case '.':
            return true;
        default:
            return isLetter(c);
    }
}

static bool isSymbolChar(char c) {
    return isSymbolCharStart(c) || isNumber(c);
}

Reader Reader_make(String src) {
    return (Reader) {
        .src = src,
        .start = 0,
        .position = 0,
        .line = 0,
    };
}

void Reader_free(Reader* reader) {
}

static bool Reader_SkipWhitespace(Reader* reader) {
    char c = Reader_current(reader);

    switch(c) {
        case '\n':
            reader->line++;
            reader->line_pos = 0;
        case ' ':
        case '\0':
        case '\f':
        case '\t':
        case '\v':
        case '\r':
            Reader_advance(reader);
            return true;
        case ';':
            while(Reader_current(reader) != '\n') {
                Reader_advance(reader);
            }
            return true;
        default:
            return false;
    }
}

static void Reader_SkipAllWhitespace(Reader* reader) {
    while(!Reader_eof(reader) && Reader_SkipWhitespace(reader));
}

static Sexp Reader_ReadList(Reader* reader, SexpAllocator* alloc);

static Sexp Reader_ReadSymbol(Reader* reader, SexpAllocator* alloc) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if(isSymbolCharStart(c)) {
        while(isSymbolChar(c)) {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = STR_SUBSTR(reader->src, reader->start, reader->position - reader->start);

        return ATOM_MAKE_V(alloc, ATOM_SYMBOL, text);
    }

    return ATOM_MAKE_NIL(alloc);
}

static Sexp Reader_ReadAtom(Reader* reader, SexpAllocator* alloc) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if(c == ')') {
        Warlock_error("Unexpected closing parenthesis ')'");
        Reader_advance(reader);
        return ATOM_MAKE_NIL(alloc);
    }

    if(c == '\'') {
        Reader_advance(reader);
        Sexp atom = Reader_ReadAtom(reader, alloc);
        return ATOM_MAKE_V(alloc, ATOM_QUOTE, atom);;
    }

    if(c == '"') {
        do {
            Reader_advance(reader);
            if(Reader_eof(reader)) {
                Warlock_error("Reached end-of-file without finding '\"' character");
                return ATOM_MAKE_NIL(alloc);
            }
        } while(Reader_current(reader) != '"');

        u64 strStart = reader->start + 1;
        u64 strLength = (reader->position - reader->start) - 1;

        Reader_advance(reader);

        String text = STR_SUBSTR(reader->src, strStart, strLength);
        return ATOM_MAKE_V(alloc, ATOM_STRING, text);
    }
    
    if(isNumber(c)) {
        while(isNumber(c) || c == '.') {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = STR_SUBSTR(reader->src, reader->start, reader->position - reader->start);

        return ATOM_MAKE_V(alloc, ATOM_NUMBER, strtod(text.raw, NULL));
    }

    if(c == '(') {
        return Reader_ReadList(reader, alloc);
    }

    if(isSymbolCharStart(c)) {
        return Reader_ReadSymbol(reader, alloc);
    }

    printf("Error invalid character [Line: %lu, Pos: %lu] :: '%02x'\n", reader->line, reader->line_pos, (unsigned char) c);
    return ATOM_MAKE_NIL(alloc);
}

static Sexp Reader_ReadList(Reader* reader, SexpAllocator* alloc) {
    Reader_SkipAllWhitespace(reader);

    Sexp start = ATOM_MAKE_NIL(alloc);
    Sexp current = start;

    if(Reader_match(reader, '(')) {
        while(!Reader_match(reader, ')')) {
            if(Reader_eof(reader)) {
                Warlock_error("Reached end-of-file without finding ')' character");
                return ATOM_MAKE_NIL(alloc);
            }

            Sexp data = Reader_ReadAtom(reader, alloc);
            Sexp next = ATOM_MAKE_NIL(alloc);
            ATOM_SET_S(current, ATOM_CONS, data, next);

            current = Sexp_Rest(alloc, current);
        
            Reader_SkipAllWhitespace(reader);
        }
    }

    return start;
}

static Sexp Reader_ReadTopLevel(Reader* reader, SexpAllocator* alloc) {
    Reader_SkipAllWhitespace(reader);

    Sexp start = ATOM_MAKE_NIL(alloc);
    Sexp current = start;

    while(!Reader_eof(reader)) {
        Sexp data = Reader_ReadAtom(reader, alloc);
        Sexp next = ATOM_MAKE_NIL(alloc);
        ATOM_SET_S(current, ATOM_CONS, data, next);
        
        current = Sexp_Rest(alloc, current);
        
        Reader_SkipAllWhitespace(reader);
    }

    return start;
}

void Reader_run(Reader* reader, SexpAllocator* alloc) {
    reader->root = Reader_ReadTopLevel(reader, alloc);
}

