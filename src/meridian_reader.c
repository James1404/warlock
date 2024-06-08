#include "meridian_reader.h"

#include "meridian_error.h"

#include <stdlib.h>

static bool Reader_eof(Reader* reader) {
    return reader->position >= reader->src.length;
}

static char Reader_peek(Reader* reader) {
    if(Reader_eof(reader)) {
        return '\0';
    }

    return String_index(reader->src, reader->position + 1);
}

static char Reader_current(Reader* reader) {
    return String_index(reader->src, reader->position);
}

static void Reader_advance(Reader* reader) {
    reader->position++;
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
        case '$':
        case '%':
        case '^':
        case '&':
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
        case ' ':
        case '\t':
        case '\r':
            Reader_advance(reader);
            return true;
        default:
            return false;
    }
}

static void Reader_SkipAllWhitespace(Reader* reader) {
    while(Reader_SkipWhitespace(reader));
}

static Atom Reader_ReadList(Reader* reader);

static Atom Reader_ReadSymbol(Reader* reader) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if(isSymbolCharStart(c)) {
        while(isSymbolChar(c)) {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start, reader->position - reader->start);

        return ATOM_SYMBOL(text);
    }

    return ATOM_NIL();
}

static Atom Reader_ReadAtom(Reader* reader) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if(c == '"') {
        do {
            Reader_advance(reader);
        } while(Reader_current(reader) != '"');

        u64 strStart = reader->start + 1;
        u64 strLength = (reader->position - reader->start) - 1;

        Reader_advance(reader);

        String text = String_substr(reader->src, strStart, strLength);
        return ATOM_STRING(text);
    }
    
    if(isNumber(c)) {
        while(isNumber(c) || c == '.') {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start, reader->position - reader->start);

        return ATOM_NUMBER(strtod(text.data, NULL));
    }

    if(c == '(') {
        return Reader_ReadList(reader);
    }

    if(isSymbolCharStart(c)) {
        return Reader_ReadSymbol(reader);
    }

    Meridian_error("Invalid atom");
    return ATOM_NIL();
}

static Atom Reader_ReadList(Reader* reader) {
    Reader_SkipAllWhitespace(reader);

    Atom list = ATOM_LIST();

    if(Reader_match(reader, '(')) {
        while(!Reader_match(reader, ')')) {
            List_push(&GET_ATOM_LIST(list), Reader_ReadAtom(reader));
            Reader_SkipAllWhitespace(reader);
        }
    }

    return list;
}

static Atom Reader_ReadTopLevel(Reader* reader) {
    Atom list = ATOM_LIST();

    while(!Reader_eof(reader)) {
        List_push(&GET_ATOM_LIST(list), Reader_ReadAtom(reader));
        Reader_SkipAllWhitespace(reader);
    }

    return list;
}

void Reader_run(Reader* reader) {
    reader->global = Reader_ReadTopLevel(reader);
}

