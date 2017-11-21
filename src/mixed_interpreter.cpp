#include "mixed_interpreter.h"

#include <thread>

using namespace std;

namespace CoreJIT {

  MixedInterpreter::MixedInterpreter(CoreIR::Module* m) :
    state(m),
    createdInterp(false),
    jitInterp(nullptr) {

    // TODO: This could segfault if the interpreter is closed and this is
    // destroyed before the thread finishes
    thread jitCompileThread([this, m]() {
        this->jitInterp = new JITInterpreter(m, this->state.getCircuitGraph());
        this->createdInterp = false;
      });
    jitCompileThread.detach();

  }


}
