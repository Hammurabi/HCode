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
 * Created by Abdullah Fadhil on 2019-10-10/11.
 */

#include <iostream>
#include "../Lexer.h"
#include "../Parser.h"
#include "../Compiler.h"
#include "../VM.h"
#include <vector>
#include <cmath>
#include <iostream>

#include <fstream>

std::string GetTest(int numArgs, const char ** args)
{
    std::ifstream file;
    std::string   data = "";

    file.open(args[numArgs - 1], std::ifstream::in);

    std::string str;
    while (std::getline(file, str))
        data += (str + "\n");

    return data;
}

int main(int numArgs, const char ** args) {
    using namespace HCode;

    std::string Test       = GetTest(numArgs, args);

    std::shared_ptr<FState> State(new FState());
    HCode::AddNatives(State.get());

    FMethod TestNative;
    TestNative.Name = "quick_incremental_loop";
    TestNative.IsNative = true;
    TestNative.NativeCallback = [] (FState& State, FSymbolTable& Table, FScope& Scope, FAssembledScript &Script) {
        auto Func = Scope.Pop();
        auto To = Scope.Pop();
        auto From = Scope.Pop();

        FScope NScope = Scope.Child(Table.Functions[Func->AsInt()].Name);
        auto X = Table.Functions[Func->AsInt()];

        if (!X.IsNative)
        {
            auto Instructions = HCode::GenerateInstructions(X.AssembledFunction, Table, Script);

            for (HCInteger i = From->AsInt(); i < To->AsInt(); i ++)
            {
                Scope.Push(State.MakeInt(i));
                State.GetResultFromOps(Instructions, 0, Instructions.size(), Script, NScope);
            }
        }
        else
        {
            for (HCInteger i = From->AsInt(); i < To->AsInt(); i ++)
            {
                Scope.Push(State.MakeInt(i));
                X.NativeCallback(State, Table, Scope, Script);
            }
        }
    };

    FAssembledScript NativeData;
    NativeData.Functions[TestNative.Name] = TestNative;


//    State->AddNativeLib("StringLib", NativeData);

    State->NewScript(Test);
    auto Result = State->GetResult("Main", FArgs());
    std::cout << Result->AsInt() << std::endl << std::endl << std::endl << std::endl;

#define IParams(x, y) long long IA = x; long long IB = y;
#define FParams(x, y) long double FA = x; long double FB = y;
//    for (int i = 0; i < 2; i ++)
//    {
//        IParams(5+i, 6+i)
//        FParams(5.+float(i), 6.+float(i))
//        assert(State->GetResult("ArithmeticIPlus", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA + IB));
//        assert(State->GetResult("ArithmeticISub", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA - IB));
//        assert(State->GetResult("ArithmeticIMul", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA * IB));
//        assert(State->GetResult("ArithmeticIDiv", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA / IB));
//        assert(State->GetResult("ArithmeticMod", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA % IB));
//
//        assert(State->GetResult("ArithmeticFPlus", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA + FB));
//        assert(State->GetResult("ArithmeticFSub", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA - FB));
//        assert(State->GetResult("ArithmeticFMul", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA * FB));
//        assert(State->GetResult("ArithmeticFDiv", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA / FB));
//    }
    return 0;
}