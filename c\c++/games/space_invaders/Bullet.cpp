#include "Bullet.h"

Bullet::Bullet() {

}

Bullet::Bullet(sf::Texture &image, float speed):
                currentFrame(0.0), alive(1), speed(speed) {
    
    sprite.setTexture(image);
    sprite.setTextureRect(sf::IntRect(0, 224, 32, 32));
    rect = sf::FloatRect(0, 0, 32, 32);
}

void Bullet::update(float myTime) {
    rect.top -= speed * myTime;

    currentFrame += 0.005 * myTime;

    if (currentFrame > 3)
        currentFrame -= 2;

    sprite.setTextureRect(sf::IntRect(32*(int)currentFrame, 224, 32, 32));

    sprite.setPosition(rect.left, rect.top);
}

Bullet::~Bullet() {

}
