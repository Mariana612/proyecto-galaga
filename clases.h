#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>

class Nave {
public:
    int x, y;   // Posición de la nave
    std::vector<std::string> art; // Arte ASCII de la nave
    std::vector<std::string> lifeArt; // Arte ASCII de las vidas
    int lives = 2; // Cantidad de vidas

    Nave(int posX, int posY) : x(posX), y(posY) {
        art = {
            "      ^     ",
            "     /-\\   ",
            "  --¦^¦^¦-- ",
        };
    }

    void initializeLifeArt() {
        lifeArt = {
            "  /-\\",
            "-|^ ^|-",
        };
    }

    void moveLeft() {   // Movimiento a la izquierda
        if (x > 0) --x;
    }

    void moveRight(int maxWidth) {  // Movimiento a la derecha
        if (x < maxWidth - static_cast<int>(14)) ++x; 
    }

    void drawNave() {   // Dibujar la nave
        for (size_t i = 0; i < art.size(); ++i) {
            mvaddstr(y + i, x, art[i].c_str());
        }
    }

    void drawLife(int maxX, int maxY) { // Dibujar las vidas
        int startX = maxX - (lifeArt[0].size() + 1) * lives;
        int startY = maxY - lifeArt.size();
        for (int i = 0; i < lives; ++i) {
            for (size_t j = 0; j < lifeArt.size(); ++j) {
                mvaddstr(startY + j, startX + i * (lifeArt[0].size() + 1), lifeArt[j].c_str());
            }
        }
    }

    void decreaseLife() { // Perder una vida
        if (lives >= 0) {
        --lives;
        x = COLS / 2 - 7; // Se coloca la nave en el centro
        y = LINES - 6;
    }
    }
};

class Enemy {

public:

    int x, y;

    std::vector<std::string> art;

    bool isAlive;



    Enemy(int posX, int posY) : x(posX), y(posY), isAlive(true) {}



    virtual void update(int playerX) = 0;



    virtual void draw() {

        if (!isAlive) return;

        for (size_t i = 0; i < art.size(); ++i) {

            mvaddstr(y + i, x, art[i].c_str());

        }

    }



    virtual ~Enemy() {}

};



class NormalEnemy : public Enemy {

private:

    int moveDirection; // 1 for moving down, -1 for moving up

    int moveCounter;

    int moveThreshold;



public:

    NormalEnemy(int posX, int posY) : Enemy(posX, posY), moveDirection(1), moveCounter(0), moveThreshold(10) { // Set threshold to 10 for slower movement

        art = {

            "  /---\\  ",

            " -- o -- ",

            "  \\---/  "

        };

    }



    void update(int playerX) override {

        moveCounter++; // Increment counter every update call

        if (moveCounter >= moveThreshold) { // Check if counter has reached the threshold

            moveCounter = 0; // Reset counter

            if ((y >= LINES - static_cast<int>(art.size()) && moveDirection == 1) || (y <= 0 && moveDirection == -1)) {

                moveDirection *= -1; // Reverse direction at the boundaries

            }

            y += moveDirection; // Move based on the direction

        }

    }

};



class TurretEnemy : public Enemy {

public:

    TurretEnemy(int posX, int posY) : Enemy(posX, posY) {

        art = {

            " [#####] ",

            " |#####| ",

            " |#####| "

        };

    }



    void update(int playerX) override {

        // This enemy does not move but will shoot in future implementations

    }

};



class BossEnemy : public Enemy {

public:

     enum State { Descending, Holding, LateralMove };

private:

    State currentState;

    int lateralDirection;  // 1 for right, -1 for left

    int moveCounter;       // Counter to control the descending speed

    int moveThreshold;     // Threshold to reach before moving downwards



public:

    BossEnemy(int posX) : Enemy(posX, 0), currentState(Descending), lateralDirection(1), moveCounter(0), moveThreshold(10) {

        art = {

            "   /---\\   ",

            "  --WWW--  ",

            " {#######} "

        };

    }

    State getCurrrentState() const{

        return currentState;

    }

    void update(int playerX) override {

        switch (currentState) {

            case Descending:

                moveCounter++; // Increment the counter each time update is called

                if (moveCounter >= moveThreshold) {

                    moveCounter = 0; // Reset counter once threshold is reached

                    y += 1; // Move down slowly

                    if (y >= LINES / 1.5) { // Change this condition to control where it stops

                        currentState = Holding; // Change state to holding

                    }

                }

                break;

            case Holding:

                // Stay in position, check for collision with player

                if (x == playerX) { // Simplified collision detection

                    currentState = LateralMove; // Change to lateral movement

                    y = 0; // Optionally move to top if that's needed after collision

                }

                break;

            case LateralMove:

                x += lateralDirection;

                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {

                    lateralDirection *= -1; // Change direction when hitting screen borders

                }

                break;

        }

    }



    void draw() override {

        if (!isAlive) return; // Do not draw if not alive

        Enemy::draw(); // Use the base class draw method

    }

};



class Enemies {

public:

    std::vector<std::unique_ptr<Enemy>> enemyList;



    void spawnEnemies(int numNormal, int numTurrets, int numBosses) {

        int startX = 10;

        int startY = 5;

        // Spawn normal and turret enemies

        for (int i = 0; i < numNormal; i++) {

            enemyList.push_back(std::make_unique<NormalEnemy>(startX, startY));

            adjustPosition(startX, startY);

        }

        for (int i = 0; i < numTurrets; i++) {

            enemyList.push_back(std::make_unique<TurretEnemy>(startX, startY));

            adjustPosition(startX, startY);

        }

    }



    void spawnBoss(int posX) {

        enemyList.push_back(std::make_unique<BossEnemy>(posX));

    }



    void adjustPosition(int& startX, int& startY) {

        startX += 12; // Space out enemies

        if (startX >= COLS - 10) {

            startX = 10;

            startY += 4;

        }

    }



    void updateEnemies(int playerX, Nave& player) {

        for (auto& enemy : enemyList) {

            enemy->update(playerX);

            checkCollision(enemy.get(), player);

        }

    }



    void checkCollision(Enemy* enemy, Nave& player) {

        // Simplified collision detection logic

        if (enemy->y == player.y && enemy->x == player.x) {

            if (auto* boss = dynamic_cast<BossEnemy*>(enemy)) {

                if (boss->getCurrrentState() == BossEnemy::Holding) {

                    player.decreaseLife();

                    enemy->isAlive = false; // Optionally remove the enemy

                }

            } else if (dynamic_cast<NormalEnemy*>(enemy)) {

                player.decreaseLife();

                enemy->isAlive = false; // Optionally remove the enemy

            }

        }

    }



    void drawEnemies() {

        for (auto& enemy : enemyList) {

            enemy->draw();

        }

    }

};


void handleInput(int ch, Nave& ship) { // Input del jugador
    switch (ch) {
        case KEY_LEFT:
            ship.moveLeft();    // Cuando se presiona la flecha izquierda se mueve a la izquierda
            break;
        case KEY_RIGHT:
            ship.moveRight(COLS);   // Cuando se presiona la flecha derecha se mueve a la derecha
            break;
        case 'd':
            ship.decreaseLife();    // Cuando se presiona la letra d, se pierde una vida
            break;
        /*case ' ':
            break;*/
    }
}    
