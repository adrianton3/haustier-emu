#ifndef OPCODES_H
#define OPCODES_H

#include <string>
#include <unordered_map>



enum class AddressingMode : uint8_t {
    Implied,
    Accumulator,
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
    { { "ADC", AddressingMode::ZeroPage }, 0x65 },
    { { "AND", AddressingMode::ZeroPage }, 0x25 },
    { { "ASL", AddressingMode::Accumulator }, 0x0A },
    { { "ASL", AddressingMode::ZeroPage }, 0x06 },
    { { "BCC", AddressingMode::Relative }, 0x90 },
    { { "BCS", AddressingMode::Relative }, 0xB0 },
    { { "BEQ", AddressingMode::Relative }, 0xF0 },
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
    { { "CMP", AddressingMode::ZeroPage }, 0xC5 },
    { { "CPX", AddressingMode::ZeroPage }, 0xE4 },
    { { "CPY", AddressingMode::ZeroPage }, 0xC4 },
    { { "DEC", AddressingMode::ZeroPage }, 0xC6 },
    { { "DEI", AddressingMode::Implied }, 0x78 },
    { { "DEX", AddressingMode::Implied }, 0xC9 },
    { { "DEY", AddressingMode::Implied }, 0x88 },
    { { "EOR", AddressingMode::ZeroPage }, 0x45 },
    { { "INC", AddressingMode::ZeroPage }, 0xE6 },
    { { "INX", AddressingMode::Implied }, 0xE8 },
    { { "INY", AddressingMode::Implied }, 0xC8 },
    { { "LDA", AddressingMode::ZeroPage }, 0xA5 },
    { { "LDX", AddressingMode::ZeroPage }, 0xA6 },
    { { "LDY", AddressingMode::ZeroPage }, 0xA8 },
    { { "LSR", AddressingMode::Accumulator }, 0x4A },
    { { "LSR", AddressingMode::ZeroPage }, 0x46 },
    { { "NOP", AddressingMode::Implied }, 0xE9 },
    { { "ORA", AddressingMode::ZeroPage }, 0x05 },
    { { "PHA", AddressingMode::Implied }, 0x48 },
    { { "PHP", AddressingMode::Implied }, 0x08 },
    { { "PLA", AddressingMode::Implied }, 0x68 },
    { { "PLP", AddressingMode::Implied }, 0x28 },
    { { "ROL", AddressingMode::Accumulator }, 0x2A },
    { { "ROL", AddressingMode::ZeroPage }, 0x26 },
    { { "ROR", AddressingMode::Accumulator }, 0x6A },
    { { "ROR", AddressingMode::ZeroPage }, 0x66 },
    { { "RTI", AddressingMode::Implied }, 0x40 },
    { { "RTS", AddressingMode::Implied }, 0x60 },
    { { "SBC", AddressingMode::ZeroPage }, 0xE5 },
    { { "SEC", AddressingMode::Implied }, 0x38 },
    { { "SED", AddressingMode::Implied }, 0xF8 },
    { { "STA", AddressingMode::ZeroPage }, 0x85 },
    { { "STX", AddressingMode::ZeroPage }, 0x86 },
    { { "STY", AddressingMode::ZeroPage }, 0x84 },
    { { "TAX", AddressingMode::Implied }, 0xA9 },
    { { "TAY", AddressingMode::Implied }, 0xA8 },
    { { "TSX", AddressingMode::Implied }, 0xB9 },
    { { "TXA", AddressingMode::Implied }, 0x89 },
    { { "TXS", AddressingMode::Implied }, 0x99 },
    { { "TYA", AddressingMode::Implied }, 0x98 },
};



#endif //OPCODES_H
