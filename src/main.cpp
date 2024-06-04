#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define W_HEIGHT 720
#define W_WIDTH 1280

static struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  Uint32 *pixels;
  bool running;
} GlobalState;

int InitSdl() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << SDL_GetError() << std::endl;
    SDL_Log("SDL Failed to init");
    return -1;
  }

  GlobalState.window = SDL_CreateWindow("Game", W_WIDTH, W_HEIGHT, 0);
  if (GlobalState.window == NULL) {
    std::cerr << SDL_GetError() << std::endl;
    return -1;
  }

  GlobalState.renderer = SDL_CreateRenderer(GlobalState.window, NULL);
  GlobalState.texture =
      SDL_CreateTexture(GlobalState.renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STATIC, W_WIDTH, W_HEIGHT);

  GlobalState.pixels = new Uint32[W_WIDTH * W_HEIGHT];
  memset(GlobalState.pixels, 255, sizeof(Uint32) * W_WIDTH * W_HEIGHT);

  GlobalState.running = true;

  return 0;
}

void Cleanup() {
  delete[] GlobalState.pixels;
  SDL_DestroyTexture(GlobalState.texture);
  SDL_DestroyRenderer(GlobalState.renderer);
  SDL_DestroyWindow(GlobalState.window);
  SDL_Quit();
}

void DrawPoint(Uint32 x, Uint32 y, Uint32 color) {
  GlobalState.pixels[(y * W_WIDTH + x)] = color;
}

void DrawRect(Uint32 x, Uint32 y, Uint32 w, Uint32 h, Uint32 color) {
  for (int r = y; r < y + h && r < W_HEIGHT; r++) {
    memset(&(GlobalState.pixels[r * W_WIDTH + x]), color, sizeof(Uint32) * w);
  }
}

void DrawCircle(int x, int y, int r, Uint32 color) {
  int low = y - r + 1 >= 0 ? -r + 1 : -y;
  int high = y + r < W_HEIGHT ? r : W_HEIGHT - y - 1;
  for (int i = low; i < high; i++) {
    int row = y + i;
    int halfw = sqrt(r * r - i * i);
    int s = std::max(x - halfw, 0);
    int w =
        std::min(x - halfw >= 0 ? 2 * halfw : halfw + x, halfw + W_WIDTH - x);
    memset(&(GlobalState.pixels[row * W_WIDTH + s]), color, sizeof(Uint32) * w);
  }
}

int main() {
  int initResult = InitSdl();

  if (initResult != 0) {
    return initResult;
  }

  int y = 100;
  int x = 100;
  int r = 50;
  int yvel = 0;
  int xvel = 0;

  while (GlobalState.running) {
    memset(GlobalState.pixels, 255, sizeof(Uint32) * W_WIDTH * W_HEIGHT);
    DrawCircle(x, y, r, 0);

    SDL_UpdateTexture(GlobalState.texture, NULL, GlobalState.pixels,
                      W_WIDTH * sizeof(Uint32));

    SDL_Event event;
    if (SDL_PollEvent(&event) > 0) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        GlobalState.running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          yvel = -1;
          break;
        case SDLK_DOWN:
          yvel = 1;
          break;
        case SDLK_LEFT:
          xvel = -1;
          break;
        case SDLK_RIGHT:
          xvel = 1;
          break;
        case SDLK_e:
          r += 1;
          break;
        case SDLK_q:
          r -= 1;
          break;
        default:
          break;
        }
        break;
      case SDL_EVENT_KEY_UP:
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (yvel < 0)
            yvel = 0;
          break;
        case SDLK_DOWN:
          if (yvel > 0)
            yvel = 0;
          break;
        case SDLK_LEFT:
          if (xvel < 0)
            xvel = 0;
          break;
        case SDLK_RIGHT:
          if (xvel > 0)
            xvel = 0;
          break;
        default:
          break;
        }
        break;
      }
    }
    y += yvel;
    y = std::max(0, std::min(y, W_HEIGHT));
    x += xvel;
    x = std::max(0, std::min(x, W_WIDTH));

    SDL_RenderClear(GlobalState.renderer);
    SDL_RenderTexture(GlobalState.renderer, GlobalState.texture, NULL, NULL);
    SDL_RenderPresent(GlobalState.renderer);
  }

  Cleanup();

  return 0;
}
