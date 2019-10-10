#ifndef RUGE_LEXER_H
#define RUGE_LEXER_H

#include <vector>
#include <stdexcept>
#include "Tokens.h"

namespace HCode
{
    void Lex(std::vector<FLexToken> &Tokens, const std::string &Script);
}


#endif //RUGE_LEXER_H
