#ifndef OPCODES_H
#define OPCODES_H

#include <string>
#include <unordered_map>



enum class AddressingMode : uint8_t {
    Absolute,
    Accumulator,
    Immediate,
    Implied,
    Relative,
    ZeroPage,
};

struct InsAndMode {
    std::string name;
    AddressingMode mode;

    bool operator== (const InsAndMode& rhs) const { return mode == rhs.mode && name == rhs.name; }
};

template<>
struct std::hash<InsAndMode> {
    std::size_t operator() (const InsAndMode& cursor) const noexcept {
        return std::hash<std::string>()(cursor.name) ^ static_cast<std::size_t>(cursor.mode);
    }
};

static std::unordered_map<InsAndMode, uint8_t> opcodes {
    { { "ADC", AddressingMode::Absolute }, 0x6D },
    { { "ADC", AddressingMode::Immediate }, 0x69 },
    { { "ADC", AddressingMode::ZeroPage }, 0x65 },

    { { "AND", AddressingMode::Absolute }, 0x2D },
    { { "AND", AddressingMode::Immediate }, 0x29 },
    { { "AND", AddressingMode::ZeroPage }, 0x25 },

    { { "ASL", AddressingMode::Absolute }, 0x0E },
    { { "ASL", AddressingMode::Accumulator }, 0x0A },
    { { "ASL", AddressingMode::ZeroPage }, 0x06 },

    { { "BCC", AddressingMode::Relative }, 0x90 },

    { { "BCS", AddressingMode::Relative }, 0xB0 },

    { { "BEQ", AddressingMode::Relative }, 0xF0 },

    { { "BIT", AddressingMode::Absolute }, 0x2C },
    { { "BIT", AddressingMode::ZeroPage }, 0x24 },

    { { "BMI", AddressingMode::Relative }, 0x30 },

    { { "BNE", AddressingMode::Relative }, 0xD0 },

    { { "BPL", AddressingMode::Relative }, 0x10 },

    { { "BRK", AddressingMode::Implied }, 0x00 },

    { { "BVC", AddressingMode::Relative }, 0x50 },

    { { "BVS", AddressingMode::Relative }, 0x70 },

    { { "CLC", AddressingMode::Implied }, 0x18 },

    { { "CLD", AddressingMode::Implied }, 0xD8 },

    { { "CLI", AddressingMode::Implied }, 0x58 },

    { { "CLV", AddressingMode::Implied }, 0xB8 },

    { { "CMP", AddressingMode::Absolute }, 0xCD },
    { { "CMP", AddressingMode::Immediate }, 0xC9 },
    { { "CMP", AddressingMode::ZeroPage }, 0xC5 },

    { { "CPX", AddressingMode::Absolute }, 0xEC },
    { { "CPX", AddressingMode::Immediate }, 0xE0 },
    { { "CPX", AddressingMode::ZeroPage }, 0xE4 },

    { { "CPY", AddressingMode::Absolute }, 0xCC },
    { { "CPY", AddressingMode::Immediate }, 0xC0 },
    { { "CPY", AddressingMode::ZeroPage }, 0xC4 },

    { { "DEC", AddressingMode::Absolute }, 0xCE },
    { { "DEC", AddressingMode::ZeroPage }, 0xC6 },

    { { "DEI", AddressingMode::Implied }, 0x78 },

    { { "DEX", AddressingMode::Implied }, 0xC9 },

    { { "DEY", AddressingMode::Implied }, 0x88 },

    { { "EOR", AddressingMode::Absolute }, 0x4D },
    { { "EOR", AddressingMode::Immediate }, 0x49 },
    { { "EOR", AddressingMode::ZeroPage }, 0x45 },

    { { "INC", AddressingMode::Absolute }, 0xEE },
    { { "INC", AddressingMode::ZeroPage }, 0xE6 },

    { { "INX", AddressingMode::Implied }, 0xE8 },

    { { "INY", AddressingMode::Implied }, 0xC8 },

    { { "JMP", AddressingMode::Absolute }, 0x4C },

    { { "JSR", AddressingMode::Absolute }, 0x20 },

    { { "LDA", AddressingMode::Absolute }, 0xAD },
    { { "LDA", AddressingMode::Immediate }, 0xA9 },
    { { "LDA", AddressingMode::ZeroPage }, 0xA5 },

    { { "LDX", AddressingMode::Absolute }, 0xAE },
    { { "LDX", AddressingMode::Immediate }, 0xA2 },
    { { "LDX", AddressingMode::ZeroPage }, 0xA6 },

    { { "LDY", AddressingMode::Absolute }, 0xAC },
    { { "LDY", AddressingMode::Immediate }, 0xA0 },
    { { "LDY", AddressingMode::ZeroPage }, 0xA8 },

    { { "LSR", AddressingMode::Absolute }, 0x4E },
    { { "LSR", AddressingMode::Accumulator }, 0x4A },
    { { "LSR", AddressingMode::ZeroPage }, 0x46 },

    { { "NOP", AddressingMode::Implied }, 0xE9 },

    { { "ORA", AddressingMode::Absolute }, 0x0D },
    { { "ORA", AddressingMode::Immediate }, 0x09 },
    { { "ORA", AddressingMode::ZeroPage }, 0x05 },

    { { "PHA", AddressingMode::Implied }, 0x48 },

    { { "PHP", AddressingMode::Implied }, 0x08 },

    { { "PLA", AddressingMode::Implied }, 0x68 },

    { { "PLP", AddressingMode::Implied }, 0x28 },

    { { "ROL", AddressingMode::Absolute }, 0x2E },
    { { "ROL", AddressingMode::Accumulator }, 0x2A },
    { { "ROL", AddressingMode::ZeroPage }, 0x26 },

    { { "ROR", AddressingMode::Absolute }, 0x6E },
    { { "ROR", AddressingMode::Accumulator }, 0x6A },
    { { "ROR", AddressingMode::ZeroPage }, 0x66 },
    { { "RTI", AddressingMode::Implied }, 0x40 },

    { { "RTS", AddressingMode::Implied }, 0x60 },

    { { "SBC", AddressingMode::Absolute }, 0xED },
    { { "SBC", AddressingMode::Immediate }, 0xE9 },
    { { "SBC", AddressingMode::ZeroPage }, 0xE5 },

    { { "SEC", AddressingMode::Implied }, 0x38 },

    { { "SED", AddressingMode::Implied }, 0xF8 },

    { { "STA", AddressingMode::Absolute }, 0x8D },
    { { "STA", AddressingMode::ZeroPage }, 0x85 },

    { { "STX", AddressingMode::Absolute }, 0x8E },
    { { "STX", AddressingMode::ZeroPage }, 0x86 },

    { { "STY", AddressingMode::Absolute }, 0x8C },
    { { "STY", AddressingMode::ZeroPage }, 0x84 },

    { { "TAX", AddressingMode::Implied }, 0xA9 },

    { { "TAY", AddressingMode::Implied }, 0xA8 },

    { { "TSX", AddressingMode::Implied }, 0xB9 },

    { { "TXA", AddressingMode::Implied }, 0x89 },

    { { "TXS", AddressingMode::Implied }, 0x99 },

    { { "TYA", AddressingMode::Implied }, 0x98 },
};



#endif //OPCODES_H
