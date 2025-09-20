#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>


enum class TokenType : uint8_t {
    Number,
    Identifier,
    Colon,
    Hash,
    ParOpen,
    ParClosed,
    Comma,
    Star,
    NewLine,
};

struct Number { int64_t value; };
struct Identifier { std::string name; };
struct Colon {};
struct Hash {};
struct ParOpen {};
struct ParClosed {};
struct Comma {};
struct Star {};
struct NewLine {};

typedef std::variant<
    Number,
    Identifier,
    Colon,
    Hash,
    ParOpen,
    ParClosed,
    Comma,
    Star,
    NewLine
> Token;

std::string stringify (const Token&);


#endif //TOKEN_H
