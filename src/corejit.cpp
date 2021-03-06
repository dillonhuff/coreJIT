#include "corejit.h"

#include <fstream>

using namespace std;
using namespace CoreIR;

namespace CoreJIT {

  class CharBufLayout : public LayoutPolicy {
    MemLayout layout;

  public:

    CharBufLayout(const MemLayout& layout_) : layout(layout_) {}

    std::string lastClkVarName(InstanceValue& clk) const {
      Select* sel = toSelect(clk.getWire());

      string str = "*((uint8_t*)(state + " + to_string(layout.lastClkOffset(sel)) + "))";
      return str;
    }

    std::string clkVarName(InstanceValue& clk) const {
      Select* sel = toSelect(clk.getWire());

      string str = "*((uint8_t*)(state + " + to_string(layout.offset(sel)) + "))";

      return str;
    }

    std::string outputVarName(CoreIR::Wireable& outSel) const {
      if (isRegisterInstance(&outSel)) {
        Instance* inst = toInstance(&outSel);
        Select* sel = inst->sel("out");
        return outputVarName(sel);
      }

      if (isMemoryInstance(&outSel)) {
        Instance* inst = toInstance(&outSel);
        Select* sel = inst->sel("wdata");

        string cTypeName = unSignedCTypeString(*(sel->getType()));

        string str = "((" + cTypeName + "*)(state + " + to_string(layout.offset(sel)) + "))";

        return str;
      }

      Select* sel = toSelect(&outSel);

      string cTypeName = unSignedCTypeString(*(sel->getType()));

      string str = "*((" + cTypeName + "*)(state + " + to_string(layout.offset(sel)) + "))";

      return str;
    }

    std::string outputVarName(const InstanceValue& val) const {
      return outputVarName(*(val.getWire()));
    }

  };

  vector<vdisc> allInputs(const NGraph& g) {
    vector<vdisc> inputs;
    for (auto& vd : g.getVerts()) {
      if (isGraphInput(g.getNode(vd))) {
        inputs.push_back(vd);
      }
    }
    return inputs;
  }

  vector<vdisc> allOutputs(const NGraph& g) {
    vector<vdisc> outputs;
    for (auto& vd : g.getVerts()) {
      if (isGraphOutput(g.getNode(vd))) {
        outputs.push_back(vd);
      }
    }
    return outputs;
  }

  string tmpVarName(const int i) {
    return "tmp_" + to_string(i);
  }

  string libCode(CoreIR::Module* m,
                 NGraph& gr,
                 const MemLayout& layout) {
    string str =
      "#include <stdint.h>\n";

    str += seMacroDef() + "\n" + maskMacroDef();

    str +=
      "void simulate(unsigned char* state) {\n";

    auto topoOrder = topologicalSort(gr);

    CharBufLayout cl(layout);
    str +=
      printSimFunctionBody(topoOrder, gr, *m, 0, cl);

    str += "\n}\n";

    return str;
  }

  uint16_t getUint16(CoreIR::Select* target,
                     const MemLayout& layout,
                     unsigned char* buffer) {
    int offset = layout.offset(target);
    return *((uint16_t*) (buffer + offset));
  }
  
  void setUint16(const uint16_t value,
                 CoreIR::Select* target,
                 const MemLayout& layout,
                 unsigned char* buffer) {
    int offset = layout.offset(target);
    *((uint16_t*)(buffer + offset)) = value;
  }

  uint8_t getUint8(CoreIR::Select* target,
                     const MemLayout& layout,
                     unsigned char* buffer) {
    int offset = layout.offset(target);
    return *((uint8_t*) (buffer + offset));
  }
  
  void setUint8(const uint8_t value,
                 CoreIR::Select* target,
                 const MemLayout& layout,
                 unsigned char* buffer) {
    int offset = layout.offset(target);
    *((uint8_t*)(buffer + offset)) = value;
  }
  
  void setClk(const uint8_t value,
              CoreIR::Select* target,
              const MemLayout& layout,
              unsigned char* buffer) {
    int offset = layout.clkOffset(target);
    *((uint8_t*)(buffer + offset)) = value;
  }

  void setClkLast(const uint8_t value,
                  CoreIR::Select* target,
                  const MemLayout& layout,
                  unsigned char* buffer) {
    int offset = layout.lastClkOffset(target);
    *((uint8_t*)(buffer + offset)) = value;
  }
  
  DylibInfo loadLibWithFunc(const std::string& targetBinary) {
    void* myLibHandle = dlopen(targetBinary.c_str(), RTLD_LOCAL);

    if (myLibHandle == nullptr) {
      printf("dlsym failed: %s\n", dlerror());
      assert(false);
    }

    cout << "lib handle = " << myLibHandle << endl;

    void* myFuncFunV;
    myFuncFunV = dlsym(myLibHandle, "_Z8simulatePh");
    if (myFuncFunV == nullptr) {
      printf("dlsym failed: %s\n", dlerror());
      assert(false);
    } else {
      printf("FOUND\n");
    }

    return {myLibHandle, myFuncFunV};
  }

  int bufferTypeWidth(CoreIR::Type& tp) {

    if (isPrimitiveType(tp)) {
      return containerTypeWidth(tp) / 8;
    }

    Type::TypeKind tk = tp.getKind();

    assert(tk == Type::TK_Named);

    NamedType& ntp =
      static_cast<NamedType&>(tp);

    assert(ntp.getName() == "clk");

    return 2; // 1 byte for this clock, one byte for last clock
  }

  MemLayout buildLayout(const NGraph& gr) {
    vector<vdisc> ins = allInputs(gr);

    cout << "# of inputs = " << ins.size() << endl;

    vector<vdisc> outs = allOutputs(gr);

    cout << "# of outputs = " << ins.size() << endl;

    MemLayout layout;
    vector<Type*> bufferLayout;
    int off = 0;

    // Add inputs to layout
    for (auto& vd : ins) {
      Select* sel =
        toSelect(gr.getNode(vd).getWire());

      layout.setOffset(sel, off);

      off += bufferTypeWidth(*(sel->getType())); 

    }

    // Add outputs to layout
    for (auto& vd : outs) {
      Select* sel =
        toSelect(gr.getNode(vd).getWire());

      layout.setOffset(sel, off);

      off += bufferTypeWidth(*(sel->getType()));
    }

    // Add registers to layout
    for (auto& vd : gr.getVerts()) {
      WireNode wd = gr.getNode(vd);

      if (isRegisterInstance(wd.getWire()) && wd.isReceiver) {
        Instance* inst = toInstance(wd.getWire());
        Select* outSel = inst->sel("out");

        layout.setOffset(outSel, off);

        off += bufferTypeWidth(*(outSel->getType()));
      }
    }

    // Add memory to layout
    for (auto& vd : gr.getVerts()) {
      WireNode wd = gr.getNode(vd);

      if (isMemoryInstance(wd.getWire()) && wd.isReceiver) {
        Instance* inst = toInstance(wd.getWire());

        Values args = inst->getModuleRef()->getGenArgs();
        uint width = (args["width"])->get<int>();
        uint depth = (args["depth"])->get<int>();
        
        // Key memory off of wdata
        Select* outSel = inst->sel("wdata");
        layout.setOffset(outSel, off);

        off += (width / 8)*depth;
      }
    }

    layout.setByteLength(off);

    return layout;
  }

  void compileCppLib(const std::string& cppName,
                     const std::string& targetBinary) {
    int ret =
      system(("clang++ -std=c++11 -fPIC -dynamiclib " + cppName + " -o " + targetBinary).c_str());

    assert(ret == 0);
  }

  JITInfo buildSimLib(CoreIR::Module* m,
                      CoreIR::NGraph& gr,
                      const std::string& libName) {
    MemLayout layout = buildLayout(gr);

    string cppCode = libCode(m, gr, layout);

    string targetBinary = "./lib" + libName + ".dylib";
    string cppName = "./" + libName + ".cpp";
    ofstream out(cppName);
    out << cppCode << endl;
    compileCppLib(cppName, targetBinary);

    DylibInfo dlib = loadLibWithFunc(targetBinary);

    return {layout, dlib};
  }

  void JITInterpreter::setRegisterDefaults() {

    for (auto& vd : g.getVerts()) {
      WireNode wd = g.getNode(vd);

      if (isRegisterInstance(wd.getWire())) {
        Instance* inst = toInstance(wd.getWire());

        Values args = inst->getModuleRef()->getGenArgs();

        auto wArg = args["width"];

        assert(wArg != nullptr);

        uint width = (args["width"])->get<int>();

        setRegister(inst->toString(), BitVec(width, 0));
        
      }
    }

  }
  
}
