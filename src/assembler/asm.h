#ifndef ASM_H
#define ASM_H

#include <vector>

#include "ParserError.h"
#include "Token.h"



std::variant<std::vector<uint8_t>, ParserError> assemble (const std::vector<Token>&);



#endif //ASM_H
