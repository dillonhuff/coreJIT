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

  std::string libCode(const CoreIR::NGraph& gr,
                      const MemLayout& layout);

  class JITInterpreter {
  protected:

    CoreIR::Module* m;
    CoreIR::NGraph& g;

    JITInfo info;
    unsigned char* buf;

  public:

    JITInterpreter(CoreIR::Module* m_,
                   CoreIR::NGraph& g_) : m(m_), g(g_) {
      info = buildSimLib(m, g, m->getName());
      buf = static_cast<unsigned char*>(malloc(info.layout.byteLength()));
      memset(buf, 0, info.layout.byteLength());
    }

    CoreIR::Select* findSelect(const std::string& name) const {
      CoreIR::ModuleDef* def = m->getDef();
      CoreIR::Wireable* w = def->sel(name);
      CoreIR::Select* s = toSelect(w);

      return s;
    }

    CoreIR::Instance* findInstance(const std::string& name) const {

      CoreIR::ModuleDef* def = m->getDef();
      auto instances = def->getInstances();
      auto instIter = instances.find(name);

      assert(instIter != std::end(instances));

      return instIter->second;
    }

    void setClock(const std::string& clkName, const int last, const int next) {
      auto sel = findSelect(clkName);
      CoreJIT::setClkLast(last, sel, info.layout, buf);
      CoreJIT::setClk(next, sel, info.layout, buf);
    }

    void setRegister(const std::string& regName,
                     const CoreIR::BitVec& bv) {
      auto inst = findInstance(regName);
      auto sel = inst->sel("out");
      setValue(sel, bv);
    }
    
    
    void setValue(const std::string& sel,
                  const CoreIR::BitVec& bv) {
      auto s = findSelect(sel);

      assert(s != nullptr);

      setValue(s, bv);
    }

    void setValue(CoreIR::Select* const sel,
                  const CoreIR::BitVec& bv) {
      // Only supports length 16 for now
      if (bv.bitLength() == 16) {
        setUint16(bv.to_type<uint16_t>(), sel, info.layout, buf);
      } else if (bv.bitLength() == 8) {
        setUint8(bv.to_type<uint8_t>(), sel, info.layout, buf);
      } else {
        std::cout << "Error: Setting " << sel->toString() << " to unsupported bit length " << bv << std::endl;
        assert(false);
      }
    }
    
    CoreIR::BitVec getBitVec(const std::string& sel) const {
      auto s = findSelect(sel);
      return getBitVec(s);
    }

    CoreIR::BitVec getBitVec(CoreIR::Select* const sel) const {
      return CoreIR::BitVec(16, getUint16(sel, info.layout, buf));
    }
    
    void execute() {
      void (*simFunc)(unsigned char*) =
        reinterpret_cast<void (*)(unsigned char*)>(info.libInfo.simFuncHandle);

      simFunc(buf);
    }

    ~JITInterpreter() {
      dlclose(info.libInfo.libHandle);
      free(buf);
    }
  };

  
}
