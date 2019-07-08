#ifndef RENDERER
#define RENDERER

#include <memory>
#include <unordered_map>

#include "SDL.h"
#include "opentok.h"

class Renderer;

class RendererManager {
public:
  RendererManager();
  ~RendererManager();
  void runEventLoop();
  
  void createRenderer(void *key);
  void destroyRenderer(void *key);
  void addFrame(void* key, const otc_video_frame *frame);

 private:
  void pushEvent(int code, void* data1, void* data2);
  void handleCustomEvent(const SDL_Event& event);
  std::unordered_map<void*, std::shared_ptr<Renderer>> renderers_;
  Uint32 event_type_ = -1;
};

class Renderer {
 public:
  Renderer(const char* window_name);
  Renderer( const Renderer & ) = delete;
  ~Renderer();

  void onFrame(otc_video_frame* frame);

 private:
 SDL_Window* window_ = nullptr;
};

#endif // RENDERER
