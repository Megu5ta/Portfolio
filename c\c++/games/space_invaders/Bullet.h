#ifndef BULLET_H_
#define BULLET_H_

#include <SFML/Graphics.hpp>

class Bullet {
public:
    sf::FloatRect rect;
    sf::Sprite sprite;
    float speed;
    bool alive;
    float currentFrame;

public:
    Bullet();
    Bullet(sf::Texture &image, float speed);
    virtual ~Bullet();

    void update(float myTime);
};

#endif // BULLET_H_
