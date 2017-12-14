#include "player.h"
#include "smartSprite.h"
#include "bullet.h"
#include "explodingSprite.h"
//#include "gamedata.h"

Player::Player( const std::string& name) :
  MultiSprite(name),
  collision(false),
  bulletName( Gamedata::getInstance().getXmlStr(name+"/bullet") ),
  bullets(),
  poolBullets(),
  minSpeed( Gamedata::getInstance().getXmlInt(bulletName+"/speedX") ),
  bulletInterval(Gamedata::getInstance().getXmlInt(bulletName+"/interval")),
  timeSinceLastShoot(0),
  observers(),
  initialVelocity(getVelocity())
{ std::cout << getVelocity() << std::endl;}

Player::Player(const Player& s) :
  MultiSprite(s), 
  collision(s.collision),
  bulletName(s.bulletName),
  bullets(s.bullets),
  poolBullets(s.poolBullets),
  minSpeed(s.minSpeed),
  bulletInterval(s.bulletInterval),
  timeSinceLastShoot(s.timeSinceLastShoot),
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

void Player::explode() {
  if ( !explosion ) {
    Sprite sprite(getName(), getPosition(), getVelocity(), images[currentFrame]);
    explosion = new ExplodingSprite(sprite);
  }
}

static int timeSinceLastExplode = 0;

void Player::update(Uint32 ticks) {
  if (explosion) {
    timeSinceLastExplode += ticks;
    explosion->update(ticks);
    if (timeSinceLastExplode >= 2000 || explosion->chunkCount() == 0) {
      timeSinceLastExplode = 0;
      delete explosion;
      explosion = NULL;
    }
  }
  else {
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
  MultiSprite::update(ticks);

  for ( Bullet& bullet : bullets ) {
    bullet.update(ticks);
  }

  }

	timeSinceLastShoot += ticks;

  if ( !collision ) advanceFrame(ticks);


  std::list<SmartSprite*>::iterator ptr = observers.begin();
  while ( ptr != observers.end() ) {
    (*ptr)->setPlayerPos( getPosition() );
    ++ptr;
  }

  //timeSinceLastShoot += ticks;
  std::list<Bullet>::iterator it = bullets.begin();
  while (it != bullets.end()) {
    it->update(ticks);
    if (it->goneTooFar()) {
      poolBullets.push_back(*it);
      it = bullets.erase(it);
    }
    else {
      it++;
    }
  }
  stop();
}

void Player::shoot() { 
  if ( timeSinceLastShoot < bulletInterval ) return;
  float deltaX = getScaledWidth();
  float deltaY = getScaledHeight()/2;
  // I need to add some minSpeed to velocity:
  //bullet.setVelocity( getVelocity() + Vector2f(minSpeed, 0) );
  if (poolBullets.empty()) {
    Bullet bullet(bulletName);
    bullet.setPosition( getPosition() + Vector2f(deltaX, deltaY) );
    if(currentAction == goLeft) {
      bullet.setVelocity( getVelocity() + Vector2f(-minSpeed, 0) );
    }
    else {
      bullet.setVelocity( getVelocity() + Vector2f(minSpeed, 0) );

    }
    //Bullet bullet(bulletName, 1, angle, pos, vel);
    bullets.push_back(bullet);
  }
  else {
    Bullet bullet = poolBullets.front();
    poolBullets.pop_front();
    bullet.setPosition( getPosition() + Vector2f(deltaX, deltaY) );
    if(currentAction == goLeft) {
      bullet.setVelocity( getVelocity() + Vector2f(-minSpeed, 0) );
    }
    else {
      bullet.setVelocity( getVelocity() + Vector2f(minSpeed, 0) );

    }
    bullet.reset();
    bullets.push_back(bullet);
  }
  timeSinceLastShoot = 0;
}

void Player::draw() const { 
  if (explosion) {
    explosion->draw();
  }
  else{
    MultiSprite::draw();
    for ( const Bullet& bullet : bullets ) {
      bullet.draw();
    }
  }
}

