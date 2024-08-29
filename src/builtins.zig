const std = @import("std");

const Ctx = @import("context.zig");
const Sexp = Ctx.Sexp;

fn Add(ctx: *Ctx, _: std.mem.Allocator, sexp: Sexp) Sexp {
    var result: f32 = 0.0;

    var current = sexp;

    while (!ctx.consTerminated(current)) {
        switch (ctx.get(current)) {
            .Cons => |cons| {
                switch (ctx.get(cons.data)) {
                    .Number => |v| result += v,
                    else => break,
                }

                current = cons.next;
            },
            else => break,
        }
    }

    return ctx.alloc(.Number{result});
}
