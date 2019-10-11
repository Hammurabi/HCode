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

#include "Lexer.h"

void HCode::Lex(std::vector<HCode::FLexToken> &Tokens, const std::string &Script)
{
    FLexToken Token("", "", 1);
    char LastChar = '\n';
    unsigned int Line = 1;
    unsigned int Offset = 0;
    bool Comment = false;

    for (int i = 0; i < Script.size(); i ++)
    {
        char Current = Script[i];
        Offset ++;

        if (Current == '/' && Script.size() > (i + 1) && Script[i + 1] == '/')
        {
            Comment = true;
            if (Token.Value.size() > 0) {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
            }
            Token = FLexToken("", "", Line);
            continue;
        }

        if (Comment && Current != '\n')
            continue;

        if (Current == '\'')
        {
            if (Token.Value.size() > 0) {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
            }
            Token = FLexToken("", "char", Line);

            Token.Type = "char";
            if (Script.size() > (i + 1))
            {
                if (Script[i + 1] == '\\')
                {
                    i ++;

                    if (Script.size() > (i + 1))
                    {
                        char NonEscaped = Script[i + 1];

                        char Escaped    = 'c';

                        switch (NonEscaped)
                        {
                            case 'b':
                                Escaped = '\b';
                                break;
                            case 'f':
                                Escaped = '\f';
                                break;
                            case 'n':
                                Escaped = '\n';
                                break;
                            case 'r':
                                Escaped = '\r';
                                break;
                            case 't':
                                Escaped = '\t';
                                break;
                            case '"':
                                Escaped = '\"';
                                break;
                            case '\\':
                                Escaped = '\\\\';
                                break;
                            default:
                                throw (std::runtime_error("escape character used on unsupported char. at '" + std::to_string(Line) + "'."));
                        }
                        Token.Value += Escaped;
                        i ++;
                    }
                    else
                        throw (std::runtime_error("\\ never closed. at '" + std::to_string(Line) + "'."));
                }
                else
                {
                    Token.Value += Script[i + 1];
                    i ++;
                }

                if (Script.size() <= (i + 1))
                    throw(std::runtime_error("quote never closed. at '" + std::to_string(Line) + "'."));

                if (Script.size() > (i + 1) && Script[i + 1] != '\'')
                    throw(std::runtime_error("quote never closed. at '" + std::to_string(Line) + "'."));

                i ++;

                if (Token.Value.size() > 0) {
                    Token.Evaluate(Offset);
                    Tokens.push_back(Token);
                }
                Token = FLexToken("", "", Line);
            } else
                throw(std::runtime_error("quote never closed. at '" + std::to_string(Line) + "'."));
        }
        else
        if (Current == '\\' && Token.Type == "string")
        {
            if (Script.size() > (i + 1))
            {
                Token.Value += Current;
                Token.Value += Script[i + 1];
                i ++;
            } else
                throw(std::runtime_error("\\ used on null chars following it. at '" + std::to_string(Line) + "'."));
        }
        else
        if (Current != '"' && Token.Type == "string")
            Token.Value += Current;
        else
        if (Current == '"')
        {
            if (Token.Type == "string")
            {
                if (Token.Value.size() > 0) {
                    Token.Evaluate(Offset);
                    Tokens.push_back(Token);
                }
                Token = FLexToken("", "", Line);
            } else if (Token.Value.size() == 0)
                Token.Type = "string";
            else if (Token.Value.size() > 0)
            {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
                Token = FLexToken("", "string", Line);
            }
        }
        else
        if (Current == ' ' || Current == '\t')
        {
            if (Token.Value.size() > 0) {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
            }
            Token = FLexToken("", "", Line);
        }
        else
        if (Current == '\n')
        {
            Comment = false;
            Line ++;
            Offset = 0;
            if (Token.Value.size() > 0) {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
            }
            Token = FLexToken("", "", Line);
        }
        else
        if (std::isdigit(Current))
        {
            if (Token.Value.size() == 0)
                Token.Type = "number";

            if ((Token.Type != "number" && Token.Type != "decimal" && Token.Type != "word") && Token.Value.size() > 0)
            {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
                Token = FLexToken("", "number", Line);
            }

            if (Token.Type != "number" && Token.Type != "decimal" && Token.Type != "word")
                throw(std::runtime_error(std::string("cannot append number '") + Current + "' to '" + Token.Value + "'."));

            Token.Value += Current;
        }
        else
        if (std::isalpha(Current) || Current == '_' || Current == '$')
        {
            if (Token.Value.size() == 0)
                Token.Type = "word";

            if ((Token.Type != "word" && Token.Type != "number" && Token.Type != "decimal") && Token.Value.size() > 0)
            {
                Token.Evaluate(Offset);
                Tokens.push_back(Token);
                Token = FLexToken("", "word", Line);
            }

            if (Token.Type == "number" || Token.Type == "decimal")
                throw(std::runtime_error(std::string("cannot append word '") + Current + "' to '" + Token.Value + "'."));

            Token.Value += Current;
        }
        else
        if (Current == '.' && Token.Type == "number")
        {
            Token.Value += Current;
            Token.Type = "decimal";
        }
        else
        if (Current == '.' && Token.Type == "decimal")
            throw(std::runtime_error("invalid decimal '" + Token.Value + "' at '" + std::to_string(Token.Line) + "'."));
        else
        {
            if (Token.Value.size() == 0)
                Token.Type = "symbol";

            if ((Token.Value + Current) == "+")
            {
                Token.Value += Current;
                Token.Type = "plus";
            }
            else
            if ((Token.Value + Current) == "{")
            {
                Token.Value += Current;
                Token.Type = "bracesopen";
            }
            else
            if ((Token.Value + Current) == "~")
            {
                Token.Value += Current;
                Token.Type = "not";
            }
            else
            if ((Token.Value + Current) == "}")
            {
                Token.Value += Current;
                Token.Type = "bracesclosed";
            }
            else
            if ((Token.Value + Current) == ",")
            {
                Token.Value += Current;
                Token.Type = "comma";
            }
            else
            if ((Token.Value + Current) == ".")
            {
                Token.Value += Current;
                Token.Type = "dot";
            }
            else
            if ((Token.Value + Current) == ";")
            {
                Token.Value += Current;
                Token.Type = "semicolon";
            }
            else
            if ((Token.Value + Current) == "-")
            {
                Token.Value += Current;
                Token.Type = "minus";
            }
            else
            if ((Token.Value + Current) == "*")
            {
                Token.Value += Current;
                Token.Type = "multiply";
            }
            else
            if ((Token.Value + Current) == "/")
            {
                Token.Value += Current;
                Token.Type = "divide";
            }
            else
            if ((Token.Value + Current) == "%")
            {
                Token.Value += Current;
                Token.Type = "modulo";
            }
            else
            if ((Token.Value + Current) == ":=")
            {
                Token.Value += Current;
                Token.Type = "lambda";
            }
            else
            if ((Token.Value + Current) == "++")
                throw(std::runtime_error("unsupported symbol type '++'."));
            else
            if ((Token.Value + Current) == "--")
                throw(std::runtime_error("unsupported symbol type '--'."));
            else
            if ((Token.Value + Current) == "+=")
            {
                Token.Value += Current;
                Token.Type = "plusequals";
            }
            else
            if ((Token.Value + Current) == "-=")
            {
                Token.Value += Current;
                Token.Type = "minusequals";
            }
            else
            if ((Token.Value + Current) == "*=")
            {
                Token.Value += Current;
                Token.Type = "multiplyequals";
            }
            else
            if ((Token.Value + Current) == "/=")
            {
                Token.Value += Current;
                Token.Type = "divideequals";
            }
            else
            if ((Token.Value + Current) == "^")
            {
                Token.Value += Current;
                Token.Type = "cast";
            }
            else
            if ((Token.Value + Current) == "%=")
            {
                Token.Value += Current;
                Token.Type = "moduloequals";
            }
            else
            if ((Token.Value + Current) == "&")
            {
                Token.Value += Current;
                Token.Type = "and";
            }
            else
            if ((Token.Value + Current) == "[")
            {
                Token.Value += Current;
                Token.Type = "bracketopen";
            }
            else
            if ((Token.Value + Current) == "]")
            {
                Token.Value += Current;
                Token.Type = "bracketclosed";
            }
            else
            if ((Token.Value + Current) == "(")
            {
                Token.Value += Current;
                Token.Type = "parenthesisopen";
            }
            else
            if ((Token.Value + Current) == ")")
            {
                Token.Value += Current;
                Token.Type = "parenthesisclosed";
            }
            else
            if ((Token.Value + Current) == "=")
            {
                Token.Value += Current;
                Token.Type = "equals";
            }
            else
            if ((Token.Value + Current) == "==")
            {
                Token.Value += Current;
                Token.Type = "equalsequals";
            }
            else
            if ((Token.Value + Current) == "!")
            {
                Token.Value += Current;
                Token.Type = "logicalnot";
            }
            else
            if ((Token.Value + Current) == "!=")
            {
                Token.Value += Current;
                Token.Type = "notequals";
            }
            else
            if ((Token.Value + Current) == "->")
            {
                Token.Value += Current;
                Token.Type = "memberaccess";
            }
            else
            if ((Token.Value + Current) == ":")
            {
                Token.Value += Current;
                Token.Type = "colon";
            }
            else
            if ((Token.Value + Current) == "<")
            {
                Token.Value += Current;
                Token.Type = "cmpl";
            }
            else
            if ((Token.Value + Current) == ">")
            {
                Token.Value += Current;
                Token.Type = "cmpg";
            }
            else
            if ((Token.Value + Current) == "<=")
            {
                Token.Value += Current;
                Token.Type = "cmple";
            }
            else
            if ((Token.Value + Current) == ">=")
            {
                Token.Value += Current;
                Token.Type = "cmpge";
            }
            else
            if ((Token.Value + Current) == "::")
            {
                Token.Value += Current;
                Token.Type = "doublecolon";
            }
            else
            {
                if (Token.Value.size() > 0) {
                    Token.Evaluate(Offset);
                    Tokens.push_back(Token);
                }

                Token = FLexToken("", "symbol", Line);
                Token.Value += Current;
            }
        }

        LastChar = Current;
    }

    Token.Evaluate(Offset);
    if (Token.Value.size() > 0) Tokens.push_back(Token);
}