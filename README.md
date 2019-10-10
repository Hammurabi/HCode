# HCode

## Hammurabi's Code

A strongly typed scripting language created for the use in games and game engines.
## Currently supported features

 - [x] Structs
 - [x] Functions
 - [x] Primitives (long long, long double)
 - [x] Operator Overloading
 - [ ] Manual Memory Management (Pointers, Malloc, Calloc, Delete)
 - [ ] Function Pointers
 - [ ] Lambdas

## Simple To use

    Variable: int = 5 //variable declaration
    VarFloat: float = Variable^float //cast to float (no implicit conversion)
    VarPointer: int ***//pointers, supported by the parser, but not the runtime env
    
    Dosomething:: void() //function declaration
	    print(5 * 5)
    end
    struct SomeStruct //structs do not hold functions
	    Field1: int
	    Field2: float
	    FuncPointer:= int(X:int, Y:int) //function pointer, supported by the parser
	    ...
    end
    operator SomeStruct (SomeStruct + int) //operator overloading
	    Res: SomeStruct
	    Res.Field1 = A.Field1 + B
	    Res.Field2 = A.Field2 + B^float //cast using ^
	    Res.FuncPointer(B, B+12) //not supperted yet
	    return Res
    end
