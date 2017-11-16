#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>

#include <dlfcn.h>

#include "coreir.h"

using namespace CoreIR;
using namespace std;

int loadLibAndPrint(const std::string& targetBinary) {
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

  return 0;

}

int loadLibAndRun(const std::string& targetBinary) {
  void* myLibHandle = dlopen(targetBinary.c_str(), RTLD_LOCAL);

  if (myLibHandle == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    return -1;
  }

  cout << "lib handle = " << myLibHandle << endl;

  void* myFuncFunV;
  myFuncFunV = dlsym(myLibHandle, "_Z8simulatePh");
  if (myFuncFunV == nullptr) {
    printf("dlsym failed: %s\n", dlerror());
    assert(false);
  } else {
    printf("FOUND\n");
  }

  void (*simFunc)(unsigned char*) =
    reinterpret_cast<void (*)(unsigned char*)>(myFuncFunV);

  unsigned char* buf = (unsigned char*) malloc(16*5);
  *((uint16_t*)(buf + 0)) = 2;
  *((uint16_t*)(buf + 2)) = 5;
  *((uint16_t*)(buf + 4)) = 7;
  *((uint16_t*)(buf + 6)) = 4;
  *((uint16_t*)(buf + 8)) = 0;

  simFunc(buf);

  cout << "Final buffer result = " << *((uint16_t*)(buf + 8)) << endl;

  free(buf);

  return 0;
}

vector<vdisc> allInputs(const NGraph& g) {
  vector<vdisc> inputs;
  for (auto& vd : g.getVerts()) {
    if (isGraphInput(g.getNode(vd))) {
      inputs.push_back(vd);
    }
  }
  return inputs;
}

vector<vdisc> allOutputs(const NGraph& g) {
  vector<vdisc> outputs;
  for (auto& vd : g.getVerts()) {
    if (isGraphOutput(g.getNode(vd))) {
      outputs.push_back(vd);
    }
  }
  return outputs;
}

int main() {

  Context* c = newContext();
  Namespace* g = c->getGlobal();
  
  if (!loadFromFile(c,"./add4.json")) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  c->runPasses({"rungenerators","flattentypes","flatten"});

  Module* m = g->getModule("add4");

  assert(m != nullptr);

  NGraph gr;
  buildOrderedGraph(m, gr);

  vector<vdisc> ins = allInputs(gr);

  cout << "# of inputs = " << ins.size() << endl;

  assert(ins.size() == 4);

  vector<vdisc> outs = allOutputs(gr);

  cout << "# of outputs = " << ins.size() << endl;

  assert(outs.size() == 1);
  
  vector<Type*> bufferLayout;
  for (auto& vd : ins) {
    bufferLayout.push_back(gr.getNode(vd).getWire()->getType());
  }
  for (auto& vd : outs) {
    bufferLayout.push_back(gr.getNode(vd).getWire()->getType());
  }

  vector<int> inputOffsets;
  int offset = 0;
  for (auto& tp : bufferLayout) {
    cout << "Offset = " << offset << endl;
    inputOffsets.push_back(offset);
    offset += containerTypeWidth(*tp);
  }

  string cppCode =
    "#include <stdint.h>\n"
    "void simulate(unsigned char* state) {\n"
    "\tuint16_t tmp0 = ((uint16_t*)state)[0] + ((uint16_t*)state)[1];\n"
    "\tuint16_t tmp1 = ((uint16_t*)state)[2] + ((uint16_t*)state)[3];\n"
    "\t((uint16_t*)state)[4] = tmp0 + tmp1;\n"
    "\treturn;\n}\n";

  string targetBinary = "./libprog.dylib";
  ofstream out("./prog.cpp");
  out << cppCode << endl;
  int ret =
    system(("clang++ -std=c++11 -fPIC -dynamiclib ./prog.cpp -o " + targetBinary).c_str());

  assert(ret == 0);

  cout << "clang call ret = " << ret << endl;

  //int loadRes = loadLibAndPrint(targetBinary);

  int loadRes = loadLibAndRun(targetBinary);

  deleteContext(c);

  //return loadRes;

  return 0;
}
