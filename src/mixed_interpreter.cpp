#include "mixed_interpreter.h"

#include <thread>

using namespace std;
using namespace CoreIR;

namespace CoreJIT {

  MixedInterpreter::MixedInterpreter(CoreIR::Module* m) :
    state(m),
    createdInterp(false),
    firstJITExe(true),
    jitInterp(nullptr) {

    // TODO: This could segfault if the interpreter is closed and this is
    // destroyed before the thread finishes
    thread jitCompileThread([this, m]() {
        this->jitInterp = new JITInterpreter(m, this->state.getCircuitGraph());
        this->createdInterp = true;
      });
    jitCompileThread.detach();

  }

  void MixedInterpreter::copyInterpState() {
    auto& g = state.getCircuitGraph();

    for (auto& vd : g.getVerts()) {
      WireNode wd = g.getNode(vd);

      if (isRegisterInstance(wd.getWire())) {
        Instance* inst = toInstance(wd.getWire());

        jitInterp->setRegister(inst->toString(), state.getRegister(inst->toString()));
        
      }
    }

    // TODO: Set inputs and memory

  }


}
