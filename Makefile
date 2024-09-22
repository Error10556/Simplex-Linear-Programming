compile: simplex.bin

simplex.bin: simplex.cpp Matrix.h Matrix.cpp
	g++ -std=c++20 simplex.cpp Matrix.cpp -o simplex.bin

run: simplex.bin
	./simplex.bin

