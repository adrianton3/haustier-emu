#ifndef PARSERERROR_H
#define PARSERERROR_H

#include <string>



struct ParserError {
    std::string message;
    int lineIndex;
};



#endif //PARSERERROR_H
