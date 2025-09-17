#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>


enum class TokenType {
    Number,
    Identifier,
    Colon,
    Hash,
    ParOpen,
    ParClosed,
    Comma,
    NewLine,
};

struct Number { int64_t value; };
struct Identifier { std::string name; };
struct Colon {};
struct Hash {};
struct ParOpen {};
struct ParClosed {};
struct Comma {};
struct NewLine {};

typedef std::variant<
    Number,
    Identifier,
    Colon,
    Hash,
    ParOpen,
    ParClosed,
    Comma,
    NewLine
> Token;

std::string stringify (const Token&);


#endif //TOKEN_H
