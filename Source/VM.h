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
    typedef long double HCFloat;

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
    private:
    };

    class FPrimitiveObject : public FObject
    {
    public:
        FPrimitiveObject(FPrimitive Prim) : Primitive(Prim) {}
        inline void        SetValue(FPrimitive Prim) override { Primitive = Prim; }
        inline HCInteger   AsInt() override { return Primitive.Integer; }
        inline HCFloat     AsFloat() override { return Primitive.Float; }
        inline FHCObject   GetField(unsigned int Field) override { return nullptr; }
        inline void SetField(unsigned int Field, FHCObject Value) override { }
        inline std::string  ToString() override { return std::to_string(AsInt()); }
    private:
        FPrimitive  Primitive;
    };

    class FPrimitiveObjectString : public FObject
    {
    public:
        inline FPrimitiveObjectString(std::string S) : String(S) {
        }
        inline void         SetValue(FPrimitive Prim) override {}
        inline HCInteger    AsInt() override { return 0; }
        inline HCFloat      AsFloat() override { return 0.; }
        inline FHCObject    GetField(unsigned int Field) override { return nullptr; }
        inline void         SetField(unsigned int Field, FHCObject Value) override {}
        inline std::string  ToString() override { return String; }
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
    private:
        std::vector<FHCObject> Fields;
    };

    class FScope
    {
    public:
        inline FScope(std::string N, std::stack<FHCObject> &S, FScope *Parent = nullptr) : FuncName(N), Stack(S), ParentScope(Parent) {}
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

        inline FHCObject Peek()
        {
            return Stack.top();
        }

        inline FHCObject Pop()
        {
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
        std::stack<FHCObject>   &Stack;
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

    std::vector<unsigned char> GenerateInstructions(FAssembledFunction Func, FSymbolTable &SymbolTable, FAssembledScript &Script);

    template <typename T1, typename T2>
    struct FTuple{
        T1 First;
        T2 Second;

        inline FTuple(T1 F, T2 S) : First(F), Second(S) {}
        inline FTuple(const FTuple<T1, T2> &O) : First(O.First), Second(O.Second) {}
    };

    class FState{
    public:
        inline FAssembledScript NewScript(std::string HumanReadable)
        {
            std::vector<FLexToken>  LexedTokens;
            std::vector<FToken>     ParsedTokens;

            Lex(LexedTokens, HumanReadable);
            Parse(ParsedTokens, LexedTokens);
            FAssembledScript Script(this);
            Compile(ParsedTokens, GlobalScope, Script);

            return Script;
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


            std::stack<FHCObject> TheStack;
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
            GetResultFromOps(Opcodes, AS, TheScope);

            if (TheScope.Size() > 0)
                return TheScope.Pop();

            return MakeInt(0);
        }

        inline void AddFunction(FMethod &Method)
        {
            Table.Insert(Method);
        }
        virtual void GetResultFromOps(std::vector<unsigned char> Opcodes, FAssembledScript &AS, FScope &FunScope);
    private:
        FGlobalScope GlobalScope;
        FSymbolTable Table;
    };
}


#endif //HCODE_VM_H
