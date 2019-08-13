clear
set -e

cc -c program.c -o program.o
cc program.o -I /usr/local/Cellar/sdl2_ttf/2.0.15/include/SDL2/ -o main main.c -lSDL2 -lSDL2_ttf
./main
