#include "Enemy.h"

Enemy::Enemy() {

}

Enemy::Enemy(sf::Texture &image, int reward):
                reward(reward), dx(0.0), dy(0.0), currentFrame(0.0), alive(1), speed(0.1), vShift(0.0) {
    
    sprite.setTexture(image);
    sprite.setTextureRect(sf::IntRect(128, 196, 64, 64));
    rect = sf::FloatRect(300, 300, 64, 64);
}

void Enemy::update(float myTime) {
    
    dx = speed * myTime;

    if (rect.left <= 31.0 || rect.left >= 1185) {
        
        dy = speed * myTime;
        dx = 0.0;
    }


    rect.left += direction* dx;
    
    vShift += dy; 
    if (vShift > 64.0) {
        dy = 64.0 - (vShift - dy);
        vShift = 0.0;

        if (rect.left <= 31.0)
            rect.left = 32.0;
        else if (rect.left >= 1185.0)
            rect.left = 1184.0;

        direction *= -1;    
    }
 
    rect.top += dy;

    currentFrame += 0.005 * myTime;
    if (currentFrame > 2)
        currentFrame -= 2.0;

    sprite.setTextureRect(sf::IntRect(64*(int)currentFrame + 128, 196, 64, 64));
    
    sprite.setPosition(rect.left, rect.top);

    dx = 0.0;
    dy = 0.0;
}


Enemy::~Enemy() {

}
