#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>


struct Number { int64_t value; };
struct Identifier { std::string name; };
struct Colon {};
struct Hash {};
struct ParOpen {};
struct ParClosed {};
struct Comma {};

typedef std::variant<
    Number,
    Identifier,
    Colon,
    Hash,
    ParOpen,
    ParClosed,
    Comma
> Token;

std::string stringify (const Token&);


#endif //TOKEN_H
