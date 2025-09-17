#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <string>
#include <vector>

#include "Token.h"


std::variant<std::vector<Token>, std::string> tokenize (const std::string&);



#endif //TOKENIZE_H
