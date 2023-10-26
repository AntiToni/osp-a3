CXX = g++
CXXFLAGS = -Wall -Werror -std=c++20
SRCS = memory_manager.cpp
EXECS = firstfit bestfit

all: $(EXECS)

$(EXECS): $(SRCS)
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXECS)