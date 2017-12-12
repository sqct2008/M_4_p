#include "shootingSprite.h"
#include "gamedata.h"

ShootingSprite::ShootingSprite(const std::string& name) :
  Sprite(name),
  bulletName( Gamedata::getInstance().getXmlStr(name+"/bullet") ),
  bullets(),
  minSpeed( Gamedata::getInstance().getXmlInt(bulletName+"/speedX") ),
  bulletInterval(Gamedata::getInstance().getXmlInt(bulletName+"/interval")),
  timeSinceLastFrame(0)
{ }

ShootingSprite::ShootingSprite(const ShootingSprite& s) :
  Sprite(s),
  bulletName(s.bulletName),
  bullets(s.bullets),
  minSpeed(s.minSpeed),
  bulletInterval(s.bulletInterval),
  timeSinceLastFrame(s.timeSinceLastFrame)
{ }

void ShootingSprite::shoot() { 
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

void ShootingSprite::draw() const { 
  Sprite::draw();
  for ( const Bullet& bullet : bullets ) {
    bullet.draw();
  }
}

void ShootingSprite::update(Uint32 ticks) { 
	timeSinceLastFrame += ticks;
  Sprite::update(ticks);
  for ( Bullet& bullet : bullets ) {
    bullet.update(ticks);
  }
}

