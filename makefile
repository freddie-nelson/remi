CPP_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*/*/*/*.cpp) $(wildcard src/*/*/*/*/*.cpp)

output: 
	g++ -std=c++20 main.cpp $(CPP_FILES) -o main.exe -lmingw32 -lglfw3 -lgdi32

clean:
	rm main.exe