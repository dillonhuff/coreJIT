#define CATCH_CONFIG_MAIN

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "corejit.h"

#include "catch.hpp"

using namespace CoreIR;
using namespace CoreJIT;
using namespace std;

//int main() {

TEST_CASE("Dynamic code generation for add4") {

  Context* c = newContext();
  Namespace* g = c->getGlobal();
  
  if (!loadFromFile(c,"./test/inputs/add4.json")) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  c->runPasses({"rungenerators","flattentypes","flatten"});

  Module* m = g->getModule("add4");

  assert(m != nullptr);

  NGraph gr;
  buildOrderedGraph(m, gr);

  MemLayout layout = buildLayout(gr);

  string cppCode = libCode(gr, layout);

  string targetBinary = "./libprog.dylib";
  string targetName = "./prog.cpp";
  ofstream out("./prog.cpp");
  out << cppCode << endl;
  int ret =
    system(("clang++ -std=c++11 -fPIC -dynamiclib ./prog.cpp -o " + targetBinary).c_str());

  assert(ret == 0);

  int loadRes = loadLibAndRun(targetBinary, layout, gr);

  assert(loadRes == 0);

  deleteContext(c);

}
