const std = @import("std");
const Atom = @import("warlock_atom.zig");

const Local = struct {
    scope: u64,
    name: []u8,
    sexp: Sexp,
};

stream: std.ArrayList(Atom),
scope: u64,
locals: std.ArrayList(Local),

nil: Sexp,

const Self = @This();

pub fn make() Self {
    const result = Self{
        .scope = 0,
    };

    result.stream.init();
    result.locals.init();

    result.nil = result.alloc(.Nil);

    return result;
}

pub fn free(self: *Self) void {
    self.stream.deinit();
    self.locals.deinit();
}

pub fn prettyPrint(self: *Self, writer: *std.io.AnyWriter, sexp: Sexp) !void {
    return switch (sexp) {
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
        .FFI => try writer.print("#intrinsic", .{}),

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
        else => try writer.print("Error", .{}),
    };
}

fn registerIntrinsic(self: *Self, name: []u8, func: IntrinsicFn, argc: i64) void {
    self.setLocal(name, .Intrinsic{
        name,
        func,
        argc,
    });
}

fn registerGlobal(self: *Self, name: []u8, sexp: Sexp) void {
    self.setLocal(name, sexp);
}

fn increment(self: *Self) void {
    self.scope += 1;
}

fn decrement(self: *Self) void {
    self.scope -= 1;

    while (self.locals.getLast().scope > self.scope) {
        self.locals.pop();
    }
}

fn setLocal(self: *Self, name: []u8, sexp: Sexp) !void {
    try self.locals.append(.{
        .name = name,
        .scope = self.scope,
        .sexp = sexp,
    });
}

fn getLocal(self: *Self, name: []u8) Sexp {
    for (self.locals.items.len..0) |i| {
        const entry = self.locals.items[i];

        if (entry == name) {
            return entry.sexp;
        }
    }

    return self.nil;
}

fn alloc(self: *Self, atom: Atom) Sexp {
    if (atom == .Nil) {
        return self.nil;
    }

    const idx = self.stream.items.len;

    self.stream.append(atom);

    return idx;
}
