#ifndef SHIP_H_
#define SHIP_H_

#include <SFML/Graphics.hpp>

class Ship {
public:
    float dx;
    sf::FloatRect rect;
    sf::Sprite sprite;
    float speed;    
    float currentFrame;
    char direction = 0; // -1: left // 0: dont move // 1: right
    bool alive;
    

public:
    Ship();
    Ship(sf::Texture &image);
    virtual ~Ship();

    void update(float myTime);
};

#endif // SHIP_H_
