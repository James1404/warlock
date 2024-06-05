#include "meridian_reader.h"

#include "meridian_error.h"

#include <stdlib.h>

static bool Reader_eof(Reader* reader) {
    return reader->position >= reader->src.length;
}

static char Reader_peek(Reader* reader) {
    if(Reader_eof(reader)) {
        return NULL;
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

static void Reader_SkipWhitespace(Reader* reader) {
    char c = Reader_current(reader);

    switch(c) {
        case '\n':
            reader->line++;
        case ' ':
        case '\t':
        case '\r':
            Reader_advance(reader);
            break;
        default:
            break;
    }
}

static Atom Reader_ReadList(Reader* reader);

static Atom Reader_ReadAtom(Reader* reader) {
    Reader_SkipWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;
    
    if(isNumber(c)) {
        while(isNumber(c) || c == '.') {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start, reader->position - reader->start);

        return Atom_make_f64(strtod(text.data, NULL));
    }

    if(isSymbolCharStart(c)) {
        while(isSymbolChar(c)) {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start, reader->position - reader->start);

        Symbol sym = { .name = text };
	
	
        return Atom_make_Symbol(sym);
    }

    if(c == '(') {
        return Reader_ReadList(reader);
    }

    Meridian_error("Invalid atom");
}

static Atom Reader_ReadList(Reader* reader) {
    Reader_SkipWhitespace(reader);

    List list = List_make();
 
    Atom atom = Atom_make_List(list);

    if(Reader_match(reader, '(')) {
        while(!Reader_match(reader, ')')) {
            Atom a = Reader_ReadAtom(reader);
            ListNode_push(list.ptr, a);
            Reader_SkipWhitespace(reader);
        }
    }

    return atom;
}

void Reader_run(Reader* reader) {
    reader->global = Reader_ReadAtom(reader);
}

