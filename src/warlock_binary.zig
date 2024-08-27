const std = @import("std");

const c = @cImport({
    @cInclude("warlock.h");
    @cInclude("warlock_atom.h");
    @cInclude("warlock_error.h");
    @cInclude("warlock_builtins.h");
});

usingnamespace @import("warlock.zig");
usingnamespace @import("warlock_atom.zig");

var running = true;

export fn REPL_Quit(alloc: *c.SexpAllocator, sexp: c.Sexp) c.Sexp {
    _ = sexp;

    running = false;

    return c.ATOM_MAKE_NIL(alloc);
}

pub fn main() !void {
    var allocator = c.SexpAllocator_make();
    defer c.SexpAllocator_free(&allocator);

    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();
    _ = stdin;

    c.Warlock_builtin(&allocator);

    while (running) {
        try stdout.print("* ", .{});

        try stdout.print("\n", .{});
    }
}
