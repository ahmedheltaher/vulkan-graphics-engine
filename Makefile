CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

App: **/*.cpp *.cpp **/*.hpp
	g++ $(CFLAGS) -o App **/*.cpp *.cpp $(LDFLAGS)

.PHONY: test clean

test: App
	./App

clean:
	rm -f App