#ifndef SWARM_H_
#define SWARM_H_

#include "Enemy.h"
#include <string>
#include <SFML/Graphics.hpp>

#define OFFSET 64

class Swarm {
public:
    sf::Texture m_t;
    std::vector<Enemy *> pEnemies;
    bool defeated;

public:
    Swarm(sf::Texture &image);
    ~Swarm();
    
    void spawnEnemy();
    void createLevel(std::string *layout);
    void spawnRow(size_t numberOfRows, size_t rowCapacity);
    void killEnemy(int position);
    void update(float myTime);
};


#endif // SWARM_H_
