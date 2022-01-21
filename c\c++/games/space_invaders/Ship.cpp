#include "Ship.h"

Ship::Ship() {
    
}

Ship::Ship(sf::Texture &image): 
            dx(0.0), direction(0), currentFrame(0.0), alive(0), speed(0.2)  {
    
    sprite.setTexture(image);
    sprite.setTextureRect(sf::IntRect(0, 0, 128, 128));
    rect = sf::FloatRect(576,600,128,128);
}

void Ship::update(float myTime) {

    dx = speed * direction;
    
    rect.left += dx * myTime;
    
// Collision with window
    if (rect.left <= 0.0)
        rect.left = 0.0;

    if (rect.left + rect.width >= 1280)
        rect.left = 1280 - rect.width;

    if (direction != 0)
        currentFrame += 0.005 * myTime;


    if (currentFrame > 4)
        currentFrame -= 3;

    if (dx == 0.0)
        sprite.setTextureRect(sf::IntRect(0, 0, 128, 128));
    
    if (dx > 0)
        sprite.setTextureRect(sf::IntRect(128*(int)currentFrame, 0, 128, 128));

    if (dx < 0)
        sprite.setTextureRect(sf::IntRect(128*(int)currentFrame + 128, 0, -128, 128));

    dx = 0;

    sprite.setPosition(rect.left, rect.top);
}

Ship::~Ship() {

}
