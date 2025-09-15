#ifndef OPCODES_H
#define OPCODES_H

#include <string>
#include <unordered_map>

#include "../temp/InsDefinition.h"



enum class AddressingMode : uint8_t {
    Implied,
    Accumulator,
    Relative,
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
    { { "ASL", AddressingMode::Accumulator }, 0x0A },
    { { "BCC", AddressingMode::Relative }, 0x90 },
    { { "BCS", AddressingMode::Relative }, 0xB0 },
    { { "BEQ", AddressingMode::Relative }, 0xF0 },
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
    { { "DEI", AddressingMode::Implied }, 0x78 },
    { { "DEX", AddressingMode::Implied }, 0xC9 },
    { { "DEY", AddressingMode::Implied }, 0x88 },
    { { "INX", AddressingMode::Implied }, 0xE8 },
    { { "INY", AddressingMode::Implied }, 0xC8 },
    { { "LSR", AddressingMode::Accumulator }, 0x4A },
    { { "NOP", AddressingMode::Implied }, 0xE9 },
    { { "PHA", AddressingMode::Implied }, 0x48 },
    { { "PHP", AddressingMode::Implied }, 0x08 },
    { { "PLA", AddressingMode::Implied }, 0x68 },
    { { "PLP", AddressingMode::Implied }, 0x28 },
    { { "ROL", AddressingMode::Accumulator }, 0x2A },
    { { "ROR", AddressingMode::Accumulator }, 0x6A },
    { { "RTI", AddressingMode::Implied }, 0x40 },
    { { "RTS", AddressingMode::Implied }, 0x60 },
    { { "SEC", AddressingMode::Implied }, 0x38 },
    { { "SED", AddressingMode::Implied }, 0xF8 },
    { { "TAX", AddressingMode::Implied }, 0xA9 },
    { { "TAY", AddressingMode::Implied }, 0xA8 },
    { { "TSX", AddressingMode::Implied }, 0xB9 },
    { { "TXA", AddressingMode::Implied }, 0x89 },
    { { "TXS", AddressingMode::Implied }, 0x99 },
    { { "TYA", AddressingMode::Implied }, 0x98 },
};



#endif //OPCODES_H
