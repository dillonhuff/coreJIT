#pragma once

#include "coreir.h"

namespace CoreJIT {

  struct MemLayout {
    std::map<CoreIR::Select*, int> offsets;
  };

  struct DylibInfo {
    void* libHandle;
    void* simFuncHandle;
  };

  MemLayout buildLayout(const CoreIR::NGraph& gr);

  int loadLibAndRun(const std::string& targetBinary,
                    const MemLayout& layout,
                    const CoreIR::NGraph& gr);

  std::string libCode(const CoreIR::NGraph& gr,
                      const MemLayout& layout);

}
