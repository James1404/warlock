const std = @import("std");

const c = @cImport({
    @cInclude("warlock.h");
    @cInclude("warlock_atom.h");
    @cInclude("warlock_common.h");
    @cInclude("warlock_error.h");
    @cInclude("warlock_eval.h");
    @cInclude("warlock_reader.h");
    @cInclude("warlock_builtins.h");
    @cInclude("warlock_string.h");
});

export fn Warlock_run_file(alloc: *c.SexpAllocator, path: c.String) c.Sexp {
    const allocator = std.heap.c_allocator;

    const sPath = path.raw[0..@intCast(path.len)];

    const buffer = std.fs.cwd().readFileAlloc(allocator, sPath, std.math.maxInt(usize)) catch {
        c.Warlock_error("Failed to read file: \"%.*s\"", path.len, path.raw);
        return 0;
    };
    defer allocator.free(buffer);

    return c.Warlock_run(alloc, .{ .raw = buffer.ptr, .len = @intCast(buffer.len) });
}
