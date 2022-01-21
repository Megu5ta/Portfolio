#include "Swarm.h"

Swarm::Swarm(sf::Texture &image): m_t(image), defeated(false) {
    
}

void Swarm::spawnEnemy() {
    pEnemies.push_back(new Enemy(m_t, 1));
}

/*
"......1......+"
"....11.11....+"
".....111.....+"
".............+"
".............+"
".............+"
*/
void Swarm::createLevel(std::string *layout) {
    int x;
    int y;

    char dir;

    for (size_t i=0; i<6; i++) {
        if (layout[i][13] == '+')
            dir = 1;
        else if (layout[i][13] == '-')
            dir = -1;

        x = OFFSET;
        y = OFFSET + (64*i);

        for (size_t j=0; j<13; j++) {
            if (layout[i][j] == '1') {
                spawnEnemy();
                pEnemies[pEnemies.size()-1]->rect = sf::FloatRect(x, y, 64, 64);
                pEnemies[pEnemies.size()-1]->direction = dir;
            }
            x += 86;
        }
    }
}

void Swarm::spawnRow(size_t numberOfRows, size_t rowCapacity) {
    if (numberOfRows > 6)
        numberOfRows = 6;

    if (rowCapacity > 13)
        rowCapacity = 13;

    int startingX = 640 - (rowCapacity/2.0 * 64 + rowCapacity/2 * 32);
    char dir;

    for (size_t i=0; i<numberOfRows; i++) {
        dir = 1 - 2*(i%2);
        for (size_t j=0; j<rowCapacity; j++) {
            spawnEnemy(); 
            pEnemies[pEnemies.size()-1]->rect = sf::FloatRect(startingX + (j*96), OFFSET + (i*64), 64, 64);
            pEnemies[pEnemies.size()-1]->direction = dir;
        }
    }
}

void Swarm::killEnemy(int position) {
    delete pEnemies[position];
    std::vector<Enemy *>::iterator it = pEnemies.begin();
    it += position;
    pEnemies.erase(it);
}

void Swarm::update(float myTime) {
    for (int i=0; i<pEnemies.size(); i++) {
        pEnemies[i]->update(myTime);

        if (pEnemies[i]->rect.top >= 720) {
            killEnemy(i);
            i--;
        }
    }
}

Swarm::~Swarm() {
    for (size_t i=0; i<pEnemies.size(); i++)
        delete pEnemies[i];

    pEnemies.clear();
}
