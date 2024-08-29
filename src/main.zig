const std = @import("std");

const Ctx = @import("context.zig");
const Sexp = Ctx.Sexp;
const Reader = @import("reader.zig");
const Eval = @import("eval.zig");

fn runFile(allocator: std.mem.Allocator, ctx: *Ctx, path: []const u8) !Sexp {
    const buffer = try std.fs.cwd().readFileAlloc(allocator, path, std.math.maxInt(usize));
    defer allocator.free(buffer);

    return run(allocator, ctx, buffer);
}

fn run(_: std.mem.Allocator, ctx: *Ctx, src: []const u8) !Sexp {
    var reader = Reader.make(src);
    const root = try reader.run(ctx);
    return try Eval.run(ctx, root);
}

var running = true;

fn REPL_Quit(ctx: *Ctx, _: Sexp) Sexp {
    running = false;
    return ctx.nil;
}

fn registerBuiltins(_: *Ctx) void {}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    defer _ = gpa.deinit();

    var ctx = try Ctx.make(allocator);
    defer ctx.free();

    const stdout = std.io.getStdOut().writer();

    registerBuiltins(&ctx);

    const root = try runFile(allocator, &ctx, "test/test.mr");
    try ctx.prettyPrint(stdout, root);
}
