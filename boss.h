#include <string>
#include <iostream>
#include <list>
#include "sprite.h"
#include "bullet.h"

class ShootingSprite : public Sprite {
public:
  ShootingSprite(const std::string& n);
  ShootingSprite(const ShootingSprite& s);

  virtual void update(Uint32 ticks);
  virtual void draw() const;
  void shoot();

private:
  std::string bulletName;
  std::list<Bullet> bullets;
  float minSpeed;
  float bulletInterval;
  float timeSinceLastFrame;
  ShootingSprite& operator=(const ShootingSprite&);
};
