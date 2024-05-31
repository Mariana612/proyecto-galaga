#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>

class Nave {
public:
    int x, y;
    std::vector<std::string> art; // El arte de la nave es un vector

    Nave(int posX, int posY) : x(posX), y(posY) { // Se ocupan dos números, que son la posición de la nave
        art = {
        "      ^     ",
        "     /-\\   ",
        "  --¦^¦^¦-- ",
        };
    }

    void moveLeft() {    //Se mueve a la izquierda
        if (x > 0) --x; // Si se está al borde de la línea, no avance
    }

    void moveRight(int maxWidth) {  //Se mueve a la derecha
        if (x < maxWidth - static_cast<int>(14)) ++x; // Si se está al borde de la línea, no avance
    }

    void draw() {
        for (size_t i = 0; i < art.size(); ++i) { // Se realiza para cada char del dibujo de la nave
            mvaddstr(y + i, x, art[i].c_str());  // Se hace un print a la pantalla en las posiciones seleccionadas
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
        /*case ' ':
            bullets.push_back(Bullet(ship.x + 2, ship.y - 1, '|')); // Adjust x position for bullet
            break;*/
    }
}    
