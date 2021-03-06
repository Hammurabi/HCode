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

#ifndef RUGE_COMPILER_H
#define RUGE_COMPILER_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include "Tokens.h"

namespace HCode
{
    class FSymbolTable;
    class FScope;
    class FState;

    struct FOpcodeInstruction
    {
        std::string                             ValueString;
        std::vector<FOpcodeInstruction>         Children;

        explicit FOpcodeInstruction(std::string Value = "");
        FOpcodeInstruction(const FOpcodeInstruction &O);
        std::string ToString(unsigned int S = 0);
    };

    struct FType
    {
        std::string     Name;
        unsigned char   Pointer = 0;
        bool            Const = 0;
        unsigned char   Reference = 0;
        bool            Function = 0;
        bool            IsArray = 0;
        unsigned long   Array = 0;

        FType(const FType &O);
        FType(std::string N, unsigned char P = 0, bool C = false, unsigned char R = 0);
        FType(FToken Token);

        bool operator ==(const FType &O) const;
        unsigned int Size();
        std::string MovArg();
        bool IsPrimitive();
        std::string RealType();
    };

    FType MakeType(std::string Type);

    struct FAssembledFunction
    {
        //these SHOULD only be used once during runtime.
        //the VM should create EOpcode instructions instead
        //for faster execution.
        std::vector<FOpcodeInstruction>         Instructions;
        std::vector<unsigned char>              OpcodeInstructions;

        FAssembledFunction();
        FAssembledFunction(const FAssembledFunction &O);

        void Append(FOpcodeInstruction Opcode);
        std::string ToString(unsigned int T = 0);
    };

    class FAssembledScript;
    typedef void (*NativeCallback) (FState&, FSymbolTable&, FScope&, FAssembledScript &Script);

    struct FMethod
    {
        std::string         Name;
        FType               ReturnType;
        std::string         Signature;
        unsigned int        Line;
        unsigned int        FunctionAddress;
        NativeCallback      NativeCallback = 0;
        FAssembledFunction  AssembledFunction;
        bool                IsNative = false;

        FMethod();
        FMethod(const FMethod &O);

        void Append(FOpcodeInstruction OP);
    };

    struct FField
    {
        std::string     Name;
        std::string     Default;
        FType           Type;
        unsigned int    FieldAddress;

        FField();
        FField(const FField &O);
    };

    struct FLocalField
    {
        std::string     Name;
        FType           Type;
        unsigned int    FieldAddress;

        FLocalField();
        FLocalField(const FLocalField &O);

        bool IsPrimitive();

        std::string RealType();
    };

    struct FStruct
    {
        std::string     Name;
        std::string     Signature;
        unsigned int    Line;

        unsigned long   Size;
        unsigned int    StructAddress;

        std::map<std::string, FField> Fields;

        inline FStruct();
        inline FStruct(const FStruct &O);
    };

    struct FAssembledScript
    {
        std::vector<FOpcodeInstruction>         Instructions;
        std::map<std::string, FStruct>          Structs;
        std::map<std::string, FMethod>          Functions;
        std::map<std::string, FMethod>          Operators;
        std::map<std::string, FField>           Fields;
        std::vector<std::string>                Errors;
        FState                                  *State;
        FAssembledScript(FState *S = nullptr);
        FAssembledScript(const FAssembledScript &O);
    };

    struct FGlobalScope
    {
        std::map<std::string, FAssembledScript> Scripts;

        FGlobalScope();
        FGlobalScope(const FGlobalScope &O);
    };

    bool BigEndianMachine();
    void IntDecay(FOpcodeInstruction &Out, int Value);
    void LongDecay(FOpcodeInstruction &Out, long Value);
    void UndecayFW(unsigned char *Out, const unsigned char *Value);
    void UndecayQW(unsigned char *Out, const unsigned char *Value);
    void UndecayDW(unsigned char *Out, const unsigned char *Value);

    std::string Error(unsigned int Line, std::string Message);
    unsigned char ArithmeticCode(std::string Type, bool &IsPointer);
    std::string BaseType(std::string RealType);

    void CompileInstruction(FToken &Token, std::map<std::string, FLocalField> &LocalFieldMap, FAssembledScript &Script, FMethod &Function, std::stack<std::string> &TempStack, bool MemberAccess);
    void CompileFunction(FMethod &Function, FToken FunctionToken, FAssembledScript &Script);
    void Compile(std::vector<FToken> &Tokens, FGlobalScope &Scope, FAssembledScript &Script);
}

#endif //RUGE_COMPILER_H
