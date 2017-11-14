#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include "sprite.h"
#include "multisprite.h"
#include "gamedata.h"
#include "engine.h"
#include "frameGenerator.h"
#include "player.h"
#include "collisionStrategy.h"
#include "splitString.h"
#include "smartSprite.h"
#include "hud.h"

Engine::~Engine() { 
  delete player;
  for( auto& item : items_drawable ){
    delete item;
  }
  for( auto& smart_item : smart_items ){
    delete smart_item;
  }
  for( auto& strategy : strategies ){
    delete strategy;
  }
  std::cout << "Terminating program" << std::endl;
}

void Engine::addMultiItem(const std::string& spriteName, int numOfItem){
  int i = 0;
  while( i < numOfItem ){
    items_drawable.push_back(new MultiSprite(spriteName));
    ++i;
  }
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IOmod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  worldSky("sky", Gamedata::getInstance().getXmlInt( "sky/factor" )),
  worldMountain("mountain", Gamedata::getInstance().getXmlInt( "mountain/factor" )),
  worldGrass("grass", Gamedata::getInstance().getXmlInt( "grass/factor" )),
  viewport( Viewport::getInstance() ),
  currentSprite(0),
  player(new Player(Gamedata::getInstance().getXmlStr( "drawableItem/player_list" ))),
  strategies(),
  collision(false),
  currentStrategy(0),
  makeVideo( Gamedata::getInstance().getXmlBool("makeVideo") ),
  showHud( true )
{

  num_of_drawable = Gamedata::getInstance().getXmlInt("drawableItem/numOfItem");
  num_of_sprite = Gamedata::getInstance().getXmlInt("drawableItem/numOfSprite");
  num_of_multiSprite = Gamedata::getInstance().getXmlInt("drawableItem/numOfMultiSprite");
  num_of_player = Gamedata::getInstance().getXmlInt("drawableItem/numOfPlayer");
  num_of_smartItem = Gamedata::getInstance().getXmlInt("drawableItem/numOfSmartItem");
  std::string multiSprite_list_str = Gamedata::getInstance().getXmlStr("drawableItem/multiSprite_list");
  std::string sprite_list_str = Gamedata::getInstance().getXmlStr("drawableItem/sprite_list");

  items_drawable.reserve(num_of_drawable);
  std::vector<std::string> multiSprite_list; 
  std::vector<std::string> sprite_list; 
  SplitString::split(multiSprite_list_str, multiSprite_list, ",");
  SplitString::split(sprite_list_str, sprite_list, ",");

  for( int i = 0; i < num_of_sprite; ++i ){
    int numOfItem = Gamedata::getInstance().getXmlInt(sprite_list[i] + "/numOfItem");
    if ( numOfItem == 1 )
      items_drawable.push_back(new Sprite(sprite_list[i]));
    //else
    //  addMultiItem(sprite_list[i], numOfItem);
  }

  for( int i = 0; i < num_of_multiSprite; ++i ){
    int numOfItem = Gamedata::getInstance().getXmlInt(multiSprite_list[i] + "/numOfItem");
    if ( numOfItem == 1 )
      items_drawable.push_back(new MultiSprite(multiSprite_list[i]));
    else
      addMultiItem(multiSprite_list[i], numOfItem);
  }

  Vector2f pos = player->getPosition();
  int w = player->getScaledWidth();
  int h = player->getScaledHeight();
  smart_items.reserve(10);
  for (int i = 0; i < 10; ++i) {
    SmartSprite* sSprite = new SmartSprite("yellowBird", pos, w, h);
    smart_items.push_back( sSprite );
    player->attach( sSprite );
  }

  
  //for( int i = 0; i < num_of_Sprite; ++i ){
  //  int numOfItem = Gamedata::getInstance().getXmlInt(multiSprite_list[i] + "/numOfItem");
  //  if ( numOfItem == 1 )
  //    items_drawable.push_back(new MultiSprite(multiSprite_list[i]));
  //  else
  //    addMultiItem(multiSprite_list[i], numOfItem);
  //}

  strategies.reserve(3);
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategies.push_back( new MidPointCollisionStrategy );

  Viewport::getInstance().setObjectToTrack(items_drawable[0]);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  worldSky.draw();
  worldMountain.draw();
  worldGrass.draw();

  if(showHud) {
    Hud::getInstance().draw();
    return;
  }

  //for( auto& world : worlds ){
  //  world.draw();
  //}

  for( auto& multiSprite : items_drawable ){
    multiSprite->draw();
  }
  for( auto& smart_item : smart_items ){
    smart_item->draw();
  }
  strategies[currentStrategy]->draw();
  if ( collision ) {
    IOmod::getInstance().writeText("Oops: Collision", 500, 90);
  }
  player->draw();
  viewport.draw();

  std::stringstream ss;
  //The Clock is a singleton so need to use getInstance
  ss << Clock::getInstance().getFps();
  IOmod::getInstance().
    writeText("FPS: "+ ss.str(), 30, 60);

  SDL_RenderPresent(renderer);
}

void Engine::checkForCollisions() {
  collision = false;
  for ( const Drawable* d : items_drawable ) {
    if ( strategies[currentStrategy]->execute(*player, *d) ) {
      collision = true;
    }
  }
  if ( collision ) {
    player->collided();
  }
  else {
    player->missed();
    collision = false;
  }

  /* FOR DETACH AND DELETE */
  //auto it = sprites.begin();
  //while ( it != sprites.end() ) {
  //  if ( strategies[currentStrategy]->execute(*player, **it) ) {
  //    //SmartSprite* doa = *it;
  //    //player->detach(doa);
  //    //delete doa;
  //    //it = sprites.erase(it);
  //    player->detach(*it);
  //    delete *it;
  //    it = sprites.erase(it);
  //  }
  //  else ++it;
  //}

}
void Engine::update(Uint32 ticks) {
  static int t = 0;
  static int maxTime = Gamedata::getInstance().getXmlInt( "Hud/maxTime" );
  ++t;
  if( t > maxTime ) {
    showHud = false;
    t = 0;
  }
  checkForCollisions();
  player->update(ticks);
  for( auto& multiSprite: items_drawable ){
    multiSprite->update(ticks);
  }
  for( auto& smart_item : smart_items ){
    smart_item->update(ticks);
  }

  worldSky.update();
  worldMountain.update();
  worldGrass.update();
  //for( auto& world : worlds ){
  //  world.update();
  //}
  viewport.update(); // always update viewport last
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % num_of_drawable;
  Viewport::getInstance().setObjectToTrack(items_drawable[currentSprite]);
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if ( keystate[SDL_SCANCODE_T] ) {
          switchSprite();
        }
        if ( keystate[SDL_SCANCODE_C] ){
          Viewport::getInstance().setObjectToTrack(player);
        }
        if ( keystate[SDL_SCANCODE_M] ) {
          currentStrategy = (1 + currentStrategy) % strategies.size();
        }
        if ( keystate[SDL_SCANCODE_F1] ) {
          showHud = showHud ? false : true;
        }
        if ( keystate[SDL_SCANCODE_F4] && !makeVideo ) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:
    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      if (keystate[SDL_SCANCODE_A]) {
        player->left();
      }
      if (keystate[SDL_SCANCODE_D]) {
        player->right();
      }
      if (keystate[SDL_SCANCODE_W]) {
        player->up();
      }
      if (keystate[SDL_SCANCODE_S]) {
        player->down();
      }
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
