#include <cstdint>
#include <unordered_map>
#include <variant>

#include "asm.h"

#include <optional>
#include <strings.h>

#include "opcodes.h"


enum class InsType : uint8_t {
    ADC,
    AND,
    ASL,
    BCC,
    BCS,
    BEQ,
    BIT,
    BMI,
    BNE,
    BPL,
    BRK,
    BVC,
    BVS,
    CLC,
    CLD,
    CLI,
    CLV,
    CMP,
    CPX,
    CPY,
    DEC,
    DEX,
    DEY,
    EOR,
    INC,
    INX,
    INY,
    JMP,
    JSR,
    LDA,
    LDX,
    LDY,
    LSR,
    NOP,
    ORA,
    PHA,
    PHP,
    PLA,
    PLP,
    ROL,
    ROR,
    RTI,
    RTS,
    SBC,
    SEC,
    SED,
    SEI,
    STA,
    STX,
    STY,
    TAX,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
};

std::unordered_map<std::string, InsType> stringInsTypeMap {
    { "ADC", InsType::ADC },
    { "AND", InsType::AND },
    { "ASL", InsType::ASL },
    { "BCC", InsType::BCC },
    { "BCS", InsType::BCS },
    { "BEQ", InsType::BEQ },
    { "BIT", InsType::BIT },
    { "BMI", InsType::BMI },
    { "BNE", InsType::BNE },
    { "BPL", InsType::BPL },
    { "BRK", InsType::BRK },
    { "BVC", InsType::BVC },
    { "BVS", InsType::BVS },
    { "CLC", InsType::CLC },
    { "CLD", InsType::CLD },
    { "CLI", InsType::CLI },
    { "CLV", InsType::CLV },
    { "CMP", InsType::CMP },
    { "CPX", InsType::CPX },
    { "CPY", InsType::CPY },
    { "DEC", InsType::DEC },
    { "DEX", InsType::DEX },
    { "DEY", InsType::DEY },
    { "EOR", InsType::EOR },
    { "INC", InsType::INC },
    { "INX", InsType::INX },
    { "INY", InsType::INY },
    { "JMP", InsType::JMP },
    { "JSR", InsType::JSR },
    { "LDA", InsType::LDA },
    { "LDX", InsType::LDX },
    { "LDY", InsType::LDY },
    { "LSR", InsType::LSR },
    { "NOP", InsType::NOP },
    { "ORA", InsType::ORA },
    { "PHA", InsType::PHA },
    { "PHP", InsType::PHP },
    { "PLA", InsType::PLA },
    { "PLP", InsType::PLP },
    { "ROL", InsType::ROL },
    { "ROR", InsType::ROR },
    { "RTI", InsType::RTI },
    { "RTS", InsType::RTS },
    { "SBC", InsType::SBC },
    { "SEC", InsType::SEC },
    { "SED", InsType::SED },
    { "SEI", InsType::SEI },
    { "STA", InsType::STA },
    { "STX", InsType::STX },
    { "STY", InsType::STY },
    { "TAX", InsType::TAX },
    { "TAY", InsType::TAY },
    { "TSX", InsType::TSX },
    { "TXA", InsType::TXA },
    { "TXS", InsType::TXS },
    { "TYA", InsType::TYA },
};

bool isIdentifierHeadChar (char ch) {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
}

bool isIdentifierTailChar (char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
}

bool isNumberDecChar (char ch) {
    return ch >= '0' && ch <= '9';
}

bool isNumberHexHeadChar (char ch) {
    return ch == '$';
}

bool isNumberHexTailChar (char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}

bool isFollowNumber (char ch) {
    return ch == ' ' || ch == '\n' || ch == ',' || ch == ';';
}


struct ChopResult {
    Token token;
    int index;
};

std::variant<ChopResult, std::string> chopIdentifier (const std::string& source, int indexStart) {
    auto index = indexStart;

    while (true) {
        if (index >= source.length() || !isIdentifierTailChar(source[index])) {
            return ChopResult {
                Identifier {source.substr(indexStart, index - indexStart) },
                index
            };
        }

        index++;
    }
}

std::variant<ChopResult, std::string> chopNumberHex (const std::string& source, int indexStart) {
    return { "can't chop numbers yet" };
}

std::variant<ChopResult, std::string> chopNumberDec (const std::string& source, int indexStart) {
    return { "can't chop numbers yet" };
}

int ignoreComment (const std::string& source, int indexStart) {
    auto index = indexStart;

    while (true) {
        if (index >= source.length() || source[index] == '\n') {
            return index;
        }

        index++;
    }
}

std::variant<std::vector<Token>, std::string> tokenize (const std::string& source) {
    std::vector<Token> tokens;
    auto index = 0;

    while (index < source.length()) {
        if (isIdentifierHeadChar(source[index])) {
            const auto result = chopIdentifier(source, index);
            if (const auto* nameAndIndex = std::get_if<ChopResult>(&result)) {
                tokens.emplace_back(nameAndIndex->token);
                index = nameAndIndex->index;
            } else {
                return { std::get<std::string>(result) };
            }
        }

        if (isNumberDecChar(source[index])) {
            const auto result = chopNumberDec(source, index);
            if (const auto* valueAndIndex = std::get_if<ChopResult>(&result)) {
                tokens.emplace_back(valueAndIndex->token);
                index = valueAndIndex->index;
            } else {
                return { std::get<std::string>(result) };
            }
        }

        if (isNumberHexHeadChar(source[index])) {
            const auto result = chopNumberHex(source, index);
            if (const auto* valueAndIndex = std::get_if<ChopResult>(&result)) {
                tokens.emplace_back(valueAndIndex->token);
                index = valueAndIndex->index;
            } else {
                return { std::get<std::string>(result) };
            }
        }

        if (source[index] == ':') {
            tokens.emplace_back(Colon {});
            index++;
            continue;
        }

        if (source[index] == '#') {
            tokens.emplace_back(Hash {});
            index++;
            continue;
        }

        if (source[index] == '(') {
            tokens.emplace_back(ParOpen {});
            index++;
            continue;
        }

        if (source[index] == ')') {
            tokens.emplace_back(ParClosed {});
            index++;
            continue;
        }

        if (source[index] == ',') {
            tokens.emplace_back(Comma {});
            index++;
            continue;
        }

        if (source[index] == '\n') {
            tokens.emplace_back(NewLine {});
            index++;
            continue;
        }

        if (source[index] == ' ') {
            index++;
            continue;
        }

        if (source[index] == ';') {
            tokens.emplace_back(NewLine {});
            index = ignoreComment(source, index);
            continue;
        }

        // index++; // for every other char out there??
    }

    return tokens;
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
    std::unordered_map<std::string, uint64_t> labels;
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

            if (matches<TokenType::Identifier, TokenType::NewLine>(tokens, index)) {
                if (getIdentifierName(tokens[index]) == "A") {
                    // accumulator
                    const InsAndMode insAndMode { name, AddressingMode::Accumulator };

                    if (!opcodes.contains(insAndMode)) {
                        return "not cool";
                    }

                    bytes.push_back(opcodes[insAndMode]);
                    index += 2;
                    continue;
                }
            }
        }

        return "not cool";
    }

    return bytes;
}