#ifndef OPCODES_H
#define OPCODES_H

#include <string>
#include <unordered_map>

#include "../temp/InsDefinition.h"



enum class AddressingMode : uint8_t {
    Implied,
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
    { { "BRK", AddressingMode::Implied } , 0x00 },
    { { "RTI", AddressingMode::Implied } , 0x40 },
    { { "RTS", AddressingMode::Implied } , 0x60 },
};



#endif //OPCODES_H
