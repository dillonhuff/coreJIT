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
    DylibInfo& dlib = simLib.libInfo;

    void (*simFunc)(unsigned char*) =
      reinterpret_cast<void (*)(unsigned char*)>(dlib.simFuncHandle);

    unsigned char* buf = (unsigned char*) malloc(16*5);

    vector<vdisc> ins = allInputs(gr);
    int value = 1;
    for (auto& in : ins) {
      Select* sel = toSelect(gr.getNode(in).getWire());
      setUint16(value, sel, layout, buf);
    }

    simFunc(buf);

    cout << "Final buffer result = " << *((uint16_t*)(buf + 8)) << endl;

    REQUIRE(*((uint16_t*)(buf + 8)) == 4);

    free(buf);

    dlclose(dlib.libHandle);
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
    string libName = "conv_3_1";

    JITInfo simLib = buildSimLib(m, gr, libName);
    MemLayout& layout = simLib.layout;
    DylibInfo& libInfo = simLib.libInfo;

    // string cppName = "./" + libName + ".cpp";
    // string targetBinary = "./lib" + libName + ".dylib";
    // int ret =
    //   system(("clang++ -std=c++11 -fPIC -dynamiclib " + cppName + " -o " + targetBinary).c_str());

    // assert(ret == 0);
    
    // MemLayout layout = buildLayout(gr);
    // DylibInfo libInfo = loadLibWithFunc(targetBinary);

    ModuleDef* def = m->getDef();

    void (*simFunc)(unsigned char*) =
      reinterpret_cast<void (*)(unsigned char*)>(libInfo.simFuncHandle);
    
    unsigned char* buf = (unsigned char*) malloc(layout.byteLength());
    memset(buf, 0, layout.byteLength());
    
    *((uint8_t*)(buf + 9)) = 0;
    *((uint8_t*)(buf + 8)) = 1;
    *((uint8_t*)(buf + 7)) = 0;
    *((uint8_t*)(buf + 6)) = 1;

    int val = 1;
    
    setClk(1, def->sel("self")->sel("clk"), layout, buf);
    setClkLast(0, def->sel("self")->sel("clk"), layout, buf);
    setUint16(val, def->sel("self")->sel("in_0"), layout, buf);    

    for (int i = 0; i < 41; i++) {

      setClk(i % 2, def->sel("self")->sel("clk"), layout, buf);
      
      simFunc(buf);

      if ((i % 2) == 0) {
        cout << "Output " << i << " = " << getUint16(m->getDef()->sel("self")->sel("out"), layout, buf) << endl;
      }

      if ((i % 2) == 1) {
        val = val + 1;
        setUint16(val, def->sel("self")->sel("in_0"), layout, buf);
      }

      setClkLast(i % 2, def->sel("self")->sel("clk"), layout, buf);

    }

    REQUIRE(getUint16(m->getDef()->sel("self")->sel("out"), layout, buf) == 205);

    free(buf);
    
    dlclose(libInfo.libHandle);
  }

  deleteContext(c);
  
}
