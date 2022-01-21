#ifndef ENEMY_H_
#define ENEMY_H_

#include <SFML/Graphics.hpp>

class Enemy {
public:
    sf::FloatRect rect;
    sf::Sprite sprite;
    float dx, dy;
    float speed;
    char direction;
    float vShift;
    bool alive;
    float currentFrame;
    int reward;

public:
    Enemy();
    Enemy(sf::Texture &image, int reward);
    virtual ~Enemy();
    
    void update(float myTime);
};

#endif // ENEMY_H_
