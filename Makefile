all:
	clang++ -std=c++11 -fPIC ./test/main.cpp -lcoreir -lcoreir-commonlib -L/Users/dillon/CppWorkspace/coreir/lib/

clean:
	rm -f test/gencode/prog.cpp a.out test/gencode/libprog.dylib
