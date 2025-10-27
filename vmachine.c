#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];

/* Register */
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

/*Register storage  */
uint16_t reg[R_COUNT];

// opcodes
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

/*Condition flags*/
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

// Main  Loop
int main(int argc, const char *argv[])
{
    // loading arguments
    if (argc < 2)
    {
        // show usage string
        printf("lc3 [image-file1] ... \n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(argv[j]))
        {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    // Setup

    // since exactly one condition flag should be set at any given time, set the Z flag
    reg[R_COND] = FL_ZRO;

    // set the pc to starting position
    // 0x3000 is the default
    enum
    {
        PC_START = 0x3000
    };
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        // FETCH
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;

        switch (op)
        {
        case OP_ADD:
        {
            // DESTINATION REGISTER (DR)
            uint16_t r0 = (instr >> 9) & 0x7;
            // first operand(sr1)
            uint16_t r1 = (instr >> 6) & 0x7;
        }
        break;
        case OP_AND:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t imm_flag = (instr >> 5) & 0x1;

            if (imm_flag)
            {
                uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                reg[r0] = reg[r1] & imm5;
            }
            else
            {
                uint16_t r2 = instr & 0x7;
                reg[r0] = reg[r1] & reg[r2];
            }
            update_flags(r0);
        }
        break;
        case OP_NOT:
            @{ NOT } break;
        case OP_BR:
            @{ BR } break;
        case OP_JMP:
            @{ JMP } break;
        case OP_JSR:
            @{ JSR } break;
        case OP_LD:
            @{ LD } break;
        case OP_LDI:
            @{ LDI } break;
        case OP_LDR:
            @{ LDR } break;
        case OP_LEA:
            @{ LEA } break;
        case OP_ST:
            @{ ST } break;
        case OP_STI:
            @{ STI } break;
        case OP_STR:
            @{ STR } break;
        case OP_TRAP:
            @{ TRAP } break;
        case OP_RES:
        case OP_RTI:
        default:
            @{ BAD OPCODE } break;
        }
    }
    //@shutdown
}

// sign extend
uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        // The if statement is only true if the original number's sign bit was a 1 (indicating it's a negative number in two's complement representation).
        x |= (0xFFFF << bit_count);
    }
    return x;
}