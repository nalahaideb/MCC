CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
SRCS = $(wildcard src/*.cpp)
HDRS = $(wildcard src/*.h)

mcc: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

.PHONY: clean

r run:
	./mcc

clean:
	rm -f mcc
	rm *~
