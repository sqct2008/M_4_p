#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include "sprite.h"
#include "multisprite.h"
#include "gamedata.h"
#include "engine.h"
#include "frameGenerator.h"

Engine::~Engine() { 
  delete star;
  delete spinningStar;
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IOmod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  worldF("front", Gamedata::getInstance().getXmlInt( "front/factor" )),
  worldB("back", Gamedata::getInstance().getXmlInt( "back/factor" )),
  viewport( Viewport::getInstance() ),
  star(new Sprite("YellowStar")),
  spinningStar(new MultiSprite("SpinningStar")),
  currentSprite(0),
  makeVideo( Gamedata::getInstance().getXmlBool("makeVideo") )
{
  int num_of_drawable = Gamedata::getInstance().getXmlInt("drawableItem/numOfItem");
  std::string drawable_list_str = Gamedata::getInstance().getXmlStr("drawableItem/drawable_name_list");
  items_drawable.reserve(num_of_drawable);
  std::vector<std::string> drawable_list; 
  drawable_list.reserve(num_of_drawable);
  SplitString::split(drawable_list_str, drawable_list, ",");

  for( auto& item : drawable_list ){
    items_drawable.push_back(new MultiSprite(item));
  }

  Viewport::getInstance().setObjectToTrack(star);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  worldB.draw();
  worldF.draw();
  //for( auto& world : worlds ){
  //  world.draw();
  //}

  star->draw();
  spinningStar->draw();
  for( auto& multiSprite: items_drawable ){
    multiSprite->draw();
  }

  viewport.draw();

  std::stringstream ss;
  //The Clock is a singleton so need to use getInstance
  ss << Clock::getInstance().getFps();
  IOmod::getInstance().
    writeText("FPS: "+ ss.str(), 30, 60);

  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  star->update(ticks);
  spinningStar->update(ticks);
  for( auto& multiSprite: items_drawable ){
    multiSprite->update(ticks);
  }

  worldB.update();
  worldF.update();
  //for( auto& world : worlds ){
  //  world.update();
  //}
  viewport.update(); // always update viewport last
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % 2;
  if ( currentSprite ) {
    Viewport::getInstance().setObjectToTrack(spinningStar);
  }
  else {
    Viewport::getInstance().setObjectToTrack(star);
  }
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
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
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
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
