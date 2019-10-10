#ifndef RUGE_TOKENS_H
#define RUGE_TOKENS_H

#include <string>
#include <stdexcept>
#include <vector>
#include <string>

namespace HCode
{
    struct FLexToken
    {
        std::string     Value;
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
