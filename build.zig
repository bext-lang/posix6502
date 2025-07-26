const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const module = b.addModule("main", .{
        .target = target,
        .link_libc = true,
    });

    module.addCSourceFiles(.{
        .files = &.{
            "src/posix6502.c",
            "src/fake6502.c",
        },
    });

    const exe = b.addExecutable(.{
        .name = "hello",
        .root_module = module,
    });

    b.installArtifact(exe);
}
