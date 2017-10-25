#include "gamedata.h"
#include "drawable.h"

Vector2f makeVelocity( const Vector2f& vel ) {
  float newvx = Gamedata::getInstance().getRandFloat(vel[0]-50,vel[0]+50);;
  float newvy = Gamedata::getInstance().getRandFloat(vel[1]-50,vel[1]+50);;
  newvx *= [](){ if(rand()%2) return -1; else return 1; }();
  newvy *= [](){ if(rand()%2) return -1; else return 1; }();

  return Vector2f(newvx, newvy);
}
