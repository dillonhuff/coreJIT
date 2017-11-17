#define CATCH_CONFIG_MAIN

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

#include "corejit.h"

#include "catch.hpp"

using namespace CoreIR;
using namespace CoreJIT;
using namespace std;

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

  JITInfo simLib = buildSimLib(m, gr);
  MemLayout& layout = simLib.layout; //buildLayout(gr);

  SECTION("5 uint 16s take up 10 bytes of space") {
    REQUIRE(layout.byteLength() == 10);
  }

  deleteContext(c);

}
