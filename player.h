#ifndef PLAYER__H
#define PLAYER__H

#include <string>
#include <list>
#include "multisprite.h"
#include "bullet.h"

class SmartSprite;
class Bullet;

class Player : public MultiSprite {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual void update(Uint32 ticks);
  virtual void draw() const;

  void shoot();

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
  std::string bulletName;
  std::list<Bullet> bullets;
  float minSpeed;
  float bulletInterval;
  float timeSinceLastFrame;
  std::list<SmartSprite*> observers;
  Vector2f initialVelocity;
};
#endif
