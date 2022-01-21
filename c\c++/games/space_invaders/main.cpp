#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <cstring>

#include "Ship.h"
#include "Bullet.h"
#include "Shot.h"
#include "Enemy.h"
#include "Swarm.h"

using namespace sf;



float myTime = 0.0;
bool g_dead = false;
bool g_levelComplete = false;
int g_money = 0;
int g_maxLevel = 1;
int g_currentLevel = 1;

int g_multiplier = 1;
int g_cooldownLevel = 0;
int g_shipSpeedLevel = 0;
int g_ballSpeedLevel = 0;

enum GAMESTATE {
    MENU,
    SUBMENU,
    SHOP,
    PLAY,
    GAME_OVER,
    END
};
GAMESTATE gameState = MENU;


void play(RenderWindow *window, Event *event, Clock *clock, Texture *gameTexture, Texture *buttonTexture, Font *font);
void menu(RenderWindow *window, Event *event, Texture *buttonTexture);
void selectLevel(RenderWindow *window, Event *event, Sprite *sprite);
void gameOver(RenderWindow *window, Event *event, Texture *buttonTexture);
void levelComplete(RenderWindow *window, Event *event, Texture *buttonTexture);
void save();
void load();

std::string* createLevel();

void shop(RenderWindow *window, Event *event, Sprite *sprite);


int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Cudzinez");

    sf::Event event;
  
    Texture buttonTexture;
    buttonTexture.loadFromFile("textures/buttons256_final.png");
    Texture gameTexture;
    gameTexture.loadFromFile("textures/textures5.png");

    Clock clock;
    
    Font font;
    if (!font.loadFromFile("fonts/karma future.ttf"))
        std::cout << "Error loading font" << std::endl;
    
    load();
    g_currentLevel = g_maxLevel;
//////////////////////////////////////////////////////////////////
    while (window.isOpen())
    {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        switch(gameState) {
            case MENU: 
                menu(&window, &event, &buttonTexture); 
                break; // MENU

            case PLAY:
                play(&window, &event, &clock, &gameTexture, &buttonTexture,  &font);   
                break; // case PLAY

            case GAME_OVER:
                if (g_dead)
                    gameOver(&window, &event, &buttonTexture);
                else {
                    if (g_currentLevel == g_maxLevel)
                        g_maxLevel++;
                    g_currentLevel++;
                    levelComplete(&window, &event, &buttonTexture);
                }
                    

                break; // case GAME_OVER     
            case END:
                save();
                window.close();
                break; // case END
            default:
                break;

        }   // switch(gameState)
    }

    return 0;
}






void play(RenderWindow *window, Event *event, Clock *clock, Texture *gameTexture, Texture *buttonTexture, Font *font) {
    Texture background;
    background.loadFromFile("textures/sky3.jpg");

    Sprite bg;
    bg.setTexture(background);
    bg.setTextureRect(IntRect(0, 0, 1280, 720));
    bg.setColor(Color(255, 255, 255, 128));
    float swap = 0.0;
    bool reflect = false;

    bool leftPressed = false;
    bool rightPressed = false;

    Ship ship(*gameTexture);
    ship.alive = true;

    Shot shot(*gameTexture);

    std::string *level = createLevel();

    Swarm swarm(*gameTexture);
    swarm.createLevel(level); 

    Text text;
    text.setFont(*font);
    text.setString("$ " + std::to_string(g_money));
    text.setPosition(16, 16);
    text.setCharacterSize(32);
    text.setFillColor(Color(0, 200, 0));
    

    // Upgrades
    ship.speed = 0.1 + g_shipSpeedLevel * 0.05;
    shot.cooldown = 15.0 - g_cooldownLevel * 0.1;
    shot.speed = 0.1 + g_ballSpeedLevel * 0.05;

    // Game Loop
    clock->restart();
    while(ship.alive && !swarm.defeated) {
        myTime = clock->getElapsedTime().asMicroseconds();
        clock->restart();

        myTime /= 800.0;

        while (window->pollEvent(*event))
        {
            if (event->type == sf::Event::Closed) {
                gameState = END;
                window->close();
            }

            if (!window->isOpen()) {
                break;
            }
            //////////////////////////////////////////////////////////
            // Controls
            if (event->type == sf::Event::KeyPressed) {
                if (event->key.code == sf::Keyboard::Left) {
                    if (ship.direction == 1)
                        ship.currentFrame = 0.0;

                    ship.direction = -1;
                    leftPressed = true;
                }
                
                if (event->key.code == sf::Keyboard::Right) {
                    if (ship.direction == -1)
                        ship.currentFrame = 0.0;

                    ship.direction = 1;
                    rightPressed = true;
                }

                if (event->key.code == sf::Keyboard::Escape) {
                    gameState = MENU;
                }
            }

            if (event->type == sf::Event::KeyReleased) {
                if (event->key.code == sf::Keyboard::Left) {
                    if (!rightPressed) {
                        ship.direction = 0;
                        ship.currentFrame = 0.0;
                    }
                    else
                        ship.direction = 1;
                    leftPressed = false;
                }

                if (event->key.code == sf::Keyboard::Right) {
                    if (!leftPressed) {
                        ship.direction = 0;
                        ship.currentFrame = 0.0;
                    }
                    else
                        ship.direction = -1;
                    rightPressed = false;
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                if (!shot.recharging) {
                    shot.recharging = true;
                    shot.spawnBullet();
                    shot.pBullets.back()->rect = FloatRect(ship.rect.left + 48, ship.rect.top - 20, 32, 32);
                }
            }

        } // while(pollEvent)
        if (!window->isOpen() || gameState != PLAY) {
            break;
        }                    
        //////////////////////////////////////////////
        // Update
        ship.update(myTime);
        shot.update(myTime);
        swarm.update(myTime);

        /////////////////////////////////////////////
        // Collision Enemy with Ship
        for (int i=0; i< swarm.pEnemies.size(); i++) {
            if (swarm.pEnemies[i]->rect.top +64 >= ship.rect.top +12)
                if (swarm.pEnemies[i]->rect.left +64 >= ship.rect.left +16 && swarm.pEnemies[i]->rect.left <= ship.rect.left +112) {
                    ship.alive = false;
                    g_dead = true;
                    i = swarm.pEnemies.size();
                    break;
                }
        }


        //////////////////////////////////////////////
        // Collision Bullet with Enemy
        for (int i=0; i<shot.pBullets.size(); i++) 
            for (int j=0; j<swarm.pEnemies.size(); j++) {
                if (shot.pBullets[i]->rect.left +24  >= swarm.pEnemies[j]->rect.left && shot.pBullets[i]->rect.left + 8 <= swarm.pEnemies[j]->rect.left + swarm.pEnemies[j]->rect.width) {
                    if (shot.pBullets[i]->rect.top + 8 <= swarm.pEnemies[j]->rect.top + swarm.pEnemies[j]->rect.height && shot.pBullets[i]->rect.top + 24 >= swarm.pEnemies[j]->rect.top){
                        shot.removeBullet(i);
                        g_money += swarm.pEnemies[j]->reward * g_multiplier;
                        swarm.killEnemy(j);
                        i--;
                        text.setString("$ " + std::to_string(g_money));
                        break;
                    }
            }
        }

        if (swarm.pEnemies.size() < 1) {
            swarm.defeated = true;
            g_levelComplete = true;
        }
        ///////////////////////////////////////////////
        // Draw Game
        ////////////////////////////////////////////////
        // Draw Background
        swap += 0.001 * myTime;
        if (swap > 1) {
            reflect = !reflect;
            swap -= 1.0;
        }

        bg.setTextureRect(IntRect(reflect*1280, 0, 1280*(1-reflect*2), 720)); 
        window->draw(bg);       
        

        ////////////////////////////////////////////////
        // Draw Text
        window->draw(text);

        /////////////////////////////////////////////////
        // Draw Enemies
        for (size_t i=0; i<swarm.pEnemies.size(); i++)
            window->draw(swarm.pEnemies[i]->sprite);
        
        //////////////////////////////////////////////////
        // Draw Ship
        window->draw(ship.sprite);
        
        //////////////////////////////////////////////////
        // Draw Bullets
        for (size_t i=0; i<shot.pBullets.size(); i++)
            window->draw(shot.pBullets[i]->sprite);
  
        window->display();
        
        if (!ship.alive || swarm.defeated)
            gameState = GAME_OVER;

    }

}



///////////////////////////////////////////////
////////////////////MENU//////////////////////
/////////////////////////////////////////////

void menu(RenderWindow *window, Event *event, Texture *buttonTexture) {                
    Sprite menuButtonPlay;
    menuButtonPlay.setTexture(*buttonTexture);
    menuButtonPlay.setTextureRect(IntRect(0, 0, 256, 256));
    
    menuButtonPlay.setPosition(566, 304);
    //menuButtonPlay.scale(2.0f, 2.0f);

    int selectedButton = 0;

    bool quit = false;
    bool change = true;

    while(!quit) {

        while (window->pollEvent(*event)) {
            if (event->type == sf::Event::Closed) {
                quit = true;
                window->close();
            }


            if (event->type == Event::KeyPressed) {
                if (event->key.code == Keyboard::Down) {
                    selectedButton++;
                    if (selectedButton > 3)
                        selectedButton = 0;
                    change = true;
                }

                if (event->key.code == Keyboard::Up) {
                    selectedButton--;
                    if (selectedButton < 0)
                        selectedButton = 3;
                    change = true;
                }

                if (event->key.code == Keyboard::Enter) {
                    if (selectedButton == 0)                    
                        gameState = PLAY;

                    else if (selectedButton == 1)
                        gameState = SUBMENU;
                    
                    else if (selectedButton == 2)
                        gameState = SHOP;

                    else if (selectedButton == 3)
                        gameState = END;                    
                    quit = true;
                    change = true;
                }
            }
        } // while(window->pollEvent(*event))
        
        if (change) {
            window->clear(Color(97, 97, 97, 128));
        
            for (size_t i=0; i<4; i++) {
                menuButtonPlay.setTextureRect(IntRect(0, i*256, 256, 256));
                menuButtonPlay.setPosition(566, 240 + (i*60));
                if (i == selectedButton) {
                    if (gameState == MENU)
                        menuButtonPlay.setTextureRect(IntRect(256, i*256, 256, 256));
                    else
                        menuButtonPlay.setTextureRect(IntRect(512, i*256, 256, 256));
                }
                window->draw(menuButtonPlay);
            }
            window->display();



            if (gameState == SUBMENU) { // Select level
                usleep(50000);
                selectLevel(window, event, &menuButtonPlay);
            } // Select level
            else if (gameState == SHOP) {
                usleep(50000);
                shop(window, event, &menuButtonPlay);
            } // Shop
            



            change = false;
        }
    }
    usleep(50000);
}

void selectLevel(RenderWindow *window, Event *event, Sprite *sprite) {

    int selectedButton = 0;

    bool quit = false;
    bool change = true;

    sprite->setTextureRect(IntRect(0, 1280, 42, 42));
    sprite->setPosition(200, 200);

    while(!quit) {

        while (window->pollEvent(*event)) {
            if (event->type == sf::Event::Closed) {
                quit = true;
                window->close();
            }


            if (event->type == Event::KeyPressed) {
                if (event->key.code == Keyboard::Left) {
                    if (selectedButton > 0)
                        selectedButton--;
                    else
                        selectedButton = g_maxLevel-1;

                    change = true;
                }

                if (event->key.code == Keyboard::Right) {
                    if (selectedButton < g_maxLevel-1)
                        selectedButton++;
                    else
                        selectedButton = 0;

                    change = true;
                }

                if (event->key.code == Keyboard::Down) {
                    selectedButton += 3;
                    if (selectedButton >= g_maxLevel)
                        selectedButton -= 3;

                    change = true;
                }

                if (event->key.code == Keyboard::Up) {
                    selectedButton -= 3;
                    if (selectedButton < 0)
                        selectedButton += 3;

                    change = true;
                }

                if (event->key.code == Keyboard::Enter) {
                    g_currentLevel = selectedButton +1;
                    std::cout << g_currentLevel << std::endl;
                    gameState = PLAY;                   
                    quit = true;
                    change = true;
                }

                if (event->key.code == Keyboard::Escape) {
                    gameState = MENU;
                    quit = true;
                }
            }
        } // while(window->pollEvent(*event))

        // Draw Buttons
        if (change) {
            window->clear(Color(97, 97, 97, 128));
            for (size_t i=0; i<g_maxLevel; i++) {
                sprite->setTextureRect(IntRect(126*(i%2), 1280 + 42*(i/2), 42, 42));
                sprite->setPosition(200 + 50*(i%3), 200 + 50*(i/3));
                
                if (i == selectedButton) { 
                    if (gameState == PLAY)
                        sprite->setTextureRect(IntRect(84 + 126*(i%2), 1280 + 42*(i/2), 42, 42));
                    else
                        sprite->setTextureRect(IntRect(42 + 126*(i%2), 1280 + 42*(i/2), 42, 42));
                }
                window->draw(*sprite);
            }
        }
        window->display();
        change = false;
    } // while(!quit)
    usleep(50000);
}


////////////////////////////////////////////////
////////////GAME OVER WINDOW///////////////////
//////////////////////////////////////////////

void gameOver(RenderWindow *window, Event *event, Texture *buttonTexture) {               
    Sprite menuButton;
    menuButton.setTexture(*buttonTexture);
    menuButton.setTextureRect(IntRect(0, 0, 256, 256));
    
    menuButton.setPosition(566, 304);
    //menuButtonPlay.scale(2.0f, 2.0f);

    int selectedButton = 0;

    bool quit = false;
    bool change = true;

    while(!quit) {

        while (window->pollEvent(*event)) {
            if (event->type == sf::Event::Closed)
                window->close();


            if (event->type == Event::KeyPressed) {
                if (event->key.code == Keyboard::Down) {
                    selectedButton++;
                    if (selectedButton > 1)
                        selectedButton = 0;
                    change = true;
                }

                if (event->key.code == Keyboard::Up) {
                    selectedButton--;
                    if (selectedButton < 0)
                        selectedButton = 1;
                    change = true;
                }

                if (event->key.code == Keyboard::Enter) {
                    if (selectedButton == 0)                    
                        gameState = PLAY;   // RESTART

                    if (selectedButton == 1)
                        gameState = MENU;    // MENU    
                    quit = true;
                    change = true;
                }
            }
        } // while(window->pollEvent(*event)) 

        if (change) {
            window->clear(Color(97, 97, 97, 128));
        
            for (size_t i=0; i<2; i++) {
                menuButton.setTextureRect(IntRect(0, 1024 - i*256, 256, 256));
                menuButton.setPosition(566, 254 + (i*60));
                if (i == selectedButton) {
                    if (gameState == GAME_OVER)
                        menuButton.setTextureRect(IntRect(256, 1024 - i*256, 256, 256));
                    else
                        menuButton.setTextureRect(IntRect(512, 1024 - i*256, 256, 256));
                }
                window->draw(menuButton);
            }
            window->display(); 
            change = false;
        }
    }
    usleep(50000);
}

///////////////////////////////////////////
//////////LEVEL COMPLETE//////////////////
/////////////////////////////////////////

void levelComplete(RenderWindow *window, Event *event, Texture *buttonTexture) {
    Sprite menuButton;
    menuButton.setTexture(*buttonTexture);
    menuButton.setTextureRect(IntRect(0, 0, 256, 256));
    
    menuButton.setPosition(566, 304);
    //menuButtonPlay.scale(2.0f, 2.0f);

    int selectedButton = 0;

    bool quit = false;
    bool change = true;

    while(!quit) {

        while (window->pollEvent(*event)) {
            if (event->type == sf::Event::Closed)
                window->close();


            if (event->type == Event::KeyPressed) {
                if (event->key.code == Keyboard::Down) {
                    selectedButton++;
                    if (selectedButton > 1)
                        selectedButton = 0;
                    change = true;
                }

                if (event->key.code == Keyboard::Up) {
                    selectedButton--;
                    if (selectedButton < 0)
                        selectedButton = 1;
                    change = true;
                }

                if (event->key.code == Keyboard::Enter) {
                    if (selectedButton == 0)                    
                        gameState = PLAY;   // RESTART

                    if (selectedButton == 1)
                        gameState = MENU;    // MENU    
                    quit = true;
                    change = true;
                }
            }
        } // while(window->pollEvent(*event)) 

        if (change) {
            window->clear(Color(97, 97, 97, 128));
        
            for (size_t i=0; i<2; i++) {

                menuButton.setTextureRect(IntRect(0, 768 * (i%2), 256, 256));
                menuButton.setPosition(566, 254 + (i*60));
                if (i == selectedButton) {
                    if (gameState == GAME_OVER)
                        menuButton.setTextureRect(IntRect(256, 768 * (i%2), 256, 256));
                    else
                        menuButton.setTextureRect(IntRect(512, 768 * (i%2), 256, 256));
                }
                window->draw(menuButton);
            }
            window->display(); 
            change = false;
        }
    }
    usleep(50000);

}


//////////////////////////////////////////
/////////////SAVE AND LOAD///////////////
////////////////////////////////////////

void save() {
    std::cout << "saving" << std::endl;
    FILE *fpSave = fopen("save", "w");
    int buffer[] = {g_maxLevel, g_money, g_multiplier, g_shipSpeedLevel, g_ballSpeedLevel, g_cooldownLevel};
    fwrite(&buffer, sizeof(int), 6, fpSave);
    fclose(fpSave);
    fpSave = NULL;

}


void load() {
    FILE *fp = fopen("save", "r");
    if (!fp) {
        std::cout << "loading failed" << std::endl;
        save();
        fp = fopen("save", "r");
    }
    int buffer[6];
    fread(&buffer, sizeof(int), 6, fp);

    g_maxLevel       = buffer[0];
    g_money          = buffer[1];
    g_multiplier     = buffer[2];
    g_shipSpeedLevel = buffer[3];
    g_ballSpeedLevel = buffer[4];
    g_cooldownLevel  = buffer[5];
    
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}



std::string* createLevel() {
    if (g_currentLevel > g_maxLevel)
        g_currentLevel = g_maxLevel;

    std::string *level = new std::string[6];

    switch(g_currentLevel) {
        case 1:
            
            level[0] = "......1......+";
            level[1] = "....11.11....-";
            level[2] = ".....111.....+";
            level[3] = "......1......-";
            level[4] = ".............+";
            level[5] = ".............-";
            
            return level;
            break;
        case 2:
            level[0] = "1.1.11.11.1.1+";
            level[1] = ".1.1..1..1.1.-";
            level[2] = ".............+";
            level[3] = ".............-";
            level[4] = ".............+";
            level[5] = ".............-";
            
            return level;
            break;
        case 3:
            level[0] = "1.1.11.11.1.1+";
            level[1] = ".1.1..1..1.1.-";
            level[2] = "....11.11....+";
            level[3] = "......1......-";
            level[4] = ".............+";
            level[5] = ".............-";
            
            return level;
            break;
        case 4:
            level[0] = "......1......+";
            level[1] = ".....1.1.....-";
            level[2] = "....1.1.1....+";
            level[3] = "...1.1.1.1...-";
            level[4] = "....1.1.1....+";
            level[5] = ".....1.1.....-";
            
            return level;
            break;
        case 5:
            level[0] = "1..1..1..1..1+";
            level[1] = ".11.11.11.11.-";
            level[2] = ".....111.....+";
            level[3] = ".1.11...11.1.-";
            level[4] = "......1......+";
            level[5] = ".............-";
            
            return level;
            break;
        case 6:
            level[0] = "1.1.1.1.1.1.1+";
            level[1] = ".1.111.111.1.-";
            level[2] = "..1.......1..+";
            level[3] = "...1..1..1...-";
            level[4] = "....1.1.1....+";
            level[5] = ".....111.....-";
            
            return level;
            break;
        case 7:
            level[0] = "1.....1.....1+";
            level[1] = ".111.....111.-";
            level[2] = ".....111.....+";
            level[3] = ".....111.....-";
            level[4] = ".111.....111.+";
            level[5] = "1.....1.....1-";
            
            return level;
            break;
        case 8:
            level[0] = "1111111111111+";
            level[1] = ".11111111111.-";
            level[2] = "..111111111..+";
            level[3] = "...1111111...-";
            level[4] = "....11111....+";
            level[5] = ".....111.....-";
            
            return level;
            break;
        case 9:
            level[0] = ".....111.....+";
            level[1] = "....11111....-";
            level[2] = "...1111111...+";
            level[3] = "..111111111..-";
            level[4] = ".11111111111.+";
            level[5] = "1111111111111-";
            
            return level;
            break;
        case 10:
            level[0] = "1111111111111+";
            level[1] = ".11111111111.-";
            level[2] = "..1...1...1..+";
            level[3] = "..1..1.1..1..-";
            level[4] = "1..1111111..1+";
            level[5] = ".11111111111.-";
            
            return level;
            break;
        case 11:
            level[0] = "1111111111111+";
            level[1] = "1.1.11.11.1.1-";
            level[2] = ".1..11.11..1.+";
            level[3] = "1.1.11.11.1.1-";
            level[4] = "1.1.11.11.1.1+";
            level[5] = "1111111111111-";
            
            return level;
            break;
        case 12:
            level[0] = "......11.....+";
            level[1] = "......11.....-";
            level[2] = "......11.....+";
            level[3] = "..111.11.111.-";
            level[4] = ".111111111111+";
            level[5] = "..1111111111.-";
            
            return level;
            break;
        default:
            break;
    }

    return level;
}



void shop(RenderWindow *window, Event *event, Sprite *sprite) {

    Font font;
    if (!font.loadFromFile("fonts/karma future.ttf"))
        std::cout << "Error loading font" << std::endl;

    Text text;
    text.setFont(font);
    text.setString("$ " + std::to_string(g_money));
    text.setPosition(16, 16);
    text.setCharacterSize(32);
    text.setFillColor(Color(0, 200, 0));

    int selectedButton = 0;

    bool quit = false;
    bool change = true;
    bool buy = false;
    int price = 0;

    sprite->setTextureRect(IntRect(256, 1280, 42, 42));
    sprite->setPosition(200, 200);

    while(!quit) {

        while (window->pollEvent(*event)) {
            if (event->type == sf::Event::Closed) {
                quit = true;
                window->close();
            }


            if (event->type == Event::KeyPressed) {
                if (event->key.code == Keyboard::Left) {
                    if (selectedButton > 0)
                        selectedButton--;
                    else
                        selectedButton = 3;

                    change = true;
                }

                if (event->key.code == Keyboard::Right) {
                    if (selectedButton < 3)
                        selectedButton++;
                    else
                        selectedButton = 0;

                    change = true;
                }

                if (event->key.code == Keyboard::Down) {
                    selectedButton += 2;
                    if (selectedButton >= 4)
                        selectedButton -= 2;

                    change = true;
                }

                if (event->key.code == Keyboard::Up) {
                    selectedButton -= 2;
                    if (selectedButton < 0)
                        selectedButton += 2;

                    change = true;
                }

                if (event->key.code == Keyboard::Enter) {
                    
                    if (selectedButton == 0) {
                        price = 20 + g_shipSpeedLevel * 30;
                        if (g_money > price) {
                            g_money -= price;
                            g_shipSpeedLevel++;
                            buy = true;
                        }
                        price = 0;
                    
                    }
                    else if (selectedButton == 1) {
                        price = 20 + g_ballSpeedLevel * 30;
                        if (g_money > price) {
                            g_money -= price;
                            g_ballSpeedLevel++;
                            buy = true;
                        }
                        price = 0;
                    }
                    else if (selectedButton == 2) {
                        price = 20 + g_cooldownLevel * 30;
                        if (g_money > price) {
                            g_money -= price;
                            g_cooldownLevel++;
                            buy = true;
                        }
                        price = 0;
                    }
                    else if (selectedButton == 3) {
                        price = 50 + g_multiplier * 70;
                        if (g_money > price) {
                            g_money -= price;
                            g_multiplier++;
                            buy = true;
                        }
                        price = 0;
                    }
                    
                    change = true;                                      
                }

                if (event->key.code == Keyboard::Escape) {
                    gameState = MENU;
                    quit = true;
                }
            }
        } // while(window->pollEvent(*event))

        // Draw Buttons
        //if (change) {
            window->clear(Color(97, 97, 97, 128));
            for (size_t i=0; i<4; i++) {
                sprite->setTextureRect(IntRect(256 + 126*(i%2), 1280 + 42*(i/2), 42, 42));
                sprite->setPosition(200 + 50*(i%2), 200 + 50*(i/2));
                
                if (i == selectedButton) { 
                    if (buy) 
                        sprite->setTextureRect(IntRect(256 + 84 + 126*(i%2), 1280 + 42*(i/2), 42, 42));
                    else
                        sprite->setTextureRect(IntRect(256 + 42 + 126*(i%2), 1280 + 42*(i/2), 42, 42));
                }
                window->draw(*sprite);
            }
        //}

        // PRICE TEXT
        text.setCharacterSize(18);
        text.setFillColor(Color(200, 255, 200));
        
        price = 20 + g_shipSpeedLevel * 30;
        if (price > g_money)
            text.setFillColor(Color(255, 200, 200));

        text.setString("$ " + std::to_string(price));
        text.setPosition(150, 210);
        window->draw(text);

        price = 20 + g_ballSpeedLevel * 30;
        if (price > g_money)
            text.setFillColor(Color(255, 200, 200));
        else
            text.setFillColor(Color(200, 255, 200));

        text.setString("$ " + std::to_string(price));
        text.setPosition(300, 210);
        window->draw(text); 
        
        price = 20 + g_cooldownLevel * 30;
        if (price > g_money)
            text.setFillColor(Color(255, 200, 200));
        else
            text.setFillColor(Color(200, 255, 200));
        
        text.setString("$ " + std::to_string(price));
        text.setPosition(150, 260);
        window->draw(text);
        
        price = 50 + g_multiplier * 70;
        if (price > g_money)
            text.setFillColor(Color(255, 200, 200));
        else
            text.setFillColor(Color(200, 255, 200));
        
        text.setString("$ " + std::to_string(price));
        text.setPosition(300, 260);
        window->draw(text);

        // LEVEL TEXT
        text.setCharacterSize(16);
        text.setFillColor(Color(200, 200, 255));
        
        text.setString("Lvl " + std::to_string(g_shipSpeedLevel));
        text.setPosition(190, 170);
        window->draw(text);

        text.setString("Lvl " + std::to_string(g_ballSpeedLevel));
        text.setPosition(260, 170);
        window->draw(text);
       
        text.setString("Lvl " + std::to_string(g_cooldownLevel));
        text.setPosition(190, 300);
        window->draw(text);

        text.setString("Lvl " + std::to_string(g_multiplier));
        text.setPosition(260, 300);
        window->draw(text);

        text.setString("$ " + std::to_string(g_money));
        text.setPosition(16, 16);
       
        text.setCharacterSize(32);
        text.setFillColor(Color(0, 200, 0));

        window->draw(text);
        window->display();
        if (buy) {
            usleep(100000);
            buy = false;
        }
        change = false;
    } // while(!quit)
    usleep(50000);
   
}

