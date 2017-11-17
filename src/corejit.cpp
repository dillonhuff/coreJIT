#include "corejit.h"

#include <fstream>

using namespace std;
using namespace CoreIR;

namespace CoreJIT {

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

  string libCode(const NGraph& gr,
                 const MemLayout& layout) {
    string str =
      "#include <stdint.h>\n"
      "void simulate(unsigned char* state) {\n";

    auto topoOrder = topologicalSort(gr);

    int nextVar = 0;
    map<Select*, int> varNames;

    for (auto& nd : topoOrder) {
      WireNode wd = gr.getNode(nd);

      if (isGraphInput(wd)) {
        Select* sel = toSelect(wd.getWire());
        varNames.insert({sel, nextVar});

        string vName = "tmp_" + to_string(nextVar);
        str += "uint16_t " + vName + " = *((uint16_t*)(state + " + to_string(layout.offsets.find(sel)->second) + "));\n";

        nextVar++;
      } else if (isGraphOutput(wd)) {

        Select* sel = toSelect(wd.getWire());
        auto inConns = getInputConnections(nd, gr);

        assert(inConns.size() == 1);

        InstanceValue arg1 = inConns[0].first;
        str += "*((uint16_t*)(state + " + to_string(layout.offsets.find(sel)->second) + ")) = tmp_" + to_string(varNames[arg1.getWire()]) + ";\n";
      
      } else {
        cout << "Instance" << endl;
        Instance* inst = toInstance(wd.getWire());

        auto inConns = getInputConnections(nd, gr);

        assert(inConns.size() == 2);

        auto outSelects = getOutputSelects(inst);

        assert(outSelects.size() == 1);

        cout << "Creating strings" << endl;

        string operand0 =
          "tmp_" + to_string(varNames[inConns[0].first.getWire()]);
        string operand1 =
          "tmp_" + to_string(varNames[inConns[1].first.getWire()]);

        string vName = "tmp_" + to_string(nextVar);

        cout << "Created temps" << endl;

        Wireable* outSel = findSelect("out", outSelects); //outSelects[0];

        cout << "Building outSelects" << endl;

        varNames.insert({toSelect(outSel), nextVar});

        cout << "Added nextVar" << endl;

        str += "uint16_t " + vName + " = " + operand0 + " + " + operand1 + ";\n";

        nextVar++;

        cout << "Done instance" << endl;
      }
    }

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

  MemLayout buildLayout(const NGraph& gr) {
    vector<vdisc> ins = allInputs(gr);

    cout << "# of inputs = " << ins.size() << endl;

    assert(ins.size() == 4);

    vector<vdisc> outs = allOutputs(gr);

    cout << "# of outputs = " << ins.size() << endl;

    assert(outs.size() == 1);

    MemLayout layout;
    vector<Type*> bufferLayout;
    int off = 0;
    for (auto& vd : ins) {
      Select* sel =
        toSelect(gr.getNode(vd).getWire());

      layout.offsets.insert({sel, off});

      off += containerTypeWidth(*(sel->getType())) / 8;
      cout << "offset = " << off << endl;
    }

    for (auto& vd : outs) {
      Select* sel =
        toSelect(gr.getNode(vd).getWire());

      layout.offsets.insert({sel, off});

      off += containerTypeWidth(*(sel->getType())) / 8;
      cout << "offset = " << off << endl;
    }

    return layout;
  }

  JITInfo buildSimLib(CoreIR::Module* m,
                      const CoreIR::NGraph& gr) {
    MemLayout layout = buildLayout(gr);

    string cppCode = libCode(gr, layout);

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
