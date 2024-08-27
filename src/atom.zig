const IntrinsicFn = fn (*SexpAllocator, Sexp) Sexp;
const Sexp = u64;

payload: union(enum) {
    Number: f64,
    Boolean: bool,
    String: []u8,
    Symbol: []u8,
    Keyword: []u8,
    Fn: struct {
        args: Sexp,
        body: Sexp,
    },
    Intrinsic: struct {
        name: []u8,
        func: IntrinsicFn,
        argc: i64,
    },
    FFI: struct {
        ptr: anyopaque,
    },
    Cons: struct {
        data: Sexp,
        next: Sexp,
    },
    Quote: Sexp,

    Nil,
},
