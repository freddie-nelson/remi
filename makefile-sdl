CPP_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*/*/*/*.cpp) $(wildcard src/*/*/*/*/*.cpp)

# sdl2
output: 
	g++ -std=c++20 main.cpp $(CPP_FILES) -o main.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf 

clean:
	rm main.exe