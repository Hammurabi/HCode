#include "Tokens.h"

void HCode::FLexToken::Evaluate(unsigned int Oset)
{
    Offset = Oset - Value.size();
    if (Type == "string")
    {
        Offset -= 2;
        return;
    }

    if (Type == "word" && Value == "const")
        Type = "const";
    else
    if (Type == "word" && Value == "import")
        Type = "import";
    else
    if (Type == "word" && Value == "script")
        Type = "script";
    else
    if (Type == "symbol")
    {
        if (Value == ".")
            Type = "dot";
        else
        if (Value == ":")
            Type = "colon";
        else
        if (Value == "::")
            Type = "doublecolon";
        else
        if (Value == "->")
            Type = "memberaccess";
        else
        if (Value == "+")
            Type = "plus";
        else
        if (Value == "-")
            Type = "minus";
        else
        if (Value == "*")
            Type = "multiply";
        else
        if (Value == "/")
            Type = "divide";
        else
        if (Value == "%")
            Type = "modulo";
        else
        if (Value == "+=")
            Type = "plus";
        else
        if (Value == "-=")
            Type = "minus";
        else
        if (Value == "*=")
            Type = "multiply";
        else
        if (Value == "/=")
            Type = "divide";
        else
        if (Value == ":=")
            Type = "lambda";
        else
        if (Value == "%=")
            Type = "modulo";
        else
        if (Value == "{")
            Type = "bracesopen";
        else
        if (Value == "}")
            Type = "bracesclosed";
        else
        if (Value == "++") //TODO: Add support
            throw (std::runtime_error("unsupported symbol type '" + Value + "'."));
        else
        if (Value == "--") //TODO: Add support
            throw (std::runtime_error("unsupported symbol type '" + Value + "'."));
        else
        if (Value == "&")
            Type = "and";
        else
        if (Value == "~")
            Type = "not";
        else
        if (Value == "^")
            Type = "cast";
        else
        if (Value == "[")
            Type = "bracketopen";
        else
        if (Value == "]")
            Type = "bracketclosed";
        else
        if (Value == "(")
            Type = "parenthesisopen";
        else
        if (Value == ")")
            Type = "parenthesisclosed";
        else
        if (Value == ",")
            Type = "comma";
        else
        if (Value == ";")
            Type = "semicolon";
        else
        if (Value == "<")
            Type = "cmpl";
        else
        if (Value == ">")
            Type = "cmpg";
        else
        if (Value == "<=")
            Type = "cmple";
        else
        if (Value == ">")
            Type = "cmpge";
        else
            throw (std::runtime_error("unsupported symbol type '" + Value + "'."));
    }
}

HCode::FLexToken::FLexToken(const HCode::FLexToken &Token)
        : Value(Token.Value), Type(Token.Type), Line(Token.Line), Offset(Token.Offset) {}

HCode::FLexToken::FLexToken(std::string V, std::string T, unsigned int L)
        : Value(V), Type(T), Line(L) {}

HCode::FToken::FToken(HCode::FLexToken T) : Token(T) {}

HCode::FToken::FToken(const HCode::FToken &T) : Token(T.Token), Children(T.Children) {}

std::string HCode::FToken::ToString(unsigned int L)
{
    std::string Spaces = "";

    for (unsigned int i = 0; i < L; i ++)
        Spaces += "\t";

    std::string Self = Spaces + "Token(" + Token.Value + ", " + Token.Type + ", " + std::to_string(Token.Line) + ")\n";

    for (auto &Child : Children)
        Self += Child.ToString(L + 1);

    return Self;
}

bool HCode::FToken::HasType(std::string Type)
{
    for (auto &Child : Children)
        if (Child.Token.Type == Type)
            return true;

    return false;
}

bool HCode::FToken::HasTypeRecursive(std::string Type)
{
    for (auto &Child : Children)
        if (Child.Token.Type == Type || Child.HasTypeRecursive(Type))
            return true;

    return false;
}

bool HCode::FToken::HasValue(std::string Value)
{
    for (auto &Child : Children)
        if (Child.Token.Value == Value)
            return true;

    return false;
}

bool HCode::FToken::HasValueRecursive(std::string Value)
{
    for (auto &Child : Children)
        if (Child.Token.Value == Value || Child.HasValueRecursive(Value))
            return true;

    return false;
}

bool HCode::FToken::IsOperator()
{
    std::string Value = Token.Value;

    return Value == "=" || Value == "+" || Value == "-" || Value == "*" || Value == "/" || Value == "%" || Value == "++"  || Value == "--" ||
           Value == "+=" || Value == "-=" || Value == "*=" || Value == "/=" || Value == "%=" || Value == ":" || Value == "->" ||
           Value == "." || Value == "!" || Value == "!=" || Value == "==" || Value == "<" || Value == ">" || Value == "<=" || Value == ">=";// || Value == ",";
}

HCode::FToken HCode::FToken::GetByName(std::string Name)
{
    for (auto &Child : Children)
        if (Child.Token.Value == Name)
            return Child;

    return FToken(FLexToken("", "", 0));
}

HCode::FToken HCode::FToken::GetByType(std::string Type)
{
    for (auto &Child : Children)
        if (Child.Token.Type == Type)
            return Child;

    return FToken(FLexToken("", "", 0));
}

void HCode::FToken::GetByTypeRecursive(std::string Type, std::vector<HCode::FToken> &Out)
{
    for (auto &Child : Children)
        if (Child.Token.Type == Type)
        {
            Out.push_back(Child);
            Child.GetByTypeRecursive(Type, Out);
        } else
            Child.GetByTypeRecursive(Type, Out);
}

int HCode::FToken::GetPrecedence()
{
    std::string Value = Token.Value;
    if (Value == "->" || Value == "." || Value == ":")
        return 18-2;
    else
    if (Value == "!")
        return 18-3;
    else
    if (Value == "*" || Value == "/" || Value == "%")
        return 18-5;
    else
    if (Value == "+" || Value == "-")
        return 18-6;
    else
    if (Value == "<" || Value == ">" || Value == "<=" || Value == ">=")
        return 18-9;
    else
    if (Value == "+=" || "+=" || Value == "-=" || Value == "*=" || Value == "/=" || Value == "%=")
        return 18-16;
    return 0;
}