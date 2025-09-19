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
            source += "BYTE " + std::to_string(bytes[index]) + "\n";
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
            source += "BYTE " + std::to_string(opcode) + "\n";
            while (index < bytes.size()) {
                source += "BYTE " + std::to_string(bytes[index]) + "\n";
                index++;
            }
            break;
        }

        source += name;

        switch (mode) {
            case AddressingMode::Absolute:
                source += " " + std::to_string((bytes[index] << 8) | bytes[index + 1]);
                break;

            case AddressingMode::AbsoluteX:
                source += " " + std::to_string((bytes[index] << 8) | bytes[index + 1]) + ", X";
                break;

            case AddressingMode::AbsoluteY:
                source += " " + std::to_string((bytes[index] << 8) | bytes[index + 1]) + ", Y";
                break;

            case AddressingMode::Accumulator:
                source += " A";
                break;

            case AddressingMode::Immediate:
                source += " #" + std::to_string((bytes[index] << 8) | bytes[index + 1]);
                break;

            case AddressingMode::Implied:
                // nothing
                break;

            case AddressingMode::Indirect:
                source += " (" + std::to_string((bytes[index] << 8) | bytes[index + 1]) + ")";
                break;

            case AddressingMode::IndirectX:
                source += " (" + std::to_string(bytes[index]) + ", X)";
                break;

            case AddressingMode::IndirectY:
                source += " (" + std::to_string(bytes[index]) + "), Y";
                break;

            case AddressingMode::Relative:
                source += " *" + std::to_string(static_cast<int8_t>(bytes[index]));
                break;

            case AddressingMode::ZeroPage:
                source += " " + std::to_string(bytes[index]);
                break;

            case AddressingMode::ZeroPageX:
                source += " " + std::to_string(bytes[index]) + ", X";
                break;

            case AddressingMode::ZeroPageY:
                source += " " + std::to_string(bytes[index]) + ", Y";
                break;
        }

        index += requiredBytes;

        source += '\n';
    }

    return source;
}