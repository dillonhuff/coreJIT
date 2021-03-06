#include "catch.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

#include "coreir/libs/commonlib.h"

#include "mixed_interpreter.h"

#include "catch.hpp"

using namespace CoreIR;
using namespace CoreJIT;
using namespace std;

TEST_CASE("Mixing interpreter and simulator") {
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

  MixedInterpreter state(m);

  BitVector one(16, "1");
  BitVector zero(16, "0");
  BitVector inVal = one;

  int val = 1;

  int lastClk = 0;
  int nextClk = 1;

  state.setClock("self.clk", lastClk, nextClk);
  state.setValue("self.in_0", BitVec(16, val));
  
  for (int i = 0; i < 41; i++) {
    nextClk = i % 2;

    state.setClock("self.clk", lastClk, nextClk);

    state.execute();

    if ((i % 2) == 0) {
      cout << "Output " << i << " = " <<
        state.getBitVec("self.out").to_type<uint16_t>() << endl;
    }

    if ((i % 2) == 1) {
      val = val + 1;

      state.setValue("self.in_0", BitVec(16, val));
    }

    lastClk = nextClk;

  }

  //REQUIRE(state.getBitVec("self.out") == BitVec(16, 205));

  deleteContext(c);
}

TEST_CASE("Harris benchmark") {
  Context* c = newContext();
  Namespace* g = c->getGlobal();
  
  CoreIRLoadLibrary_commonlib(c);  

  if (!loadFromFile(c,"./test/inputs/_harris.json")) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  c->runPasses({"rungenerators","flattentypes","flatten", "wireclocks-coreir"});

  Module* m = g->getModule("DesignTop");

  REQUIRE(m != nullptr);

  int nRuns = 1;

  while (nRuns <= 1e7) {
    BitVector one(16, "1");
    BitVector zero(16, "0");
    BitVector inVal = one;


    int val = 1;

    int lastClk = 0;
    int nextClk = 1;

    cout << "nRuns = " << nRuns << endl;

    auto start = std::chrono::high_resolution_clock::now();    

    MixedInterpreter state(m);
    state.setClock("self.clk", lastClk, nextClk);
    state.setValue("self.in_0", BitVec(16, val));

    for (int i = 0; i < nRuns; i++) {
      nextClk = i % 2;

      state.setClock("self.clk", lastClk, nextClk);

      state.execute();

      // if ((i % 2) == 0) {
      //   cout << "Output " << i << " = " <<
      //     state.getBitVec("self.out").to_type<uint16_t>() << endl;
      // }

      // if ((i % 2) == 1) {
      //   val = val + 1;
      //   state.setValue("self.in_0", BitVec(16, val));
      // }

      lastClk = nextClk;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << "Time to compute " << nRuns <<
      " half cycles = " << time_ms << " ms" << endl;
    cout << "out_0 = " << state.getBitVec("self.out") << endl;
  
    nRuns = nRuns * 10;

    sleep(1);
  }

  //REQUIRE(state.getBitVec("self.out") == BitVec(16, 205));

  deleteContext(c);
}
