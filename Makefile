INCLUDES = -I"C:/Mihael/libs/sdl2/include" -I"./deps"
LIBS_PATH = -L"C:/Mihael/libs/sdl2/lib"
LIBS = -lSDL2main -lSDL2

CXX = g++
CXXFLAGS = -o2 -std=c++17 $(INCLUDES)

src = $(wildcard ./src/*.cpp) $(wildcard ./src/gl/*.cpp) $(wildcard ./src/gl/stb_image/*.cpp) $(wildcard ./deps/glad/*.cpp)
obj = $(src:.cpp=.o)

Woxel: $(obj)
		mkdir -p build
		cp -r resources/ build/
		$(CXX) $(CXXFLAGS) -o build/$@ $^ $(LIBS_PATH) $(LIBS)
		rm -f $(obj)
