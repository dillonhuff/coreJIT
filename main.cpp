#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>

#include <dlfcn.h>

using namespace std;

int main() {
  string targetBinary = "./libprog.dylib";
  ofstream out("./prog.cpp");

  out <<
    "#include <iostream>\n"
    "#define EXPORT __attribute__((visibility(\"default\")))\n"
    "class EXPORT sillyStruct { public:\n\tint i; float j; };\n"
    "sillyStruct* newSillyStruct() { return new sillyStruct(); }\n"
    "int sillyFunc(sillyStruct& i) { return i.i; }\n"
    "int myFunc(int i) {\n"
    "\treturn 12 + i;\n"
    "}\n\n"
    "float otherFunc(float i) {\n"
    "\treturn i / 12.3;\n"
    "}\n\n" << endl;

  int ret =
    system(("clang++ -std=c++11 -fPIC -dynamiclib ./prog.cpp -o " + targetBinary).c_str());

  cout << "clang call ret = " << ret << endl;

  void* myLibHandle = dlopen(targetBinary.c_str(), RTLD_LOCAL);

  if (myLibHandle == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    return -1;
  }

  cout << "lib handle = " << myLibHandle << endl;

  void* myFuncFunV;
  myFuncFunV = dlsym(myLibHandle, "_Z6myFunci");
  if (myFuncFunV == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    assert(false);
  } else {
    printf("FOUND\n");
  }

  int (*myFuncCall)(int) =
    reinterpret_cast<int (*)(int)>(myFuncFunV);

  cout << "myFunc = " << myFuncCall(12) << endl;

  myFuncFunV = dlsym(myLibHandle, "_Z9otherFuncf");
  if (myFuncFunV == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    assert(false);
  } else {
    printf("FOUND!\n");
  }

  float (*otherFuncCall)(float) =
    reinterpret_cast<float (*)(float)>(myFuncFunV);

  cout << "otherFuncCall = " << otherFuncCall(12) << endl;

  myFuncFunV = dlsym(myLibHandle, "_Z14newSillyStructv");
  if (myFuncFunV == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    assert(false);
  } else {
    printf("FOUND!\n");
  }

  dlclose(myLibHandle);

}
