#ifndef ASM_H
#define ASM_H

#include <string>
#include <vector>

#include "Token.h"



std::variant<std::vector<Token>, std::string> tokenize (const std::string&);

std::variant<std::vector<uint8_t>, std::string> assemble (const std::string&);



#endif //ASM_H
