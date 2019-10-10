//
// Created by Ragnarr on 2019-10-10.
//

#include "VM.h"
#include <iostream>

std::vector<unsigned char> HCode::GenerateInstructions(HCode::FAssembledFunction Func, HCode::FSymbolTable &SymbolTable,
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
                    double D = std::stod(Instruction.Children[0].ValueString);
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
                throw (std::runtime_error("opcode not valid '" + Instruction.ValueString + "'."));
        }
    }
    return Func.OpcodeInstructions;
}

void HCode::FState::GetResultFromOps(std::vector<unsigned char> Instructions, HCode::FAssembledScript &Script, HCode::FScope &Scope) {
    unsigned char FWORD_REG[32];
    for (unsigned int Index = 0; Index < Instructions.size(); Index ++)
    {
        EOpcode Opcode = static_cast<EOpcode>(Instructions[Index]);
        switch (Opcode)
        {
            case EOpcode ::PUSH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "PUSH" << std::endl;
#endif
                if (Index + 8 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'push'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Scope.Push(MakeInt( *((long *) FWORD_REG) ));
                Index += 8;
            }
                break;
            case EOpcode ::FPUSH:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "FPUSH" << std::endl;
#endif
                if (Index + 8 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'fpush'."));

                Scope.Push(MakeFloat( *((long double *) (Instructions.data() + Index + 1)) ));
                Index += 8;
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
                if (Index + 8 >= Instructions.size())
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
                if (Index + 8 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'getnth'."));
                auto V = Scope.Pop();
                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Scope.Pop()->SetField(*((long *) FWORD_REG), V);
                Index += 8;
            }
                break;
            case EOpcode::NEWINSTANCE:
            {
                if (Index + 8 >= Instructions.size())
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
                if (Index + 2 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'array'."));

                UndecayDW(FWORD_REG, Instructions.data() + Index + 1);
                unsigned short ALength = *((unsigned short *) FWORD_REG);
                Index += 2;
                if ((Index + ALength) >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'array'."));
                std::string S = "";
                for (unsigned short X = 0; X < ALength; X ++)
                    S += Instructions[Index + X];
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
                if (Index + 8 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'call'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 8;

                FScope NScope = Scope.Child(Table.Functions[*((long *) FWORD_REG)].Name);

                auto &X = Table.Functions[*((long *) FWORD_REG)].AssembledFunction;
                auto Instructions = GenerateInstructions(X, Table, Script);
                GetResultFromOps(Instructions, Script, NScope);
//                        NScope.DeleteScope(Heap);
//                        Heap.CycleGC();
            }
                break;
            case EOpcode ::INVK:
            {
#ifdef HC_OUTPUT_CODES
                std::cerr<< "INVK" << std::endl;
#endif
                if (Index + 8 >= Instructions.size())
                    throw (std::runtime_error("corrupt instruction set provided. 'invk'."));

                UndecayFW(FWORD_REG, Instructions.data() + Index + 1);
                Index += 8;

                FScope NScope = Scope.Child(Table.Functions[*((long *) FWORD_REG)].Name);
                Table.Functions[*((long *) FWORD_REG)].NativeCallback(*this, Table, NScope);
            }
                break;
    #define AOPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeInt(A->AsInt() x B->AsInt()));}
                case EOpcode ::IADD:   AOPEXE(+)        break;
                case EOpcode ::ISUB:   AOPEXE(-)        break;
                case EOpcode ::IDIV:   AOPEXE(/)        break;
                case EOpcode ::IMUL:   AOPEXE(*)        break;
                case EOpcode ::MOD:    AOPEXE(%)        break;
                case EOpcode ::ICMPL:  AOPEXE(<)        break;
                case EOpcode ::ICMPG:  AOPEXE(>)        break;
                case EOpcode ::ICMPLE: AOPEXE(<=)        break;
                case EOpcode ::ICMPGE: AOPEXE(>=)        break;
    #undef AOPEXE
    #define AOPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeFloat(A->AsFloat() x B->AsFloat()));}
    #define AIPEXE(x) { auto B = Scope.Pop(); auto A = Scope.Pop(); Scope.Push(MakeInt(A->AsFloat() x B->AsFloat()));}
                case EOpcode ::FADD:   AOPEXE(+)        break;
                case EOpcode ::FSUB:   AOPEXE(-)        break;
                case EOpcode ::FDIV:   AOPEXE(/)        break;
                case EOpcode ::FMUL:   AOPEXE(*)        break;
                case EOpcode ::FCMPL:  AIPEXE(<)        break;
                case EOpcode ::FCMPG:  AIPEXE(>)        break;
                case EOpcode ::FCMPLE: AIPEXE(<=)        break;
                case EOpcode ::FCMPGE: AIPEXE(>=)        break;
                case EOpcode ::EQUAL: Scope.Push(MakeInt(Scope.Pop()->AsInt() == Scope.Pop()->AsInt())); break;
    #undef AOPEXE
        }
    }
}
