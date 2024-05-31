#include "meridian_parser.h"

Parser Parser_make(String src) {
    return (Parser) {
        .src = src,
        .position = 0,
        .start = 0,
        .line = 0,
    };
}

void Parser_free(Parser* parser) {
}

static bool Parser_eof(Parser* parser) {
    return parser->position >= parser->src.length;
}

static char Parser_peek(Parser* parser) {
    if(Parser_eof(parser)) {
        return NULL;
    }

    return String_index(parser->src, parser->position + 1);
}

static char Parser_current(Parser* parser) {
    return String_index(parser->src, parser->position);
}

static void Parser_advance(Parser* parser) {
    parser->position++;
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

static bool isSymbolChar(char c) {
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

static void Parse_atom(Parser* parser) {
}


static void Parse_list(Parser* parser) {
}

void Parser_run(Parser* parser) {
    while(parser->position < parser->src.length) {
        char c = Parser_current(parser);
        parser->start = parser->position;

        Parser_advance(parser);

    }
}
