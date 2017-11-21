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

  MixedInterpreter interp(m);

  cout << "Sleeping" << endl;

  sleep(2);

  interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_1$raddr$reg0",
                     BitVec(8, 1));
  interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_1$waddr$reg0",
                     BitVec(8, 0));
  interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_2$raddr$reg0",
                     BitVec(8, 1));
  interp.setRegister("lb_p4_clamped_stencil_update_stream$mem_2$waddr$reg0",
                     BitVec(8, 0));

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
  

  deleteContext(c);
}
