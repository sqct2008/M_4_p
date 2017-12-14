#ifndef PLAYER__H
#define PLAYER__H

#include <string>
#include <list>
#include "multisprite.h"

class SmartSprite;
class Bullet;

class Player : public MultiSprite {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual void update(Uint32 ticks);
  virtual void draw() const;

  void shoot();
  virtual void explode();

  void collided() { collision = true; }
  void missed() { collision = false; }
  Player& operator=(const Player&);

  void attach( SmartSprite* o ) { observers.push_front(o); } 
  void detach( SmartSprite* o );

  std::list<Bullet>& getBulletList() { return bullets; } 
  int getPoolSize() const { return poolBullets.size(); }
  int getShootSize() const { return bullets.size(); }

  void right();
  void left();
  void up();
  void down();
  void stop();
private:
  bool collision;
  ExplodingSprite* explosion = nullptr;
  std::string bulletName;
  std::list<Bullet> bullets;
  std::list<Bullet> poolBullets;
  float minSpeed;
  float bulletInterval;
  float timeSinceLastShoot;
  std::list<SmartSprite*> observers;
  Vector2f initialVelocity;
};
#endif
