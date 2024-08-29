const std = @import("std");

pub fn info(comptime fmt: []const u8, args: anytype) !void {
    const writer = std.io.getStdOut().writer();
    try writer.print("INFO :: ", .{});
    try writer.print(fmt, args);
    try writer.print("\n", .{});
}

pub fn warn(comptime fmt: []const u8, args: anytype) !void {
    const writer = std.io.getStdOut().writer();
    try writer.print("WARN :: ", .{});
    try writer.print(fmt, args);
    try writer.print("\n", .{});
}

pub fn err(comptime fmt: []const u8, args: anytype) !void {
    const writer = std.io.getStdOut().writer();
    try writer.print("ERROR :: ", .{});
    try writer.print(fmt, args);
    try writer.print("\n", .{});
}
