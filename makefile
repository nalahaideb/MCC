CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
NOWARNVAR = -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
SRCS = $(wildcard src/*.cpp)
HDRS = $(wildcard src/*.h)

mcc: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

n: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(NOWARNVAR) -o $@ $(SRCS)

.PHONY: clean

r run:
	./mcc

g:
	dot -Tpng ast.dot -o ast.png

clean:
	rm -f mcc
	rm *.out
	rm *.err


