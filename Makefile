CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

Tutorial: **/*.cpp *.cpp **/*.hpp
	g++ $(CFLAGS) -o Tutorial **/*.cpp *.cpp $(LDFLAGS)

.PHONY: test clean

test: Tutorial
	./Tutorial

clean:
	rm -f Tutorial