#include "Shot.h"

Shot::Shot(sf::Texture &image): m_t(image), cooldown(10.0), rechargingSpeed(0.01), timeAfterShot(0.0), recharging(0), speed(0.1) {
    
}

void Shot::spawnBullet() {
    pBullets.push_back(new Bullet(m_t, speed));
    recharging = true;
}

void Shot::update(float myTime) {
    if (recharging) {
        timeAfterShot += rechargingSpeed * myTime;

        if (timeAfterShot >= cooldown) {
            recharging = false;
            timeAfterShot = 0.0;
        }
    }

    for (int i=0; i<pBullets.size(); i++) {
        pBullets[i]->update(myTime);
        
        if (pBullets[i]->rect.top <= 0.0) {
            removeBullet(i);
            i--;
        }
    }
    
}

void Shot::removeBullet(size_t position) {
    delete pBullets[position];
    std::vector<Bullet *>::iterator it = pBullets.begin();
    it += position;
    pBullets.erase(it);
}

Shot::~Shot() {
    for (size_t i=0; i<pBullets.size(); i++)
        delete pBullets[i];
    
    pBullets.clear();
}
