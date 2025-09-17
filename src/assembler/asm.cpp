#include <cstdint>
#include <unordered_map>
#include <variant>

#include "opcodes.h"
#include "tokenize.h"

#include "asm.h"



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
};

std::variant<std::vector<uint8_t>, std::string> assemble (const std::string& source) {
    const auto tokensOrError = tokenize(source);

    if (const auto* error = std::get_if<std::string>(&tokensOrError)) {
        return *error;
    }

    auto tokens = std::get<std::vector<Token>> (tokensOrError);
    tokens.emplace_back(NewLine {});

    std::vector<uint8_t> bytes;
    std::unordered_map<std::string, uint16_t> labels;
    std::vector<Link> links;

    auto index = 0;
    while (index < tokens.size()) {
        if (matches<TokenType::NewLine>(tokens, index)) {
            index++;
            continue;
        }

        if (matches<TokenType::Identifier, TokenType::Colon, TokenType::NewLine>(tokens, index)) {
            const auto& name = getIdentifierName(tokens[index]);
            labels[name] = bytes.size();
            index += 3;
            continue;
        }

        if (matches<TokenType::Identifier>(tokens, index)) {
            const auto& name = getIdentifierName(tokens[index]);
            index++;

            if (matches<TokenType::NewLine>(tokens, index)) {
                // implied
                const InsAndMode insAndMode { name, AddressingMode::Implied };

                if (!opcodes.contains(insAndMode)) {
                    return "not cool";
                }

                bytes.push_back(opcodes[insAndMode]);
                index++;
                continue;
            }

            if (matches<TokenType::Hash, TokenType::Number, TokenType::NewLine>(tokens, index)) {
                // immediate
                const InsAndMode insAndMode { name, AddressingMode::Immediate };

                if (!opcodes.contains(insAndMode)) {
                    return name + " is not available with immediate addressing";
                }

                const auto value = getNumberValue(tokens[index + 1]);

                if (!std::in_range<uint8_t>(value)) {
                    return "immediate value must be in the $00..$FF range";
                }

                bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });

                index += 3;
                continue;
            }

            if (matches<TokenType::Number, TokenType::NewLine>(tokens, index)) {
                const auto value = getNumberValue(tokens[index]);

                if (std::in_range<uint8_t>(value)) {
                    // zero-page
                    const InsAndMode insAndMode { name, AddressingMode::ZeroPage };

                    if (!opcodes.contains(insAndMode)) {
                        return name + " is not available with zero-page addressing";
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value) });
                } else if (std::in_range<uint16_t>(value)) {
                    // absolute
                    const InsAndMode insAndMode { name, AddressingMode::Absolute };

                    if (!opcodes.contains(insAndMode)) {
                        return name + " is not available with absolute addressing";
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], getByte<0>(value), getByte<1>(value) });
                } else {
                    return "argument too large";
                }

                index += 2;
                continue;
            }

            if (matches<TokenType::Identifier, TokenType::NewLine>(tokens, index)) {
                if (getIdentifierName(tokens[index]) == "A") {
                    // accumulator
                    const InsAndMode insAndMode { name, AddressingMode::Accumulator };

                    if (!opcodes.contains(insAndMode)) {
                        return "not cool";
                    }

                    bytes.push_back(opcodes[insAndMode]);
                } else {
                    // relative
                    const InsAndMode insAndMode { name, AddressingMode::Relative };

                    if (!opcodes.contains(insAndMode)) {
                        return "not cool";
                    }

                    bytes.insert(bytes.end(), { opcodes[insAndMode], 0x00 });
                    links.emplace_back(bytes.size() - 1, getIdentifierName(tokens[index]));
                }

                index += 2;
                continue;
            }
        }

        return "not cool";
    }

    for (const auto& link : links) {
        if (!labels.contains(link.name)) {
            return "not cool";
        }

        const auto delta = static_cast<int64_t>(labels[link.name]) - static_cast<int64_t>(link.offset) - 1;

        if (!std::in_range<int8_t>(delta)) {
            return "not cool";
        }

        bytes[link.offset] = delta;
    }

    return bytes;
}