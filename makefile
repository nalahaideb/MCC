#need to set g++ ver to 13.3, force use of make 4.3
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
SRCS = $(wildcard src/*.cpp)
HDRS = $(wildcard src/*.h)

mcc: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

.PHONY: clean

clean:
	rm -f mcc
	rm *~
