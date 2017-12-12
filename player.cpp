#include "player.h"
#include "smartSprite.h"
//#include "gamedata.h"

Player::Player( const std::string& name) :
  MultiSprite(name),
  collision(false),
  bulletName( Gamedata::getInstance().getXmlStr(name+"/bullet") ),
  bullets(),
  minSpeed( Gamedata::getInstance().getXmlInt(bulletName+"/speedX") ),
  bulletInterval(Gamedata::getInstance().getXmlInt(bulletName+"/interval")),
  timeSinceLastFrame(0),
  observers(),
  initialVelocity(getVelocity())
{ std::cout << getVelocity() << std::endl;}

Player::Player(const Player& s) :
  MultiSprite(s), 
  collision(s.collision),
  bulletName(s.bulletName),
  bullets(s.bullets),
  minSpeed(s.minSpeed),
  bulletInterval(s.bulletInterval),
  timeSinceLastFrame(s.timeSinceLastFrame),
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
	timeSinceLastFrame += ticks;
  MultiSprite::update(ticks);
  for ( Bullet& bullet : bullets ) {
    bullet.update(ticks);
  }

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

void Player::shoot() { 
  if ( timeSinceLastFrame < bulletInterval ) return;
  float deltaX = getScaledWidth();
  float deltaY = getScaledHeight()/2;
  // I need to add some minSpeed to velocity:
  Bullet bullet(bulletName);
  bullet.setPosition( getPosition() +Vector2f(deltaX, deltaY) );
  bullet.setVelocity( getVelocity() + Vector2f(minSpeed, 0) );
  bullets.push_back( bullet );
  timeSinceLastFrame = 0;
}

void Player::draw() const { 
  MultiSprite::draw();
  for ( const Bullet& bullet : bullets ) {
    bullet.draw();
  }
}

