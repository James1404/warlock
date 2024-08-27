const std = @import("std");
const Atom = @import("warlock_atom.zig");
const Ctx = @import("warlock_context.zig");

src: []u8,
start: u64,
pos: u64,
line: u64,
line_offset: u64,

root: Atom.Sexp,

const Self = @This();

fn run(self: *Self, ctx: Ctx) void {}
