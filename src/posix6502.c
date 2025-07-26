#include <stdint.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#define FLAG_IMPLEMENTATION
#define FLAG_PUSH_DASH_DASH_BACK
#include "flag.h"

// Forward declarations for definitions from fake6502.c
void reset6502();
void step6502();
void rts();
extern uint16_t pc;
extern uint8_t sp, a, y;

#define DEFAULT_LOAD_OFFSET 0x8000

static uint8_t MEMORY[1<<16];

uint8_t read6502(uint16_t address)
{
    return MEMORY[address];
}

void write6502(uint16_t address, uint8_t value)
{
    MEMORY[address] = value;
}

void load_rom_at(String_Builder rom, uint16_t offset) {
    for (size_t i = 0; i < rom.count; ++i) {
        uint8_t x = rom.items[i];
        assert(i + offset < sizeof(MEMORY));
        MEMORY[i + offset] = x;
    }
}

void usage(void)
{
    fprintf(stderr, "posix6502 - a 6502 emulator in POSIX environment. Based on fake6502 by Mike Chambers.\n");
    fprintf(stderr, "Usage: %s [OPTIONS] <rom> [--] [run arguments]\n", flag_program_name());
    fprintf(stderr, "OPTIONS:\n");
    flag_print_options(stderr);
}

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Run_Args;

int main(int argc, char **argv)
{
    // TODO: it would be nice if the default value of -load-offset was displayed in hex in the -help message
    uint64_t *load_offset = flag_uint64("load-offset", DEFAULT_LOAD_OFFSET, "Offset in the memory to load the rom at");
    bool *help = flag_bool("help", false, "Display this help message");
    const char *rom_path = NULL;
    Run_Args run_args = {0};

    while (argc > 0) {
        if (!flag_parse(argc, argv)) {
            usage();
            flag_print_error(stderr);
            return 1;
        }
        argc = flag_rest_argc();
        argv = flag_rest_argv();
        if (argc > 0) {
            if (strcmp(*argv, "--") == 0) {
                UNUSED(shift(argv, argc)); // skip the dash-dash
                while (argc > 0) {
                    da_append(&run_args, shift(argv, argc));
                }
            } else {
                if (rom_path == NULL) {
                    rom_path = shift(argv, argc);
                } else {
                    usage();
                    fprintf(stderr, "ERROR: you can load and execute only one rom at a time\n");
                    return 1;
                }
            }
        }
    }

    if (*help) {
        usage();
        return 0;
    }

    if (rom_path == 0) {
        usage();
        fprintf(stderr, "ERROR: no rom is provided\n");
        return 1;
    }

    if (run_args.count > 0) {
        TODO("passing run arguments to the program is not implemented yet");
    }

    String_Builder rom = {0};
    if (!read_entire_file(rom_path, &rom)) return 1;

    load_rom_at(rom, *load_offset);

    reset6502();
    pc = *load_offset;

    // set reset to $0000 to exit on reset
    MEMORY[0xFFFC] = 0;
    MEMORY[0xFFFD] = 0;

    while (pc != 0) { // The convetion is stop executing when pc == $0000
        uint8_t prev_sp = sp & 0xFF;
        uint8_t opcode  = MEMORY[pc];
        step6502();

        uint8_t curr_sp = sp & 0xFF;
        if (opcode == 0x48 && curr_sp > prev_sp) { // PHA instruction
            nob_log(ERROR, "Stack overflow detected");
            nob_log(ERROR, "SP changed from $%02X to $%02X after PHA instruction", prev_sp, curr_sp);
            return 1;
        }

        if (pc == 0xFFEF) { // Emulating POSIX's putchar while also being compatible with wozmon's ECHO routine
            putchar(a);
            rts();
        }
    }
    // print exit code (in Y:A)
    uint32_t code = (((uint32_t)y) << 8) | ((uint32_t)a);
    return code;
}
