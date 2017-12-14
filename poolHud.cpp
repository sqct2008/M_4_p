#include <sstream>
#include "ioMod.h"
#include "renderContext.h"
#include "poolHud.h"
#include "player.h"

PoolHud::PoolHud(const Player& p) :
  player(p),
  rc(RenderContext::getInstance()),
  renderer(rc -> getRenderer()),
  io(IOmod::getInstance()),
  poolHudRect({500, 70, 280, 140}),
  showHud(true)
{}


void PoolHud::draw() const {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255/2);
  SDL_RenderFillRect(renderer, &poolHudRect);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255/2);
  SDL_RenderDrawRect(renderer, &poolHudRect);

  std::stringstream ss;
  //io.setFontColor(0, 0, 0); 
  ss << "Bullets : "<< player.getShootSize();
  io.writeText(ss.str(), 520, 100);

  ss.clear();
  ss.str("");
  ss << "Pool : " << player.getPoolSize(); 
  io.writeText(ss.str(), 520, 130);

  ss.clear();
  ss.str("");
  ss << "H show THIS"; 
  io.writeText(ss.str(), 520, 160);   
}
