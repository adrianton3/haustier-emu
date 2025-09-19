#include <unordered_map>

#include "assembler/opcodes.h"

#include "disasm.h"



static const auto opcodesInv = [] {
    std::unordered_map<uint8_t, InsAndMode> opcodesInv;

    for (const auto& entry : opcodes) {
        opcodesInv[entry.second] = entry.first;
    }

    return opcodesInv;
}();

std::string hex8 (uint8_t value) {
    char s[3];
    sprintf(s, "%02x", value);
    return std::string { s };
}

std::string hex16 (uint16_t value) {
    char s[5];
    sprintf(s, "%04x", value);
    return std::string { s };
}

std::string disassemble (const std::vector<uint8_t>& bytes) {
    std::string source;

    auto index = 0;
    while (true) {
        if (index >= bytes.size()) {
            return source;
        }

        const auto opcode = bytes[index];
        index++;

        if (!opcodesInv.contains(opcode)) {
            source += "BYTE $" + hex8(opcode) + "          ; " + hex8(opcode) + "\n";
            continue;
        }

        const auto& [name, mode] = opcodesInv.find(opcode)->second;

        const auto requiredBytes = [mode] {
            switch (mode) {
                case AddressingMode::Absolute: [[fallthrough]];
                case AddressingMode::AbsoluteX: [[fallthrough]];
                case AddressingMode::AbsoluteY: return 2;
                case AddressingMode::Accumulator: return 0;
                case AddressingMode::Immediate: return 1;
                case AddressingMode::Implied: return 0;
                case AddressingMode::Indirect: return 2;
                case AddressingMode::IndirectX: [[fallthrough]];
                case AddressingMode::IndirectY: return 1;
                case AddressingMode::Relative: return 1;
                case AddressingMode::ZeroPage: [[fallthrough]];
                case AddressingMode::ZeroPageX: [[fallthrough]];
                case AddressingMode::ZeroPageY: return 1;
                default: return 0;
            }
        }();

        if (index + requiredBytes > bytes.size()) {
            source += "BYTE $" + hex8(opcode) + "          ; " + hex8(bytes[index]) + "\n";
            while (index < bytes.size()) {
                source += "BYTE $" + hex8(bytes[index]) + "          ; " + hex8(bytes[index]) + "\n";
                index++;
            }
            break;
        }

        source += name;

        switch (mode) {
            case AddressingMode::Absolute:
                source += " $" + hex16((bytes[index] << 8) | bytes[index + 1]) +
                    "         ; " + hex8(opcode) + " " + hex8(bytes[index]) + " " + hex8(bytes[index + 1]);
                break;

            case AddressingMode::AbsoluteX:
                source += " $" + hex16((bytes[index] << 8) | bytes[index + 1]) + ", X" +
                    "    ; " + hex8(opcode) + " " + hex8(bytes[index]) + " " + hex8(bytes[index + 1]);
                break;

            case AddressingMode::AbsoluteY:
                source += " $" + hex16((bytes[index] << 8) | bytes[index + 1]) + ", Y" +
                    "      ; " + hex8(opcode) + " " + hex8(bytes[index]) + " " + hex8(bytes[index + 1]);
                break;

            case AddressingMode::Accumulator:
                source += " A             ; " + hex8(opcode);
                break;

            case AddressingMode::Immediate:
                source += " #$" + hex8((bytes[index] << 8) | bytes[index + 1]) +
                    "          ; " + hex8(opcode) + " " + hex8(bytes[index]) + " " + hex8(bytes[index + 1]);
                break;

            case AddressingMode::Implied:
                source += "               ; " + hex8(opcode);
                break;

            case AddressingMode::Indirect:
                source += " ($" + hex16((bytes[index] << 8) | bytes[index + 1]) + ")" +
                    "       ; " + hex8(opcode) + " " + hex8(bytes[index]) + " " + hex8(bytes[index + 1]);
                break;

            case AddressingMode::IndirectX:
                source += " ($" + hex8(bytes[index]) + ", X)" +
                    "    ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;

            case AddressingMode::IndirectY:
                source += " ($" + hex8(bytes[index]) + "), Y" +
                    "      ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;

            case AddressingMode::Relative:
                source += " *" + std::to_string(static_cast<int8_t>(bytes[index])) +
                    "           ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;

            case AddressingMode::ZeroPage:
                source += " $" + hex8(bytes[index]) +
                    "           ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;

            case AddressingMode::ZeroPageX:
                source += " $" + hex8(bytes[index]) + ", X" +
                    "        ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;

            case AddressingMode::ZeroPageY:
                source += " $" + hex8(bytes[index]) + ", Y" +
                    "        ; " + hex8(opcode) + " " + hex8(bytes[index]);
                break;
        }

        index += requiredBytes;

        source += '\n';
    }

    return source;
}