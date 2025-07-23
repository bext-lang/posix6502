#include <stdint.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

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

int main(int argc, char **argv)
{
    const char *program_name = shift(argv, argc);

    if (argc <= 0) {
        nob_log(ERROR, "Usage: %s <rom>", program_name);
        nob_log(ERROR, "No rom path is provided");
        return 1;
    }

    const char *rom_path = shift(argv, argc);

    String_Builder rom = {0};
    if (!read_entire_file(rom_path, &rom)) return 1;

    load_rom_at(rom, DEFAULT_LOAD_OFFSET);

    reset6502();
    pc = DEFAULT_LOAD_OFFSET;

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
