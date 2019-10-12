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

#include "Parser.h"
#include <iostream>

void HCode::OC(std::string TypeName, std::string Open, std::string Closed, std::vector<HCode::FLexToken> &Tokens,
               std::vector<HCode::FLexToken> &Input)
{
    FLexToken TokeA = * Input.begin();
    if (Input[0].Value != Open)
        throw(std::runtime_error("no '" + TypeName + "' found. at '" + std::to_string(Input[0].Line) + "'."));

    Input.erase(Input.begin());

    unsigned int MustClose = 1;

    while (Input.size() > 0)
    {
        if (Input.begin()->Value == Open)
        {
            Tokens.push_back(*Input.begin());
            MustClose ++;
        }
        else
        if (Input.begin()->Value == Closed)
        {
            if (-- MustClose == 0)
            {
                Input.erase(Input.begin());
                return;
            }
            else
                Tokens.push_back(*Input.begin());
        }
        else
            Tokens.push_back(*Input.begin());

        Input.erase(Input.begin());
    }

//    for (auto &P : Tokens)
//    {
////        std::cerr << "f " << P.Value << std::endl;
//    }
//    if (Input.size() > 0)
//        for (auto &P : Input)
//        {
//            std::cerr << "A " << P.Value << std::endl;
//        }

    throw(std::runtime_error(TypeName + "s not fully closed. at '" + std::to_string(TokeA.Line) + "'."));
}

void HCode::InfixPostfix(std::vector<HCode::FToken> &InOut)
{
    std::vector<FToken> Postfix;

    std::stack<FToken> Stack;
    FToken C(FLexToken("", "", 0));

    for (int i = 0; i < InOut.size(); i++)
    {
        C = InOut[i];

        if (C.IsOperator())
        {
            if (!Stack.empty() && C.GetPrecedence() <= Stack.top().GetPrecedence())
            {
                Postfix.push_back(Stack.top());
                Stack.pop();
            }

            Stack.push(C);
        } else
            Postfix.push_back(C);
    }

    while (!Stack.empty())
    {
        Postfix.push_back(Stack.top());
        Stack.pop();
    }

    InOut.clear();
    InOut.insert(InOut.end(), Postfix.begin(), Postfix.end());

    std::vector<FToken> Temp;

    for (unsigned long I = 0; I < InOut.size(); I ++)
    {
        if (InOut.size() > (I+2) && (InOut[I + 2].Token.Value == "." || InOut[I + 2].Token.Value == "->" || InOut[I + 2].Token.Value == ":"))
        {
            FToken MemberAccess(InOut[I + 2].Token);
            MemberAccess.Token.Type = "memberaccess";
            MemberAccess.Children.push_back(InOut[I]);
            MemberAccess.Children.push_back(InOut[I + 1]);
            I += 2;
            while (InOut.size() > (I+2) && (InOut[I + 2].Token.Value == "." || InOut[I + 2].Token.Value == "->" || InOut[I + 2].Token.Value == ":"))
            {
                MemberAccess.Children.push_back(InOut[I + 1]);
                I += 2;
            }

            Temp.push_back(MemberAccess);
        }
        else
            Temp.push_back(InOut[I]);
    }

    InOut.clear();
    InOut.insert(InOut.end(), Temp.begin(), Temp.end());
}

void HCode::RearrangeRightHanded(std::vector<HCode::FToken> &InOut)
{
    std::vector<FToken> Out;

    bool Continue = false;
    for (auto & T : InOut)
        if (T.Token.Value == ",")
        {
            Continue = true;
            break;
        }

    if (!Continue)
    {
        InfixPostfix(InOut);
        return;
    }

    std::vector<FToken> Temp;
    while (InOut.size())
    {
        FToken Toke = InOut[0];
        InOut.erase(InOut.begin());

        if (Toke.Token.Value == ",")
        {
            Toke.Children.insert(Toke.Children.end(), Temp.begin(), Temp.end());
            InfixPostfix(Toke.Children);

            Out.push_back(Toke);
            Temp.clear();
        }
        else
            Temp.push_back(Toke);
    }

    if (Temp.size() > 0)
    {
        FToken Toke(FLexToken(",", "comma", Temp[0].Token.Line));
        Toke.Children.insert(Toke.Children.end(), Temp.begin(), Temp.end());
        InfixPostfix(Toke.Children);

        Out.push_back(Toke);
        Temp.clear();
    }

    InOut.clear();
    InOut.insert(InOut.end(), Out.begin(), Out.end());
}

void HCode::RearrangeLeftHanded(std::vector<HCode::FToken> &InOut)
{
    if (InOut.size() == 0)
        return;

    std::vector<FToken> Out;

    std::vector<FToken> Temp;
    int Line = InOut[0].Token.Line;

    while (InOut.size())
    {
        FToken Toke = InOut[0];
        InOut.erase(InOut.begin());

        if (Toke.Token.Line != Line)
        {
            FToken Statement(FLexToken("statement", "statement", Line));

            Statement.Children.insert(Statement.Children.end(), Temp.begin(), Temp.end());
            InfixPostfix(Statement.Children);
            Out.push_back(Statement);
            Temp.clear();
            Temp.push_back(Toke);
        }
        else
            Temp.push_back(Toke);

        Line = Toke.Token.Line;
    }

    if (Temp.size() > 0)
    {
        FToken Statement(FLexToken("statement", "statement", Line));

        Statement.Children.insert(Statement.Children.end(), Temp.begin(), Temp.end());
        InfixPostfix(Statement.Children);
        Out.push_back(Statement);
        Temp.clear();
    }

    InOut.clear();
    InOut.insert(InOut.end(), Out.begin(), Out.end());
}

void HCode::RearrangeTokens(std::vector<HCode::FToken> &InOut, bool RightHanded, bool InBody) {
    if (RightHanded)
        RearrangeRightHanded(InOut);
    else
    if (InBody)
        RearrangeLeftHanded(InOut);
}

#define IEQ(x) (Input[0].Value == x)
void
HCode::Parse(std::vector<HCode::FToken> &Tokens, std::vector<HCode::FLexToken> Input, bool RightHand, bool FuncDecl,
             bool FuncCall, bool OnlyOnce, bool InBody, bool AllowComma)
{
    while (Input.size() > 0)
    {
        if (Input[0].Value == "if")
        {
            FLexToken IfToken = Input[0];
            IfToken.Type = "if";
            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("if requires condition '" + std::to_string(IfToken.Line) + "'."));

            std::vector<FLexToken> Condition;
            while ( Input.size() > 0 && Input[0].Value != "{" && Input[0].Line == IfToken.Line )
            {
                Condition.push_back(* Input.begin().base());
                Input.erase(Input.begin());
            }

            if (Input.size() == 0)
                throw(std::runtime_error("if requires body '" + std::to_string(IfToken.Line) + "'."));

            if (Input.begin()->Value != "{")
                throw(std::runtime_error("if requires body (missing '{') '" + std::to_string(IfToken.Line) + "'."));

            std::vector<FLexToken> BodyTokes;
            OC("braces", "{", "}", BodyTokes, Input);

            FToken IfToke(IfToken);
            FToken ConditionToke(IfToken);
            FToken BodyToke(IfToken);
            IfToke.Token.Value = "if";
            IfToke.Token.Type = "if";
            ConditionToke.Token.Value = "parenthesis";
            ConditionToke.Token.Type = "parenthesis";
            BodyToke.Token.Value = "body";
            BodyToke.Token.Type = "body";

            Parse(ConditionToke.Children, Condition, true);
            Parse(BodyToke.Children, BodyTokes);

            IfToke.Children.push_back(ConditionToke);
            IfToke.Children.push_back(BodyToke);

            Tokens.push_back(IfToke);
        }
        else
        if (Input[0].Value == "for")
        {
            FLexToken IfToken = Input[0];
            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("for requires condition '" + std::to_string(IfToken.Line) + "'."));

            std::vector<FLexToken> Condition;
            while ( Input.size() > 0 && Input[0].Value != "{" && Input[0].Line == IfToken.Line )
            {
                Condition.push_back(* Input.begin().base());
                Input.erase(Input.begin());
            }

            if (Input.size() == 0)
                throw(std::runtime_error("for requires body '" + std::to_string(IfToken.Line) + "'."));

            if (Input.begin()->Value != "{")
                throw(std::runtime_error("for requires body (missing '{') '" + std::to_string(IfToken.Line) + "'."));

            std::vector<FLexToken> BodyTokes;
            OC("braces", "{", "}", BodyTokes, Input);

            FToken IfToke(IfToken);
            FToken ConditionToke(IfToken);
            FToken BodyToke(IfToken);
            IfToke.Token.Value = "for";
            IfToke.Token.Type = "for";
            ConditionToke.Token.Value = "parenthesis";
            ConditionToke.Token.Type = "parenthesis";
            BodyToke.Token.Value = "body";
            BodyToke.Token.Type = "body";

            std::vector<std::vector<FLexToken>> Statements;
            std::vector<FLexToken> Temp;
            for (auto &Toke : Condition)
            {
                if (Toke.Value == ",")
                {
                    Statements.push_back(std::vector<FLexToken> (Temp));
                    Temp.clear();
                }
                else
                    Temp.push_back(Toke);
            }

            if (Temp.size())
                Statements.push_back(Temp);

            if (Statements.size() != 3)
                throw(std::runtime_error("for-loops require 3-step conditions '" + std::to_string(IfToken.Line) + "'."));

            unsigned int Step = 0;

            for (auto &A: Statements)
            {
                FToken Statement(A[0]);
                Statement.Token.Value   = "statement";
                Statement.Token.Type    = "statement";

                Parse(Statement.Children, A, Step ++);
                ConditionToke.Children.push_back(Statement);
            }

            RearrangeRightHanded(ConditionToke.Children);
            Parse(BodyToke.Children, BodyTokes);

            IfToke.Children.push_back(ConditionToke);
            IfToke.Children.push_back(BodyToke);

            Tokens.push_back(IfToke);
//            std::cerr << IfToke.ToString() << std::endl;
        }
        else
        if (Input[0].Type == "import")
        {
            FLexToken ImportToken = Input[0];
            ImportToken.Type = "import";
            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("import requires name '" + std::to_string(ImportToken.Line) + "'."));

            if (Input[0].Type != "word")
                throw(std::runtime_error("import requires valid name '" + std::to_string(ImportToken.Line) + "'."));

            FLexToken ImportName = Input[0];
            ImportName.Type = "name";
            Input.erase(Input.begin());

            FToken Token(ImportToken);
            Token.Children.push_back(FToken(ImportName));

            Tokens.push_back(Token);
        }
        else
        if (Input[0].Type == ("script"))
        {
            FLexToken ScriptName = Input[0];
            if (RightHand)
                throw(std::runtime_error("script name declarations cannot be made on the right side of a statement. at '" + std::to_string(ScriptName.Line) + "'."));
            FToken Script(ScriptName);
            Script.Token.Type = "scriptname";
            Input.erase(Input.begin());
            if (Input.size() == 0)
                throw(std::runtime_error("script qualifier must be followed by a name. at '" + std::to_string(ScriptName.Line) + "'."));
            if (Input[0].Type != "word")
                throw(std::runtime_error("script qualifier must be followed by a valid name. at '" + std::to_string(ScriptName.Line) + "'."));

            Script.Token.Value = Input[0].Value;
            Input.erase(Input.begin());

            Tokens.push_back(Script);
        }
        else
        if (Input[0].Value == "static_cast")
        {
            FLexToken StaticCastToken = Input[0];
            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("static cast requires <T> and (...). at '" + std::to_string(StaticCastToken.Line) + "'."));

            if (Input[0].Value != "<")
                throw(std::runtime_error("static cast requires <T> and (...). at '" + std::to_string(StaticCastToken.Line) + "'."));

            if (Input.size() < 6)
                throw(std::runtime_error("static cast requires <T> and (...). at '" + std::to_string(StaticCastToken.Line) + "'."));

            FLexToken AlligatorMouthOpenToken = Input[0];
            Input.erase(Input.begin());
            FLexToken TypeNameInAlligatorTokens = Input[0];
            Input.erase(Input.begin());
            FLexToken AlligatorMouthClosedToken = Input[0];
            Input.erase(Input.begin());

            if (AlligatorMouthOpenToken.Value != "<" || TypeNameInAlligatorTokens.Type != "word" || AlligatorMouthClosedToken.Value != ">")
                throw(std::runtime_error("static cast requires <T> and (...). at '" + std::to_string(StaticCastToken.Line) + "'."));

            if (Input.size() == 0 || Input[0].Value != "(")
                throw(std::runtime_error("static cast requires <T> and (...). at '" + std::to_string(StaticCastToken.Line) + "'."));

            FLexToken ParenthesisToken = Input[0];
            ParenthesisToken.Type = "parenthesis";
            ParenthesisToken.Value = "parenthesis";
            FToken Prn(ParenthesisToken);
            std::vector<FLexToken> Tokes;
            OC("parenthesis", "(", ")", Tokes, Input);
            Parse(Prn.Children, Tokes, true, true);

            FToken StaticCast(StaticCastToken);
            StaticCast.Token.Type = "staticcast";
            TypeNameInAlligatorTokens.Type = "typename";
            StaticCast.Children.push_back(FToken(TypeNameInAlligatorTokens));
            StaticCast.Children.push_back(FToken(Prn));

            Tokens.push_back(StaticCast);
        }
        else
            //Field declaration
        if (Input[0].Value == "[")
        {
            FLexToken BToke = Input[0];
            std::vector<FLexToken> Tokes;
            OC("bracket", "[", "]", Tokes, Input);

            if (Tokes.size() > 0)
            {
                FToken IdentifierOperation(BToke);

                int References = 0;
                int Dereferences = 0;

                if (Tokes.size() > 0 && Tokes[0].Value == "*") {
                    Dereferences ++;
                    IdentifierOperation.Token.Value = "dereference";
                    IdentifierOperation.Token.Type = "dereference";
                    Tokes.erase(Tokes.begin());
                }
                if (Tokes.size() > 0 && Tokes[0].Value == "&") {
                    References ++;
                    IdentifierOperation.Token.Value = "referencefield";
                    IdentifierOperation.Token.Type = "referencefield";
                    Tokes.erase(Tokes.begin());
                }

                if (References > 0 && Dereferences > 0)
                    throw(std::runtime_error("cannot reference and dereference in the same logic block. " + std::to_string(BToke.Line)));

                Parse(IdentifierOperation.Children, Tokes);

                Tokens.push_back(IdentifierOperation);
            }
            else
            {
                throw(std::runtime_error("brackets but no value. " + std::to_string(BToke.Line)));
            }
        }
        else
        if (Input.size() > 1 && Input[0].Type == "word" && Input[1].Value == ":=")
        {
            FLexToken FieldName = Input[0];
            Input.erase(Input.begin());
            Input.erase(Input.begin());

            unsigned char   IsPointer   = 0;
            unsigned long   ArraySize   = 0;
            bool            IsArray     = 0;
            bool            IsConst     = 0;
            unsigned char   IsReference = 0;
            bool            IsTyped     = 0;
            FLexToken       Nam         = FLexToken("", "", FieldName.Line);
            std::string ReturnSignature = "";

            while (Input.size() > 0 && Input[0].Value != "(" && Input[0].Line == Nam.Line)
            {
                if (Input[0].Value == "*")
                    IsPointer ++;
                else
                if (Input[0].Value == "const" && !IsConst)
                    IsConst = true;
                else
                if (Input[0].Value == "&")
                    IsReference ++;
                else
                if (Input[0].Type == "word" && !IsTyped)
                {
                    Nam = Input[0];
                    Nam.Type = "returns";
                    IsTyped = true;
                }
                else
                    throw(std::runtime_error("token '" + Input[0].Value + " (" + Input[0].Type + ")" + "' is out of place (function declaration) at '" + std::to_string(Input[0].Line) + "'."));

                ReturnSignature += Input[0].Value;
                Input.erase(Input.begin());
            }


            if (!IsTyped)
                throw(std::runtime_error("function declaration has no valid return type. at '" + std::to_string(Nam.Line) + "'."));


            FToken ReturnType(Nam);
            if (IsPointer > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsPointer), "is_pointer", Nam.Line)));
            if (IsConst)
                ReturnType.Children.push_back(FToken(FLexToken("is_const", "is_const", Nam.Line)));
            if (IsReference > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsReference), "is_reference", Nam.Line)));

            if (IsReference && IsPointer)
                throw(std::runtime_error("'" + Nam.Value + "' cannot be both a pointer and a reference. at '" + std::to_string(Nam.Line) + "'."));

            if (Input.size() == 0)
                throw(std::runtime_error("lambdas must be followed by parenthesis. at '" + std::to_string(FieldName.Line) + "'."));

            if (Input[0].Value != "(")
                throw(std::runtime_error("lambdas must be followed by parenthesis. at '" + std::to_string(FieldName.Line) + "'."));


            std::string Signature = "";
            std::vector<FLexToken> ParenthesisTokens;
            OC("parenthesis", "(", ")", ParenthesisTokens, Input);
            for (auto &Toke : ParenthesisTokens) Signature += Toke.Value;
            FToken Parenthesis(FLexToken("parenthesis", "parenthesis", FieldName.Line));
            Signature = ReturnSignature + "(" + Signature + ")";

            Parse(Parenthesis.Children, ParenthesisTokens, false, true);

            if (Input.size() && Input[0].Value == "{")
            {
                std::vector<FLexToken> BodyTokens;


                OC("braces", "{", "}", BodyTokens, Input);
                FToken Body(FLexToken("body", "body", Nam.Line));
                Parse(Body.Children, BodyTokens, false, true, false, false, true);

                FToken Field(FieldName);
                Field.Token.Type = "lambda";
                Field.Children.push_back(ReturnType);
                Field.Children.push_back(Parenthesis);
                Field.Children.push_back(FToken(FLexToken("is_function", "is_function", Nam.Line)));
                Field.Children.push_back(Body);

                Tokens.push_back(Field);
            }
            else
            if (Input.size() && Input[0].Value == "=")
            {
                std::vector<FLexToken> EqualsTokens;
                Input.erase(Input.begin());

                while (Input.size() && Input[0].Line == Nam.Line)
                {
                    EqualsTokens.push_back(Input[0]);
                    Input.erase(Input.begin());
                }

                if (EqualsTokens.size() == 0)
                    throw(std::runtime_error("equals must be followed by value. at '" + std::to_string(Nam.Line) + "'."));


                FToken Body(FLexToken("equals", "equals", Nam.Line));
                Parse(Body.Children, EqualsTokens, true);

                FToken Field(FieldName);
                Field.Token.Type = "field";
                Field.Token.Value = Signature;
                FToken Name(FieldName);
                Name.Token.Type = "name";
                Field.Children.push_back(Name);
                Field.Children.push_back(FToken(FLexToken(FieldName.Value, "name", Nam.Line)));
                Field.Children.push_back(FToken(FLexToken("is_function", "is_function", Nam.Line)));
                Field.Children.push_back(Parenthesis);
                Field.Children.push_back(Body);
//                for (auto &Child : ReturnType.Children)
//                    Field.Children.push_back(Child);

                Tokens.push_back(Field);
            }
            else
            {
                FToken Field(FieldName);
                Field.Token.Type = "field";
                Field.Token.Value = Signature;
                FToken Name(FieldName);
                Name.Token.Type = "name";
                Field.Children.push_back(Name);
                Field.Children.push_back(FToken(FLexToken("is_function", "is_function", Nam.Line)));
                Field.Children.push_back(Parenthesis);
//                for (auto &Child : ReturnType.Children)
//                    Field.Children.push_back(Child);

                Tokens.push_back(Field);
            }

//            std::cerr << (Tokens.end()-1)->ToString() << std::endl;
        }
        else
        if (Input.size() > 1 && Input[0].Type == "word" && Input[1].Value == ":")
        {
            if (Input.size() > 0 && Input[0].Type == "word")
            {
                FLexToken BToke = Input[0];
                if (RightHand)
                    throw(std::runtime_error("field declarations cannot be made on the right side of a statement. at '" + std::to_string(BToke.Line) + "'."));

                FToken FieldDeclaration(BToke);

                FieldDeclaration.Token.Type = "field";

                FLexToken FieldNameT = Input[0];
                Input.erase(Input.begin());
                FieldNameT.Type = "name";
                if (FieldNameT.Value == "NULL")
                    throw(std::runtime_error("'NULL' is a reserved word. at '" + std::to_string(FieldNameT.Line) + "'."));

                FToken FieldName(FieldNameT);
                std::string Typ= "";

                if (Input[0].Value != ":")
                    throw(std::runtime_error("field declarations missing colon. at '" + std::to_string(BToke.Line) + "'."));

                Input.erase(Input.begin());

                std::vector<FLexToken> Tokes;

                unsigned char Pointers      = 0;
                unsigned char Reference     = 0;
                bool          IsFunction    = false;
                bool          IsArray       = 0;
                unsigned long ArraySize     = 0;

                while (Input.size() > 0 && (Input[0].Line == BToke.Line) && (Input[0].Value != "," && Input[0].Value != ";" && Input[0].Value != "="))
                {
                    Tokes.push_back(Input[0]);
                    if (Input[0].Value == "(")
                    {
                        auto T = FLexToken(Input[0]);
                        OC("parenthesis", "(", ")", Tokes, Input);

                        T.Value = ")";
                        T.Type = "parenthesisclosed";
                        Tokes.push_back(T);
                    } else
                        Input.erase(Input.begin());
                }

                for (auto Iterator = Tokes.begin(); Iterator != Tokes.end(); Iterator ++)
                {
                    FLexToken Toke = * (Iterator);

                    if (Toke.Type == "word" && Typ.size() == 0) {
                        Typ = Toke.Value;

                        FieldDeclaration.Token.Value = Typ;
                        FieldDeclaration.Children.push_back(FieldName);
                    } else if (Toke.Type == "word" && Typ.size() > 0) {
                        throw(std::runtime_error("field declaration contains two types. " + std::to_string(BToke.Line)));
                    }
                    else
                    if (Toke.Value == "[")
                    {
                        std::vector<FLexToken> Brackets;

                        auto It2 = Iterator + 1;
                        auto It3 = Iterator + 2;
                        IsArray = 1;

                        if (It2 == Tokes.end())
                        {
                            throw(std::runtime_error("array declaration takes an optional argument of 'Size: const Int'. at '" + std::to_string(FieldNameT.Line) + "'."));
                        }

                        bool Closed = false;

                        if (It2 == Tokes.end())
                        {
                            throw(std::runtime_error("array declaration takes an optional argument of 'Size: const Int'. at '" + std::to_string(FieldNameT.Line) + "'."));
                        }
                        if ((* It2).Type == "number")
                            ArraySize = std::stol((* It2).Value);
                        else
                        if ((* It2).Value == "]")
                        {
                            ArraySize = 0;
                            Closed = true;
                            Iterator = It2;
                        }
                        else
                        {
                            throw(std::runtime_error("array declaration takes an optional argument of 'Size: const Int'. at '" + std::to_string(FieldNameT.Line) + "'."));
                        }

                        if (!Closed)
                        {
                            if (It3 == Tokes.end() || (* It3).Value != "]")
                            {
                                throw(std::runtime_error("array bracket is never closed. at '" + std::to_string(FieldNameT.Line) + "'."));
                            }

                            Iterator = It3;
                        }
                    }
                    else if (Toke.Type == "and") {
                        Reference++;
                    } else if (Toke.Type == "const") {
                        FLexToken T = Toke;
                        T.Value = "is_const";
                        FieldDeclaration.Children.push_back(FToken(T));
                    } else if (Toke.Type == "multiply") {
                        Pointers++;
                    }
                    else {
                        throw(std::runtime_error("unknown symbol '" + Toke.Value + " (" + Toke.Type + ")" + "' in field declaration '" + FieldDeclaration.Token.Value + "'. " +
                                                 std::to_string(BToke.Line) + " " + std::to_string(Toke.Offset)));
                    }
                }

                FToken PP(FLexToken(std::to_string(Pointers), "is_pointer", FieldDeclaration.Token.Line));
                FToken RR(FLexToken(std::to_string(Reference), "is_reference", FieldDeclaration.Token.Line));
                FToken AA(FLexToken(std::to_string(ArraySize), "is_array", FieldDeclaration.Token.Line));

                if (Pointers > 0 && Reference > 0)
                {
                    throw(std::runtime_error("field cannot be a pointer and a reference '" + FieldDeclaration.Token.Value + "' in field declaration. " + std::to_string(BToke.Line)));
                }

                FieldDeclaration.Children.push_back(PP);
                FieldDeclaration.Children.push_back(RR);
                if (IsArray)
                    FieldDeclaration.Children.push_back(AA);


                if (Typ.size() == 0)
                {
                    throw(std::runtime_error("field declaration contains no types. " + std::to_string(BToke.Line)));
                }

                if (Input.size() > 0 && Input[0].Type == "equals")
                {
                    FToken EqualsToken(Input[0]);
                    Input.erase(Input.begin());

                    unsigned int line = EqualsToken.Token.Line;

                    std::vector<FLexToken> EqualTokes;

                    while (Input.size() > 0 && Input[0].Line == line)
                    {
                        EqualTokes.push_back(Input[0]);
                        Input.erase(Input.begin());
                    }

                    Parse(EqualsToken.Children, EqualTokes, true);
                    if (EqualTokes.size() == 0)
                        throw(std::runtime_error("field declaration has assignment but no value. " + std::to_string(BToke.Line)));

                    FieldDeclaration.Children.push_back(EqualsToken);
                }

                Tokens.push_back(FieldDeclaration);
            }
        }
        else
        if (IEQ("struct"))
        {
            FLexToken Struct = Input[0];
            Struct.Type     = "struct";
            Struct.Value    = "struct";
            Input.erase(Input.begin());

            if (RightHand)
                throw(std::runtime_error("struct declarations cannot be made on the right side of a statement. at '" + std::to_string(Struct.Line) + "'."));

            if (Input.size() == 0)
                throw(std::runtime_error("struct declaration is not valid, missing '::'. " + std::to_string(Struct.Line)));

            if (Input[0].Value != "::")
                throw(std::runtime_error("struct declaration is not valid, missing '::'. " + std::to_string(Struct.Line)));

            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("struct declaration has no name. " + std::to_string(Struct.Line)));

            if (Input[0].Type != "word")
                throw(std::runtime_error("struct declaration has no valid name. " + std::to_string(Struct.Line)));

            FLexToken Nam = Input[0];
            Nam.Type = "name";
            if (Nam.Value == "NULL" || Nam.Value == "struct" || Nam.Value == "defun" || Nam.Value == "fun" || Nam.Value == "subroutine")
                throw(std::runtime_error("'" + Nam.Value + "' is a reserved word. at '" + std::to_string(Nam.Line) + "'."));
            Input.erase(Input.begin());




            std::vector<FLexToken> BodyTokes;

            OC("braces", "{", "}", BodyTokes, Input);

            FToken Bdy(FLexToken("body", "body", 0));

            Parse(Bdy.Children, BodyTokes, false, false, false, false, true);

            FToken TheStruct(Struct);

            TheStruct.Children.push_back(Nam);
            TheStruct.Children.push_back(Bdy);

            //if there is a function inside the struct,
            //this point in code will not be reached due
            //to the function not having an "end" keyword.
            //Todo: make this error reachable.
            if (Bdy.HasType("fun"))
            {
                throw(std::runtime_error("structs cannot hold functions. " + std::to_string(Struct.Line)));
            }

            Tokens.push_back(TheStruct);
        }
        else
        if (Input[0].Value == "~")
        {
            FLexToken Fun = Input[0];
            Fun.Type    = "destructor";
            Fun.Value   = "";

            Input.erase(Input.begin());
            if (RightHand)
                throw(std::runtime_error("destructor function declarations cannot be made on the right side of a statement. at '" + std::to_string(Fun.Line) + "'."));

            if (Input.size() == 0)
                throw(std::runtime_error("destructor function declaration requires a typename. " + std::to_string(Fun.Line)));

            if (Input[0].Type != "word")
                throw(std::runtime_error("destructor function declaration requires a valid typename. " + std::to_string(Fun.Line)));

            Fun.Value = Input[0].Value;
            Input.erase(Input.begin());

            if (Input.size() == 0)
                throw(std::runtime_error("destructor function declaration has no parenthesis. " + std::to_string(Fun.Line)));

            if (Input[0].Value != "(")
                throw(std::runtime_error("destructor function declaration has no parenthesis. " + std::to_string(Fun.Line)));

            std::vector<FLexToken> Parenthesis;
            OC("parenthesis", "(", ")", Parenthesis, Input);
            if (Parenthesis.size() != 0)
                throw(std::runtime_error("destructor function declaration does not require any arguments. " + std::to_string(Fun.Line)));


            FToken Token(Fun);
            FToken ParenthesisToken(FLexToken("parenthesis", "parenthesis", Fun.Line));
            FToken FieldToken(FLexToken(Fun.Value, "field", Fun.Line));
            FToken FieldTokenName(FLexToken("A", "name", Fun.Line));
            FToken FieldTokenReference(FLexToken("1", "is_reference", Fun.Line));

            FieldToken.Children.push_back(FieldTokenName);
            FieldToken.Children.push_back(FieldTokenReference);
            ParenthesisToken.Children.push_back(FieldToken);
            std::vector<FLexToken> BodyTokes;

            OC("braces", "{", "}", BodyTokes, Input);

            FToken Bdy(FLexToken("body", "body", 0));

            Parse(Bdy.Children, BodyTokes, false, false, false, false, true);

            Token.Children.push_back(ParenthesisToken);
            Token.Children.push_back(Bdy);

            Tokens.push_back(Token);
        }
        else
        if (Input[0].Value == "operator")
        {
            FLexToken Fun = Input[0];
            Fun.Type    = "operator";
            Fun.Value   = "operator";

            Input.erase(Input.begin());
            if (RightHand)
            {
                throw(std::runtime_error("operator function declarations cannot be made on the right side of a statement. at '" + std::to_string(Fun.Line) + "'."));
            }

            if (Input.size() == 0)
            {
                throw(std::runtime_error("operator function declaration has no return type. " + std::to_string(Fun.Line)));
            }

            if (Input[0].Value != "::")
                throw(std::runtime_error("operator function declaration is not valid, missing '::'. " + std::to_string(Fun.Line)));
            Input.erase(Input.begin());

            FLexToken Nam("", "returns", Fun.Line);
            unsigned char   IsPointer   = 0;
            bool            IsConst     = 0;
            unsigned char   IsReference = 0;
            bool            IsTyped     = 0;

            while (Input.size() > 0 && Input[0].Value != "(")
            {
                if (Input[0].Value == "*")
                    IsPointer ++;
                else
                if (Input[0].Value == "const" && !IsConst)
                    IsConst = true;
                else
                if (Input[0].Value == "&")
                    IsReference ++;
                else
                if (Input[0].Type == "word" && !IsTyped)
                {
                    Nam = Input[0];
                    Nam.Type = "returns";
                    IsTyped = true;
                }
                else
                {
                    throw(std::runtime_error("token '" + Input[0].Value + " (" + Input[0].Type + ")" + "' is out of place (function declaration) at '" + std::to_string(Input[0].Line) + "'."));
                }

                Input.erase(Input.begin());
            }

            if (!IsTyped)
                throw(std::runtime_error("operator function declaration has no valid return type. at '" + std::to_string(Nam.Line) + "'."));

            FToken ReturnType(Nam);
            if (IsPointer > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsPointer), "is_pointer", Nam.Line)));
            if (IsConst)
                ReturnType.Children.push_back(FToken(FLexToken("is_const", "is_const", Nam.Line)));
            if (IsReference > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsReference), "is_reference", Nam.Line)));

            if (IsReference && IsPointer)
            {
                throw(std::runtime_error("'" + Nam.Value + "' cannot be both a pointer and a reference. at '" + std::to_string(Nam.Line) + "'."));
            }


            if (Input.size() == 0)
            {
                throw(std::runtime_error("operator function declaration has no parenthesis. " + std::to_string(Fun.Line)));
            }

            if (Input[0].Value != "(")
            {
                throw(std::runtime_error("operator function declaration has no parenthesis. " + std::to_string(Fun.Line)));
            }

            std::vector<FLexToken> Parenthesis;
            OC("parenthesis", "(", ")", Parenthesis, Input);

            if (Parenthesis.size() != 3)
            {
                throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>). " + std::to_string(Fun.Line)));
            }

            FToken TypeNameA    = Parenthesis[0];
            FToken Operator     = Parenthesis[1];
            FToken TypeNameB    = Parenthesis[2];

            if (Parenthesis[0].Type != "word" || !Operator.IsOperator() || Parenthesis[2].Type != "word")
            {
                throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>). " + std::to_string(Fun.Line)));
            }

            TypeNameA.Token.Type = "typename_a";
            TypeNameB.Token.Type = "typename_b";
            Operator.Token.Type = "operator";

            std::vector<FToken> Operators;
            if (Operator.Token.Value == "~")
            {
                if (Input.size() == 0)
                    throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>) and operator types in brackets [+ - ... * /]. " + std::to_string(Fun.Line)));
                if (Input[0].Value != "[")
                    throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>) and operator types in brackets [+ - ... * /]. " + std::to_string(Fun.Line)));
                std::vector<FLexToken> Brackets;
                OC("brackets", "[", "]", Brackets, Input);

                if (Brackets.size() == 0)
                    throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>) and operator types in brackets [+ - ... * /]. " + std::to_string(Fun.Line)));

                for (auto &X : Brackets)
                {
                    FToken OperatorToken(X);
                    if (!OperatorToken.IsOperator())
                        throw(std::runtime_error("operator function declaration takes 3 arguments (<Typename A> <Operator> <Typename B>) and operator types in brackets [+ - ... * /], symbol provided ('" + X.Value + "') is not an operator. " + std::to_string(Fun.Line)));
                    Operators.push_back(OperatorToken);
                }
            }
            else
                Operators.push_back(Operator);

            std::vector<FLexToken> BodyTokes;
            OC("braces", "{", "}", BodyTokes, Input);

            for (auto &Op : Operators)
            {
                FToken Token(Fun);
                FToken ParenthesisToken(FLexToken("parenthesis", "parenthesis", Fun.Line));
                FToken Field1(FLexToken(TypeNameA.Token.Value, "field", Fun.Line));
                FToken Field2(FLexToken(TypeNameB.Token.Value, "field", Fun.Line));

                Field1.Children.push_back(FToken(FLexToken("A", "name", Fun.Line)));
                Field1.Children.push_back(FToken(FLexToken("1", "is_reference", Fun.Line)));
                Field1.Children.push_back(FToken(FLexToken("is_const", "is_const", Fun.Line)));

                Field2.Children.push_back(FToken(FLexToken("B", "name", Fun.Line)));
                Field2.Children.push_back(FToken(FLexToken("1", "is_reference", Fun.Line)));
                Field2.Children.push_back(FToken(FLexToken("is_const", "is_const", Fun.Line)));
                ParenthesisToken.Children.push_back(Field1);
                ParenthesisToken.Children.push_back(Field2);

                Token.Children.push_back(ParenthesisToken);
                Token.Children.push_back(ReturnType);
                Token.Children.push_back(TypeNameA);
                Token.Children.push_back(TypeNameB);

                std::vector<FLexToken> FixedTokes;
                for (auto &Toke : BodyTokes)
                {
                    if (Toke.Value == "~")
                    {
                        FLexToken Toke2(Op.Token);
                        Toke2.Line = Toke.Line;

                        FixedTokes.push_back(Toke2);
                    }
                    else
                        FixedTokes.push_back(Toke);
                }

                Op.Token.Type = "operator";
                Token.Children.push_back(Op);
                FToken Bdy(FLexToken("body", "body", 0));
                Parse(Bdy.Children, FixedTokes, false, false, false, false, true);
                Token.Children.push_back(Bdy);

                Tokens.push_back(Token);
            }
        }
        else
        if (Input[0].Value == "clone")
        {
            FLexToken Fun = Input[0];
            Fun.Type    = "clone";
            Fun.Value   = "clone";

            Input.erase(Input.begin());
            if (RightHand)
            {
                throw(std::runtime_error("copy function declarations cannot be made on the right side of a statement. at '" + std::to_string(Fun.Line) + "'."));
            }

            if (Input.size() == 0)
            {
                throw(std::runtime_error("copy function declaration has no return type. " + std::to_string(Fun.Line)));
            }

            if (Input[0].Value != "::")
                throw(std::runtime_error("copy function declaration is not valid, missing '::'. " + std::to_string(Fun.Line)));
            Input.erase(Input.begin());

            FLexToken Nam("", "returns", Fun.Line);
            unsigned char   IsPointer   = 0;
            bool            IsConst     = 0;
            unsigned char   IsReference = 0;
            bool            IsTyped     = 0;

            while (Input.size() > 0 && Input[0].Value != "(")
            {
                if (Input[0].Value == "*")
                    IsPointer ++;
                else
                if (Input[0].Value == "const" && !IsConst)
                    IsConst = true;
                else
                if (Input[0].Value == "&")
                    IsReference ++;
                else
                if (Input[0].Type == "word" && !IsTyped)
                {
                    Nam = Input[0];
                    Nam.Type = "returns";
                    IsTyped = true;
                }
                else
                    throw(std::runtime_error("token '" + Input[0].Value + " (" + Input[0].Type + ")" + "' is out of place (function declaration) at '" + std::to_string(Input[0].Line) + "'."));

                Input.erase(Input.begin());
            }


            if (!IsTyped)
            {
                throw(std::runtime_error("copy function declaration has no valid return type. at '" + std::to_string(Nam.Line) + "'."));
            }


            FToken ReturnType(Nam);
            if (IsPointer > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsPointer), "is_pointer", Nam.Line)));
            if (IsConst)
                ReturnType.Children.push_back(FToken(FLexToken("is_const", "is_const", Nam.Line)));
            if (IsReference > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsReference), "is_reference", Nam.Line)));

            if (IsReference && IsPointer)
            {
                throw(std::runtime_error("'" + Nam.Value + "' cannot be both a pointer and a reference. at '" + std::to_string(Nam.Line) + "'."));
            }


            if (Input.size() == 0)
                throw(std::runtime_error("copy function declaration has no parenthesis. " + std::to_string(Fun.Line)));

            if (Input[0].Value != "(")
                throw(std::runtime_error("copy function declaration has no parenthesis. " + std::to_string(Fun.Line)));

            std::vector<FLexToken> Parenthesis;
            OC("parenthesis", "(", ")", Parenthesis, Input);

            if (Parenthesis.size() != 1)
                throw(std::runtime_error("copy function declaration takes 1 arguments (<Typename A>). " + std::to_string(Fun.Line)));

            FToken TypeNameA    = Parenthesis[0];

            if (Parenthesis[0].Type != "word")
                throw(std::runtime_error("copy function declaration takes 1 arguments (<Typename A>). " + std::to_string(Fun.Line)));

            TypeNameA.Token.Type = "typename_a";

            std::vector<FLexToken> BodyTokes;
            OC("braces", "{", "}", BodyTokes, Input);

                FToken Token(Fun);
                FToken ParenthesisToken(FLexToken("parenthesis", "parenthesis", Fun.Line));
                FToken Field1(FLexToken(TypeNameA.Token.Value, "field", Fun.Line));

                Field1.Children.push_back(FToken(FLexToken("A", "name", Fun.Line)));
                Field1.Children.push_back(FToken(FLexToken("1", "is_reference", Fun.Line)));
                Field1.Children.push_back(FToken(FLexToken("is_const", "is_const", Fun.Line)));

                ParenthesisToken.Children.push_back(Field1);

                Token.Children.push_back(ParenthesisToken);
                Token.Children.push_back(ReturnType);
                Token.Children.push_back(TypeNameA);

                FToken Bdy(FLexToken("body", "body", 0));
                Parse(Bdy.Children, BodyTokes, false, false, false, false, true);
                Token.Children.push_back(Bdy);

                Tokens.push_back(Token);
        }
        else
        if (Input.size() > 2 && Input[0].Type == "word" && Input[1].Value == "::")
        {
            FLexToken Fun = Input[0];
            Fun.Type = "fun";
            Input.erase(Input.begin());
            Input.erase(Input.begin());
            if (RightHand)
            {
                throw(std::runtime_error("function declarations cannot be made on the right side of a statement. at '" + std::to_string(Fun.Line) + "'."));
            }

            if (Input.size() == 0)
            {
                throw(std::runtime_error("function declaration has no return type. " + std::to_string(Fun.Line)));
            }

            FToken Func(Fun);
            FLexToken Nam = Input[0];
            if (Nam.Value == "NULL" || Nam.Value == "struct" || Nam.Value == "defun" || Nam.Value == "fun" || Nam.Value == "subroutine")
            {
                throw(std::runtime_error("'" + Nam.Value + "' is a reserved word. at '" + std::to_string(Nam.Line) + "'."));
            }

            unsigned char   IsPointer   = 0;
            bool            IsConst     = 0;
            unsigned char   IsReference = 0;
            bool            IsTyped     = 0;
            std::string ReturnSignature = "";

            while (Input.size() > 0 && Input[0].Value != "(")
            {
                if (Input[0].Value == "*")
                    IsPointer ++;
                else
                if (Input[0].Value == "const" && !IsConst)
                    IsConst = true;
                else
                if (Input[0].Value == "&")
                    IsReference ++;
                else
                if (Input[0].Type == "word" && !IsTyped)
                {
                    Nam = Input[0];
                    Nam.Type = "returns";
                    IsTyped = true;
                }
                else
                {
                    throw(std::runtime_error("token '" + Input[0].Value + " (" + Input[0].Type + ")" + "' is out of place (function declaration) at '" + std::to_string(Input[0].Line) + "'."));
                }

                ReturnSignature += Input[0].Value;

                Input.erase(Input.begin());
            }


            if (!IsTyped)
            {
                throw(std::runtime_error("function declaration has no valid return type. at '" + std::to_string(Nam.Line) + "'."));
            }

            FToken ReturnType(Nam);
            if (IsPointer > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsPointer), "is_pointer", Nam.Line)));
            if (IsConst)
                ReturnType.Children.push_back(FToken(FLexToken("is_const", "is_const", Nam.Line)));
            if (IsReference > 0)
                ReturnType.Children.push_back(FToken(FLexToken(std::to_string(IsReference), "is_reference", Nam.Line)));

            if (IsReference && IsPointer)
            {
                throw(std::runtime_error("'" + Nam.Value + "' cannot be both a pointer and a reference. at '" + std::to_string(Nam.Line) + "'."));
            }

            Func.Children.push_back(ReturnType);

            if (Input.size() == 0)
            {
                throw(std::runtime_error("function declaration has no parenthesis. " + std::to_string(Fun.Line)));
            }

            if (Input[0].Value != "(")
            {
                throw(std::runtime_error("function declaration has no parenthesis. " + std::to_string(Fun.Line)));
            }

            FLexToken ParenthesisToken = FLexToken(Input[0]);
            ParenthesisToken.Type = "parenthesis";
            ParenthesisToken.Value = "parenthesis";
            FToken Prn(ParenthesisToken);
            std::vector<FLexToken> Tokes;
            OC("parenthesis", "(", ")", Tokes, Input);
            Parse(Prn.Children, Tokes, false, true);
            std::string Signature = "";
            for (auto &Toke : Tokes)
                Signature += Toke.Value;
            Signature = ReturnSignature + "(" + Signature + ")";

            Func.Children.push_back(Prn);

            std::vector<FLexToken> BodyTokes;

            OC("braces", "{", "}", BodyTokes, Input);
            FToken Bdy(FLexToken("body", "body", 0));

            Parse(Bdy.Children, BodyTokes, false, false, false, false, true);

            Func.Children.push_back(Bdy);
            FToken SignatureToken(Nam);
            SignatureToken.Token.Value  = Signature;
            SignatureToken.Token.Type   = "signature";
            Func.Children.push_back(SignatureToken);

            Tokens.push_back(Func);
        }
        else
        if (Input[0].Type == "word")
        {
            FToken Identifier(Input[0]);
            Identifier.Token.Type = "identifier";
            Input.erase(Input.begin());

            if (Identifier.Token.Value == "return")
            {
                if (Input.size() > 0 && Input[0].Line == Identifier.Token.Line)
                {
                    std::vector<FLexToken> ToReturnTokes;

                    while (Input.size() > 0 && Input[0].Line == Identifier.Token.Line)
                    {
                        ToReturnTokes.push_back(Input[0]);
                        Input.erase(Input.begin());
                    }

                    Identifier.Token.Type = "return_value";

                    Parse(Identifier.Children, ToReturnTokes, true);

                    Tokens.push_back(Identifier);
                }
                else
                {
                    Identifier.Token.Type = "return";
                    Tokens.push_back(Identifier);
                }
            }
            else
            if (Input.size() > 0 && (IEQ("=") || IEQ("+=") || IEQ("-=") || IEQ("*=") || IEQ("/=") || IEQ("%=")))
            {
                FToken Operator(Input[0]);
                FToken OperatorRight(Input[0]);
                OperatorRight.Token.Type = "right";
                OperatorRight.Token.Value = "right";
                Input.erase(Input.begin());
//                        Operator.Children.push_back(Identifier);
                std::vector<FLexToken> Tokes;

                while (Input.size() > 0 && Input[0].Line == Operator.Token.Line)
                {
                    Tokes.push_back(Input[0]);
                    Input.erase(Input.begin());
                }

                if (Tokes.size() == 0)
                {
                    throw(std::runtime_error("cannot perform operation '" + Operator.Token.Value + "' without a right hand assignment."));
                }

                Parse(OperatorRight.Children, Tokes, true);

                Operator.Children.push_back(OperatorRight);

                if (Operator.Token.Value == "=")
                {
                    Identifier.Children.push_back(Operator);
                    Tokens.push_back(Identifier);
                }
                else
                {
                    Tokens.push_back(Identifier);
                    Tokens.push_back(Operator);
                }
            }
            else
            if (Input.size() > 0 && (IEQ("++") || IEQ("--")))
            {
                FToken Operator(Input[0]);
                Input.erase(Input.begin());
                Operator.Children.push_back(Identifier);

                Tokens.push_back(Operator);
            }
            else
            if (Input.size() > 0 && IEQ("("))
            {
                FToken Prn(Input[0]);
                std::vector<FLexToken> Tokes;
                OC("parenthesis", "(", ")", Tokes, Input);

                Identifier.Token.Type = "call";
                Parse(Prn.Children, Tokes, true, false, true);
                Identifier.Children.push_back(Prn);
                Tokens.push_back(Identifier);
            }
            else
                Tokens.push_back(Identifier);
        }
        else
        if (Input[0].Type == "number")
        {
            if (!RightHand)
            {
                throw(std::runtime_error("inputs cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }
            FToken Integer(Input[0]);
            Integer.Token.Type = "int";
            Tokens.push_back(Integer);
            Input.erase(Input.begin());
        }
        else
        if (Input[0].Type == "decimal")
        {
            if (!RightHand)
            {
                throw(std::runtime_error("inputs cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }
            FToken Decimal(Input[0]);
            Decimal.Token.Type = "double";
            Tokens.push_back(Decimal);
            Input.erase(Input.begin());
        }

        else
        if (IEQ("+") || IEQ("-") || IEQ("*") || IEQ("/") || IEQ("%") || IEQ(">") || IEQ("<") || IEQ(">=") || IEQ("<=") || IEQ("==") || IEQ("!="))
        {
            if (!RightHand)
            {
                throw(std::runtime_error("operators cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }
            FToken Operator(Input[0]);
            Tokens.push_back(Operator);
            Input.erase(Input.begin());
        }

        else
        if (IEQ("^"))
        {
            if (!RightHand)
            {
                throw(std::runtime_error("operators cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }
            FToken Operator(Input[0]);
            Input.erase(Input.begin());

            if (Input.size() == 0)
            {
                throw(std::runtime_error("cast operator '^' must be followed by a typename. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }

            if (Input[0].Type != "word" && Input[0].Value != "(")
            {
                throw(std::runtime_error("cast operator '^' must be followed by a valid typename. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            }

            if (Input[0].Value == "(")
            {
                std::vector<FLexToken> Tokes;
                OC("parenthesis", "(", ")", Tokes, Input);
                if (Tokes.size() == 0)
                {
                    throw(std::runtime_error("cast operator '^' must be followed by a valid typename. '" + Input[0].Value + "' at '" + std::to_string(Operator.Token.Line) + "'."));
                }

                std::string TypeName0 = "";
                for (auto &Token : Tokes)
                {
                    if (Token.Value != "*" && Token.Value != "&" && Token.Type != "word")
                    {
                        throw(std::runtime_error("cast operator '^' must be followed by a valid typename.  Unknown symbol found '" + Token.Value + "' at '" + std::to_string(Operator.Token.Line) + "'."));
                    }
                    TypeName0 += Token.Value;
                }

                FToken TypeName(Tokes[0]);
                TypeName.Token.Type = "typename";
                TypeName.Token.Value = TypeName0;

                Operator.Children.push_back(TypeName);
                Tokens.push_back(Operator);
            }
            else
            {
                FToken TypeName(Input[0]);
                Input.erase(Input.begin());
                TypeName.Token.Type = "typename";

                Operator.Children.push_back(TypeName);
                Tokens.push_back(Operator);
            }
        }

        else
        if (IEQ("=="))
        {
            if (!RightHand)
                throw(std::runtime_error("operators cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            FToken Operator(Input[0]);
            Tokens.push_back(Operator);
            Input.erase(Input.begin());
        }
        else
        if (IEQ("("))
        {
            FToken Parenthesis(Input[0]);
            Parenthesis.Token.Value = "parenthesis";
            Parenthesis.Token.Type  = "parenthesis";

            std::vector<FLexToken> PTokes;
            OC("parenthesis", "(", ")", PTokes, Input);
            Parse(Parenthesis.Children, PTokes, RightHand);
            Tokens.push_back(Parenthesis);
        }

        else
        if (IEQ(","))
        {
            if (!FuncDecl && !FuncCall && !AllowComma)
                throw(std::runtime_error("comma not allowed. at '" + std::to_string(Input[0].Line) + "'."));

            FToken Comma(Input[0]);
            if (FuncCall || AllowComma)
                Tokens.push_back(Comma);
            Input.erase(Input.begin());
        }
        else
        if (Input[0].Type == "string")
        {
            if (!RightHand)
                throw(std::runtime_error("inputs be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            FToken String(Input[0]);
            Tokens.push_back(String);
            Input.erase(Input.begin());
        }
        else
        if (Input[0].Type == "char")
        {
            if (!RightHand)
                throw(std::runtime_error("inputs cannot be used in the left hand of a statement. '" + Input[0].Value + "' at '" + std::to_string(Input[0].Line) + "'."));
            FToken Char(Input[0]);
            Char.Token.Value = std::to_string((unsigned int) Char.Token.Value[0]);
            Tokens.push_back(Char);
            Input.erase(Input.begin());
        }
        else
        if (IEQ(".") || IEQ(":") || IEQ("->"))
        {
            FToken Operator(Input[0]);
            Tokens.push_back(Operator);
            Input.erase(Input.begin());
        }

        else
            throw(std::runtime_error("token '" + Input[0].Value + " (" + Input[0].Type + ")" + "' is out of place at '" + std::to_string(Input[0].Line) + "'."));

        if (OnlyOnce)
            RearrangeTokens(Tokens, RightHand, InBody);
    }

    RearrangeTokens(Tokens, RightHand, InBody);
}
#undef IEQ
