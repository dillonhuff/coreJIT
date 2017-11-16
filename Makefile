all:
	clang++ -std=c++11 -fPIC main.cpp

clean:
	rm -f prog.cpp a.out libprog.dylib
