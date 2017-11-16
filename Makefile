all:
	clang++ -std=c++11 -fPIC main.cpp -lcoreir -lcoreir-commonlib -L/Users/dillon/CppWorkspace/coreir/lib/

clean:
	rm -f prog.cpp a.out libprog.dylib
