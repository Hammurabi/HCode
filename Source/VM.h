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

#ifndef HCODE_VM_H
#define HCODE_VM_H


#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <stack>
#include <iostream>

namespace HCode
{
    typedef long long   HCInteger;
    typedef double      HCFloat;

    union FPrimitive
    {
        HCInteger   Integer;
        HCFloat     Float;
    };

    class FObject;
    typedef std::shared_ptr<FObject> FHCObject;
    class FObject
    {
    public:
        inline FObject() {}
        inline virtual void         SetValue(FPrimitive Prim) {}
        inline virtual HCInteger    AsInt() {}
        inline virtual HCFloat      AsFloat() {}
        inline virtual FHCObject    GetField(unsigned int Field) { return nullptr; }
        inline virtual void         SetField(unsigned int Field, FHCObject Value) {}
        inline virtual std::string  ToString() { return ""; }
        inline virtual FHCObject    Copy() { return nullptr; }
        inline virtual void         Append(FHCObject Object) { }
        inline virtual void         Remove(FHCObject Object) { }
    private:
    };

    class FPrimitiveObject : public FObject
    {
    public:
        FPrimitiveObject(FPrimitive Prim) : Primitive(Prim) {}
        inline void                 SetValue(FPrimitive Prim) override { Primitive = Prim; }
        inline HCInteger            AsInt() override { return Primitive.Integer; }
        inline HCFloat              AsFloat() override { return Primitive.Float; }
        inline FHCObject            GetField(unsigned int Field) override { return nullptr; }
        inline void                 SetField(unsigned int Field, FHCObject Value) override { }
        inline std::string          ToString() override { return std::to_string(AsInt()); }
        inline FHCObject            Copy() override { return FHCObject(new FPrimitiveObject(FPrimitive(Primitive))); }
        inline void                 Append(FHCObject Object) override { }
    private:
        FPrimitive  Primitive;
    };

    class FPrimitiveObjectString : public FObject
    {
    public:
        inline FPrimitiveObjectString(std::string S) : String(S) {
        }
        inline void                 SetValue(FPrimitive Prim) override {}
        inline HCInteger            AsInt() override { return 0; }
        inline HCFloat              AsFloat() override { return 0.; }
        inline FHCObject            GetField(unsigned int Field) override { return nullptr; }
        inline void                 SetField(unsigned int Field, FHCObject Value) override {}
        inline std::string          ToString() override { return String; }
        inline FHCObject            Copy() override { return FHCObject(new FPrimitiveObjectString(std::string(String))); }
        inline void                 Append(FHCObject Object) override { }
    private:
        std::string String;
    };

    class FOOObject : public FObject
    {
    public:
        //TODO: should take FStruct*
        inline FOOObject(unsigned int Length) {
            for (unsigned int X = 0; X < Length; X ++)
                Fields.push_back(FHCObject(new FPrimitiveObject({0x0000000000000000})));
        }
        inline void         SetValue(FPrimitive Prim) override {}
        inline HCInteger    AsInt() override { return 0; }
        inline HCFloat      AsFloat() override { return 0.; }
        inline FHCObject    GetField(unsigned int Field) override { if (Field >= Fields.size()) return FHCObject(new FPrimitiveObject({0x0000000000000000})); return Fields[Field]; }
        inline void         SetField(unsigned int Field, FHCObject Value) override { if (Field >= Fields.size()) return; Fields[Field] = Value; }
        inline std::string  ToString() override { return "obj"; }
        inline void         Append(FHCObject Object) override { Fields.push_back(Object); }
        inline void         Remove(FHCObject Object) override { HCInteger Index = Object->AsInt(); if (Index < Fields.size()) Fields.erase(Fields.begin() + Index); }
    private:
        std::vector<FHCObject> Fields;
    };

    class FStack : public std::stack<FHCObject>
    {
    public:
        inline void Swap(unsigned long I)
        {
            std::vector<FHCObject> Objects;
            auto B = c[size() - I];
            unsigned long Index = size() - I;
            for (unsigned long X = Index; X < size() - 1; X ++)
                c[X] = c[X + 1];
            c[size() - 1] = B;
//            auto T = top();
//            auto B = c[size() - I];
//            c[size() - I] = T;
//            c[size() - 1] = B;
        }
    private:
        using std::stack<FHCObject>::c;
    };

    class FScope
    {
    public:
        inline FScope(std::string N, FStack &S, FScope *Parent = nullptr) : FuncName(N), Stack(S), ParentScope(Parent) {}
        inline void Cache(unsigned char L)
        {
            ElCache[L] = Pop();
        }
        inline void Uncache(unsigned char L)
        {
            Push(ElCache[L]);
        }
        inline void Push(FHCObject Object)
        {
            Stack.push(Object);
        }
        inline void Swap(unsigned long Index)
        {
            Stack.Swap(Index + 1);
        }

        inline FHCObject Peek()
        {
            return Stack.top();
        }

        inline FHCObject Pop() {
            auto Object = Stack.top();
            Stack.pop();
            return Object;
        }

        //TODO: Impl later.
        inline void Throw()
        {
        }

        inline void ThrowUncaught()
        {
        }

        inline FScope Child(std::string Name)
        {
            return FScope(Name, Stack, this);
        }

        inline unsigned long long Size()
        {
            return Stack.size();
        }
    private:
        std::string             FuncName;
        FScope                  *ParentScope;
        FStack                  &Stack;
        FHCObject               ElCache[256];
    };

    class FArgs
    {
    public:
        inline FArgs Append(FHCObject O)
        {
            Args.push_back(O);

            return * this;
        }
        inline std::vector<FHCObject> Get()
        {
            return Args;
        }
    private:
        std::vector<FHCObject> Args;
    };

    enum EOpcode
    {
        PUSH,
        FPUSH,
        PUSH0,
        POP,
        RETURN,
        ARRAY, //array <len> <data>

        CACHE, CACHE0, CACHE1, CACHE2, CACHE3, CACHE4,
        UNCACHE, UNCACHE0, UNCACHE1, UNCACHE2, UNCACHE3, UNCACHE4,
        CALL, //call <func>
        INVK, //call function (pop pointer from stack)


//        MOV, //mov <src> <dst> <type>
//        MOVWHS, //mov word heap to stack
//        MOVDHS, //mov dword heap to stack
//        MOVQHS, //mov qword heap to stack
//        MOVFHS, //mov fword heap to stack
//        MOVAHS, //mov ADDRS heap to stack
//
//        MOVWSH, //mov word stack to heap
//        MOVDSH, //mov dword stack to heap
//        MOVQSH, //mov qword stack to heap
//        MOVFSH, //mov fword stack to heap
//        MOVASH, //mov ADDRS stack to heap

        GETNTH, //Access an array[n], array could be of any type.
        SETNTH, //Access and set an array[n].
        NEWINSTANCE,


        IADD, ISUB, IDIV, IMUL, MOD, ICMPL, ICMPG, ICMPLE, ICMPGE,
        FADD, FSUB, FDIV, FMUL, FCMPL, FCMPG, FCMPLE, FCMPGE,
        EQUAL,
        NOTEQUAL,

        IF,
        ICAST,//cast to int
        FCAST,//cast to float
        DYNMK,//invoke a function dynamically (from function pointer on the stack)
        DUP,//duplicate the top element of the stack
        SWP,//duplicate the top element of the stack
        LOOP,//loop
        IFNOT_RETURN,//if the condition is not true, then break a loop
        JUMP,
    };

    struct FSymbolTable
    {
        std::map<std::string, unsigned int> Symbols;
        std::vector<FMethod>                Functions;
        std::vector<FStruct*>               Structs;

        inline void Insert(FMethod &Method)
        {
            if (Symbols.find(Method.Name) != Symbols.end())
                return;
            Symbols[Method.Name] = Functions.size();
            Functions.push_back(Method);
        }

        inline void Insert(FStruct *Struct)
        {
            if (Symbols.find(std::string(std::string("*t_") + Struct->Name)) != Symbols.end())
                return;
            Symbols[std::string(std::string("*t_") + Struct->Name)] = Structs.size();
            Structs.push_back(Struct);
        }

        inline std::map<std::string, unsigned int>::iterator FindStruct(std::string Name)
        {
            return Symbols.find(std::string(std::string("*t_") + Name));
        }
    };

    std::vector<unsigned char> GenerateInstructions(FAssembledFunction &Func, FSymbolTable &SymbolTable, FAssembledScript &Script);

    template <typename T1, typename T2>
    struct FTuple{
        T1 First;
        T2 Second;

        inline FTuple(T1 F, T2 S) : First(F), Second(S) {}
        inline FTuple(const FTuple<T1, T2> &O) : First(O.First), Second(O.Second) {}
    };



    class FState{
    public:
        inline FAssembledScript NewScript(std::string HumanReadable, FAssembledScript NativeData = FAssembledScript())
        {
            std::vector<FLexToken>  LexedTokens;
            std::vector<FToken>     ParsedTokens;

            Lex(LexedTokens, HumanReadable);
            Parse(ParsedTokens, LexedTokens);
            FAssembledScript Script(this);
            for (auto &Field : NativeData.Fields) Script.Fields[Field.first] = Field.second;
            for (auto &Funcn : NativeData.Functions) Script.Functions[Funcn.first] = Funcn.second;
            for (auto &Struc : NativeData.Structs) Script.Structs[Struc.first] = Struc.second;
            Compile(ParsedTokens, GlobalScope, Script);

            return Script;
        }
        inline void AddNativeLib(std::string Name, FAssembledScript NativeLib)
        {
            if (GlobalScope.Scripts.find(Name) != GlobalScope.Scripts.end())
                throw std::runtime_error("redeclaration of library '" + Name + "'.");
            GlobalScope.Scripts[Name] = NativeLib;
        }
        inline FHCObject MakeInt(HCInteger I)
        {
            FPrimitive Prim;
            Prim.Integer = I;
            return FHCObject(new FPrimitiveObject(Prim));
        }
        inline FHCObject MakeFloat(HCFloat F)
        {
            FPrimitive Prim;
            Prim.Float = F;
            return FHCObject(new FPrimitiveObject(Prim));
        }
        inline FHCObject MakeObject(unsigned int L)
        {
            return FHCObject(new FOOObject(L));
        }
        inline FHCObject MakeString(std::string String)
        {
            return FHCObject(new FPrimitiveObjectString(String));
        }

        inline FHCObject GetResult(std::string Function, FArgs Arguments)
        {
            auto Tuple = FindFunction(Function);

            if (Tuple.Second.Name != Function)
                return MakeInt(0);
            if (!Tuple.Second.IsNative && Tuple.Second.AssembledFunction.Instructions.size() == 0)
                return MakeInt(0);

            std::vector<unsigned char> Instructions = GenerateInstructions(Tuple.Second.AssembledFunction, Table, Tuple.First);


            FStack TheStack;
            FScope TheScope(Function, TheStack);

            for (auto Arg : Arguments.Get())
                TheScope.Push(Arg);

            return GetResult(Instructions, Tuple.First, TheScope);
        }

        inline FTuple<FAssembledScript, FMethod> FindFunction(std::string FuncName)
        {
//            auto SymbolLookup = Table.Functions.find(FuncName);
//            if (SymbolLookup != Table.Functions.end())
//                return SymbolLookup->second;
            for (auto &Script : GlobalScope.Scripts)
            {
                auto Find = Script.second.Functions.find(FuncName);
                if (Find != Script.second.Functions.end())
                {
                    Table.Insert(Find->second);
                    return FTuple<FAssembledScript, FMethod> (Script.second, Find->second);
                }
            }

            return FTuple<FAssembledScript, FMethod> (FAssembledScript(this), FMethod());
        }

        inline FHCObject GetResult(std::vector<unsigned char> Opcodes, FAssembledScript &AS, FScope &TheScope)
        {
            GetResultFromOps(Opcodes, 0, Opcodes.size(), AS, TheScope);

            if (TheScope.Size() > 0)
                return TheScope.Pop();

            return MakeInt(0);
        }

        inline void AddFunction(FMethod &Method)
        {
            Table.Insert(Method);
        }
        virtual void GetResultFromOps(std::vector<unsigned char> Opcodes, unsigned int InstructionBegin, unsigned int InstructionEnd, FAssembledScript &AS, FScope &FunScope, bool IsLoop = false);
    private:
        FGlobalScope GlobalScope;
        FSymbolTable Table;
    };

    void AddNatives(FState *State);
}


#endif //HCODE_VM_H
