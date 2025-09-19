#include <variant>

#include "tokenize.h"

#include "ParserError.h"


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
    return ch == ' ' || ch == '\n' || ch == ',' || ch == ';' || ch == ')';
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

ChopResult chopIdentifier (const std::string& source, int indexStart) {
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

std::variant<ChopResult, ParserError> chopNumberHex (const std::string& source, int indexStart, int lineIndex) {
    indexStart++;
    if (indexStart >= source.size() || !isNumberHexTailChar(source[indexStart])) {
        return ParserError { "expected a hex number after '$'", lineIndex };
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

            return ParserError { std::get<std::string>(result), lineIndex };
        }

        if (!isNumberHexTailChar(source[index])) {
            return ParserError { std::string { "unexpected '" } + source[index] + '\'', lineIndex };
        }

        index++;
    }
}

std::variant<ChopResult, ParserError> chopNumberDec (const std::string& source, int indexStart, int lineIndex) {
    auto index = indexStart;

    while (true) {
        // remove this loop since strtol does it itself ?
        if (index >= source.length() || isFollowNumber(source[index])) {
            const auto result = parseNumber<10>(source.c_str() + indexStart);

            if (const auto* value = std::get_if<int64_t>(&result)) {
                return ChopResult {
                    Number { *value },
                    index
                };
            }

            return ParserError { std::get<std::string>(result), lineIndex };
        }

        if (!isNumberDecChar(source[index])) {
            return ParserError { std::string { "unexpected '" } + source[index] + '\'', lineIndex };
        }

        index++;
    }
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

std::variant<std::vector<Token>, ParserError> tokenize (const std::string& source) {
    std::vector<Token> tokens;
    auto index = 0;
    auto lineIndex = 0;

    while (index < source.length()) {
        if (isIdentifierHeadChar(source[index])) {
            const auto nameAndIndex = chopIdentifier(source, index);
            tokens.emplace_back(nameAndIndex.token);
            index = nameAndIndex.index;
        }

        if (isNumberDecChar(source[index])) {
            const auto result = chopNumberDec(source, index, lineIndex);
            if (const auto* valueAndIndex = std::get_if<ChopResult>(&result)) {
                tokens.emplace_back(valueAndIndex->token);
                index = valueAndIndex->index;
            } else {
                return { std::get<ParserError>(result) };
            }
        }

        if (isNumberHexHeadChar(source[index])) {
            const auto result = chopNumberHex(source, index, lineIndex);
            if (const auto* valueAndIndex = std::get_if<ChopResult>(&result)) {
                tokens.emplace_back(valueAndIndex->token);
                index = valueAndIndex->index;
            } else {
                return { std::get<ParserError>(result) };
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
            lineIndex++;
            continue;
        }

        if (source[index] == ' ') {
            index++;
            continue;
        }

        if (source[index] == ';') {
            tokens.emplace_back(NewLine {});
            index = ignoreComment(source, index);
            lineIndex++;
            continue;
        }

        // index++; // for every other char out there??
    }

    if (tokens.empty() || tokens.back().index() != static_cast<size_t>(TokenType::NewLine)) {
        tokens.emplace_back(NewLine {});
    }

    return tokens;
}