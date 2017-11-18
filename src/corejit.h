#pragma once

#include "coreir.h"

#include <dlfcn.h>

namespace CoreJIT {

  struct MemLayout {
  protected:

  public:

    MemLayout() {}

    std::map<CoreIR::Select*, int> offsets;

    int byteLength() const {
      int len = 0;

      for (auto& elem : offsets) {
        len += containerTypeWidth(*(elem.first->getType())) / 8;
      }
      
      return len;
    }
  };

  struct DylibInfo {
    void* libHandle;
    void* simFuncHandle;
  };

  class JITInfo {
  public:
    MemLayout layout;
    DylibInfo libInfo;

    int byteLength() const { return layout.byteLength(); }
  };

  std::vector<CoreIR::vdisc> allInputs(const CoreIR::NGraph& g);
  std::vector<CoreIR::vdisc> allOutputs(const CoreIR::NGraph& g);

  void setUint16(const uint16_t value,
                 CoreIR::Select* target,
                 const MemLayout& layout,
                 unsigned char* buffer);

  JITInfo buildSimLib(CoreIR::Module* m,
                      CoreIR::NGraph& gr);

  DylibInfo loadLibWithFunc(const std::string& targetBinary);  

  MemLayout buildLayout(const CoreIR::NGraph& gr);

  int loadLibAndRun(const std::string& targetBinary,
                    const MemLayout& layout,
                    const CoreIR::NGraph& gr);

  std::string libCode(const CoreIR::NGraph& gr,
                      const MemLayout& layout);

}
