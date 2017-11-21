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

  sleep(1);

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

  REQUIRE(state.getBitVec("self.out") == BitVec(16, 205));

  deleteContext(c);
}
