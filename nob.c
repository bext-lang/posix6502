#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "./src/nob.h"

#define BUILD_FOLDER "./build/"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Cmd cmd = {0};
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, "./src/posix6502.c", "./src/fake6502.c");
    nob_cc_output(&cmd, BUILD_FOLDER"posix6502");
    if (!cmd_run_sync_and_reset(&cmd)) return 1;
    return 0;
}
