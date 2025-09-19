#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <string>
#include <vector>

#include "ParserError.h"
#include "Token.h"



std::variant<std::vector<Token>, ParserError> tokenize (const std::string&);



#endif //TOKENIZE_H
