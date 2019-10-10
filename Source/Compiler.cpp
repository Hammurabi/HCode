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

#include "Compiler.h"

std::string HCode::FOpcodeInstruction::ToString(unsigned int S)
{
    std::string Spaces = "";
    std::string Result = ValueString + "\n";

    for (unsigned int I = 0; I < S; I ++)
        Spaces += "\t";

    std::string ChildrensResult = "";

    for (auto &Child : Children)
        ChildrensResult += Child.ToString(S + 1);

    return Spaces + Result + ChildrensResult;
}
HCode::FOpcodeInstruction::FOpcodeInstruction(const HCode::FOpcodeInstruction &O) : ValueString(O.ValueString), Children(O.Children) {}
HCode::FOpcodeInstruction::FOpcodeInstruction(std::string Value) : ValueString(Value) {}
bool HCode::BigEndianMachine()
{
    union {
        unsigned int i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}
void HCode::LongDecay(HCode::FOpcodeInstruction &Out, long Value)
{
    bool BigEndian  = BigEndianMachine();
    unsigned char *IntegerMap    = (unsigned char *) &Value;

    if (BigEndian)
        for (int i = 0; i < 8; i ++) Out.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
    else
        for (int i = 0; i < 8; i ++) Out.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[7 - i])));
}
void HCode::UndecayFW(unsigned char *Out, const unsigned char *Value)
{
    bool BigEndian  = BigEndianMachine();

    if (BigEndian)
        for (int i = 0; i < 8; i ++) Out[i] = Value[i];
    else
        for (int i = 0; i < 8; i ++) Out[i] = Value[7 - i];
}
void HCode::UndecayDW(unsigned char *Out, const unsigned char *Value)
{
    bool BigEndian  = BigEndianMachine();

    if (BigEndian)
        for (int i = 0; i < 2; i ++) Out[i] = Value[i];
    else
        for (int i = 0; i < 2; i ++) Out[i] = Value[1 - i];
}
std::string HCode::Error(unsigned int Line, std::string Message)
{
    return Message + std::string(" at '") + std::to_string(Line) + std::string("'.");
}

unsigned char HCode::ArithmeticCode(std::string Type, bool &IsPointer)
{
    if (Type.size() > 0 && Type[Type.size() - 1] == '*')
    {
        IsPointer = true;
        return 3;
    }
    else
    if (Type == "#unknown")
        return 0;
    else
    if (Type == "int" || Type == "long" || Type == "char")
        return 1;
    else
    if (Type == "float" || Type == "double")
        return 2;
    else
        return 4;
}

std::string HCode::BaseType(std::string RealType)
{
    std::string BType = "";

    for (unsigned char C = 0; C < RealType.size(); C ++)
    {
        if (RealType[C] == '*' || RealType[C] == '&' || RealType[C] == '[')
            return BType;

        BType += RealType[C];
    }

    return BType;
}

HCode::FType::FType(const HCode::FType &O) : Name(O.Name), Pointer(O.Pointer), Const(O.Const), Reference(O.Reference)
{
}

HCode::FType::FType(std::string N, unsigned char P, bool C, unsigned char R) : Name(N), Pointer(P), Const(C), Reference(R)
{
}

HCode::FType::FType(HCode::FToken Token)
{
    Name = Token.Token.Value;
    Const = Token.GetByType("is_const").Token.Value.size() > 0;
    Pointer = 0;
    Reference = 0;
    Function = 0;

    if (Token.GetByType("is_pointer").Token.Value.size() > 0)
        Pointer = std::stoi(Token.GetByType("is_pointer").Token.Value);
    if (Token.GetByType("is_reference").Token.Value.size() > 0)
        Pointer = std::stoi(Token.GetByType("is_reference").Token.Value);
    if (Token.GetByType("is_function").Token.Value.size() > 0)
        Function = true;
    if (Token.GetByType("is_function").Token.Value.size() > 0)
    {
        IsArray = 1;
        Array = std::stol(Token.GetByType("is_function").Token.Value);
    }
}

bool HCode::FType::operator==(const HCode::FType &O) const
{
    return O.Name == Name && O.Pointer == Pointer && Const == O.Const && Reference == O.Reference;
}

unsigned int HCode::FType::Size()
{
    if (Function || Pointer || Reference)
        return 8;

    if (Name == "word") { return 1; }
    else
    if (Name == "dword") { return 2; }
    else
    if (Name == "qword") { return 4; }
    else
    if (Name == "fword") { return 8; } //these should all be one else statement.
    else
    if (Name == "int") { return 8; } //these should all be one else statement.
    else
    if (Name == "float") { return 8; } //these should all be one else statement.
    else
        return 8;
}

std::string HCode::FType::MovArg()
{
    if (Function || Pointer || Reference)
        return "aword";

    if (Name == "word") { return "word"; }
    else
    if (Name == "dword") { return "dword"; }
    else
    if (Name == "qword") { return "qword"; }
    else
    if (Name == "fword") { return "fword"; } //these should all be one else statement.
    else
    if (Name == "int") { return "fword"; } //these should all be one else statement.
    else
    if (Name == "float") { return "fword"; } //these should all be one else statement.
    else
        return "aword";
}

bool HCode::FType::IsPrimitive()
{
    return (Function || Name == "word" || Name == "dword" || Name == "qword" || Name == "fword" || Name == "int" || Name == "float");
}

std::string HCode::FType::RealType()
{
    std::string P = "";
    std::string R = "";
    std::string A = "";
    //TODO: Add function pointer type


    for (int I = 0; I < Pointer; I ++) P += "*";
    for (int I = 0; I < Reference; I ++) R += "&";
    if (IsArray)
    {
        if (Array)
            A = "[" + std::to_string(Array) + "]";
        else
            A = "[]";
    }

    return Name + A + P + R;
}

HCode::FAssembledFunction::FAssembledFunction() {
}
HCode::FAssembledFunction::FAssembledFunction(const HCode::FAssembledFunction &O) : Instructions(O.Instructions), OpcodeInstructions(O.OpcodeInstructions) {}

void HCode::FAssembledFunction::Append(HCode::FOpcodeInstruction Opcode)
{
    Instructions.push_back(Opcode);
}

std::string HCode::FAssembledFunction::ToString(unsigned int T)
{
    std::string Result = "";

    for (auto &Instruction : Instructions)
        Result += Instruction.ToString(T);

    return Result;
}

HCode::FMethod::FMethod() : ReturnType("") {}

HCode::FMethod::FMethod(const HCode::FMethod &O) : Name(O.Name), ReturnType(O.ReturnType), Signature(O.Signature), Line(O.Line), FunctionAddress(O.FunctionAddress), NativeCallback(O.NativeCallback), AssembledFunction(O.AssembledFunction), IsNative(O.IsNative)
{
}

void HCode::FMethod::Append(HCode::FOpcodeInstruction OP)
{
    AssembledFunction.Append(OP);
}
HCode::FField::FField() : Type("")
{
}
HCode::FField::FField(const FField &O) : Name(O.Name), Type(O.Type), FieldAddress(O.FieldAddress)
{
}
HCode::FLocalField::FLocalField() : Type("") {}
HCode::FLocalField::FLocalField(const FLocalField &O) : Name(O.Name), Type(O.Type), FieldAddress(O.FieldAddress) {}
bool HCode::FLocalField::IsPrimitive()
{
    return Type.IsPrimitive();
}
std::string HCode::FLocalField::RealType()
{
    return Type.RealType();
}
HCode::FStruct::FStruct() {}
HCode::FStruct::FStruct(const HCode::FStruct &O) : Name(O.Name), Signature(O.Signature), Line(O.Line), Size(O.Size), StructAddress(O.StructAddress), Fields(O.Fields) {}

HCode::FGlobalScope::FGlobalScope() {
}

HCode::FGlobalScope::FGlobalScope(const FGlobalScope &O): Scripts(O.Scripts) {}

void HCode::CompileInstruction(HCode::FToken &Token, std::map<std::string, HCode::FLocalField> &LocalFieldMap,
                               HCode::FAssembledScript &Script, HCode::FMethod &Function,
                               std::stack<std::string> &TempStack, bool MemberAccess = false)
{
    if (Token.Token.Type == "return")
    {
        Function.Append(FOpcodeInstruction("return"));

        if (Function.ReturnType.RealType() != "void" && Function.ReturnType.RealType() != "nil" && Function.ReturnType.RealType() != "Nil" & Function.ReturnType.RealType() != "NIL")
            throw std::runtime_error(Error(Token.Token.Line, std::string("function '") + Function.Name + std::string("' has return type of '") + Function.ReturnType.RealType() + std::string("'.")));
    }
    else
    if (Token.Token.Type == "return_value")
    {
        for (auto &Child : Token.Children) CompileInstruction(Child, LocalFieldMap, Script, Function, TempStack);

        Function.Append(FOpcodeInstruction("return"));

        if (Function.ReturnType.RealType() == "void" || Function.ReturnType.RealType() == "nil" || Function.ReturnType.RealType() == "Nil" || Function.ReturnType.RealType() == "NIL")
            throw std::runtime_error(Error(Token.Token.Line, std::string("function '") + Function.Name + std::string("' has no return type.")));

        if (Function.ReturnType.RealType() != TempStack.top())
            throw std::runtime_error(Error(Token.Token.Line, std::string("function '") + Function.Name + std::string("' requires return type to be '") + Function.ReturnType.RealType() + std::string("'") + ", value provided is '" + TempStack.top() + "'."));

        TempStack.pop();
    }
    else
    if (Token.Token.Type == "comma") {
        for (auto &Child : Token.Children)
            CompileInstruction(Child, LocalFieldMap, Script, Function, TempStack);
    }
    else
    if (Token.Token.Type == "field") {
        FToken Equals = Token.GetByType("equals");

        FLocalField Field;
        Field.Type = Token.Token.Value;
        Field.Name = Token.GetByType("name").Token.Value;
        Field.FieldAddress = LocalFieldMap.size();

        FToken PointerToken = Token.GetByType("is_pointer");
        FToken ReferenceToken = Token.GetByType("is_reference");
        FToken FuncToken = Token.GetByType("is_function");
        FToken ArrayToken = Token.GetByType("is_array");

        if (PointerToken.Token.Value.size() > 0)
            Field.Type.Pointer = std::stoi(PointerToken.Token.Value);

        if (ArrayToken.Token.Value.size() > 0)
        {
            Field.Type.IsArray = true;
            Field.Type.Array = std::stol(ArrayToken.Token.Value);
        }

        if (FuncToken.Token.Value.size() > 0)
            Field.Type.Function = true;

        Field.Type.Const = (Token.GetByType("is_const").Token.Value.size() > 0);

        if (ReferenceToken.Token.Value.size() > 0)
            Field.Type.Reference = std::stoi(ReferenceToken.Token.Value);

        if (LocalFieldMap.find(Field.Name) != LocalFieldMap.end())
            throw std::runtime_error(Error(Token.Token.Line, std::string("redeclaration of field '") + Field.Name + std::string("'.")));

        if (Equals.Token.Value.size() == 0)
        {
            if (Field.IsPrimitive())
            {
                FOpcodeInstruction Push("push");
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Push.Children.push_back(FOpcodeInstruction("0"));
                Function.Append(Push);
            }
            else
            if (Field.Type.Function)
            {
            }
            else
            {
                auto Struct = Script.Structs.find(Field.Type.Name);
                if (Struct == Script.Structs.end())
                    throw std::runtime_error(Error(Token.Token.Line, std::string("field of type '") + Field.RealType() + std::string("' is not declared.")));

                if (Field.Type.Pointer)
                {
                }
                else
                {
                    FOpcodeInstruction CallFunc("stalloc");
                    LongDecay(CallFunc, Struct->second.Fields.size());
                    Function.Append(CallFunc);
                }
            }

            TempStack.push(Field.RealType());
        }

        LocalFieldMap[Field.Name] = Field;

        for (auto &Child : Equals.Children) CompileInstruction(Child, LocalFieldMap, Script, Function, TempStack);

        if (Field.RealType() != TempStack.top())
            throw std::runtime_error(Error(Token.Token.Line, std::string("field of type '") + Field.RealType() + std::string("' does not match assignment of '" + TempStack.top() + "'.")));

        TempStack.pop();

        switch (Field.FieldAddress)
        {
            case 0:     Function.Append(FOpcodeInstruction("cache0")); break;
            case 1:     Function.Append(FOpcodeInstruction("cache1")); break;
            case 2:     Function.Append(FOpcodeInstruction("cache2")); break;
            case 3:     Function.Append(FOpcodeInstruction("cache3")); break;
            case 4:     Function.Append(FOpcodeInstruction("cache4")); break;
            default:
                FOpcodeInstruction Cache = FOpcodeInstruction("cache");
                Cache.Children.push_back(FOpcodeInstruction(std::to_string((unsigned char) Field.FieldAddress)));
                Function.Append(Cache);
                break;
        }
    }
    else
    if (Token.Token.Type == "double")
    {
        FOpcodeInstruction Push("push");
        bool BigEndian              = BigEndianMachine();
        double Integer              = std::stod(Token.Token.Value);
        unsigned char *IntegerMap   = (unsigned char *) &Integer;

        if (BigEndian)
            for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
        else
            for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[7 - i])));
        Push.Children.push_back(FOpcodeInstruction(Token.Token.Value));
        Function.Append(Push);
        TempStack.push("float");
    }
    else
    if (Token.Token.Type == "int")
    {
        FOpcodeInstruction Push("push");
        bool BigEndian  = BigEndianMachine();
        long Integer    = std::stol(Token.Token.Value);
        unsigned char *IntegerMap    = (unsigned char *) &Integer;

        if (BigEndian)
            for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
        else
            for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[7 - i])));

        Function.Append(Push);
        TempStack.push("int");
    }
    else
    if (Token.Token.Type == "string")
    {
        FOpcodeInstruction Push("const_array");
        bool BigEndian  = BigEndianMachine();
        unsigned short Integer      = Token.Token.Value.size();
        unsigned char *IntegerMap   = (unsigned char *) &Integer;

        if (BigEndian)
            for (int i = 0; i < 2; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
        else
            for (int i = 0; i < 2; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[1 - i])));


//                for (unsigned int LIndex = 0; LIndex < Token.Token.Value.size(); LIndex ++)
//                    Push.Children.push_back(FOpcodeInstruction(std::to_string(Token.Token.Value[LIndex])));

        Push.Children.push_back(FOpcodeInstruction(Token.Token.Value));

        Function.Append(Push);
        TempStack.push("word*");
    }
    else
    if (Token.Token.Type == "right")
    {
        for (auto &Token0 : Token.Children)
            CompileInstruction(Token0, LocalFieldMap, Script, Function, TempStack);
    }
    else
    if (Token.Token.Type == "identifier")
    {
        if (MemberAccess)
        {
            if (TempStack.size() == 0)
                throw std::runtime_error(Error(Token.Token.Line, std::string("reference stack is empty, cannot find member '") + Token.Token.Value + std::string("'.")));

            std::string RealType = BaseType(TempStack.top());
            TempStack.pop();
            bool Pointer = false;

            if (ArithmeticCode(RealType, Pointer) != 4)
                throw std::runtime_error(Error(Token.Token.Line, std::string("cannot do a memberaccess operation on primitive type '") + RealType + "' to access member '" + Token.Token.Value + std::string("'.")));

            auto Result = Script.Structs.find(RealType);

            if (Result == Script.Structs.end())
                throw std::runtime_error(Error(Token.Token.Line, std::string("type '") + RealType + "' cannot be founded, cannot access member '" + Token.Token.Value + std::string("'.")));

            auto MemberResult = Result->second.Fields.find(Token.Token.Value);

            if (MemberResult == Result->second.Fields.end())
                throw std::runtime_error(Error(Token.Token.Line, std::string("reference to undeclared member field '") + Token.Token.Value + std::string("' in type '") + RealType + "'."));

            unsigned int Location = MemberResult->second.FieldAddress;

            if (Token.GetByType("equals").Token.Value.size() > 0)
            {
                for (auto &Token0 : Token.GetByType("equals").Children)
                    CompileInstruction(Token0, LocalFieldMap, Script, Function, TempStack);

                if (TempStack.size() == 0)
                    throw std::runtime_error(Error(Token.Token.Line, std::string("reference stack is empty, trying to assign value to '") + MemberResult->second.Name + std::string("'.")));
                if (MemberResult->second.Type.RealType() != TempStack.top())
                    throw std::runtime_error(Error(Token.Token.Line, std::string("field of type '") + MemberResult->second.Type.RealType() + std::string("' does not match assignment of '" + TempStack.top() + "'.")));
                TempStack.pop();
                bool X;

                FOpcodeInstruction MOV("setn");
//                        MOV.Children.push_back(FOpcodeInstruction("to_heap"));
//                        MOV.Children.push_back(MemberResult->second.Type.MovArg());
                LongDecay(MOV, Location);
                Function.Append(MOV);
            }
            else
            {
                FOpcodeInstruction MOV("getn");
//                        MOV.Children.push_back(FOpcodeInstruction("to_stack"));
//                        MOV.Children.push_back(MemberResult->second.Type.MovArg());
                LongDecay(MOV, Location);
                Function.Append(MOV);
                TempStack.push(MemberResult->second.Type.RealType());
            }
        }
        else
        {
            if (LocalFieldMap.find(Token.Token.Value) == LocalFieldMap.end())
                throw std::runtime_error(Error(Token.Token.Line, std::string("reference to undeclared variable '") + Token.Token.Value + std::string("'.")));

            FLocalField Field   = LocalFieldMap[Token.Token.Value];
            unsigned int Loc    = Field.FieldAddress;
            std::string RTyp    = Field.RealType();

            if (Token.GetByType("equals").Token.Value.size() > 0)
            {
                for (auto &Token0 : Token.GetByType("equals").Children)
                    CompileInstruction(Token0, LocalFieldMap, Script, Function, TempStack);


                if (TempStack.size() == 0)
                    throw std::runtime_error(Error(Token.Token.Line, std::string("reference stack is empty, trying to assign value to '") + Field.Name + std::string("'.")));
                if (RTyp != TempStack.top())
                    throw std::runtime_error(Error(Token.Token.Line, std::string("field of type '") + RTyp + std::string("' does not match assignment of '" + TempStack.top() + "'.")));

                TempStack.pop();

                switch (Loc)
                {
                    case 0:     Function.Append(FOpcodeInstruction("cache0")); break;
                    case 1:     Function.Append(FOpcodeInstruction("cache1")); break;
                    case 2:     Function.Append(FOpcodeInstruction("cache2")); break;
                    case 3:     Function.Append(FOpcodeInstruction("cache3")); break;
                    case 4:     Function.Append(FOpcodeInstruction("cache4")); break;
                    default:
                        FOpcodeInstruction Uncache = FOpcodeInstruction("cache");
                        Uncache.Children.push_back(FOpcodeInstruction(std::to_string((unsigned char) Loc)));
                        Function.Append(Uncache);
                        break;
                }
            }
            else
            {
                switch (Loc)
                {
                    case 0:     Function.Append(FOpcodeInstruction("uncache0")); break;
                    case 1:     Function.Append(FOpcodeInstruction("uncache1")); break;
                    case 2:     Function.Append(FOpcodeInstruction("uncache2")); break;
                    case 3:     Function.Append(FOpcodeInstruction("uncache3")); break;
                    case 4:     Function.Append(FOpcodeInstruction("uncache4")); break;
                    default:
                        FOpcodeInstruction Uncache = FOpcodeInstruction("uncache");
                        Uncache.Children.push_back(FOpcodeInstruction(std::to_string((unsigned char) Loc)));
                        Function.Append(Uncache);
                        break;
                }
            }

//                std::cout <<  Token.Token.Value.c_str() << " " << LocalFieldMap[Token.Token.Value].RealType().c_str() << std::endl;

            TempStack.push(LocalFieldMap[Token.Token.Value].RealType());
        }
    }
    else
    if (Token.Token.Type == "memberaccess")
    {
        unsigned int I = 0;
        for (auto &T : Token.Children)
            CompileInstruction(T, LocalFieldMap, Script, Function, TempStack, I ++);
    }
    else
    if (Token.IsOperator())
    {
        bool IsPointer = false;

        if (TempStack.size() < 2)
            throw std::runtime_error(Error(Token.Token.Line, std::string("operation '") + Token.Token.Value + " (" + Token.Token.Type + std::string(")' takes 2 arguments.")));

        std::string TypeNameA = TempStack.top();
        unsigned char TypeA = ArithmeticCode(TypeNameA, IsPointer);
        TempStack.pop();
        std::string TypeNameB = TempStack.top();
        unsigned char TypeB = ArithmeticCode(TypeNameB, IsPointer);
        TempStack.pop();

        std::string OperatorType = TypeNameB + Token.Token.Value + TypeNameA;
        std::string OperatorBaseType = BaseType(TypeNameB) + Token.Token.Value + BaseType(TypeNameA);

//                if (TypeA == 0 || TypeB == 0)
//                    throw std::runtime_error(Error(Token.Token.Line, std::string("cannot perform arithmetic operation '") + Token.Token.Value + " (" + Token.Token.Type + std::string("' on (unknown type). use cast to specify type.")));
//
//                if (TypeA != TypeB)
//                    throw std::runtime_error(Error(Token.Token.Line, std::string("cannot perform arithmetic operation '") + Token.Token.Value + " (" + Token.Token.Type + std::string("' on incompatible types '" + TypeNameA + "' and '" + TypeNameB + "'.")));
//
//                if ((TypeA != 1 || TypeB != 1) && IsPointer)
//                    throw std::runtime_error(Error(Token.Token.Line, std::string("cannot perform arithmetic operation '") + Token.Token.Value + " (" + Token.Token.Type + std::string("' on [WORD *] and [DOUBLE].")));

        if (Token.Token.Value == "==")
        {
            Function.Append(FOpcodeInstruction("equals"));
            return;
        }

        if ((TypeA == 3 && TypeB == 1) || (TypeB == 3 && TypeA == 1))
        {
            Function.Append(FOpcodeInstruction("i" + Token.Token.Type));
            if (TypeA == 3)
                TempStack.push(TypeNameA);
            else
                TempStack.push(TypeNameB);
        }
        else
        if (OperatorBaseType == "int" + Token.Token.Value + "int")
        {
            Function.Append(FOpcodeInstruction("i" + Token.Token.Type));
            TempStack.push("int");
        }
        else
        if (OperatorBaseType == "float" + Token.Token.Value + "float")
        {
            Function.Append(FOpcodeInstruction("f" + Token.Token.Type));
            TempStack.push("float");
        }
        else
        {
            auto Result = Script.Functions.find(OperatorType);
            if (Result != Script.Functions.end())
            {
                FOpcodeInstruction Call("call");
                Call.Children.push_back(FOpcodeInstruction(OperatorType));
                Function.Append(Call);

                TempStack.push(Result->second.ReturnType.RealType());
            }
            else
                throw std::runtime_error(Error(Token.Token.Line, std::string("operator '") + OperatorType + std::string("' is not overloaded.")));
        }



//                if (TypeA == 1)
//                    Function.Append(FOpcodeInstruction("i" + Token.Token.Type));
//                else
//                if (TypeA == 2)
//                    Function.Append(FOpcodeInstruction("f" + Token.Token.Type));
//                else
//                if (TypeA == 3)
//                {
//
//                    auto Result = Script.Functions.find(OperatorType);
//                    if (Result != Script.Functions.end())
//                    {
//                        FOpcodeInstruction Call("call");
//                        Call.Children.push_back(FOpcodeInstruction(OperatorType));
//                        Function.Append(Call);
//
//                        TempStack.push(Result->second.ReturnType.RealType());
//                    }
//                    else
//                        throw std::runtime_error(Error(Token.Token.Line, std::string("operator '") + OperatorType + std::string("' is not overloaded.")));
//                }

//                if (TypeA == 1)
//                    TempStack.push("int");
//                else if (TypeA == 2)
//                    TempStack.push("float");
    }
    else
    if (Token.Token.Type == "staticcast")
    {
        std::string TypeName = Token.GetByType("typename").Token.Value;
        if (TypeName.size() == 0)
            throw std::runtime_error(Error(Token.Token.Line, std::string("static_cast<T>() '") + Token.Token.Value + " (" + std::string(")' must take a typename.")));

        for (auto &Toke : Token.GetByType("parenthesis").Children)
            CompileInstruction(Toke, LocalFieldMap, Script, Function, TempStack);

        FOpcodeInstruction SCast("static_cast");
        SCast.Children.push_back(FOpcodeInstruction(TypeName));

        Function.Append(SCast);
        TempStack.pop();
        TempStack.push(TypeName);
    }
    else
    if (Token.Token.Type == "cast")
    {
        std::string TypeName = Token.GetByType("typename").Token.Value;
        if (TypeName.size() == 0)
            throw std::runtime_error(Error(Token.Token.Line, std::string("cast<T>() '") + Token.Token.Value + " (" + std::string(")' must take a typename.")));

        for (auto &Toke : Token.GetByType("parenthesis").Children)
            CompileInstruction(Toke, LocalFieldMap, Script, Function, TempStack);

        FOpcodeInstruction SCast("cast");
        SCast.Children.push_back(FOpcodeInstruction(TypeName));

        Function.Append(SCast);
        TempStack.pop();
        TempStack.push(TypeName);
    }
    else
    if (Token.Token.Type == "lambda")
    {
    }
    else
    if (Token.Token.Type == "parenthesis")
    {
        for (auto &Toke : Token.Children)
            CompileInstruction(Toke, LocalFieldMap, Script, Function, TempStack);
    }
    else
    if (Token.Token.Type == "statement")
    {
        for (auto &Toke : Token.Children)
            CompileInstruction(Toke, LocalFieldMap, Script, Function, TempStack);
    }
    else
    if (Token.Token.Type == "call")
    {
        std::string FunctionName = Token.Token.Value;

        bool CompilerSpecific = FunctionName == "throw" || FunctionName == "shrink_heap"  || FunctionName == "delete" || FunctionName == "print" || FunctionName == "gc" || FunctionName == "free" || FunctionName == "throw_uncaught" || FunctionName == "malloc" || FunctionName == "calloc" || FunctionName == "sizeof" || FunctionName == "assert" || FunctionName == "memcpy";

        if (!CompilerSpecific)
        {
            auto FindResult = Script.Functions.find(FunctionName);
            if (FindResult == Script.Functions.end())
                throw std::runtime_error(Error(Token.Token.Line, std::string("function '") + Token.Token.Value + " (" + std::string(")' could not be found.")));
        }

        if (FunctionName != "sizeof")
            for (auto &Toke : Token.GetByType("parenthesisopen").Children)
                CompileInstruction(Toke, LocalFieldMap, Script, Function, TempStack);

        if (CompilerSpecific)
        {
            FOpcodeInstruction FunCall("call");
            FunCall.Children.push_back(FOpcodeInstruction(FunctionName));

            if ( FunctionName == "assert" )
            {
                std::string FailedText  = "Assertion failed, line " + std::to_string(Token.Token.Line) + ".";

                FOpcodeInstruction Push("push");
                bool BigEndian  = BigEndianMachine();
                long Integer     = Token.Token.Line;
                unsigned char *IntegerMap    = (unsigned char *) &Integer;

                if (BigEndian)
                    for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
                else
                    for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[7 - i])));

                Function.Append(Push);
                Function.Append(FunCall);
                TempStack.push(Function.ReturnType.RealType());
            }
            else
            if ( FunctionName == "print" )
            {
                auto Tokes = Token.GetByType("parenthesisopen").Children;
//                        if (Tokes.size() != 1)
//                            throw std::runtime_error(Error(Token.Token.Line, std::string("print takes one argument.")));

                std::string Type = TempStack.top();
                TempStack.pop();

                if (Type == "int")
                    FunCall.Children[0].ValueString = "print_long";
                else
                if (Type == "float")
                    FunCall.Children[0].ValueString = "print_float";
                else
                if (Type == "word*")
                    FunCall.Children[0].ValueString = "print_string";
                else
                    FunCall.Children[0].ValueString = "print_address";

                TempStack.push("void");
                Function.Append(FunCall);
            }
            else
            if ( FunctionName == "sizeof" )
            {
                auto Tokes = Token.GetByType("parenthesisopen").Children;
                if (Tokes.size() != 1)
                    throw std::runtime_error(Error(Token.Token.Line, std::string("sizeof takes one argument.")));

                if (Tokes[0].Token.Type != "identifier")
                    throw std::runtime_error(Error(Token.Token.Line, std::string("sizeof takes one argument (TypeName).")));

                std::string Type = Tokes[0].Token.Value;
                FType TType(Type);
                long Integer    = 0;

                if (TType.IsPrimitive())
                    Integer = TType.Size();
                else
                {
                    auto Iterator = Script.Structs.find(Type);
                    if (Script.Structs.end() == Iterator)
                        throw std::runtime_error(Error(Token.Token.Line, std::string("provided typename '" + Type + "' does not exist.")));
                    Integer = Iterator->second.Size;
                }

                FOpcodeInstruction Push("push");
                bool BigEndian  = BigEndianMachine();
                unsigned char *IntegerMap    = (unsigned char *) &Integer;

                if (BigEndian)
                    for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[i])));
                else
                    for (int i = 0; i < 8; i ++) Push.Children.push_back(FOpcodeInstruction(std::to_string(IntegerMap[7 - i])));

                Function.Append(Push);
                TempStack.push("int");
            }
            else
            if ( FunctionName == "malloc" )
            {
                if (TempStack.size() == 0 || Token.GetByType("parenthesisopen").Children.size() == 0)
                    throw std::runtime_error(Error(Token.Token.Line, std::string("malloc takes one argument (Size).")));

                FType TType(TempStack.top());
                TempStack.pop();


                TempStack.push("void*");
                Function.Append(FunCall);
            }
            else
            if ( FunctionName == "calloc" )
            {
                if (TempStack.size() < 2 || Token.GetByType("parenthesisopen").Children.size() != 2)
                    throw std::runtime_error(Error(Token.Token.Line, std::string("calloc takes two arguments (Size, Clear).")));

                FType TType(TempStack.top());
                TempStack.pop();

                TempStack.push("void*");
                Function.Append(FunCall);
            } else
            {
                Function.Append(FunCall);
                TempStack.push(Function.ReturnType.RealType());
            }
        }
        else
        {
            FOpcodeInstruction FunCall("call");
            FunCall.Children.push_back(FOpcodeInstruction(FunctionName));
            Function.Append(FunCall);

            TempStack.push(Script.Functions[FunctionName].ReturnType.RealType());
        }
    }
    else
        throw std::runtime_error(Error(Token.Token.Line, std::string("unsupported token '") + Token.Token.Value + " (" + Token.Token.Type + std::string(")' reached.")));
}

void HCode::CompileFunction(HCode::FMethod &Function, HCode::FToken FunctionToken, HCode::FAssembledScript &Script)
{
    FToken                              Prnt = FunctionToken.GetByType("parenthesis");
    FToken                              Body = FunctionToken.GetByType("body");
    std::map<std::string, FLocalField>  LocalFields;
    unsigned int NumFields = 0;
    std::stack<std::string>             TempStack;

    unsigned int NumArguments           = Prnt.Children.size();
    unsigned int ArgLoc = 0;

    for (auto &Token : Prnt.Children)
    {
        if (Token.Token.Type != "field")
            throw std::runtime_error(Error(Token.Token.Line, std::string("token of type '") + Token.Token.Type + std::string("' is not allowed inside function parenthesis.")));

        FLocalField Field;
        Field.Type = Token.Token.Value;
        Field.Name = Token.GetByType("name").Token.Value;
        Field.FieldAddress = -- NumArguments;

        FToken PointerToken = Token.GetByType("is_pointer");
        FToken ReferenceToken = Token.GetByType("is_reference");
        FToken FuncToken = Token.GetByType("is_function");
        FToken ArrayToken = Token.GetByType("is_array");

        if (PointerToken.Token.Value.size() > 0)
            Field.Type.Pointer = std::stoi(PointerToken.Token.Value);

        if (ArrayToken.Token.Value.size() > 0)
        {
            Field.Type.IsArray = true;
            Field.Type.Array = std::stol(ArrayToken.Token.Value);
        }

        if (FuncToken.Token.Value.size() > 0)
            Field.Type.Function = true;

        Field.Type.Const = (Token.GetByType("is_const").Token.Value.size() > 0);

        if (ReferenceToken.Token.Value.size() > 0)
            Field.Type.Reference = std::stoi(ReferenceToken.Token.Value);

        if (LocalFields.find(Field.Name) != LocalFields.end())
            throw std::runtime_error(Error(Token.Token.Line, std::string("redeclaration of field '") + Field.Name + std::string("'.")));

        LocalFields[Field.Name] = Field;


        switch (ArgLoc ++)
        {
            case 0:     Function.Append(FOpcodeInstruction("cache0")); break;
            case 1:     Function.Append(FOpcodeInstruction("cache1")); break;
            case 2:     Function.Append(FOpcodeInstruction("cache2")); break;
            case 3:     Function.Append(FOpcodeInstruction("cache3")); break;
            case 4:     Function.Append(FOpcodeInstruction("cache4")); break;
            default:
                FOpcodeInstruction Cache = FOpcodeInstruction("cache");
                Cache.Children.push_back(FOpcodeInstruction(std::to_string((unsigned char) ArgLoc)));
                Function.Append(Cache);
                break;
        }
    }

    for (auto &Token : Body.Children)
        CompileInstruction(Token, LocalFields, Script, Function, TempStack);
}

void HCode::Compile(std::vector<HCode::FToken> &Tokens, HCode::FGlobalScope &Scope, HCode::FAssembledScript &Script)
{
    FMethod MallocFunc;
    FMethod CallocFunc;
    FMethod SizeOfFunc;
    FMethod ThrowFunc;
    FMethod ThrowUncaughtFunc;
    FMethod StaticCast;

    std::string Name = "";

    for (auto &Token : Tokens)
    {
        if (Token.Token.Type == "import")
        {
            std::string ToImport = Token.Children[0].Token.Value;
            auto Import = Scope.Scripts.find(ToImport);
            if (Import == Scope.Scripts.end())
                throw std::runtime_error("cannot find library '" + ToImport + "'. at '." + std::to_string(Token.Token.Line) + "'.");
            FAssembledScript IScript = Import->second;
            for (auto &F : IScript.Fields)
            {
                Script.Fields[F.first] = F.second;
            }
            for (auto &F : IScript.Structs)
            {
                Script.Structs[F.first] = F.second;
            }
            for (auto &F : IScript.Functions)
            {
                Script.Functions[F.first] = F.second;
            }
        }
    }

    for (auto &Token : Tokens)
    {
        if (Token.Token.Type == "scriptname")
            Name = Token.Token.Value;
        else
        if (Token.Token.Type == "struct")
        {
            FStruct Struct;

            Struct.Name = Token.GetByType("name").Token.Value;
            Struct.Signature = Struct.Name;
            Struct.Size = 0;

            if (Script.Structs.find(Struct.Name) != Script.Structs.end())
                throw std::runtime_error("redeclaration of type '" + Struct.Name + "'. at '." + std::to_string(Token.Token.Line) + "'.");

            FToken Body = Token.GetByType("body");

            std::vector<FToken> Fields;
            Body.GetByTypeRecursive("field", Fields);
            for (auto &ChildToken : Fields)
            {
                if (ChildToken.Token.Type == "field")
                {
                    FField Field;
                    Field.Type = ChildToken.Token.Value;
                    Field.Name = ChildToken.GetByType("name").Token.Value;

                    FToken PointerToken = ChildToken.GetByType("is_pointer");
                    FToken ReferenceToken = ChildToken.GetByType("is_reference");
                    FToken FuncToken = ChildToken.GetByType("is_function");
                    FToken ArrayToken = ChildToken.GetByType("is_array");

                    if (PointerToken.Token.Value.size() > 0)
                        Field.Type.Pointer = std::stoi(PointerToken.Token.Value);

                    if (ArrayToken.Token.Value.size() > 0)
                    {
                        Field.Type.IsArray = true;
                        Field.Type.Array = std::stol(ArrayToken.Token.Value);
                    }

                    if (FuncToken.Token.Value.size() > 0)
                        Field.Type.Function = true;

                    Field.Type.Const = (Token.GetByType("is_const").Token.Value.size() > 0);

                    if (ReferenceToken.Token.Value.size() > 0)
                        Field.Type.Reference = std::stoi(ReferenceToken.Token.Value);

                    Field.FieldAddress = Struct.Fields.size();//Struct.Size;

                    unsigned int FieldSize = Field.Type.Size();

                    Struct.Size += FieldSize;
                    if (Struct.Fields.find(Field.Name) != Struct.Fields.end())
                        throw std::runtime_error("redeclaration of field '" + Field.Name + "'. at '." + std::to_string(ChildToken.Token.Line) + "'.");
                    Struct.Fields[Field.Name] = (Field);
                }
            }

            Script.Structs[Struct.Name] = Struct;
        }
    }

    for (auto &Token : Tokens)
    {
        if (Token.Token.Type == "fun")
        {
            FMethod Function;
            Function.Name = Token.Token.Value;
            Function.ReturnType = Token.GetByType("returns").Token.Value;

            FToken PointerToken = Token.GetByType("returns").GetByType("is_pointer");
            FToken ReferenceToken = Token.GetByType("returns").GetByType("is_reference");
            FToken FuncToken = Token.GetByType("returns").GetByType("is_function");
            FToken ArrayToken = Token.GetByType("returns").GetByType("is_array");

            if (PointerToken.Token.Value.size() > 0)
                Function.ReturnType.Pointer = std::stoi(PointerToken.Token.Value);

            if (ArrayToken.Token.Value.size() > 0)
            {
                Function.ReturnType.IsArray = true;
                Function.ReturnType.Array = std::stol(ArrayToken.Token.Value);
            }

            if (FuncToken.Token.Value.size() > 0)
                Function.ReturnType.Function = true;

            Function.ReturnType.Const = (Token.GetByType("returns").GetByType("is_const").Token.Value.size() > 0);

            if (ReferenceToken.Token.Value.size() > 0)
                Function.ReturnType.Reference = std::stoi(ReferenceToken.Token.Value);

            Function.Signature = Token.GetByType("parenthesis").ToString();

            if (Script.Functions.find(Function.Name) != Script.Functions.end())
                throw std::runtime_error("redeclaration of function '" + Function.Name + "'. at '." + std::to_string(Token.Token.Line) + "'.");

            if (Function.Name.size() > 0)
                Script.Functions[Function.Name] = Function;
        }
        else
        if (Token.Token.Type == "destructor")
        {
            FMethod Function;
            Function.Name = "destroy~" + Token.Token.Value;
            Function.ReturnType.Name = "void";
            Function.Signature = Token.GetByType("parenthesis").ToString();

            if (Script.Functions.find(Function.Name) != Script.Functions.end())
                throw std::runtime_error("redeclaration of destructor '" + Function.Name + "'. at '." + std::to_string(Token.Token.Line) + "'.");

            if (Function.Name.size() > 0)
                Script.Functions[Function.Name] = Function;
        }
        else
        if (Token.Token.Type == "operator")
        {
            FMethod Function;
            Function.Name = Token.GetByType("typename_a").Token.Value + Token.GetByType("operator").Token.Value + Token.GetByType("typename_b").Token.Value;
            Function.ReturnType = Token.GetByType("returns").Token.Value;

            FToken PointerToken = Token.GetByType("returns").GetByType("is_pointer");
            FToken ReferenceToken = Token.GetByType("returns").GetByType("is_reference");
            FToken FuncToken = Token.GetByType("returns").GetByType("is_function");
            FToken ArrayToken = Token.GetByType("returns").GetByType("is_array");

            if (PointerToken.Token.Value.size() > 0)
                Function.ReturnType.Pointer = std::stoi(PointerToken.Token.Value);

            if (ArrayToken.Token.Value.size() > 0)
            {
                Function.ReturnType.IsArray = true;
                Function.ReturnType.Array = std::stol(ArrayToken.Token.Value);
            }

            if (FuncToken.Token.Value.size() > 0)
                Function.ReturnType.Function = true;

            Function.ReturnType.Const = (Token.GetByType("returns").GetByType("is_const").Token.Value.size() > 0);

            if (ReferenceToken.Token.Value.size() > 0)
                Function.ReturnType.Reference = std::stoi(ReferenceToken.Token.Value);

            Function.Signature = Token.GetByType("parenthesis").ToString();

            if (Script.Functions.find(Function.Name) != Script.Functions.end())
                throw std::runtime_error("redeclaration of function '" + Function.Name + "'. at '." + std::to_string(Token.Token.Line) + "'.");

            if (Function.Name.size() > 0)
                Script.Functions[Function.Name] = Function;
        }
    }

    for (auto &Token : Tokens)
    {
        if (Token.Token.Type == "fun")
        {
            FMethod &Function = Script.Functions[Token.Token.Value];
            CompileFunction(Function, Token, Script);
        }
        else
        if (Token.Token.Type == "operator")
        {
            std::string Name = Token.GetByType("typename_a").Token.Value + Token.GetByType("operator").Token.Value + Token.GetByType("typename_b").Token.Value;

            FMethod &Function = Script.Functions[Name];
            CompileFunction(Function, Token, Script);
        }
        else
        if (Token.Token.Type == "destructor")
        {
            std::string Name = "destroy~" + Token.Token.Value;

            FMethod &Function = Script.Functions[Name];
            CompileFunction(Function, Token, Script);
        }
    }

    if (Name.size() > 0)
    {
        if (Scope.Scripts.find(Name) != Scope.Scripts.end())
            throw std::runtime_error("redeclaration of library '" + Name + "'.");
        Scope.Scripts[Name] = Script;
    }
    else
        Scope.Scripts[("!cgen_" + Scope.Scripts.size())] = Script;
}

HCode::FAssembledScript::FAssembledScript(HCode::FState *S) : State(S) {
}

HCode::FAssembledScript::FAssembledScript(const FAssembledScript &O) : State(O.State), Instructions(O.Instructions), Structs(O.Structs), Functions(O.Functions), Operators(O.Operators), Fields(O.Fields)
{
}

