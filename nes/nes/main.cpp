#include <stdio.h>
extern "C" {
    FILE __iob_func[3] = { *stdin, *stdout, *stderr };
#include "nes_main.h"
#include "include/sdl/SDL.h"
};

const int screen_w = 320, screen_h = 240;
const int texture_w = 320, texture_h = 240;
SDL_Renderer* sdlRenderer = NULL;
SDL_Texture* sdlTexture = NULL;
uint16 buffer[320*240];

// will be called by ppu.c
// when a scan line render finish, call it.
extern "C" void updateBuffer(int y_axes, uint16 *Buffer_scanline)
{
    uint16* pBuffer = buffer;
    for (size_t index = 8; index < 256; index++)
        pBuffer[y_axes * texture_w + 32 + index] = Buffer_scanline[index];

    // update OK, render it to SDL.
    {
        SDL_Rect sdlRect = { 0, 0, screen_w, screen_h };
        SDL_UpdateTexture(sdlTexture, NULL, buffer, texture_w * sizeof(uint16));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
        SDL_RenderPresent(sdlRenderer);
    }
}

extern "C" uint16 getKeyEvent() {
    uint16 ret = 0; 

    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
        exit(0);

    //A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
    if (state[SDL_SCANCODE_A])
        ret |= (0x1 << 0);
    if (state[SDL_SCANCODE_B])
        ret |= (0x1 << 1);
    if (state[SDL_SCANCODE_C])
        ret |= (0x1 << 2);
    if (state[SDL_SCANCODE_D])
        ret |= (0x1 << 3);
    if (state[SDL_SCANCODE_UP])
        ret |= (0x1 << 4);
    if (state[SDL_SCANCODE_DOWN])
        ret |= (0x1 << 5);
    if (state[SDL_SCANCODE_LEFT])
        ret |= (0x1 << 6);
    if (state[SDL_SCANCODE_RIGHT])
        ret |= (0x1 << 7);
    return ret;
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }
    SDL_Window *screen;
    //SDL 2.0 Support for multiple windows
    screen = SDL_CreateWindow("mali", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screen_w, screen_h, SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN*/);
    if (!screen) {
        return -1;
    }
    sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, texture_w, texture_h);
    
    nes_main(); //loop in simulator
    
    SDL_Quit();
    return 0;
}