#include "player.h"
#include "smartSprite.h"

Player::Player( const std::string& name) :
  MultiSprite(name),
  collision(false),
  observers(),
  initialVelocity(getVelocity())
{ std::cout << getVelocity() << std::endl;}

Player::Player(const Player& s) :
  MultiSprite(s), 
  collision(s.collision),
  observers(s.observers),
  initialVelocity(s.getVelocity())
  { }

Player& Player::operator=(const Player& s) {
  MultiSprite::operator=(s);
  collision = s.collision;
  initialVelocity = s.initialVelocity;
  return *this;
}

void Player::stop() { 
  setVelocity(Vector2f(0,0));
}

void Player::right() { 
  if ( getX() < worldWidth-getScaledWidth()) {
    setVelocityX(fabs(initialVelocity[0]));
    currentAction = goRight;
  }
} 
void Player::left()  { 
  if ( getX() > 0) {
    setVelocityX(-fabs(initialVelocity[0]));
    currentAction = goLeft;
  }
} 
void Player::up()    { 
  if ( getY() > 0) {
    setVelocityY( -fabs(initialVelocity[1]) );
  }
} 
void Player::down()  { 
  if ( getY() < worldHeight-getScaledHeight()) {
    setVelocityY( fabs(initialVelocity[1]) );
  }
}

void Player::detach( SmartSprite* o ) {
  std::list<SmartSprite*>::iterator ptr = observers.begin();
  while ( ptr != observers.end() ) {
    if ( *ptr == o ) {
      ptr = observers.erase(ptr);
      return;
    }
    ++ptr;
  }
}

void Player::update(Uint32 ticks) {
  if ( !collision ) advanceFrame(ticks);

  if ( getVelocityX() > 0) {
    if( numberOfActions > 1 ){
      if( currentAction != goRight ){
        currentAction = goRight;
      }
    }
  }
  if ( getVelocityX() < 0) {
    if( numberOfActions > 1 ){
      if( currentAction != goLeft ){
        currentAction = goLeft;
      }
    }
  }

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  std::list<SmartSprite*>::iterator ptr = observers.begin();
  while ( ptr != observers.end() ) {
    (*ptr)->setPlayerPos( getPosition() );
    ++ptr;
  }

  stop();
}

