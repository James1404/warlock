const std = @import("std");

const c = @cImport({
    @cInclude("warlock_builtins.h");
    @cInclude("warlock.h");
    @cInclude("warlock_atom.h");
    @cInclude("warlock_error.h");
    @cInclude("warlock_eval.h");
});

fn AtomMake(alloc: *SexpAllocator, ty: c.AtomType) Sexp {
    return c.SexpAllocator_alloc(alloc, .{ .ty = alloc });
}

fn AtomMakeS(alloc: *SexpAllocator, ty: c.AtomType) Sexp {
    return c.SexpAllocator_alloc(alloc, .{ .ty = alloc });
}

export fn Sexp_Add(alloc: *SexpAllocator, sexp: Sexp) Sexp {
    const result = 
}
