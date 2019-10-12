/**
 *
 * MIT License
 *
 * Copyright (c) 2019 Abdullah Fadhil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */

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
    void Parse(std::vector<FToken> &Tokens, std::vector<FLexToken> Input, bool RightHand = false, bool FuncDecl = false, bool FuncCall = false, bool OnlyOnce = false, bool InBody = false, bool AllowComma = false);
}

#endif //RUGE_PARSER_H
