#include <variant>

#include "tokenize.h"



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

template<int Base>
std::variant<int64_t, std::string> parseNumber (const char* start) {
    // errno = 0;
    const auto value = std::strtol(start, nullptr, Base);
    // check errno

    return { value };
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
    indexStart++;
    if (indexStart >= source.size() || !isNumberHexTailChar(source[indexStart])) {
        return { "expected a hex number after '$'" };
    }

    auto index = indexStart;

    while (true) {
        // remove this loop since strtol does it itself ?
        if (index >= source.length() || isFollowNumber(source[index])) {
            const auto result = parseNumber<16>(source.c_str() + indexStart);

            if (const auto* value = std::get_if<int64_t>(&result)) {
                return ChopResult {
                    Number { *value },
                    index
                };
            }

            return { std::get<std::string>(result) };
        }

        if (!isNumberHexTailChar(source[index])) {
            return { std::string { "unexpected '" } + source[index + 1] + '\'' };
        }

        index++;
    }
}

std::variant<ChopResult, std::string> chopNumberDec (const std::string& source, int indexStart) {
    return { "can't chop nnumbers yet" };
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