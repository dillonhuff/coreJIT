#pragma once

#include "coreir/simulator/interpreter.h"

#include "corejit.h"

namespace CoreJIT {

  class MixedInterpreter {
  protected:
    CoreIR::SimulatorState state;
    bool createdInterp;

    JITInterpreter* jitInterp;

  public:

    MixedInterpreter(CoreIR::Module* m);

    bool isJITCreated() const { return createdInterp; }

    void execute() {
      std::cout << "Calling execute" << std::endl;

      //state.execute();

      if (isJITCreated()) {
        std::cout << "Created JIT!" << std::endl;

        jitInterp->execute();
      } else {
        std::cout << "No JIT yet!" << std::endl;

        state.execute();
      }
    }

    void setRegister(const std::string& name,
                     const CoreIR::BitVec& bv) {
      state.setRegister(name, bv);
    }

    void setClock(const std::string& name,
                  const unsigned char clkLast,
                  const unsigned char clk) {
      state.setClock(name, clkLast, clk);
    }

    void setValue(const std::string& name, const CoreIR::BitVec& bv) {
      state.setValue(name, bv);
    }

    CoreIR::BitVec getBitVec(const std::string& name) {
      return state.getBitVec(name);
    }
    
    ~MixedInterpreter() {
      delete jitInterp;
    }

  };

}
