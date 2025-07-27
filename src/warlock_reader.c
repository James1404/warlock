#include "warlock_reader.h"

#include "warlock_atom.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool Reader_eof(Reader* reader) {
    return !(reader->position < reader->src.len);
}

static char Reader_current(Reader* reader) {
    if (Reader_eof(reader)) {
        return '\0';
    }

    return String_idx(reader->src, reader->position);
}

static void Reader_advance(Reader* reader) {
    if (Reader_eof(reader))
        return;

    reader->position++;
    reader->line_pos++;
}

static bool Reader_match(Reader* reader, char expected) {
    if (Reader_current(reader) == expected) {
        Reader_advance(reader);
        return true;
    }

    return false;
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isNumber(char c) { return c >= '0' && c <= '9'; }

static bool isSymbolCharStart(char c) {
    switch (c) {
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

static bool isSymbolChar(char c) { return isSymbolCharStart(c) || isNumber(c); }

Reader Reader_make(String src) {
    return (Reader){
        .src = src,
        .start = 0,
        .position = 0,
        .line = 0,
    };
}

void Reader_free(Reader* reader) {}

static void Reader_pushLine(Reader* reader, Sexp sexp) {
    if (!reader->lines) {
        reader->linesAllocated = 8;
        reader->linesLen = 0;
        reader->lines = malloc(sizeof(Sexp) * reader->linesAllocated);
    }

    if (reader->linesLen >= reader->linesAllocated) {
        reader->linesAllocated *= 2;

        Sexp* temp =
            realloc(reader->lines, sizeof(Sexp) * reader->linesAllocated);
        reader->lines = temp; // TODO: Add error handling
    }

    reader->lines[reader->linesLen++] = sexp;
}

static bool Reader_SkipWhitespace(Reader* reader) {
    char c = Reader_current(reader);

    switch (c) {
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
        while (Reader_current(reader) != '\n') {
            Reader_advance(reader);
        }
        return true;
    default:
        return false;
    }
}

static void Reader_SkipAllWhitespace(Reader* reader) {
    while (!Reader_eof(reader) && Reader_SkipWhitespace(reader))
        ;
}

static Sexp Reader_ReadList(Reader* reader, Environment* env);

static Sexp Reader_ReadSymbol(Reader* reader, Environment* env) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if (isSymbolCharStart(c)) {
        while (isSymbolChar(c)) {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start,
                                 reader->position - reader->start);

        return ATOM_MAKE_V(env, ATOM_SYMBOL, text);
    }

    return ATOM_MAKE_NIL(env);
}

static Sexp Reader_ReadAtom(Reader* reader, Environment* env) {
    Reader_SkipAllWhitespace(reader);

    char c = Reader_current(reader);
    reader->start = reader->position;

    if (c == ')') {
        Warlock_error("Unexpected closing parenthesis ')'");
        Reader_advance(reader);
        return ATOM_MAKE_NIL(env);
    }

    if (c == '\'') {
        Reader_advance(reader);
        Sexp atom = Reader_ReadAtom(reader, env);
        return ATOM_MAKE_V(env, ATOM_QUOTE, atom);
        ;
    }

    if (c == '"') {
        do {
            Reader_advance(reader);
            if (Reader_eof(reader)) {
                Warlock_error(
                    "Reached end-of-file without finding '\"' character");
                return ATOM_MAKE_NIL(env);
            }
        } while (Reader_current(reader) != '"');

        u64 strStart = reader->start + 1;
        u64 strLength = (reader->position - reader->start) - 1;

        Reader_advance(reader);

        String text = String_substr(reader->src, strStart, strLength);
        return ATOM_MAKE_V(env, ATOM_STRING, text);
    }

    if (isNumber(c)) {
        while (isNumber(c) || c == '.') {
            Reader_advance(reader);
            c = Reader_current(reader);
        }

        String text = String_substr(reader->src, reader->start,
                                 reader->position - reader->start);

        return ATOM_MAKE_V(env, ATOM_NUMBER, strtod(text.raw, NULL));
    }

    if (c == '(') {
        return Reader_ReadList(reader, env);
    }

    if (isSymbolCharStart(c)) {
        return Reader_ReadSymbol(reader, env);
    }

    printf("Error invalid character [Line: %lu, Pos: %lu] :: '%c'\n",
           reader->line, reader->line_pos, c);
    return ATOM_MAKE_NIL(env);
}

static Sexp Reader_ReadList(Reader* reader, Environment* env) {
    Reader_SkipAllWhitespace(reader);

    Sexp start = ATOM_MAKE_NIL(env);
    Sexp current = start;

    if (Reader_match(reader, '(')) {
        while (!Reader_match(reader, ')')) {
            if (Reader_eof(reader)) {
                Warlock_error(
                    "Reached end-of-file without finding ')' character");
                return ATOM_MAKE_NIL(env);
            }

            Sexp data = Reader_ReadAtom(reader, env);
            Sexp next = ATOM_MAKE_NIL(env);
            ATOM_SET_S(current, ATOM_CONS, data, next);

            current = Sexp_Rest(env, current);

            Reader_SkipAllWhitespace(reader);
        }
    }

    return start;
}

static void Reader_ReadTopLevel(Reader* reader, Environment* env) {
    Reader_SkipAllWhitespace(reader);

    while (!Reader_eof(reader)) {
        Sexp data = Reader_ReadAtom(reader, env);
        Reader_pushLine(reader, data);

        Reader_SkipAllWhitespace(reader);
    }
}

void Reader_run(Reader* reader, Environment* env) {
    Reader_ReadTopLevel(reader, env);
}
