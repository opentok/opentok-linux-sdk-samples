#include "renderer.h"

#include <iostream>
#include <cassert>

enum kEventTypes {
  kRendererCreate = 0,
  kRendererDraw = 1,
  kRendererDestroy = 2
};

RendererManager::RendererManager() {
  SDL_Init(SDL_INIT_VIDEO);
  event_type_ = SDL_RegisterEvents(1);
}

RendererManager::~RendererManager() {
  SDL_Quit();
}

void RendererManager::runEventLoop() {
  SDL_Event event;
  bool quit = false;
  while (!quit)    {
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      if (event.type == SDL_KEYDOWN) {
        SDL_Keycode keyPressed = event.key.keysym.sym;
        switch (keyPressed) {
          case SDLK_ESCAPE:
            quit = true;
            break;
        }
      }
      if (event.type == event_type_) {
        handleCustomEvent(event);
      }
    }
  }
}

void RendererManager::handleCustomEvent(const SDL_Event& event) {
  assert(event.type == event_type_);
  if (event.user.code == kRendererCreate) {
    renderers[event.user.data1] = std::make_shared<Renderer>("test");
  }
  if (event.user.code == kRendererDraw) {
    if (renderers.count(event.user.data1) == 0) {
      std::cout << "Trying to draw on non created renderer" << std::endl;
    } else {
      otc_video_frame*  frame = (otc_video_frame*)event.user.data2;
      renderers[event.user.data1]->onFrame(frame);
      otc_video_frame_delete(frame);
    }
  }
  if (event.user.code == kRendererDestroy) {
    if (renderers.count(event.user.data1) == 0) {
      std::cout << "Trying to destroy a non existent renderer" << std::endl;
    } else {
      renderers.erase(event.user.data1);
    }
  }
}

void RendererManager::createRenderer(void* key) {
  std::cout << "CreateRenderer with key: " << key << std::endl;
  pushEvent(kRendererCreate, key, NULL);
}

// Can be called from different threads
void RendererManager::addFrame(void* key, const otc_video_frame *frame) {
  pushEvent(kRendererDraw, key, (void*)otc_video_frame_convert(OTC_VIDEO_FRAME_FORMAT_ARGB32, frame));
}

void RendererManager::destroyRenderer(void *key) {
  std::cout << "DestroyRenderer with key: " << key << std::endl;
  pushEvent(kRendererDestroy, key, NULL);
}


void RendererManager::pushEvent(int code, void* data1, void* data2) {
  SDL_Event event;
  SDL_zero(event);
  event.type = event_type_;
  event.user.code = code;
  event.user.data1 = data1;
  event.user.data2 = data2;
  SDL_PushEvent(&event);
}

Renderer::Renderer(const char* name) {
  window_ = SDL_CreateWindow(name,
                             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             640, 480, SDL_WINDOW_RESIZABLE
  );
  if (!window_) {
    std::cout << "Could not create window" << std::endl;
  } else {
    std::cout << "Window created successfully" << std::endl;
  }
}
Renderer::~Renderer() {
  if (window_) {
    SDL_DestroyWindow(window_);
  }
}

void Renderer::onFrame(otc_video_frame* frame) {
  if (!window_) {
    return;
  }
  SDL_Surface* surface_ =  SDL_GetWindowSurface(window_);

  auto pixels = otc_video_frame_get_plane_binary_data(frame, static_cast<enum otc_video_frame_plane>(0));
  SDL_Surface* sdl_frame = SDL_CreateRGBSurfaceFrom(
      const_cast<unsigned char*>(pixels),
      otc_video_frame_get_width(frame),
      otc_video_frame_get_height(frame),
      32,
      otc_video_frame_get_width(frame) * 4,
      0,0,0,0);

  SDL_BlitScaled(sdl_frame, NULL, surface_, NULL); // blit it to the screen
  SDL_FreeSurface(sdl_frame);

  SDL_UpdateWindowSurface(window_);

}
