#ifndef RUGE_PARSER_H
#define RUGE_PARSER_H

#include <vector>
#include <stack>
#include <string>
#include <stdexcept>
#include "Tokens.h"

namespace HCode
{
    void OC(std::string TypeName, std::string Open, std::string Closed, std::vector<FLexToken> &Tokens, std::vector<FLexToken> &Input);
    void InfixPostfix(std::vector<FToken> &InOut);
    void RearrangeRightHanded(std::vector<FToken> &InOut);
    void RearrangeLeftHanded(std::vector<FToken> &InOut);
    void RearrangeTokens(std::vector<FToken> &InOut, bool RightHanded, bool InBody);
    void Parse(std::vector<FToken> &Tokens, std::vector<FLexToken> Input, bool RightHand = false, bool FuncDecl = false, bool FuncCall = false, bool OnlyOnce = false, bool InBody = false);
}

#endif //RUGE_PARSER_H
