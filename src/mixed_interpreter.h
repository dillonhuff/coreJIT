#pragma once

#include "coreir/simulator/interpreter.h"

#include "corejit.h"

namespace CoreJIT {

  class MixedInterpreter {
  protected:
    CoreIR::SimulatorState state;
    bool createdInterp;
    bool firstJITExe;

    JITInterpreter* jitInterp;

  public:

    MixedInterpreter(CoreIR::Module* m);

    bool isJITCreated() const { return createdInterp; }

    void copyInterpState();

    void execute() {
      std::cout << "Calling execute" << std::endl;

      if (isJITCreated()) {
        std::cout << "Created JIT!" << std::endl;

        if (firstJITExe) {
          firstJITExe = false;
          copyInterpState();
        }

        jitInterp->execute();
      } else {
        std::cout << "No JIT yet!" << std::endl;

        state.execute();
      }
    }

    void setRegister(const std::string& name,
                     const CoreIR::BitVec& bv) {
      if (isJITCreated()) {
        jitInterp->setRegister(name, bv);
      } else {
        state.setRegister(name, bv);
      }
    }

    void setClock(const std::string& name,
                  const unsigned char clkLast,
                  const unsigned char clk) {
      if (isJITCreated()) {
        jitInterp->setClock(name, clkLast, clk);
      } else {
        state.setClock(name, clkLast, clk);
      }
    }

    void setValue(const std::string& name, const CoreIR::BitVec& bv) {
      if (isJITCreated()) {
        jitInterp->setValue(name, bv);
      } else {
        state.setValue(name, bv);
      }
    }

    CoreIR::BitVec getBitVec(const std::string& name) {
      if (isJITCreated()) {
        return jitInterp->getBitVec(name);
      } else {
        return state.getBitVec(name);
      }
    }
    
    ~MixedInterpreter() {
      delete jitInterp;
    }

  };

}
