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
#include <iostream>

void AddNatives(std::shared_ptr<HCode::FState> State)
{
    using namespace HCode;

    FMethod PrintLong;
    PrintLong.Name = "print_long";
    PrintLong.IsNative = true;
    PrintLong.NativeCallback = [] (FState& State, const FSymbolTable& Table, FScope& Scope) {
        std::cout << Scope.Pop()->AsInt() << std::endl;
    };

    State->AddFunction(PrintLong);

    FMethod PrintFloat;
    PrintFloat.Name = "print_float";
    PrintFloat.IsNative = true;
    PrintFloat.NativeCallback = [] (FState& State, const FSymbolTable& Table, FScope& Scope) {
        std::cout << Scope.Pop()->AsFloat() << std::endl;
    };

    State->AddFunction(PrintFloat);

    FMethod PrintString;
    PrintString.Name = "print_string";
    PrintString.IsNative = true;
    PrintString.NativeCallback = [] (FState& State, const FSymbolTable& Table, FScope& Scope) {
        std::cout << Scope.Pop()->ToString().c_str() << std::endl;
    };

    State->AddFunction(PrintString);

    FMethod PrintObject;
    PrintObject.Name = "print_address";
    PrintObject.IsNative = true;
    PrintObject.NativeCallback = [] (FState& State, const FSymbolTable& Table, FScope& Scope) {
        std::cout << Scope.Pop()->ToString().c_str() << std::endl;
    };

    State->AddFunction(PrintObject);
}

int main() {
    using namespace HCode;

    std::string Test       = ""
                             "ArithmeticIPlus::int(A: int, B: int)\n"
                             "Res: int = A + B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticISub::int(A: int, B: int)\n"
                             "Res: int = A - B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticIMul::int(A: int, B: int)\n"
                             "Res: int = A * B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticIDiv::int(A: int, B: int)\n"
                             "Res: int = A / B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticMod::int(A: int, B: int)\n"
                             "Res: int = A % B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"

                             "ArithmeticFPlus::float(A: float, B: float)\n"
                             "Res: float = A + B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticFSub::float(A: float, B: float)\n"
                             "Res: float = A - B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticFMul::float(A: float, B: float)\n"
                             "Res: float = A * B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             "ArithmeticFDiv::float(A: float, B: float)\n"
                             "Res: float = A / B\n"
                             "print(Res)\n"
                             "return Res\n"
                             "end\n"
                             ""
                             "struct FStruct\n"
                             "Field1: int\n"
                             "Field2: int\n"
                             "end\n"
                             ""
                             "operator FStruct(FStruct + int)\n"
                             "Result: FStruct\n"
                             "Result.Field1 = A.Field1 + B\n"
                             "Result.Field2 = A.Field2 + B\n"
                             "return Result\n"
                             "end\n"
                             ""
                             "Main:: void()\n"
                             "TestS: FStruct\n"
                             "TestS.Field1 = 12\n"
                             "TestS.Field2 = 22\n"
                             "Test1: FStruct = TestS + 5\n"
                             "print(TestS.Field1)\n"
                             "print(TestS.Field2)\n"
                             "print(Test1.Field1)\n"
                             "print(Test1.Field2)\n"
                             "end\n"
                             "";

    std::shared_ptr<FState> State(new FState());
    AddNatives(State);
    State->NewScript(Test);
    auto Result = State->GetResult("Main", FArgs());
    std::cout << Result->AsInt() << std::endl;

#define IParams(x, y) long long IA = x; long long IB = y;
#define FParams(x, y) long double FA = x; long double FB = y;
    for (int i = 0; i < 10; i ++)
    {
        IParams(5+i, 6+i)
        FParams(5.+float(i), 6.+float(i))
        assert(State->GetResult("ArithmeticIPlus", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA + IB));
        assert(State->GetResult("ArithmeticISub", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA - IB));
        assert(State->GetResult("ArithmeticIMul", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA * IB));
        assert(State->GetResult("ArithmeticIDiv", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA / IB));
        assert(State->GetResult("ArithmeticMod", FArgs().Append(State->MakeInt(IA)).Append(State->MakeInt(IB)))->AsInt() == (IA % IB));

        assert(State->GetResult("ArithmeticFPlus", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA + FB));
        assert(State->GetResult("ArithmeticFSub", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA - FB));
        assert(State->GetResult("ArithmeticFMul", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA * FB));
        assert(State->GetResult("ArithmeticFDiv", FArgs().Append(State->MakeFloat(FA)).Append(State->MakeFloat(FB)))->AsFloat() == (FA / FB));
    }
    return 0;
}