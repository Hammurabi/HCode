# HCode

## Hammurabi's Code

A scripting language created for the use in games and game engines.
## Currently supported features and roadmap

 - [x] Structs
 - [x] Functions
 - [x] Primitives (long long, long double)
 - [x] Operator Overloading
 - [ ] Manual Memory Management (Pointers, Malloc, Calloc, Delete)
 - [x] Function Pointers
 - [ ] Lambdas
 - [ ] Strongly Typed
 - [x] For Loops
 - [x] If Statements
 - [ ] Elseif, Else
 - [ ] ASM output

## Simple To use
The purpose of this language is to be able to create libraries with minimal lines of code, a template function is created and the compiler would do the rest (see operator overloading).

The other reason I created this language is simple, none of the available scripting languages felt right for my game engine, and therefore I decided to create HCode specifically for my engine.    

I needed full control of the internals of the VM and a dynamic, somewhat object oriented language.
Whenever a feature is implemented, (eventually) I would be able to compile the script into ASM.

    import Collections
    import StringLib
    
    Variable: int = 5 //variable declaration (64bit signed)
    VarFloat: float = Variable^float //cast to float (64bit) (no implicit conversion)
    VarPointer: int ***//pointers, supported by the parser, but not the runtime env
    Vector: vector = vector()//must import Collections
    
    //any object can be passed here
    //will add type checking later.
    Vector.push_back(...)
    //the following functions take int index
    //as an argument.
    Vector.erase(...)
    Vector.get(...)
    
    if Vector.get(1) != Vector.get(6){
        print("Oh No!")
    }
    
    
     for x:int=0, x<4, x=x+1{
       for y:int=0, y<4, y=y+1{
         print(quick_concat(quick_concat("x: ", to_string(x)),   quick_concat(" y: ", to_string(y))))
       }
     }
    
    Dosomething:: void() {//function declaration
	    print(5 * 5)
    }
    struct:: SomeStruct {//structs do not hold functions
	    Field1: int
	    Field2: float
	    FuncPointer:= int(this: SomeStruct, X:int, Y:int) //function pointer
	    ...
    }
    
    operator:: SomeStruct (SomeStruct + int) {//operator overloading
	    Res: SomeStruct
	    Res.Field1 = A.Field1 + B
	    Res.Field2 = A.Field2 + B^float //cast using ^
	    Res.FuncPointer(B, B+12) //using the '.' operator tells the compiler
	    //that the first argument is (Res)
	    //so we don't need to pass "this" argument
	    return Res
    }
    //One definition fits all.
    //using '~' will create a generic operator
    //function, it will require a list of operators
    //after the function declaration declared in brackets.
    //[+ - /] for example tells the compiler to generate
    //operator functions for addition, subtraction, and division
    operator:: Vector4(Vector4~float) [+ - * /] {
      //~ will be replaced
      //by real operators
      return Vector4(A.x~B, A.y~B, A.z~B, A.w~B)
    }
    //can be called with Var: Type = make_copy(SomeVar)
    //the compiler will find the suitable copy constructor
    //for the variable type and use it.
    clone:: SomeStruct(SomeStruct){ //copy constructor
        Result: SomeStruct
        ...
        return Result
    }
