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
 * Created by Abdullah Fadhil on 2019-10-10.
 */

#include "VM.h"
#include <iostream>
#include <cmath>

std::vector<unsigned char> HCode::GenerateInstructions(HCode::FAssembledFunction &Func, HCode::FSymbolTable &SymbolTable,
                                                       HCode::FAssembledScript &Script)
{
    if (Func.OpcodeInstructions.size() == 0)
    {
        for (auto &Instruction : Func.Instructions)
        {
            if ("push" == Instruction.ValueString)
            {
                if (Instruction.Children[0].ValueString == "0" && Instruction.Children[1].ValueString == "0" && Instruction.Children[2].ValueString == "0"
                    && Instruction.Children[3].ValueString == "0" && Instruction.Children[4].ValueString == "0" && Instruction.Children[5].ValueString == "0"
                    && Instruction.Children[6].ValueString == "0" && Instruction.Children[7].ValueString == "0")
                    Func.OpcodeInstructions.push_back(EOpcode::PUSH0);
                else
                {
                    Func.OpcodeInstructions.push_back(EOpcode::PUSH);
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[1].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[2].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[3].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[4].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[5].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[6].ValueString));
                    Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[7].ValueString));
                }
            }
            else
            if ("fpush" == Instruction.ValueString)
            {
                if (Instruction.Children[0].ValueString == "0.0" || Instruction.Children[0].ValueString == ".0" || Instruction.Children[0].ValueString == "0.")
                    Func.OpcodeInstructions.push_back(EOpcode::PUSH0);
                else
                {
                    Func.OpcodeInstructions.push_back(EOpcode::FPUSH);
                    HCFloat D = std::stod(Instruction.Children[0].ValueString);
                    if (BigEndianMachine())
                    {
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[7]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[6]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[5]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[4]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[3]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[2]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[1]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[0]));
                    }
                    else{
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[0]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[1]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[2]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[3]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[4]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[5]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[6]));
                        Func.OpcodeInstructions.push_back((((unsigned char *)(&D))[7]));
                    }
                }
            }
            else
            if ("const_array" == Instruction.ValueString)
            {
                Func.OpcodeInstructions.push_back(EOpcode::ARRAY);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[1].ValueString));
                for (unsigned int X = 0; X < Instruction.Children[2].ValueString.size(); X ++)
                    Func.OpcodeInstructions.push_back(Instruction.Children[2].ValueString[X]);
            }
            else
            if ("call" == Instruction.ValueString)
            {
                auto Symbol = SymbolTable.Symbols.find(Instruction.Children[0].ValueString);

                if (Symbol == SymbolTable.Symbols.end())
                {
                    auto Function = Script.Functions.find(Instruction.Children[0].ValueString);

                    if (Function == Script.Functions.end())
                        throw (std::runtime_error("symbol '" + Instruction.Children[0].ValueString + "' not found. \n" + Instruction.ToString()));
                    SymbolTable.Insert(Function->second);
                    Symbol = SymbolTable.Symbols.find(Instruction.Children[0].ValueString);
                }

                if (SymbolTable.Functions[Symbol->second].IsNative)
                    Func.OpcodeInstructions.push_back(EOpcode::INVK);
                else
                    Func.OpcodeInstructions.push_back(EOpcode::CALL);

                auto Decay = FOpcodeInstruction("decay");
                LongDecay(Decay, Symbol->second);
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[3].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[4].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[5].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[6].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[7].ValueString));
            }
            else
            if ("function_reference" == Instruction.ValueString)
            {
                auto Symbol = SymbolTable.Symbols.find(Instruction.Children[0].ValueString);

                if (Symbol == SymbolTable.Symbols.end())
                {
                    auto Function = Script.Functions.find(Instruction.Children[0].ValueString);

                    if (Function == Script.Functions.end())
                        throw (std::runtime_error("symbol '" + Instruction.Children[0].ValueString + "' not found. \n" + Instruction.ToString()));
                    SymbolTable.Insert(Function->second);
                    Symbol = SymbolTable.Symbols.find(Instruction.Children[0].ValueString);
                }

                Func.OpcodeInstructions.push_back(EOpcode::PUSH);

                auto Decay = FOpcodeInstruction("decay");
                LongDecay(Decay, Symbol->second);
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[3].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[4].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[5].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[6].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Decay.Children[7].ValueString));
            }
            else
            if ("stalloc" == Instruction.ValueString) {
                Func.OpcodeInstructions.push_back(EOpcode::NEWINSTANCE);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[3].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[4].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[5].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[6].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[7].ValueString));
            }
            else
            if ("setn" == Instruction.ValueString) {
                Func.OpcodeInstructions.push_back(EOpcode::SETNTH);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[3].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[4].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[5].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[6].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[7].ValueString));
            }
            else
            if ("getn" == Instruction.ValueString) {
                Func.OpcodeInstructions.push_back(EOpcode::GETNTH);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[3].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[4].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[5].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[6].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[7].ValueString));
            }
            else
            if ("if" == Instruction.ValueString) {
                Func.OpcodeInstructions.push_back(EOpcode::IF);
                FAssembledFunction IfFunc;
                IfFunc.Instructions = Instruction.Children[0].Children;
                auto Instructions = GenerateInstructions(IfFunc, SymbolTable, Script);
                unsigned int Len = Instructions.size();

                FOpcodeInstruction LenInstruction("len");
                IntDecay(LenInstruction, Len);

                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[3].ValueString));

                Func.OpcodeInstructions.insert(Func.OpcodeInstructions.end(), Instructions.begin(), Instructions.end());
            }
            else
            if ("if_loop" == Instruction.ValueString) {
                Func.OpcodeInstructions.push_back(EOpcode::LOOP);
                FAssembledFunction IfFunc;
                IfFunc.Instructions = Instruction.Children[0].Children;
                FAssembledFunction IfBody;
                IfBody.Instructions = Instruction.Children[1].Children;
                auto Instructions = GenerateInstructions(IfFunc, SymbolTable, Script);
                Instructions.push_back(EOpcode::IFNOT_RETURN);
                auto MoreInstrucs = GenerateInstructions(IfBody, SymbolTable, Script);
                unsigned int Len = Instructions.size() + MoreInstrucs.size();

                FOpcodeInstruction LenInstruction("len");
                IntDecay(LenInstruction, Len);

                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[0].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[1].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[2].ValueString));
                Func.OpcodeInstructions.push_back(std::stoi(LenInstruction.Children[3].ValueString));

                Func.OpcodeInstructions.insert(Func.OpcodeInstructions.end(), Instructions.begin(), Instructions.end());
                Func.OpcodeInstructions.insert(Func.OpcodeInstructions.end(), MoreInstrucs.begin(), MoreInstrucs.end());
            }
            else
            if ("iplus" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::IADD);
            else
            if ("iminus" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::ISUB);
            else
            if ("imultiply" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::IMUL);
            else
            if ("idivide" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::IDIV);
            else
            if ("imodulo" == Instruction.ValueString || "fmodulo" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::MOD);
            else
            if ("fplus" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FADD);
            else
            if ("fminus" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FSUB);
            else
            if ("fmultiply" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FMUL);
            else
            if ("fdivide" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FDIV);
            else
            if ("cache" == Instruction.ValueString)
            {
                Func.OpcodeInstructions.push_back(EOpcode::CACHE);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
            }
            else
            if ("cache0" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::CACHE0);
            else
            if ("cache1" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::CACHE1);
            else
            if ("cache2" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::CACHE2);
            else
            if ("cache3" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::CACHE3);
            else
            if ("cache4" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::CACHE4);
            else
            if ("uncache" == Instruction.ValueString)
            {
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
            }
            else
            if ("uncache0" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE0);
            else
            if ("uncache1" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE1);
            else
            if ("uncache2" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE2);
            else
            if ("uncache3" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE3);
            else
            if ("uncache4" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::UNCACHE4);
            else
            if ("return" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::RETURN);
            else
            if ("icmpg" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::ICMPG);
            else
            if ("icmpl" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::ICMPL);
            else
            if ("fcmpg" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FCMPG);
            else
            if ("fcmpl" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FCMPL);
            else
            if ("icmpge" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::ICMPGE);
            else
            if ("icmple" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::ICMPLE);
            else
            if ("fcmpge" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FCMPGE);
            else
            if ("fcmple" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::FCMPLE);
            else
            if ("equals" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::EQUAL);
            else
            if ("not_equals" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::NOTEQUAL);
            else
            if ("cast" == Instruction.ValueString)
            {
                if ("int" == Instruction.Children[0].ValueString)
                    Func.OpcodeInstructions.push_back(EOpcode::ICAST);
                else
                if ("float" == Instruction.Children[0].ValueString)
                    Func.OpcodeInstructions.push_back(EOpcode::FCAST);
            }
            else
            if ("invoke_dynamic" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::DYNMK);
            else
            if ("dup" == Instruction.ValueString)
                Func.OpcodeInstructions.push_back(EOpcode::DUP);
            else
            if ("swp" == Instruction.ValueString)
            {
                Func.OpcodeInstructions.push_back(EOpcode::SWP);
                Func.OpcodeInstructions.push_back(std::stoi(Instruction.Children[0].ValueString));
            }
            else
                throw (std::runtime_error("opcode not valid '" + Instruction.ValueString + "'."));
        }
    }
    return Func.OpcodeInstructions;
}

void HCode::AddNatives(HCode::FState *State)
{
    using namespace HCode;

    FMethod MakeCopy;
    MakeCopy.Name = "!clone";
    MakeCopy.IsNative = true;
    MakeCopy.NativeCallback = [] (FState &State, FSymbolTable &Table, FScope &Scope, FAssembledScript &Script) {
        Scope.Push(Scope.Pop()->Copy());
    };

    State->AddFunction(MakeCopy);


    FMethod PrintLong;
    PrintLong.Name = "print_long";
    PrintLong.IsNative = true;
    PrintLong.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        std::cout << Scope.Pop()->AsInt() << std::endl;
    };

    State->AddFunction(PrintLong);

    FMethod PrintFloat;
    PrintFloat.Name = "print_float";
    PrintFloat.IsNative = true;
    PrintFloat.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        std::cout << Scope.Pop()->AsFloat() << std::endl;
    };

    State->AddFunction(PrintFloat);

    FMethod PrintString;
    PrintString.Name = "print_string";
    PrintString.IsNative = true;
    PrintString.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        std::cout << Scope.Pop()->ToString().c_str() << std::endl;
    };

    State->AddFunction(PrintString);

    FMethod PrintObject;
    PrintObject.Name = "print_address";
    PrintObject.IsNative = true;
    PrintObject.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        std::cout << Scope.Pop()->ToString().c_str() << std::endl;
    };

    State->AddFunction(PrintObject);

    FMethod SqrtFloatObject;
    SqrtFloatObject.Name = "sqrt";
    SqrtFloatObject.IsNative = true;
    SqrtFloatObject.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeFloat(sqrt(Scope.Pop()->AsFloat())));
    };

    State->AddFunction(SqrtFloatObject);

    FMethod Sin;
    Sin.Name = "sin";
    Sin.IsNative = true;
    Sin.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeFloat(sin(Scope.Pop()->AsFloat())));
    };

    State->AddFunction(Sin);

    FMethod Cos;
    Cos.Name = "cos";
    Cos.IsNative = true;
    Cos.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeFloat(cos(Scope.Pop()->AsFloat())));
    };

    State->AddFunction(Cos);

    FMethod Tan;
    Tan.Name = "tan";
    Tan.IsNative = true;
    Tan.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeFloat(tan(Scope.Pop()->AsFloat())));
    };

    State->AddFunction(Tan);

    FMethod Pow;
    Pow.Name = "pow";
    Pow.IsNative = true;
    Pow.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto B = Scope.Pop();
        auto A = Scope.Pop();

        Scope.Push(State.MakeFloat(pow(A->AsInt(), B->AsInt())));
    };

    State->AddFunction(Pow);

    FAssembledScript StringLibrary;


    FMethod Concatenate;
    Concatenate.Name = "quick_concat";
    Concatenate.ReturnType.Name = "word";
    Concatenate.ReturnType.Pointer = 1;
    Concatenate.IsNative = true;
    Concatenate.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto B = Scope.Pop();
        auto A = Scope.Pop();

        Scope.Push(State.MakeString(A->ToString() + B->ToString()));
    };

    StringLibrary.Functions[Concatenate.Name] = Concatenate;


    FMethod FToString;
    FToString.Name = "fto_string";
    FToString.ReturnType.Name = "word";
    FToString.ReturnType.Pointer = 1;
    FToString.IsNative = true;
    FToString.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeString(std::to_string(Scope.Pop()->AsFloat())));
    };

    StringLibrary.Functions[FToString.Name] = FToString;


    FMethod IToString;
    IToString.Name = "to_string";
    IToString.ReturnType.Name = "word";
    IToString.ReturnType.Pointer = 1;
    IToString.IsNative = true;
    IToString.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeString(std::to_string(Scope.Pop()->AsInt())));
    };

    StringLibrary.Functions[IToString.Name] = IToString;


    FMethod STOD;
    STOD.Name = "stod";
    STOD.ReturnType.Name = "float";
    STOD.IsNative = true;
    STOD.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeFloat(std::stold(Scope.Pop()->ToString())));
    };

    StringLibrary.Functions[STOD.Name] = STOD;


    FMethod STOL;
    STOL.Name = "stoi";
    STOL.ReturnType.Name = "int";
    STOL.IsNative = true;
    STOL.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        Scope.Push(State.MakeInt(std::stoll(Scope.Pop()->ToString())));
    };

    StringLibrary.Functions[STOL.Name] = STOL;


    FMethod Substr;
    Substr.Name = "substr";
    Substr.ReturnType.Name = "word";
    Substr.ReturnType.Pointer = 1;
    Substr.IsNative = true;
    Substr.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        unsigned long long To = Scope.Pop()->AsInt();
        unsigned long long Fr = Scope.Pop()->AsInt();
        std::string TheString = Scope.Pop()->ToString();

        if (To > TheString.size())
            Scope.Push(State.MakeString(TheString));
        else
        if (Fr >= TheString.size())
            Scope.Push(State.MakeString(TheString));
        else
        {
            std::string SubString = "";

            for (unsigned long long X = Fr; X < To; X ++)
                SubString += TheString[X];

            Scope.Push(State.MakeString(SubString));
        }
    };

    StringLibrary.Functions[Substr.Name] = Substr;

    FStruct VectorType;
    VectorType.Name = "vector";
    FField VLength;
    VLength.Name = "length";
    VLength.Type = MakeType("int");
    VLength.FieldAddress = 0;
    VectorType.Fields["length"] = VLength;


    FField VPushBack;
    VPushBack.Name = "push_back";
    VPushBack.Type.Name = "void(Obj:void*)";
    VPushBack.Type.Function = true;
    VPushBack.FieldAddress = 1;
    VectorType.Fields["push_back"] = VPushBack;

    FField VErase;
    VErase.Name = "erase";
    VErase.Type.Name = "void(Index:int)";
    VErase.Type.Function = true;
    VErase.FieldAddress = 2;
    VectorType.Fields["erase"] = VErase;

    FField VGet;
    VGet.Name = "get";
    VGet.Type.Name = "void*(Index:int)";
    VGet.Type.Function = true;
    VGet.FieldAddress = 3;
    VectorType.Fields["get"] = VGet;

    VectorType.Size = VectorType.Fields.size() * 8;

    FMethod Vector;
    Vector.Name = "vector";
    Vector.ReturnType.Name = "vector";
    Vector.ReturnType.Pointer = 0;
    Vector.IsNative = true;
    Vector.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto Symbol = Table.Symbols.find("vector_push_back");
        if (Symbol == Table.Symbols.end())
        {
            auto Function = Script.Functions.find("vector_push_back");

            if (Function == Script.Functions.end())
                throw (std::runtime_error("symbol 'vector_push_back' not found. \n"));
            Table.Insert(Function->second);
            Symbol = Table.Symbols.find("vector_push_back");
        }
        auto Symbol2 = Table.Symbols.find("vector_erase");
        if (Symbol2 == Table.Symbols.end())
        {
            auto Function = Script.Functions.find("vector_erase");

            if (Function == Script.Functions.end())
                throw (std::runtime_error("symbol 'vector_erase' not found. \n"));
            Table.Insert(Function->second);
            Symbol2 = Table.Symbols.find("vector_erase");
        }
        auto Symbol3 = Table.Symbols.find("vector_get");
        if (Symbol3 == Table.Symbols.end())
        {
            auto Function = Script.Functions.find("vector_get");

            if (Function == Script.Functions.end())
                throw (std::runtime_error("symbol 'vector_get' not found. \n"));
            Table.Insert(Function->second);
            Symbol3 = Table.Symbols.find("vector_get");
        }
        Scope.Push(State.MakeObject(4));
        Scope.Peek()->SetField(0, State.MakeInt(0));
        Scope.Peek()->SetField(1, State.MakeInt(Symbol->second));
        Scope.Peek()->SetField(2, State.MakeInt(Symbol2->second));
        Scope.Peek()->SetField(3, State.MakeInt(Symbol3->second));
    };

    FMethod VectorPushBack;
    VectorPushBack.Name = "vector_push_back";
    VectorPushBack.ReturnType.Name = "void";
    VectorPushBack.ReturnType.Pointer = 0;
    VectorPushBack.IsNative = true;
    VectorPushBack.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto B = Scope.Pop();
        auto A = Scope.Pop();

        A->Append(B);
        A->GetField(0)->SetValue({A->GetField(0)->AsInt() + 1});
    };

    FMethod VectorErase;
    VectorErase.Name = "vector_erase";
    VectorErase.ReturnType.Name = "void";
    VectorErase.ReturnType.Pointer = 0;
    VectorErase.IsNative = true;
    VectorErase.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto B = Scope.Pop();
        auto A = Scope.Pop();

        if (A->GetField(0)->AsInt() > 0)
        {
            A->Remove(B);
            A->GetField(0)->SetValue({A->GetField(0)->AsInt() - 1});
        }
    };

    FMethod VectorGet;
    VectorGet.Name = "vector_get";
    VectorGet.ReturnType.Name = "void";
    VectorGet.ReturnType.Pointer = 0;
    VectorGet.IsNative = true;
    VectorGet.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto B = Scope.Pop();
        auto A = Scope.Pop();

        HCInteger Size = A->GetField(0)->AsInt();

        if (Size > 0)
        {
            HCInteger Index = B->AsInt();
            if (Index < Size)
                Scope.Push(A->GetField(4 + Index));
        }
    };

    FAssembledScript CollectionsLib;
    CollectionsLib.Structs[VectorType.Name] = VectorType;
    CollectionsLib.Functions[Vector.Name] = Vector;
    CollectionsLib.Functions[VectorPushBack.Name] = VectorPushBack;
    CollectionsLib.Functions[VectorErase.Name] = VectorErase;
    CollectionsLib.Functions[VectorGet.Name] = VectorGet;

    State->AddNativeLib("Collections", CollectionsLib);
    State->AddNativeLib("StringLib", StringLibrary);
}

//#define HC_OUTPUT_CODES

void HCode::FState::GetResultFromOps(std::vector<unsigned char> Instructions, unsigned int InstructionBegin, unsigned int InstructionEnd, HCode::FAssembledScript &Script, HCode::FScope &Scope, bool IsLoop) {

    unsigned char FWORD_REG[32];
    Looper:
    for (unsigned int Index = InstructionBegin; Index < InstructionEnd; Index ++)
    {
        EOpcode Opcode = static_cast<EOpcode>(Instructions[Index]);
        switch (Opcode)
        {
            case EOpcode ::PUSH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "PUSH" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'push'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Scope.Push(MakeInt( *((HCInteger *) FWORD_REG) ));
                Index += 8;
            }
                break;
            case EOpcode ::FPUSH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FPUSH" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'fpush'."));

                Scope.Push(MakeFloat( *((HCFloat *) (Instructions.data() + Index + 1)) ));
                Index += 8;
            }
                break;
            case EOpcode ::DUP:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "DUP" << std::endl;
#endif
                Scope.Push(Scope.Peek());
            }
                break;
            case EOpcode ::SWP:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "SWP" << std::endl;
#endif
                if (Index + 1 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'swp'."));

                Scope.Swap(Instructions[Index + 1]);
                Index ++;
            }
                break;
            case EOpcode ::PUSH0:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "PUSH0" << std::endl;
#endif
                Scope.Push(MakeInt( 0 ));
                break;
            case EOpcode::GETNTH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "GETNTH" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'getnth'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Scope.Push(Scope.Pop()->GetField(*((long *) FWORD_REG)));
                Index += 8;
            }
                break;
            case EOpcode::SETNTH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "SETNTH" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'getnth'."));
                auto V = Scope.Pop();
                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Scope.Pop()->SetField(*((long *) FWORD_REG), V);
                Index += 8;
            }
                break;
            case EOpcode::IF:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "IF" << std::endl;
#endif
                if (Index + 4 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'if'."));
                UndecayQW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 4;

                if (Scope.Pop()->AsInt() == 0)
                    Index += *((unsigned int *) FWORD_REG);
            }
                break;
            case EOpcode::IFNOT_RETURN:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "IFNOT_RETURN" << std::endl;
#endif
                if (Scope.Pop()->AsInt() == 0)
                    return;
            }
                break;
            case EOpcode::LOOP:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "LOOP" << std::endl;
#endif
                if (Index + 4 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'loop'."));
                UndecayQW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 4;
                GetResultFromOps(Instructions, Index + 1, Index + *((unsigned int *) FWORD_REG) + 1, Script, Scope, true);
                Index += *((unsigned int *) FWORD_REG);
            }
                break;
            case EOpcode::NEWINSTANCE:
            {
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'setnth'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 8;

                Scope.Push(MakeObject(*((long *) FWORD_REG)));
            }
                break;
            case EOpcode ::POP:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "POP" << std::endl;
#endif
                Scope.Pop();
                break;
            case EOpcode ::RETURN:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "RETURN" << std::endl;
#endif
                return;
            case EOpcode ::ARRAY:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ARRAY" << std::endl;
#endif
                if (Index + 2 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'array'."));

                UndecayDW(FWORD_REG, Instructions.data() + Index + 1);
                unsigned short ALength = *((unsigned short *) FWORD_REG);
                Index += 2;

                if ((Index + ALength) >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'array'."));
                std::string S = "";
                for (unsigned short X = 0; X < ALength; X ++)
                    S += Instructions[Index + 1 + X];
                Index += ALength;
                Scope.Push(MakeString(S));
            }
                break;
            case EOpcode ::CACHE0:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CACHE0" << std::endl;
#endif
                Scope.Cache(0);
                break;
            case EOpcode ::CACHE1:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CACHE1" << std::endl;
#endif
                Scope.Cache(1);
                break;
            case EOpcode ::CACHE2:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CACHE2" << std::endl;
#endif
                Scope.Cache(2);
                break;
            case EOpcode ::CACHE3:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CACHE3" << std::endl;
#endif
                Scope.Cache(3);
                break;
            case EOpcode ::CACHE4:
                Scope.Cache(4);
                break;
            case EOpcode ::CACHE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CACHE" << std::endl;
#endif
                Scope.Cache(Instructions[Index + 1]);
                Index ++;
                break;
            case EOpcode ::UNCACHE0:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE0" << std::endl;
#endif
                Scope.Uncache(0);
                break;
            case EOpcode ::UNCACHE1:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE1" << std::endl;
#endif
                Scope.Uncache(1);
                break;
            case EOpcode ::UNCACHE2:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE2" << std::endl;
#endif
                Scope.Uncache(2);
                break;
            case EOpcode ::UNCACHE3:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE3" << std::endl;
#endif
                Scope.Uncache(3);
                break;
            case EOpcode ::UNCACHE4:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE4" << std::endl;
#endif
                Scope.Uncache(4);
                break;
            case EOpcode ::UNCACHE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "UNCACHE" << std::endl;
#endif
                Scope.Uncache(Instructions[Index + 1]);
                Index ++;
                break;
            case EOpcode ::CALL:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "CALL" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'call'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 8;

                FScope NScope = Scope.Child(Table.Functions[*((long *) FWORD_REG)].Name);

                auto &X = Table.Functions[*((long *) FWORD_REG)].AssembledFunction;
                auto Instructions = GenerateInstructions(X, Table, Script);
                GetResultFromOps(Instructions, 0, Instructions.size(), Script, NScope);
//                        NScope.DeleteScope(Heap);
//                        Heap.CycleGC();
            }
                break;
            case EOpcode ::INVK:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "INVK" << std::endl;
#endif
                if (Index + 8 >= InstructionEnd)
                    throw (std::runtime_error("corrupt instruction set provided. 'invk'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 8;

                FScope NScope = Scope.Child(Table.Functions[*((long *) FWORD_REG)].Name);
                Table.Functions[*((long *) FWORD_REG)].NativeCallback(*this, Table, NScope, Script);
            }
                break;
            case EOpcode ::DYNMK:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "DYNMK" << std::endl;
#endif
                auto Func = Table.Functions[Scope.Pop()->AsInt()];
                FScope NScope = Scope.Child(Func.Name);
                if (Func.IsNative)
                Func.NativeCallback(*this, Table, NScope, Script);
                else
                {
                    auto Instructions = GenerateInstructions(Func.AssembledFunction, Table, Script);
                    GetResultFromOps(Instructions, 0, Instructions.size(), Script, NScope);
                }
            }
                break;
    #define AOPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeInt(A->AsInt() x B->AsInt()));}
                case EOpcode ::IADD:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "IADD" << std::endl;
#endif
                    AOPEXE(+)
                    break;
                case EOpcode ::ISUB:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ISUB" << std::endl;
#endif
                    AOPEXE(-)
                    break;
                case EOpcode ::IDIV:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "IDIV" << std::endl;
#endif
                AOPEXE(/)
                break;
                case EOpcode ::IMUL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "IMUL" << std::endl;
#endif
                AOPEXE(*)
                    break;
                case EOpcode ::MOD:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "MOD" << std::endl;
#endif
                AOPEXE(%)
                    break;
                case EOpcode ::ICMPL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ICMPL" << std::endl;
#endif
                AOPEXE(<)
                    break;
                case EOpcode ::ICMPG:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ICMPG" << std::endl;
#endif
                AOPEXE(>)
                    break;
                case EOpcode ::ICMPLE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ICMPLE" << std::endl;
#endif
                AOPEXE(<=)
                    break;
                case EOpcode ::ICMPGE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ICMPGE" << std::endl;
#endif
                AOPEXE(>=)
                    break;
    #undef AOPEXE
    #define AOPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeFloat(A->AsFloat() x B->AsFloat()));}
    #define AIPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeInt(A->AsFloat() x B->AsFloat()));}
                case EOpcode ::FADD:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FADD" << std::endl;
#endif
                AOPEXE(+)        break;
                case EOpcode ::FSUB:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FSUB" << std::endl;
#endif
                AOPEXE(-)        break;
                case EOpcode ::FDIV:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FDIV" << std::endl;
#endif
                AOPEXE(/)        break;
                case EOpcode ::FMUL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FMUL" << std::endl;
#endif
                AOPEXE(*)        break;
                case EOpcode ::FCMPL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FCMPL" << std::endl;
#endif
                AIPEXE(<)        break;
                case EOpcode ::FCMPG:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FCMPG" << std::endl;
#endif
                AIPEXE(>)        break;
                case EOpcode ::FCMPLE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FCMPLE" << std::endl;
#endif
                AIPEXE(<=)        break;
                case EOpcode ::FCMPGE:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FCMPGE" << std::endl;
#endif
                AIPEXE(>=)        break;
            case EOpcode ::EQUAL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "EQUAL" << std::endl;
#endif
                Scope.Push(MakeInt(Scope.Pop()->AsInt() == Scope.Pop()->AsInt())); break;
            case EOpcode ::NOTEQUAL:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "NOTEQUAL" << std::endl;
#endif
                Scope.Push(MakeInt(Scope.Pop()->AsInt() != Scope.Pop()->AsInt())); break;
    #undef AOPEXE
            case EOpcode ::ICAST:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "ICAST" << std::endl;
#endif
                Scope.Push(MakeInt(Scope.Pop()->AsFloat()));
                break;
            case EOpcode ::FCAST:
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FCAST" << std::endl;
#endif
                Scope.Push(MakeFloat(Scope.Pop()->AsInt()));
                break;
        }
    }

    if (IsLoop)
        goto Looper;
}
