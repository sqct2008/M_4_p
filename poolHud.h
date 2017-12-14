#include <SDL2/SDL.h>

class Player;
class IOmod;
class RenderContext;

class PoolHud {
public:
  PoolHud(const Player&);
  void draw() const;
  bool isShowHud() {return showHud;}
  PoolHud(const PoolHud&) = delete;
  PoolHud& operator=(const PoolHud&) = delete;   
  void setShow() { showHud = showHud ? false : true; }
private:
  const Player& player;
  const RenderContext* rc;
  SDL_Renderer* const renderer;
  IOmod& io;
  SDL_Rect poolHudRect;
  bool showHud;
};

