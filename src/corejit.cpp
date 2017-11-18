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
      assert(false);
    }

    std::string clkVarName(InstanceValue& clk) const {
      assert(false);
    }

    std::string outputVarName(CoreIR::Wireable& outSel) const {
      if (isRegisterInstance(&outSel)) {
        Instance* inst = toInstance(&outSel);
        Select* sel = inst->sel("out");
        return outputVarName(sel);
      }

      if (isMemoryInstance(&outSel)) {
        assert(false);
      }

      Select* sel = toSelect(&outSel);
      string str = "*((uint16_t*)(state + " + to_string(layout.offsets.find(sel)->second) + "))";

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
      "#include <stdint.h>\n"
      "void simulate(unsigned char* state) {\n";

    auto topoOrder = topologicalSort(gr);

    CharBufLayout cl(layout);
    str +=
      printSimFunctionBody(topoOrder, gr, *m, 0, cl);

    str += "\n}\n";

    return str;
  }

  void setUint16(const uint16_t value,
                 CoreIR::Select* target,
                 const MemLayout& layout,
                 unsigned char* buffer) {
    int offset = layout.offsets.find(target)->second;
    *((uint16_t*)(buffer + offset)) = value;
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

  int loadLibAndRun(const std::string& targetBinary,
                    const MemLayout& layout,
                    const NGraph& gr) {

    DylibInfo dlib = loadLibWithFunc(targetBinary);

    void (*simFunc)(unsigned char*) =
      reinterpret_cast<void (*)(unsigned char*)>(dlib.simFuncHandle);

    unsigned char* buf = (unsigned char*) malloc(16*5);

    vector<vdisc> ins = allInputs(gr);
    int value = 2;
    for (auto& in : ins) {
      Select* sel = toSelect(gr.getNode(in).getWire());
      setUint16(value, sel, layout, buf);
      value += 3;
    }

    simFunc(buf);

    cout << "Final buffer result = " << *((uint16_t*)(buf + 8)) << endl;

    free(buf);

    dlclose(dlib.libHandle);

    return 0;
  }

  int bufferTypeWidth(CoreIR::Type& tp) {
    cout << "Getting width for " << tp.toString() << endl;

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

      layout.offsets.insert({sel, off});

      cout << "Select type = " << (sel->getType())->toString() << endl;
      off += bufferTypeWidth(*(sel->getType())); 
      cout << "offset = " << off << endl;
    }

    // Add outputs to layout
    for (auto& vd : outs) {
      Select* sel =
        toSelect(gr.getNode(vd).getWire());

      layout.offsets.insert({sel, off});

      off += bufferTypeWidth(*(sel->getType()));
      cout << "offset = " << off << endl;
    }

    // Add registers to layout
    for (auto& vd : gr.getVerts()) {
      WireNode wd = gr.getNode(vd);

      if (isRegisterInstance(wd.getWire()) && wd.isReceiver) {
        Instance* inst = toInstance(wd.getWire());
        Select* outSel = inst->sel("out");
        layout.offsets.insert({outSel, off});

        off += bufferTypeWidth(*(outSel->getType()));
        cout << "register offset = " << off << endl;
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
        layout.offsets.insert({outSel, off});

        off += (width / 8)*depth;
        cout << "memory offset = " << off << endl;
      }
    }

    layout.setByteLength(off);

    return layout;
  }

  JITInfo buildSimLib(CoreIR::Module* m,
                      CoreIR::NGraph& gr) {
    MemLayout layout = buildLayout(gr);

    string cppCode = libCode(m, gr, layout);

    string targetBinary = "./libprog.dylib";
    string cppName = "./prog.cpp";
    ofstream out(cppName);
    out << cppCode << endl;
    int ret =
      system(("clang++ -std=c++11 -fPIC -dynamiclib " + cppName + " -o " + targetBinary).c_str());

    assert(ret == 0);

    DylibInfo dlib = loadLibWithFunc(targetBinary);

    return {layout, dlib};
  }

}
