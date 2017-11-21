#pragma once

#include "coreir.h"

#include <dlfcn.h>

namespace CoreJIT {

  int bufferTypeWidth(CoreIR::Type& tp);

  struct MemLayout {
  protected:

    int byteLen;

  public:

    MemLayout() : byteLen(0) {}

    std::map<CoreIR::Select*, int> offsets;

    void setOffset(CoreIR::Select* sel, const int off) {
      offsets.insert({sel, off});
    }

    int offset(CoreIR::Select* sel) const {
      return offsets.find(sel)->second;
    }

    int clkOffset(CoreIR::Select* sel) const {
      return offsets.find(sel)->second;
    }

    int lastClkOffset(CoreIR::Select* sel) const {
      return (offsets.find(sel)->second) + 1;
    }
    
    int byteLength() const {
      return byteLen;
    }

    void setByteLength(const int newLen) {
      byteLen = newLen;
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

  uint16_t getUint16(CoreIR::Select* target,
                     const MemLayout& layout,
                     unsigned char* buffer);

  void setUint8(const uint8_t value,
                 CoreIR::Select* target,
                 const MemLayout& layout,
                 unsigned char* buffer);

  uint8_t getUint8(CoreIR::Select* target,
                   const MemLayout& layout,
                   unsigned char* buffer);
  
  void setClk(const uint8_t value,
              CoreIR::Select* target,
              const MemLayout& layout,
              unsigned char* buffer);

  void setClkLast(const uint8_t value,
                  CoreIR::Select* target,
                  const MemLayout& layout,
                  unsigned char* buffer);
  
  JITInfo buildSimLib(CoreIR::Module* m,
                      CoreIR::NGraph& gr,
                      const std::string& cppName);

  DylibInfo loadLibWithFunc(const std::string& targetBinary);  

  MemLayout buildLayout(const CoreIR::NGraph& gr);

  int loadLibAndRun(const std::string& targetBinary,
                    const MemLayout& layout,
                    const CoreIR::NGraph& gr);

  std::string libCode(const CoreIR::NGraph& gr,
                      const MemLayout& layout);

}
