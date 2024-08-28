const std = @import("std");
const Atom = @import("atom.zig");
const Sexp = Atom.Sexp;

const Local = struct {
    scope: u64,
    name: []u8,
    sexp: Sexp,
};

const StreamType = std.ArrayList(Atom);
const LocalsType = std.ArrayList(Local);

stream: StreamType,
scope: u64,
locals: LocalsType,

nil: Sexp,

const Self = @This();

pub fn make(allocator: std.mem.Allocator) !Self {
    var result = Self{
        .stream = StreamType.init(allocator),
        .locals = LocalsType.init(allocator),
        .scope = 0,

        .nil = 0,
    };

    result.nil = try result.alloc(.{ .payload = .Nil });

    return result;
}

pub fn free(self: *Self) void {
    self.stream.deinit();
    self.locals.deinit();
}

pub fn get(self: *Self, sexp: Sexp) *Atom {
    return &self.stream.items[sexp];
}

pub fn prettyPrint(self: *Self, writer: anytype, sexp: Sexp) !void {
    return switch (self.get(sexp).payload) {
        .Number => |v| try writer.print("{d}", .{v}),
        .Boolean => |v| try writer.print("{s}", .{if (v) "#t" else "#f"}),
        .String => |v| try writer.print("\"{s}\"", .{v}),
        .Symbol => |v| try writer.print("{s}", .{v}),
        .Keyword => |v| try writer.print(":{s}", .{v}),

        .Fn => |v| {
            try writer.print("(", .{});
            try writer.print("fn ", .{});
            try self.prettyPrint(writer, v.args);
            try writer.print(" -> ", .{});
            try self.prettyPrint(writer, v.body);
            try writer.print(")", .{});
        },
        .Intrinsic => try writer.print("#intrinsic", .{}),

        .Cons => |v| {
            try writer.print("(", .{});
            try self.prettyPrint(writer, v.data);
            try self.prettyPrint(writer, v.next);
            try writer.print(")", .{});
        },
        .Quote => |v| {
            try writer.print("'", .{});
            try self.prettyPrint(writer, v);
        },

        .Nil => try writer.print("Nil", .{}),
    };
}

pub fn registerIntrinsic(self: *Self, name: []u8, func: Atom.IntrinsicFn, argc: i64) void {
    self.setLocal(name, .Intrinsic{
        name,
        func,
        argc,
    });
}

pub fn registerGlobal(self: *Self, name: []u8, sexp: Sexp) void {
    self.setLocal(name, sexp);
}

pub fn increment(self: *Self) void {
    self.scope += 1;
}

pub fn decrement(self: *Self) void {
    self.scope -= 1;

    while (self.locals.getLast().scope > self.scope) {
        self.locals.pop();
    }
}

pub fn setLocal(self: *Self, name: []u8, sexp: Sexp) !void {
    try self.locals.append(.{
        .name = name,
        .scope = self.scope,
        .sexp = sexp,
    });
}

pub fn getLocal(self: *Self, name: []u8) Sexp {
    for (self.locals.items.len..0) |i| {
        const entry = self.locals.items[i];

        if (entry == name) {
            return entry.sexp;
        }
    }

    return self.nil;
}

pub fn alloc(self: *Self, atom: Atom) !Sexp {
    if (atom.payload == .Nil) {
        return self.nil;
    }

    const idx = self.stream.items.len;

    try self.stream.append(atom);

    return idx;
}
