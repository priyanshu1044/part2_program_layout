CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: cpu assembler

cpu: src/main.cpp src/cpu.cpp src/memory.cpp
	$(CXX) $(CXXFLAGS) -o cpu src/main.cpp src/cpu.cpp src/memory.cpp

assembler: src/assembler.cpp
	$(CXX) $(CXXFLAGS) -o assembler src/assembler.cpp


# Compile Reference C Program
factorial_c: programs/factorial.c
	gcc -o factorial_c programs/factorial.c

clean:
	rm -f cpu assembler factorial_c *.bin
