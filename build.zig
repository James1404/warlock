const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "warlock",
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("src/warlock_binary.zig"),
        .link_libc = true,
    });

    exe.addCSourceFiles(.{ .flags = &.{"-std=c99"}, .files = &.{
        "src/warlock.c",
        "src/warlock_binary.c",
        "src/warlock_allocator.c",
        "src/warlock_atom.c",
        "src/warlock_builtins.c",
        "src/warlock_reader.c",
        "src/warlock_eval.c",
        "src/warlock_opcodes.c",
        "src/warlock_vm.c",
        "src/warlock_error.c",
        "src/warlock_string.c",
    } });

    // var srcDir = try std.fs.cwd().openDir("src", .{ .iterate = true });
    // defer srcDir.close();

    // var srcIterator = srcDir.iterate();
    // while (try srcIterator.next()) |file| {
    //     if (file.kind != .file) continue;
    //     std.debug.print("{s}\n", .{file.name});
    //}

    exe.addIncludePath(.{ .cwd_relative = "include" });

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const exe_unit_tests = b.addTest(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    const run_exe_unit_tests = b.addRunArtifact(exe_unit_tests);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_exe_unit_tests.step);
}
