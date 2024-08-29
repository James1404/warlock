const std = @import("std");

const Ctx = @import("context.zig");
const Sexp = Ctx.Sexp;
const Messages = @import("messages.zig");

src: []const u8,
start: u64,
pos: u64,
line: u64,
line_offset: u64,

const Self = @This();

const ReaderError = error{
    UnexpectedCharacter,
    InvalidCharacter,
    EndOfFile,
};

pub fn make(src: []const u8) Self {
    return .{
        .src = src,
        .start = 0,
        .pos = 0,
        .line = 1,
        .line_offset = 0,
    };
}

fn eof(self: Self) bool {
    return !(self.pos < self.src.len);
}

fn current(self: *Self) ?u8 {
    if (self.eof()) {
        return null;
    }

    return self.src[self.pos];
}

fn advance(self: *Self) void {
    if (self.eof()) {
        return;
    }

    self.pos += 1;
    self.line_offset += 1;
}

fn match(self: *Self, expected: u8) bool {
    if (self.current() == expected) {
        self.advance();
        return true;
    }

    return false;
}

fn isSymbolCharStart(c: u8) bool {
    return switch (c) {
        '+', '-', '/', '*' => true,
        '<', '>', '!', '=' => true,
        '_', '#', '@', '?' => true,
        '$', '%', '^', '&', '.' => true,
        else => std.ascii.isAlphabetic(c),
    };
}

fn isSymbolCharMiddle(c: u8) bool {
    return isSymbolCharStart(c) or std.ascii.isDigit(c);
}

fn skipWhiteSpace(self: *Self) bool {
    const cc = std.ascii.control_code;
    return switch (self.current().?) {
        '\n' => {
            self.line += 1;
            self.line_offset = 0;

            self.advance();
            return true;
        },
        ' ', '\t', '\r', cc.ff, cc.vt => {
            self.advance();
            return true;
        },
        ';' => {
            while (self.current() != '\n') {
                self.advance();
            }
            return true;
        },
        else => false,
    };
}

fn skipAllWhitespace(self: *Self) void {
    while (!self.eof() and self.skipWhiteSpace()) {}
}

fn readList(self: *Self, ctx: *Ctx) !Sexp {
    self.skipAllWhitespace();

    const start = try ctx.alloc(.{
        .Cons = .{
            .data = try ctx.alloc(.Nil),
            .next = try ctx.alloc(.Nil),
        },
    });

    var crnt = start;

    if (self.match('(')) {
        while (!self.match(')')) {
            if (self.eof()) {
                // TODO: error
                return ctx.nil;
            }

            switch (ctx.getr(crnt).*) {
                .Cons => |*cons| {
                    cons.*.data = try self.readAtom(ctx);
                    cons.*.next = try ctx.alloc(.{
                        .Cons = .{
                            .data = try ctx.alloc(.Nil),
                            .next = try ctx.alloc(.Nil),
                        },
                    });

                    crnt = cons.next;
                    self.skipAllWhitespace();
                },
                else => break,
            }
        }
    }

    return start;
}

fn readSymbol(self: *Self, ctx: *Ctx) !Sexp {
    self.skipAllWhitespace();

    var c = self.current().?;
    self.start = self.pos;

    if (isSymbolCharStart(c)) {
        while (isSymbolCharMiddle(c)) {
            self.advance();
            c = self.current().?;
        }

        const text = self.src[self.start..self.pos];
        return ctx.alloc(.{ .String = text });
    }

    return ctx.nil;
}

fn readAtom(self: *Self, ctx: *Ctx) !Sexp {
    self.skipAllWhitespace();

    const c = self.current().?;

    self.start = self.pos;

    if (c == ')') {
        try Messages.err("Unexpected character '{c}'", .{c});
        self.advance();
        return ReaderError.UnexpectedCharacter;
    }

    if (c == '\'') {
        self.advance();
        const atom = try self.readAtom(ctx);
        return ctx.alloc(.{ .Quote = atom });
    }

    if (c == '(') {
        return self.readList(ctx);
    }

    if (isSymbolCharStart(c)) {
        return self.readSymbol(ctx);
    }

    try Messages.err("Invalid charater '{c}'", .{c});
    return ReaderError.UnexpectedCharacter;
}

fn readTopLevel(self: *Self, ctx: *Ctx) !Sexp {
    const start = try ctx.alloc(.{
        .Cons = .{ .data = ctx.nil, .next = ctx.nil },
    });
    var crnt = start;

    while (!self.eof()) {
        switch (ctx.get(crnt)) {
            .Cons => |cons| {
                ctx.set(cons.data, try self.readAtom(ctx));
                ctx.set(cons.data, try ctx.alloc(.{ .Cons = .{ .data = ctx.nil, .next = ctx.nil } }));
                crnt = ctx.get(crnt).Cons.next;

                self.skipAllWhitespace();
            },
            else => {},
        }
    }

    return start;
}

pub fn run(self: *Self, ctx: *Ctx) !Sexp {
    return self.readTopLevel(ctx);
}
