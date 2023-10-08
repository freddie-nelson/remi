CPP_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*/*/*/*.cpp) $(wildcard src/*/*/*/*/*.cpp)

# sfml
output: 
	g++ -std=c++20 main.cpp $(CPP_FILES) -o main.exe -lmingw32 -lsfml-main -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32

clean:
	rm main.exe