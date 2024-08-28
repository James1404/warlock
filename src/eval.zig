const std = @import("std");
const Atom = @import("atom.zig");
const Ctx = @import("context.zig");

pub fn run(ctx: *Ctx, root: Atom.Sexp) !Atom.Sexp {
    _ = root;

    return ctx.nil;
}
