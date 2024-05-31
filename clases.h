#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>

class Nave {
public:
    int x, y;               // Position of the ship
    std::vector<std::string> art; // ASCII art of the ship
    std::vector<std::string> lifeArt; // ASCII art of a life
    int lives = 2; // Number of lives

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

    void moveLeft() {
        if (x > 0) --x;
    }

    void moveRight(int maxWidth) {
        if (x < maxWidth - static_cast<int>(14)) ++x; 
    }

    void drawNave() {
        for (size_t i = 0; i < art.size(); ++i) {
            mvaddstr(y + i, x, art[i].c_str());
        }
    }

    void drawLife(int maxX, int maxY) {
        int startX = maxX - (lifeArt[0].size() + 1) * lives;
        int startY = maxY - lifeArt.size();
        for (int i = 0; i < lives; ++i) {
            for (size_t j = 0; j < lifeArt.size(); ++j) {
                mvaddstr(startY + j, startX + i * (lifeArt[0].size() + 1), lifeArt[j].c_str());
            }
        }
    }

    void decreaseLife() {
        if (lives >= 0) {
        --lives;
        napms(1000); // Wait for a second
        x = COLS / 2 - 7; // Respawn the ship at the center
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

    virtual void update() = 0;

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

    void update() override {
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

    void update() override {
        // This enemy does not move but will shoot in future implementations
    }
};

class BossEnemy : public Enemy {
public:
    BossEnemy(int posX, int posY) : Enemy(posX, posY) {
        art = {
            "   /---\\   ",
            "  --WWW--  ",
            " {#######} "
        };
    }

    void update() override {
        // Special behavior like stealing the player's ship can be added here
    }
};

class Enemies {
public:
    std::vector<std::unique_ptr<Enemy>> enemyList;

    void spawnEnemies(int numNormal, int numTurrets, int numBosses) {
        int startX = 10;
        int startY = 5;
        // Spawn normal enemies
        for (int i = 0; i < numNormal; i++) {
            enemyList.push_back(std::make_unique<NormalEnemy>(startX, startY));
            adjustPosition(startX, startY);
        }
        // Spawn turret enemies
        for (int i = 0; i < numTurrets; i++) {
            enemyList.push_back(std::make_unique<TurretEnemy>(startX, startY));
            adjustPosition(startX, startY);
        }
        // Spawn boss enemies
        for (int i = 0; i < numBosses; i++) {
            enemyList.push_back(std::make_unique<BossEnemy>(startX, startY));
            adjustPosition(startX, startY);
        }
    }

    void adjustPosition(int& startX, int& startY) {
        startX += 12; // Space out enemies
        if (startX >= COLS - 10) {
            startX = 10;
            startY += 4;
        }
    }

    void updateEnemies() {
        for (auto& enemy : enemyList) {
            enemy->update();
        }
    }

    void drawEnemies() {
        for (auto& enemy : enemyList) {
            enemy->draw();
        }
    }
};

void handleInput(int ch, Nave& ship /*std::vector<Bullet>& bullets*/) {
    switch (ch) {
        case KEY_LEFT:            // Cuando se presiona la flecha izquierda se mueve a la izquierda
            ship.moveLeft();
            break;
        case KEY_RIGHT:
            ship.moveRight(COLS); // Cuando se presiona la flecha derecha se mueve a la derecha
            break;
        case 'd':
            ship.decreaseLife();
            break;
        /*case ' ':
            bullets.push_back(Bullet(ship.x + 2, ship.y - 1, '|')); // Adjust x position for bullet
            break;*/
    }
}    
