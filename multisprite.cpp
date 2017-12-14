#include "multisprite.h"
#include "gamedata.h"
#include "renderContext.h"
#include "explodingSprite.h"

void MultiSprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = currentAction * numberOfFrames + (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

MultiSprite::MultiSprite( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           makeVelocity(Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY")))
           ),
  images( RenderContext::getInstance()->getImages(name) ),
  currentFrame(0),
  currentAction(goRight),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  numberOfActions( Gamedata::getInstance().getXmlInt(name+"/actions") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval")),
  timeSinceLastFrame( 0 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

MultiSprite::MultiSprite(const MultiSprite& s) :
  Drawable(s), 
  images(s.images),
  currentFrame(s.currentFrame),
  currentAction(s.currentAction),
  numberOfFrames( s.numberOfFrames ),
  numberOfActions( s.numberOfActions ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight )
  { }

MultiSprite& MultiSprite::operator=(const MultiSprite& s) {
  Drawable::operator=(s);
  images = (s.images);
  currentFrame = (s.currentFrame);
  currentAction = (s.currentAction);
  numberOfFrames = ( s.numberOfFrames );
  numberOfActions = ( s.numberOfActions );
  frameInterval = ( s.frameInterval );
  timeSinceLastFrame = ( s.timeSinceLastFrame );
  worldWidth = ( s.worldWidth );
  worldHeight = ( s.worldHeight );
  return *this;
}

void MultiSprite::explode() {
  if ( !explosion ) {
    Sprite sprite(getName(), getPosition(), getVelocity(), images[currentFrame]);
    explosion = new ExplodingSprite(sprite);
  }
}

void MultiSprite::draw() const { 
  if (explosion) {
    explosion->draw();
  }
  else {
    images[currentFrame]->draw(getX(), getY(), getScale());
  }  
}

void MultiSprite::update(Uint32 ticks) { 
  if (explosion) {
    explosion->update(ticks);
    if (explosion->chunkCount() == 0) {
      //delete explosion;
      //explosion = NULL;
    }
    return;
  }

  advanceFrame(ticks);

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( getY() < 0) {
    setVelocityY( fabs( getVelocityY() ) );
  }
  if ( getY() > worldHeight-getScaledHeight()) {
    setVelocityY( -fabs( getVelocityY() ) );
  }

  if ( getX() < 0) {
    setVelocityX( fabs( getVelocityX() ) );
    if( numberOfActions > 1 ){
      if( currentAction == goLeft ){
        currentAction = goRight;
        //currentAction %= numberOfActions;
      }
    }
  }
  if ( getX() > worldWidth-getScaledWidth()) {
    setVelocityX( -fabs( getVelocityX() ) );
    if( numberOfActions > 1 ){
      if( currentAction == goRight ){
        currentAction = goLeft;
        //currentAction %= numberOfActions;
      }
    }
  }  
}
