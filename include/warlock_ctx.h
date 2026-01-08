#include "warlock_arena.h"
#include "warlock_atom.h"
#include "warlock_common.h"

typedef struct Local_ {
    u64 scope;
    char* name;
    Sexp sexp;
} Local_;

typedef struct ctx {
    Arena arena;

    u64 scope;

    u64 localsLen, localsAllocated;
    Local_* locals;
} ctx;

ctx ctx_make(void);
void ctx_free(ctx* env);

void* ctx_malloc(ctx* env, usize len);
Sexp ctx_alloc(ctx* env, Atom atom);

void ctx_incScope(ctx* env);
void ctx_decScope(ctx* env);

void ctx_setLocal(ctx* env, char* name, Sexp sexp);
Sexp ctx_getLocal(ctx* env, char* name);

void ctx_print(ctx* env, Sexp sexp);

bool ctx_ConsTerminated(ctx* env, Sexp sexp);
u64 ctx_ConsLen(ctx* env, Sexp sexp);

char* ctx_format_string(ctx* env, const char* fmt, ...);
char* ctx_copy_string(ctx* env, char* str);
bool ctx_string_equal(ctx* env, char* lhs, char* rhs);
char* ctx_string_substr(ctx* env, char* lhs, usize start, usize len);
