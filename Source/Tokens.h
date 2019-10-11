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

#ifndef RUGE_TOKENS_H
#define RUGE_TOKENS_H

#include <string>
#include <stdexcept>
#include <vector>
#include <string>

namespace HCode
{
    enum ETokenType
    {
    };
    struct FLexToken
    {
        std::string     Value;
        /**
         * TODO: use enum instead of string.
         */
        std::string     Type;
        unsigned int    Line;
        unsigned int    Offset;

        FLexToken(std::string V, std::string T, unsigned int L);
        FLexToken(const FLexToken &Token);

        void Evaluate(unsigned int Oset);
    };

    struct FToken
    {
        FLexToken           Token;
        std::vector<FToken> Children;
        FToken(FLexToken T);
        FToken(const FToken &T);
        std::string ToString(unsigned int L = 0);
        bool HasType(std::string Type);
        bool HasTypeRecursive(std::string Type);
        bool HasValue(std::string Value);
        bool HasValueRecursive(std::string Value);
        bool IsOperator();
        FToken GetByName(std::string Name);
        FToken GetByType(std::string Type);
        void GetByTypeRecursive(std::string Type, std::vector<FToken> &Out);
        int GetPrecedence();
    };
}

#endif //RUGE_TOKENS_H
