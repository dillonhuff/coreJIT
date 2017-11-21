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

      if (isJITCreated()) {
        std::cout << "Created JIT!" << std::endl;
      } else {
        std::cout << "No JIT yet!" << std::endl;
      }
    }

    ~MixedInterpreter() {
      delete jitInterp;
    }

  };

}
