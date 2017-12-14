#include <vector>
#include <SDL2/SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"

class Player;
class SmartSprite;
class MultiSprite;
class CollisionStrategy;
class PoolHud;

class Engine {
public:
  Engine ();
  ~Engine ();
  bool play();
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
  MultiSprite* boss;
  std::vector<Drawable*> far_drawable;
  std::vector<Drawable*> items_drawable;
  std::vector<SmartSprite*> smart_items;
  std::vector<CollisionStrategy*> strategies;
  bool collision;
  int currentStrategy;

  bool makeVideo;
  bool showHud;
  bool gameOver;
  PoolHud* poolHud;

  void draw() const;
  void update(Uint32);

  Engine& operator=(const Engine&);
  //void printScales() const;
  void checkForCollisions();
  void checkForBulletsCollisions();
  void addMultiItem(std::vector<Drawable*>&, const std::string&, int);
  int smartCount;
};
