#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define assert(expr) if (!(expr)) {printf("assert failed\n%s\n%s line %d\n", #expr, __func__, __LINE__); __builtin_trap() ;}

#define _map_w 12
#define _map_h 12
#define _map_size (_map_w * _map_h * sizeof(char))

typedef uint8_t u8;
typedef uint16_t u16;
typedef unsigned int uint;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
	u8 a;
} Color;


typedef struct {
	void * memory;
	u16 limit;
	u16 high_water_mark;
} Memory;

typedef enum {U, R, L, D} Direction;

typedef struct {
    int x;
    int y;
    Direction direction;
} Person;

typedef struct {
    int x;
    int y;
} Object;


typedef struct {
	/* memory */
	void * memory;
	Memory temporary_memory;
	int memory_i;
	/* function pointers */
    void * drawRect;
	void * saveMap;
	void * loadMap;
	/* data */
    int input;
    int is_initialized;
    int reload_map;
	int stage;
	char * map;
    Person player;
    Object goal;
    uint clock;
	/* sdl */
    void * renderer;
} State;

