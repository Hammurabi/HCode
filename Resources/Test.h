ArithmeticPlus::int(A: int, B: int)
  Res: int = A + B
  print(Res)
  return Res
end
ArithmeticSub::int(A: int, B: int)
  Res: int = A - B
  print(Res)
  return Res
end
ArithmeticMul::int(A: int, B: int)
  Res: int = A * B
  print(Res)
  return Res
end
ArithmeticDiv::int(A: int, B: int)
  Res: int = A / B
  print(Res)
  return Res
end
ArithmeticMod::int(A: int, B: int)
  Res: int = B % A
  print(Res)
  return Res
end
struct FStruct
  Field1: int
  Field2: int
end
operator FStruct(FStruct + int)
  Result: FStruct
  Result.Field1 = A.Field1 + B
  Result.Field2 = A.Field2 + B
  return Result
end
Main:: void()
  TestS: FStruct
  TestS.Field1 = 12
  TestS.Field2 = 22
  Test1: FStruct = TestS + 5
  print(TestS.Field1)
  print(TestS.Field2)
  print(Test1.Field1)
  print(Test1.Field2)
end
