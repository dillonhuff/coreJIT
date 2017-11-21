#define CATCH_CONFIG_MAIN

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

#include "coreir/libs/commonlib.h"

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

  JITInfo simLib = buildSimLib(m, gr, "add4");
  MemLayout& layout = simLib.layout;

  SECTION("5 uint 16s take up 10 bytes of space") {
    REQUIRE(layout.byteLength() == 10);
  }

  SECTION("Execution of simulate produces expected result") {

    JITInterpreter interp(m, gr);
    interp.setValue("self.in_0", BitVec(16, 1));
    interp.setValue("self.in_1", BitVec(16, 1));
    interp.setValue("self.in_2", BitVec(16, 1));
    interp.setValue("self.in_3", BitVec(16, 1));

    interp.execute();

    REQUIRE(interp.getBitVec("self.out") == BitVec(16, 4));

  }

  deleteContext(c);

}

TEST_CASE("Dynamic code generation for conv_3_1") {
  Context* c = newContext();
  Namespace* g = c->getGlobal();
  
  CoreIRLoadLibrary_commonlib(c);  

  if (!loadFromFile(c,"./test/inputs/conv_3_1.json")) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  c->runPasses({"rungenerators","flattentypes","flatten", "wireclocks-coreir"});

  Module* m = g->getModule("DesignTop");

  REQUIRE(m != nullptr);

  NGraph gr;
  buildOrderedGraph(m, gr);

  SECTION("DesignTop requires more than 16 + 16 + 8 + 8 bits to store") {
    MemLayout layout = buildLayout(gr);

    int bl = layout.byteLength();

    cout << "Byte length = " << bl << endl;

    // 2 input
    // 2 output
    // 2 last + current clock
    // Registers
    // 2 * 10 Memory
    REQUIRE(layout.byteLength() == 50);
  }

  SECTION("Generating code for the actual JIT") {
    //string libName = "conv_3_1";

    JITInterpreter interp(m, gr);

    // JITInfo simLib = buildSimLib(m, gr, libName);
    // MemLayout& layout = simLib.layout;
    // DylibInfo& libInfo = simLib.libInfo;

    // // string cppName = "./" + libName + ".cpp";
    // // string targetBinary = "./lib" + libName + ".dylib";
    // // int ret =
    // //   system(("clang++ -std=c++11 -fPIC -dynamiclib " + cppName + " -o " + targetBinary).c_str());

    // // assert(ret == 0);
    
    // // MemLayout layout = buildLayout(gr);
    // // DylibInfo libInfo = loadLibWithFunc(targetBinary);

    interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_1$raddr$reg0", BitVec(8, 1));
    interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_1$waddr$reg0", BitVec(8, 0));
    interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_2$raddr$reg0", BitVec(8, 1));
    interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_2$waddr$reg0", BitVec(8, 0));

    int val = 1;

    int lastClk = 0;
    int nextClk = 1;

    interp.setClock("self.clk", lastClk, nextClk);
    interp.setValue("self.in_0", BitVec(16, val));

    for (int i = 0; i < 41; i++) {
      nextClk = i % 2;

      interp.setClock("self.clk", lastClk, nextClk);

      interp.execute();

      if ((i % 2) == 0) {
        cout << "Output " << i << " = " <<
          interp.getBitVec("self.out").to_type<uint16_t>() << endl;
      }

      if ((i % 2) == 1) {
        val = val + 1;

        interp.setValue("self.in_0", BitVec(16, val));
      }

      lastClk = nextClk;

    }



    REQUIRE(interp.getBitVec("self.out") == BitVec(16, 205));

  }

  deleteContext(c);
  
}
