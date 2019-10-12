script VectorMath

struct:: Vector2{
  x: float
  y: float
}
struct:: Vector3{
  x: float
  y: float
  z: float
}
struct:: Vector4{
  x: float
  y: float
  z: float
  w: float
}

Vector2:: Vector2(x: float, y: float) {
  Result: Vector2
  Result.x = x
  Result.y = y
  return Result
}

operator:: Vector2(Vector2~Vector2) [+ - * /] {
  return Vector2(A.x~B.x, A.y~B.y)
}

operator:: Vector2(Vector2~float) [+ - * /] {
  return Vector2(A.x~B, A.y~B)
}

operator:: Vector2(float~Vector2) [+ - * /] {
  return Vector2(A~B.x, A~B.y)
}

Vector3:: Vector3(x: float, y: float, z: float) {
  Result: Vector3
  Result.x = x
  Result.y = y
  Result.z = z
  return Result
}

operator:: Vector3(Vector3~Vector3) [+ - * /] {
  return Vector3(A.x~B.x, A.y~B.y, A.z~B.z)
}

operator:: Vector3(Vector3~float) [+ - * /] {
  return Vector3(A.x~B, A.y~B, A.z~B)
}

operator:: Vector3(float~Vector3) [+ - * /] {
  return Vector3(A~B.x, A~B.y, A~B.z)
}

Vector4:: Vector4(x: float, y: float, z: float, w: float) {
  Result: Vector4
  Result.x = x
  Result.y = y
  Result.z = z
  Result.w = w
  return Result
}

operator:: Vector4(Vector4~Vector4) [+ - * /] {
  return Vector4(A.x~B.x, A.y~B.y, A.z~B.z, A.w~B.w)
}

operator:: Vector4(Vector4~float) [+ - * /] {
  return Vector4(A.x~B, A.y~B, A.z~B, A.w~B)
}

operator:: Vector4(float~Vector4) [+ - * /] {
  return Vector4(A~B.x, A~B.y, A~B.z, A~B.w)
}

Dot:: float(A: Vector3, B: Vector3) {
  return A.x*B.x + A.y*B.y + A.z*B.z
}
Magnitude:: float(Vector: Vector3) {
  return sqrt(Vector.x*Vector.x + Vector.y*Vector.y + Vector.z*Vector.z)
}
Normalize:: Vector3(Vector: Vector3) {
  Length: float = Magnitude(Vector)
  return Vector3(Vector.x/Length, Vector.y/Length, Vector.z/Length)
}
PrintSomeStuff:: void(i: int) {
  print(i)
}

import StringLib

struct:: TestyBoy {
  name: word*
  toString:= word*(this: TestyBoy, num: int, num2: int, s: word*)
}

clone:: Vector3 (Vector3) {
}

clone:: TestyBoy (TestyBoy) {
  Clone: TestyBoy
  Clone.name = make_copy(A.name)
  Clone.toString = A.toString
  return Clone
}

ToString:: word*(this: TestyBoy, num: int, num2: int, s: word*){
  print(this.name)
  print(num)
  print(num2)
  print(s)
}

import Collections
import StringLib

Main:: void() {
  Zero: Vector3     = Vector3(0.,0.,0.)
  Up: Vector3       = Vector3(0.,1.,0.)

  VectorA: Vector3  = Vector3(0.,1.,0.)
  VectorB: Vector3  = Vector3(0.,0.,0.) - Vector3(0.,1.,0.)
  VectorD: Vector3  = Vector3(12.,1.,1.)
  string: word *    = "woopty doo"

  VectorC: Vector3 = VectorA + VectorB
  DotProd: float   = Magnitude(VectorA)//Dot(Normalize(VectorA), Normalize(VectorB))

  print(Normalize(VectorD).x)
  print(Normalize(VectorD).y)
  print(Normalize(VectorD).z)
  print(DotProd)

  v: vector = vector()
  print("----")
  print(v.length)
  v.push_back(15)
  v.push_back(16)
  print(v.length)
  print(v.get(1))
  print("----")

  test: TestyBoy
  test.name = "test"
  test.toString = ToString
  test.toString(12, 16, "test2")

  test2: TestyBoy = make_copy(test)

  testF: float = stod("15.245")

  print("-------------")
  for x: int = 0, x < 12, x = x + 1 {
    if x % 5 == 0{
        print(x)
     }
  }
  print("-------------")
  for x:int=0, x<4, x=x+1{
    for y:int=0, y<4, y=y+1{
      print(quick_concat(quick_concat("x: ", to_string(x)),   quick_concat(" y: ", to_string(y))))
    }
  }
  TestTTTT: int = 5
  print (TestTTTT)
  print(testF)
  if 1^float != 2^float {
    print(15)
  }
}
