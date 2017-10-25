#ifndef PLAYER__H
#define PLAYER__H

#include <string>
#include <list>
#include "multisprite.h"

class SmartSprite;

class Player : public MultiSprite {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual void update(Uint32 ticks);

  void collided() { collision = true; }
  void missed() { collision = false; }
  Player& operator=(const Player&);

  void attach( SmartSprite* o ) { observers.push_front(o); } 
  void detach( SmartSprite* o );

  void right();
  void left();
  void up();
  void down();
  void stop();
private:
  bool collision;
  std::list<SmartSprite*> observers;
  Vector2f initialVelocity;
};
#endif
