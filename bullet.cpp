#include <iostream>
#include <cmath>
#include "bullet.h"

void Bullet::update(Uint32 ticks) { 
  Vector2f pos = getPosition();
  Sprite::update(ticks);
  distance += ( hypot(getX()-pos[0], getY()-pos[1]) );
  if ( getX() < 0 ) tooFar = true;
  if ( getX() > worldWidth-getScaledWidth()) tooFar = true;
  if (distance > maxDistance) tooFar = true;
}

