CXX = g++ # Choose the correct compiler (e.g., g++ for C++)
CXXFLAGS = -std=c++11 -Wall -Wextra # Any additional compiler flags
.PHONY: all clean cleanobj count

SRCS = main.cpp compute_ldpc.cpp ldpc_operations.cpp # List of source files
OBJS = $(SRCS:.cpp=.o) # List of object files derived from source files
EXEC = bms # Name of the resulting executable

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXEC) $(OBJS)

cleanobj:
	rm -f $(OBJS)

count:
	wc -l $(SRCS) # Count lines in source files
