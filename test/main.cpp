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

  REQUIRE(m != nullptr);

  NGraph gr;
  buildOrderedGraph(m, gr);

  MemLayout layout = buildLayout(gr);

  string cppCode = libCode(gr, layout);

  string targetBinary = "./libprog.dylib";
  string cppName = "./prog.cpp";
  ofstream out(cppName);
  out << cppCode << endl;
  int ret =
    system(("clang++ -std=c++11 -fPIC -dynamiclib " + cppName + " -o " + targetBinary).c_str());

  REQUIRE(ret == 0);

  int loadRes = loadLibAndRun(targetBinary, layout, gr);

  REQUIRE(loadRes == 0);

  deleteContext(c);

}
