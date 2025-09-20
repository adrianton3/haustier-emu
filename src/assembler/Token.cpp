#include "Token.h"


struct TokenStringifier {
    std::string operator() (const Identifier& identifier) const { return identifier.name; }
    std::string operator() (const Number& number) const { return std::to_string(number.value); }
    std::string operator() (const Colon&) const { return { ':' }; }
    std::string operator() (const Hash&) const { return { '#' }; }
    std::string operator() (const ParOpen&) const { return { '(' }; }
    std::string operator() (const ParClosed&) const { return { ')' }; }
    std::string operator() (const Comma&) const { return { ',' }; }
    std::string operator() (const Star&) const { return { '*' }; }
    std::string operator() (const NewLine&) const { return { "NL" }; }
};

std::string stringify(const Token& token) {
    return std::visit(TokenStringifier {}, token);
}