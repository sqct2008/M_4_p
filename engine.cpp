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
#include "boss.h"
#include "hud.h"
#include "poolHud.h"

Engine::~Engine() {
  delete player;
  delete boss;
  delete poolHud;
  for( auto& item : items_drawable ){
    delete item;
  }
  for( auto& smart_item : smart_items ){
    delete smart_item;
  }
  for( auto& far_item : far_drawable ){
    delete far_item;
  }
  for( auto& strategy : strategies ){
    delete strategy;
  }
  std::cout << "Terminating program" << std::endl;
}

void Engine::addMultiItem(std::vector<Drawable*>& dList, const std::string& spriteName, int numOfItem){
  int i = 0;
  while( i < numOfItem ){
    dList.push_back(new MultiSprite(spriteName));
    ++i;
  }
    //player->attach( smart_items[i] );
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
  boss(new MultiSprite(Gamedata::getInstance().getXmlStr( "drawableItem/Boss" ))),
  far_drawable(),
  items_drawable(),
  smart_items(),
  strategies(),
  collision(false),
  currentStrategy(1),
  makeVideo( Gamedata::getInstance().getXmlBool("makeVideo") ),
  showHud( true ),
  gameOver(false),
  poolHud(new PoolHud(*player)),
  smartCount(0)
{
  clock.startClock();

  num_of_drawable = Gamedata::getInstance().getXmlInt("drawableItem/numOfItem");
  num_of_sprite = Gamedata::getInstance().getXmlInt("drawableItem/numOfSprite");
  num_of_multiSprite = Gamedata::getInstance().getXmlInt("drawableItem/numOfMultiSprite");
  num_of_player = Gamedata::getInstance().getXmlInt("drawableItem/numOfPlayer");
  num_of_smartItem = Gamedata::getInstance().getXmlInt("drawableItem/numOfSmartItem");
  int num_of_farItem = Gamedata::getInstance().getXmlInt("drawableItem/numOfFarItem");

  std::string multiSprite_list_str;
  std::string sprite_list_str;
  std::string smart_list_str;
  std::string far_list_str;

  if(num_of_multiSprite) {
    multiSprite_list_str = Gamedata::getInstance().getXmlStr("drawableItem/multiSprite_list");
    std::vector<std::string> multiSprite_list;
    SplitString::split(multiSprite_list_str, multiSprite_list, ",");
    for( int i = 0; i < num_of_multiSprite; ++i ){
      int numOfItem = Gamedata::getInstance().getXmlInt(multiSprite_list[i] + "/numOfItem");
      if ( numOfItem == 1 )
        items_drawable.push_back(new MultiSprite(multiSprite_list[i]));
      else
        addMultiItem(items_drawable, multiSprite_list[i], numOfItem);
    }
  }
  if(num_of_sprite) {
    sprite_list_str = Gamedata::getInstance().getXmlStr("drawableItem/sprite_list");
    std::vector<std::string> sprite_list;
    SplitString::split(sprite_list_str, sprite_list, ",");
    for( int i = 0; i < num_of_sprite; ++i ){
      int numOfItem = Gamedata::getInstance().getXmlInt(sprite_list[i] + "/numOfItem");
      if ( numOfItem == 1 )
        items_drawable.push_back(new Sprite(sprite_list[i]));
      else
        addMultiItem(items_drawable, sprite_list[i], numOfItem);
    }

  }
  if(num_of_smartItem) {
    smart_list_str = Gamedata::getInstance().getXmlStr("drawableItem/smart_list");
    std::vector<std::string> smart_list;
    SplitString::split(smart_list_str, smart_list, ",");
    Vector2f pos = player->getPosition();
    int w = player->getScaledWidth();
    int h = player->getScaledHeight();
    for (int i = 0; i < num_of_smartItem; ++i) {
      for(int j=0; j<3; j++) {
        smart_items.push_back(new SmartSprite(smart_list[i], pos, w, h));
        player->attach( smart_items[i+j] );
      }
    }
    smartCount = num_of_smartItem * 3;
  }
  if(num_of_farItem) {
    far_list_str = Gamedata::getInstance().getXmlStr("drawableItem/far_list");
    std::vector<std::string> far_list;
    SplitString::split(far_list_str, far_list, ",");
    for( int i = 0; i < num_of_farItem; ++i ){
      int numOfItem = Gamedata::getInstance().getXmlInt(far_list[i] + "/numOfItem");
      if ( numOfItem == 1 )
        far_drawable.push_back(new MultiSprite(far_list[i]));
      else
        addMultiItem(far_drawable, far_list[i], numOfItem);
    }

    for(auto it : far_drawable) {
      it -> setScale(0.4);
      it -> setVelocityX(it->getVelocityX()*0.7);
      it -> setVelocityY(it->getVelocityY()*0.2);
    }

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

  Viewport::getInstance().setObjectToTrack(player);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  worldSky.draw();

  if(showHud) {
    worldMountain.draw();
    worldGrass.draw();
    Hud::getInstance().draw();
    SDL_RenderPresent(renderer);
    return;
  }
  else if(gameOver)
    clock.pause();
  worldMountain.draw();
  for(auto it : far_drawable) {
    it -> draw();
  }
  worldGrass.draw();

  //for( auto& world : worlds ){
  //  world.draw();
  //}

  for( auto& multiSprite : items_drawable ){
    multiSprite->draw();
  }
  for( auto& smart_item : smart_items ){
    smart_item->draw();
  }
  if(poolHud -> isShowHud())
    poolHud -> draw();
  strategies[currentStrategy]->draw();
  //if ( collision ) {
  //  IOmod::getInstance().writeText("Oops: Collision", 500, 90);
  //}
  player->draw();
  boss->draw();
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
  if ( strategies[currentStrategy]->execute(*player, *boss) )
    collision = true;

  //for ( const Drawable* d : items_drawable ) {
  //  if ( strategies[currentStrategy]->execute(*player, *d) ) {
  //    collision = true;
  //  }
  //}
  if ( collision ) {
    //player->collided();
    player->explode();    
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

void Engine::checkForBulletsCollisions() {
  std::list<Bullet>& bulletList = player->getBulletList();
  if (bulletList.size() > 0) {
    std::list<Bullet>::iterator it = bulletList.begin();
    while (it != bulletList.end()) {
      auto its = smart_items.begin();
      while (its != smart_items.end()) {
        SmartSprite* doa = *its;
        if ( strategies[currentStrategy]->execute(*it, *doa) && !doa->isCollided()) {         
          //unsigned int balloonColor = doa->getCurrentFrame();
          player->detach(doa);
          doa->collided();
          doa->explode();    
          smartCount--;
        }
        its++;     
      } 
      it++;  
    }
  }

}

static int t = 0;
void Engine::update(Uint32 ticks) {
  player->update(ticks);
  boss->update(ticks);

  for( auto& far_item : far_drawable ){
    far_item->update(ticks);
  }
  for( auto& multiSprite: items_drawable ){
    multiSprite->update(ticks);
  }
  for( auto& smart_item : smart_items ){
    smart_item->update(ticks);
  }

  worldSky.update();
  worldMountain.update();
  worldGrass.update();
  // handle hud update
  static int maxTime = Gamedata::getInstance().getXmlInt( "Hud/maxTime" );
  if(showHud) {
    if( t < maxTime )
        ++t;
    else {
        showHud = false;
        t = 0;
    }
  }
  else {
    checkForBulletsCollisions();
    checkForCollisions();
    viewport.update(); // always update viewport last
  }
    //std::cout << smartCount << std::endl;
  if (smartCount == 0) {
    gameOver = true;
  }
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % num_of_drawable;
  Viewport::getInstance().setObjectToTrack(items_drawable[currentSprite]);
}

bool Engine::play() {
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
        if ( keystate[SDL_SCANCODE_H] ) {
          poolHud -> setShow();
        }
        if ( keystate[SDL_SCANCODE_F1] ) {
          showHud = showHud ? false : true;
          t = 0;
        }
        if ( keystate[SDL_SCANCODE_F4] && !makeVideo ) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
        if ( keystate[SDL_SCANCODE_F5] ) {
          std::cout << "Restart the game" << std::endl;
          return true;
        }
      }
    }

    // In this section of the event loop we allow key bounce:
    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
        if ( keystate[SDL_SCANCODE_SPACE] ) {
          player -> shoot();
        }
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
  return false;
}
