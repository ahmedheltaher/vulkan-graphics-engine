include .env

CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi


vertSources = $(shell find Shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find Shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
${TARGET}: **/*.cpp *.cpp **/*.hpp
	g++ $(CFLAGS) -o ${TARGET} **/*.cpp *.cpp $(LDFLAGS)

# make shader targets
%.vert.spv: %.vert
	${GLSLC} $< -o $@

%.frag.spv: %.frag
	${GLSLC} $< -o $@

.PHONY: test clean

test: ${TARGET}
	./${TARGET}

clean:
	rm -f ${TARGET}