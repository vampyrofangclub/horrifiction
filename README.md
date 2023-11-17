# Horrifiction
Small C++ header-only library which provides unique approach for type erasure

(requires C++20 for now)

This library consists of two helpers

```
template <typename T> constexpr int horrifiction_get_id_from_type()
```
Returns unique type id for provided type T

```
constexpr bool horrifiction_get_type_from_id(int typeId, void* ptr, auto fn)
```
Matches provided "typeId" and then invokes provided "fn" lambda callback 
with argument which formed from casting provided "ptr" void* pointer 
to corresponding type. Returns bool flag if matching was succeeded

## Examples of usages

### Examples of getting unique id for different types
```
horrifiction_get_id_from_type<int>() //0
horrifiction_get_id_from_type<double>() //1
horrifiction_get_id_from_type<int>() //0

struct MyStruct { int value; };
horrifiction_get_id_from_type<MyStruct>() //2

auto myLambda = [](auto val){  return val + 1; };
horrifiction_get_id_from_type<decltype(myLambda)>() //3
```
### Example of manual type erasure
```
struct MyStruct { int value; };
auto myObj = new MyStruct{123};
int myObjTypeId = horrifiction_get_id_from_type<decltype(myObj)>();
void* myObjPtr = myStruct;
//and now matching back
horrifiction_get_type_from_id(myObjTypeId, myObjPtr, [](auto myObj){
  std::cout << myObj->value; //123
});
```
Same with lambda
```
auto myLambda = [](auto val){  return val + 1; };
void* myLambdaPtr = &myLambda;
int myLambdaTypeId = horrifiction_get_id_from_type<decltype(&myLambda)>();
//and now matching back
horrifiction_get_type_from_id(myLambdaTypeId, myLambdaPtr, [](auto myLambda){
  std::cout << (*myLambda)(123); //124
});
```
### Example of possible Any wrapper for convenient type erasure
```
struct Any {
  void* ptr;
  int typeId;
  auto operator()(auto fn) { return horrifiction_get_type_from_id(typeId, ptr, fn); }
  static auto create(auto obj) {
    return Any{obj, horrifiction_get_id_from_type<decltype(obj)>()};
  }
};
```
1) Example of using this Any wrapper for storing objects of different types inside vector container
```
struct Rect {
    int width;
    int height;
};
struct Circle {
    int radius;
};

auto getShape(){
  if(std::rand() % 2){
    return Any::create(new Rect{10, 20});
  } else {
    return Any::create(new Circle{15});
  }
}

#include <vector>
int main(){
  auto shapes = std::vector<Any>();
  shapes.push_back(getShape());
  shapes.push_back(getShape());
  shapes.push_back(Any::create(new Rect{15, 30}));
  shapes.push_back(Any::create(new Circle{20}));
  for(auto shape: shapes){
    shape([](auto obj){
      if constexpr(std::is_same_v<Rect*, decltype(obj)>){
        std::cout << "Rect(" << obj->width << "," << obj->height << ")\n";
      }
      if constexpr(std::is_same_v<Circle*, decltype(obj)>){
        std::cout << "Circle(" << obj->radius << ")\n";
      }
    });
  }
}
```
2) Example of using Any wrapper for returning different generic lambdas from function 
(note: It's currently only possible using this library)
```
auto getFn(bool flag) {
  if (flag) {
    return Any::create(new auto([=](auto val) { 
      return val + 1; 
    }));
  } else {
    auto someVal = 10;
    return Any::create(new auto([=](auto val) { 
      return someVal + val; 
    }));
  }
}

int main(){
  auto obj = getFn(true);
  obj([](auto fn) {
    std::cout << (*fn)(42) << "\n"; //43
    std::cout << (*fn)(42.42) << "\n"; //43.42
  });
  auto obj2 = getFn(false);
  obj2([](auto fn) {
    std::cout << (*fn)(42) << "\n"; //52
    std::cout << (*fn)(42.42) << "\n"; //52.42
  });
}
```
Here is godbolt demo of these examples: [https://godbolt.org/z/6GfqE3bes](https://godbolt.org/z/1KGeb7Gra)
