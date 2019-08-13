#define SCREEN_W 1200
#define SCREEN_H 800
#include "SDL2/SDL.h"

#include "program.h"
extern void program(State * state);

int len(char * string) {
	int i = 0;
	for(;string[i] != '\0';++i);
	return i;
}
int highest(u16 highest, char * string) {
	int string_len = len(string);
	if (string_len > highest) {
		highest = string_len;
	}
	return highest;
}

char * zero(char * string) {
	return &string[len(string)];
}

void saveMap(Memory * t, int stage, u8 * map, int width, int height) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			sprintf(zero(t->memory), "%d ", map[y * width + x]);
		}
		sprintf(zero(t->memory), "\n");
	}
	t->high_water_mark = highest(t->high_water_mark, t->memory);
	
	char filename[50];
	sprintf(filename, "map_%d.data", stage);
	
	FILE *f = fopen(filename, "w");
	fwrite(t->memory, len(t->memory), 1, f);
	fclose(f);
}

void loadMap(Memory * t, int stage, u8 * map) {
    long int length;
    char * buffer = (char *)t->memory;
    {
        char filename[50];
        sprintf(filename, "map_%d.data", stage);
        FILE *f = fopen(filename, "r");
        if(f) {
            fseek (f, 0, SEEK_END);
            length = ftell (f); // get current position in stream
            fseek (f, 0, SEEK_SET);
            fread (buffer, length, 1, f);
            t->high_water_mark = highest(t->high_water_mark, buffer);
            fclose(f);
        }
        else { 
            return;
            printf("couldn't load file\n");
        }
    }

    int buffer_i = 0;
    for (int i = 0; i < length; ++i) {
        if (buffer[i] == ' ') {
        }
        else if (buffer[i] == '\n') {
        }
        else if (buffer[i] == '\0') {
        }
        else if (buffer[i] == ' ') {
        }
        else {
            map[buffer_i++] = buffer[i];
        }
    }

}

void drawRect(int x, int y, int w, int h, Color color, void * renderer){
	SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

    int test = SDL_RenderFillRect((SDL_Renderer*) renderer, 
                &(SDL_Rect) {x, y, w, h});
    assert(!test);

}


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, 0);
    SDL_Window* window = SDL_CreateWindow( "Economy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );
    SDL_Event e;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    State * state = malloc(sizeof(State));
	state->is_initialized = 0;
	state->memory = malloc(10000);
	state->memory_i = 0;
    state->drawRect = drawRect;
    state->saveMap = saveMap;
    state->loadMap = loadMap;
    state->input = 0;
    state->renderer = renderer;
    int run = 1;
    Color White = {255, 255, 255};  
    Color Black= {0, 0, 0};  
    while(run) { 
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_KEYDOWN ) 
            {      
                int key = e.key.keysym.sym;
                if( key  == SDLK_ESCAPE) {
                    run = 0;
                }
                else {
                    state->input = 1;
                }

            } 
        } 

        // Clear the entire screen to our selected color.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        program(state);
        SDL_Delay(500);   
        SDL_RenderPresent(renderer);
    } // while run
}
