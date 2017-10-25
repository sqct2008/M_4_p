#include <vector>
#include <SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"

class Player;
class SmartSprite;
class CollisionStrategy;

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

  int currentSprite;
  int num_of_drawable = 0;
  int num_of_sprite = 0;
  int num_of_multiSprite = 0;
  int num_of_player = 0;
  int num_of_smartItem = 0;

  Player* player;
  std::vector<Drawable*> items_drawable;
  std::vector<CollisionStrategy*> strategies;
  std::vector<SmartSprite*> smart_items;
  bool collision;
  int currentStrategy;

  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine& operator=(const Engine&);
  //void printScales() const;
  void checkForCollisions();
  void addMultiItem(const std::string&, int);
};
