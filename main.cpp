#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>

#include <dlfcn.h>

#include "coreir.h"

using namespace CoreIR;
using namespace std;

struct MemLayout {
  std::map<CoreIR::Select*, int> offsets;
};

void setUint16(const uint16_t value,
               CoreIR::Select* target,
               const MemLayout& layout,
               unsigned char* buffer) {
  int offset = layout.offsets.find(target)->second;
  *((uint16_t*)(buffer + offset)) = value;
}

int loadLibAndRun(const std::string& targetBinary,
                  const MemLayout& mem) {
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

  dlclose(myLibHandle);

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

  MemLayout layout;
  vector<Type*> bufferLayout;
  int off = 0;
  for (auto& vd : ins) {
    Select* sel =
      toSelect(gr.getNode(vd).getWire());

    layout.offsets.insert({sel, off});

    off += containerTypeWidth(*(sel->getType()));
    cout << "offset = " << off << endl;
  }

  for (auto& vd : outs) {
    Select* sel =
      toSelect(gr.getNode(vd).getWire());

    layout.offsets.insert({sel, off});

    off += containerTypeWidth(*(sel->getType()));
    cout << "offset = " << off << endl;
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

  int loadRes = loadLibAndRun(targetBinary, layout);

  deleteContext(c);

  return loadRes;
}
