CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
SRCS = $(wildcard src/*.cpp)
HDRS = $(wildcard src/*.h)

mcc: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

.PHONY: clean

r run:
	./mcc
g:
	dot -Tpng ast.dot -o ast.png

clean:
	rm -f mcc
	rm *.out
	rm *.err


