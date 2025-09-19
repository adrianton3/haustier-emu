#include <cstdint>
#include <unordered_map>
#include <variant>

#include "opcodes.h"
#include "tokenize.h"

#include "asm.h"

#include "ParserError.h"


template <uint64_t Index>
constexpr uint8_t getByte (uint64_t value) {
    return (value >> (Index * 8)) & 0xff;
}

std::string getIdentifierName (const Token& identifier) {
    return std::get<Identifier>(identifier).name;
}

int64_t getNumberValue (const Token& number) {
    return std::get<Number>(number).value;
}

template <int Offset>
bool matchesUnsafe (const std::vector<Token>& tokens, size_t index) {
    return true;
}

template <int Offset, TokenType Head, TokenType ...Tail>
bool matchesUnsafe (const std::vector<Token>& tokens, size_t index) {
    return tokens[index + Offset].index() == static_cast<size_t>(Head) && matchesUnsafe<Offset + 1, Tail...>(tokens, index);
}

template <TokenType ...Types>
bool matchesUnsafe (const std::vector<Token>& tokens, size_t index) {
    return matchesUnsafe<0, Types...>(tokens, index);
}

template <TokenType ...Types>
bool matches (const std::vector<Token>& tokens, size_t index) {
    return index + sizeof...(Types) <= tokens.size() && matchesUnsafe<0, Types...>(tokens, index);
}

struct Link {
    uint16_t offset;
    std::string name;
    int lineIndex;
};

std::variant<std::vector<uint8_t>, ParserError> assemble (const std::vector<Token>& tokens) {
    std::vector<uint8_t> bytes;
    std::unordered_map<std::string, uint16_t> labels;
    std::vector<Link> links;

    auto index = 0;
    auto lineIndex = -1;

    while (index < tokens.size()) {
        lineIndex++;

        if (matches<TokenType::NewLine>(tokens, index)) {
            index++;
            continue;
        }

        if (matches<TokenType::Identifier, TokenType::Colon, TokenType::NewLine>(tokens, index)) {
            const auto& name = getIdentifierName(tokens[index]);

            if (labels.contains(name)) {
                return ParserError { "Label '" + name + "' already declared", lineIndex };
            }

            labels[name] = bytes.size();

            index += 3;
            continue;
        }

        if (matches<TokenType::Identifier>(tokens, index)) {
            const auto& name = getIdentifierName(tokens[index]);
            index++;

            if (name != "BYTE" && name != "WORD" && !insNames.contains(name)) {
                return ParserError { "Unrecognized instruction '" + name + '\'', lineIndex };
            }

            if (matches<TokenType::NewLine>(tokens, index)) {
                // implied
                const InsAndMode insAndMode { name, AddressingMode::Implied };

                if (!opcodes.contains(insAndMode)) {
                    return ParserError { name + " is not available with implied addressing", lineIndex };
                }

                bytes.push_back(opcodes[insAndMode]);
                index++;
                continue;
            }

            if (matches<TokenType::Number, TokenType::NewLine>(tokens, index)) {
                const auto value = getNumberValue(tokens[index]);

                if (name == "BYTE") {
                    if (!std::in_range<uint8_t>(value)) {
                        return ParserError { std::to_string(value) + " does not fit in a byte", lineIndex };
                    }

                    bytes.push_back(static_cast<uint8_t>(value));
                    index += 2;
                    continue;
                }

                if (name == "WORD") {
                    if (!std::in_range<uint16_t>(value)) {
                        return ParserError { std::to_string(value) + " does not fit in a word", lineIndex };
                    }

                    bytes.insert(bytes.end(), { getByte<0>(value), getByte<1>(value) });
                    index += 2;
                    continue;
                }

                if (std::in_range<uint8_t>(value)) {
                    // zero-page
                    const InsAndMode insAndMode { name, AddressingMode::ZeroPage };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with zero-page addressing", lineIndex };
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });
                } else if (std::in_range<uint16_t>(value)) {
                    // absolute
                    const InsAndMode insAndMode { name, AddressingMode::Absolute };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with absolute addressing", lineIndex };
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value), getByte<1>(value) });
                } else {
                    return ParserError { std::to_string(value) + " does not fit in a word", lineIndex };
                }

                index += 2;
                continue;
            }

            if (matches<TokenType::Identifier, TokenType::NewLine>(tokens, index)) {
                if (getIdentifierName(tokens[index]) == "A") {
                    // accumulator
                    const InsAndMode insAndMode { name, AddressingMode::Accumulator };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with accumulator addressing", lineIndex };
                    }

                    bytes.push_back(opcodes[insAndMode]);
                } else {
                    // relative
                    const InsAndMode insAndMode { name, AddressingMode::Relative };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with relative addressing", lineIndex };
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], 0x00 });
                    links.emplace_back(bytes.size() - 1, getIdentifierName(tokens[index]), lineIndex);
                }

                index += 2;
                continue;
            }

            if (matches<TokenType::Hash, TokenType::Number, TokenType::NewLine>(tokens, index)) {
                // immediate
                const InsAndMode insAndMode { name, AddressingMode::Immediate };

                if (!opcodes.contains(insAndMode)) {
                    return ParserError { name + " is not available with immediate addressing", lineIndex };
                }

                const auto value = getNumberValue(tokens[index + 1]);

                if (!std::in_range<uint8_t>(value)) {
                    return ParserError { std::to_string(value) + " does not fit in a byte", lineIndex };
                }

                bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });

                index += 3;
                continue;
            }

            if (matches<TokenType::Number, TokenType::Comma, TokenType::Identifier, TokenType::NewLine>(tokens, index)) {
                const auto xy = getIdentifierName(tokens[index + 2]);
                if (xy != "X" && xy != "Y") {
                    return ParserError { "expected X or Y as offsets", lineIndex };
                }

                const auto value = getNumberValue(tokens[index]);

                if (std::in_range<uint8_t>(value)) {
                    // zero-page
                    const InsAndMode insAndMode { name, xy == "X" ? AddressingMode::ZeroPageX : AddressingMode::ZeroPageY };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with zero-page addressing", lineIndex };
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });
                } else if (std::in_range<uint16_t>(value)) {
                    // absolute
                    const InsAndMode insAndMode { name, xy == "X" ? AddressingMode::AbsoluteX : AddressingMode::AbsoluteY };

                    if (!opcodes.contains(insAndMode)) {
                        return ParserError { name + " is not available with absolute addressing", lineIndex };
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value), getByte<1>(value) });
                } else {
                    return ParserError { std::to_string(value) + " does not fit in a word", lineIndex };
                }

                index += 4;
                continue;
            }

            if (matches<TokenType::ParOpen, TokenType::Number, TokenType::ParClosed, TokenType::NewLine>(tokens, index)) {
                const InsAndMode insAndMode { name, AddressingMode::Indirect };

                if (!opcodes.contains(insAndMode)) {
                    return ParserError { name + " is not available with indirect addressing", lineIndex };
                }

                const auto value = getNumberValue(tokens[index + 1]);

                if (!std::in_range<uint16_t>(value)) {
                    return ParserError { std::to_string(value) + " does not fit in a word", lineIndex };
                }

                bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value), getByte<1>(value) });

                index += 4;
                continue;
            }

            if (
                matches<TokenType::ParOpen, TokenType::Number, TokenType::Comma, TokenType::Identifier, TokenType::ParClosed, TokenType::NewLine>(tokens, index) &&
                getIdentifierName(tokens[index + 3]) == "X"
            ) {
                const InsAndMode insAndMode { name, AddressingMode::IndirectX };

                if (!opcodes.contains(insAndMode)) {
                    return ParserError { name + " is not available with indirect-x addressing", lineIndex };
                }

                const auto value = getNumberValue(tokens[index + 1]);

                if (!std::in_range<uint8_t>(value)) {
                    return ParserError { std::to_string(value) + " does not fit in a byte", lineIndex };
                }

                bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });

                index += 6;
                continue;
            }

            if (
                matches<TokenType::ParOpen, TokenType::Number, TokenType::ParClosed, TokenType::Comma, TokenType::Identifier, TokenType::NewLine>(tokens, index) &&
                getIdentifierName(tokens[index + 4]) == "Y"
            ) {
                const InsAndMode insAndMode { name, AddressingMode::IndirectY };

                if (!opcodes.contains(insAndMode)) {
                    return ParserError { name + " is not available with indirect-y addressing", lineIndex };
                }

                const auto value = getNumberValue(tokens[index + 1]);

                if (!std::in_range<uint8_t>(value)) {
                    return ParserError { std::to_string(value) + " does not fit in a byte", lineIndex };
                }

                bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });

                index += 6;
                continue;
            }
        }

        return ParserError { "expecting a label, BYTE, WORD or instruction", lineIndex };
    }

    for (const auto& link : links) {
        if (!labels.contains(link.name)) {
            return ParserError { "label '" + link.name + "' is undeclared", link.lineIndex };
        }

        const auto delta = static_cast<int64_t>(labels[link.name]) - static_cast<int64_t>(link.offset) - 1;

        if (!std::in_range<int8_t>(delta)) {
            return ParserError { "jump target is too far from jump instruction", lineIndex };
        }

        bytes[link.offset] = delta;
    }

    return bytes;
}