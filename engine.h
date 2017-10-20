#include <vector>
#include <SDL.h>
#include <sstream>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "splitString.h"

class Engine {
public:
  Engine ();
  ~Engine ();
  void play();
  void switchSprite();
  Engine( const Engine& ) = delete;

private:
  const RenderContext* rc;
  const IOmod& io;
  Clock& clock;

  SDL_Renderer * const renderer;
  //std::vector<World> worlds;
  World worldSky;
  World worldMountain;
  World worldGrass;
  Viewport& viewport;

//  Drawable* star;
//  Drawable* spinningStar;
  int currentSprite;
  int num_of_drawable = 0;

  std::vector<Drawable*> items_drawable;

  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine& operator=(const Engine&);
  void printScales() const;
  void checkForCollisions();
};
