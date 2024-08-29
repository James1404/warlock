const std = @import("std");
const Ctx = @import("context.zig");
const Sexp = Ctx.Sexp;

pub fn run(ctx: *Ctx, root: Sexp) !Sexp {
    _ = root;

    return ctx.nil;
}
