#include <map>
#include <memory>

#include "SDL.h"
#include "opentok.h"

class Renderer;

class RendererManager {
 public:
  RendererManager();
  ~RendererManager();
  void runEventLoop();  // must be called from same thread as the one who created the instance

  // Thread safe functions
  void addFrame(void* key, const otc_video_frame *frame);
  void createRenderer(void *key);
  void destroyRenderer(void *key);

 private:
  void pushEvent(int code, void* data1, void* data2);
  void handleCustomEvent(const SDL_Event& event);
  std::map<void*, std::shared_ptr<Renderer>> renderers;
  Uint32 event_type_ = -1;
};

class Renderer {
 public:
  Renderer(const char* window_name);
  Renderer( const Renderer & ) = delete;
  ~Renderer();

  void onFrame(otc_video_frame* frame);
 private:

  SDL_Window*    window_ = NULL;
};

