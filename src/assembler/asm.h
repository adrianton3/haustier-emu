#ifndef ASM_H
#define ASM_H

#include <vector>

#include "Token.h"



std::variant<std::vector<uint8_t>, std::string> assemble (const std::vector<Token>&);



#endif //ASM_H
