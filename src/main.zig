const std = @import("std");

const Ctx = @import("context.zig");
const Sexp = @import("atom.zig").Sexp;

fn runFile(allocator: std.mem.Allocator, ctx: Ctx, path: []u8) !Sexp {
    const buffer = try std.fs.cwd().readFileAlloc(allocator, path, std.math.maxInt(usize));
    defer allocator.free(buffer);

    return run(allocator, ctx, buffer);
}

fn run(allocator: std.mem.Allocator, ctx: Ctx, src: []u8) !Sexp {}

var running = true;

fn REPL_Quit(ctx: *Ctx, _: Sexp) Sexp {
    running = false;
    return ctx.nil;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    var alloactor = gpa.allocator();
    defer gpa.deinit();

    var ctx = Ctx.make();
    defer ctx.free();

    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();
    _ = stdin;

    c.Warlock_builtin(&allocator);

    while (running) {
        try stdout.print("* ", .{});

        try stdout.print("\n", .{});
    }
}
