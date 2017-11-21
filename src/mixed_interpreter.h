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

    ~MixedInterpreter() {
      delete jitInterp;
    }

  };

}
