#ifndef SHOT_H_
#define SHOT_H_

#include "Bullet.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Shot {
public:
    sf::Texture m_t;
    std::vector<Bullet *> pBullets;
    float speed;
    float cooldown;
    float timeAfterShot;
    float rechargingSpeed;
    bool recharging;    

public:
    Shot(sf::Texture &image);
    virtual ~Shot();

    void spawnBullet();
    void removeBullet(size_t pos);
    void update(float myTime);
};


#endif // SHOT_H_
