all:
	g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL3