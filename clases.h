#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

bool showSecondShip = false; // Flag to show the second ship

class Bala {
public:
    int xposi, yposi;

    Bala(int posX, int posY) : xposi(posX), yposi(posY) {}

    // Método para mover la bala hacia arriba
    void moveUp() {
        yposi = yposi - 1;
    }
    
    void drawBala() {
        mvaddch(yposi, xposi, '|'); // Dibuja la bala en la posición (x, y)
        refresh();
    }

    
};

class Nave {
public:
    int x, y;               // Position of the ship
    std::vector<std::string> art; // ASCII art of the ship
    std::vector<std::string> lifeArt; // ASCII art of a life
    int lives = 2; // Number of lives
    std::vector<Bala> balas; 
    

    Nave(int posX, int posY) : x(posX), y(posY) {
        art = {
            "    ^    ",
            "   /-\\  ",
            "--¦^¦^¦--",
        };
    }

    void initializeLifeArt() {
        lifeArt = {
            "  /-\\ ",
            "-|^ ^|-",
        };
    }

    int width() const {
        return art.empty() ? 0 : art[0].size();
    }

    int height() const {
        return art.size();
    }
    
    void moveLeft() {   // Movimiento a la izquierda
        if (x > 0) --x;
    }

    void moveRight(int maxWidth) {  // Movimiento a la derecha
        if (x < maxWidth - static_cast<int>(9)) ++x; 
    }

    void drawNave() {   // Draw the ship
        for (size_t i = 0; i < art.size(); ++i) {
            mvaddstr(y + i, x, art[i].c_str());
        }
        if (showSecondShip) { // Draw the second ship if the flag is set
            for (size_t i = 0; i < art.size(); ++i) {
                mvaddstr(y + i, x + 11, art[i].c_str()); // Position the second ship to the right
            }
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
    void shoot() {
        int centerX = x + (art[0].size() / 2);
        Bala tempbala = Bala(centerX, y);
        balas.push_back(tempbala);
        //tempbala.drawBala();
    }

    void updateBalasPos() {
        // itera por lista de balas usando el indice
        // si se elimina una bala, se corrige el indice
        for(int i = 0; i < balas.size(); i++) {

            Bala& bullet = balas[i];
            bullet.moveUp();

            // si bala se sale de pantalla, se destruye
            if (bullet.yposi < 0) {
                removeBala(i);
                continue;
            }
        }
    }

    void drawBalas()
    {
        for(auto& bullet : balas) {
            bullet.drawBala();
        }
    }

    void removeBala(int balaIndex)
    {
        Bala& bullet = balas[balaIndex];
        balas.erase(balas.begin() + balaIndex);
        bullet.~Bala();
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

    virtual void update(int playerX, int playerY) = 0;

    virtual int width() const {
        return art.empty() ? 0 : art[0].size();
    }

    virtual int height() const {
        return art.size();
    }

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
    float moveProbability; // Probability that this enemy will attempt to move each cycle

public:
    NormalEnemy(int posX, int posY) : Enemy(posX, posY), moveDirection(1), moveCounter(0), moveThreshold(5), moveProbability(0.5) { // 50% probability of moving
        art = {
            "  /---\\  ",
            "  \\-o-/  "
        };
        // Randomize the move probability between 30% to 70% for variation
        moveProbability = 0.3f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(0.4f)));
    }

    void update(int playerX, int playerY) override {
        if (rand() % 100 < moveProbability * 100) { // Convert probability to a percentage and compare
            moveCounter++; // Increment counter every update call
            if (moveCounter >= moveThreshold) { // Check if counter has reached the threshold
                moveCounter = 0; // Reset counter
                if ((y >= 2+ playerY - static_cast<int>(art.size()) && moveDirection == 1) || (y <= 0 && moveDirection == -1)) {
                    moveDirection *= -1; // Reverse direction at the boundaries
                }
                y += moveDirection; // Move based on the direction
            }
        }
    }

};

class TurretEnemy : public Enemy {
private:
    int moveCounter = 0; // Counter to control movement speed
    int moveThreshold = 30; // Move every 30 updates

public:

    TurretEnemy(int posX, int posY) : Enemy(posX, posY) {
        art = {
            "[#####]",
            "|#####|"
        };
    }



    void update(int playerX, int playerY) override {
        moveCounter++;
        if (moveCounter >= moveThreshold) {
            moveCounter = 0; // Reset counter
            x--; // Move left
            if (x + width() < 0) { // Check if the entire width of the enemy has moved off-screen
                x = COLS - 1; // Reset position to the far right side of the screen
            }
        }
    }
};

class BossEnemy : public Enemy {

public:
     enum State { Descending, Holding, LateralMove, HasPlayer };

private:
    State currentState;
    int lateralDirection;  // 1 for right, -1 for left
    int moveCounter;       // Counter to control the descending speed
    int moveThreshold;     // Threshold to reach before moving downwards

public:

    BossEnemy(int posX) : Enemy(posX, 0), currentState(Descending), lateralDirection(1), moveCounter(0), moveThreshold(10) {
        art = {
            "  /---\\",
            " --WWW--",
            "{#######}"
        };
    }

    State getCurrentState() const{
        return currentState;
    }

    void setCurrentState(State state) {
        currentState = state;
    }

    void update(int playerX, int playerY) override {
        switch (currentState) {

            case Descending:
                moveCounter++; // Increment the counter each time update is called
                if (moveCounter >= moveThreshold) {
                    moveCounter = 0; // Reset counter once threshold is reached
                    y += 1; // Move down slowly
                    if (y >= LINES / 1.2) { // Change this condition to control where it stops
                        currentState = Holding; // Change state to holding
                    }
                }
                break;
            case Holding:
                // Stay in position, check for collision with player
                if (x == playerX) { // Simplified collision detection
                    currentState = HasPlayer; // Change to lateral movement
                    y = 0; // Optionally move to top if that's needed after collision
                } else if (x != playerX) { // Simplified collision detection
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

            case HasPlayer:
                x += lateralDirection;

                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {
                    lateralDirection *= -1; // Change direction when hitting screen borders
                }
                break;
        }
    }

    int width() const override {
        return art.empty() ? 0 : art[0].size();
    }

    int height() const override {
        return art.size();
    }

    void draw() override {
        if (!isAlive) return; // Do not draw if not alive
        Enemy::draw(); // Use the base class draw method
    }
};

class Enemies {
public:

    std::vector<std::unique_ptr<Enemy>> enemyList;
    std::vector<std::pair<int, int>> initialPositions;

      void spawnSingleRowOfEnemies() {
        int enemyWidth = 12;  // Approximate width of the NormalEnemy
        int turretWidth = 10; // Approximate width of the TurretEnemy
        int spacing = 10;     // Desired space between NormalEnemies
        int startX = 10;
        int startY = 5;       // Starting Y position for the normal enemies
        int turretY = startY + 4;  // Y position for turret enemies, slightly below normal enemies

        int maxEnemiesPerRow = (COLS - startX) / (enemyWidth + spacing); // Calculate how many enemies fit per row

        // Clear previous data in initialPositions
        initialPositions.clear();
        enemyList.clear();

        // Calculate position for NormalEnemies
        for (int i = 0; i < maxEnemiesPerRow; ++i) {
            int posX = startX + i * (enemyWidth + spacing);
            if (posX + enemyWidth >= COLS) break; // Ensure enemies do not spawn off-screen
            initialPositions.push_back({posX, startY});  // Record initial position
            enemyList.push_back(std::make_unique<NormalEnemy>(posX, startY));
            
            // Calculate position for TurretEnemies in the spaces between NormalEnemies
             if (i % 2 == 0 && i < maxEnemiesPerRow - 1) { // Spawn turret every other normal enemy
            int turretPosX = posX + enemyWidth + (spacing - turretWidth) / 2;
            initialPositions.push_back({turretPosX, turretY});  // Record initial position for Turret
            enemyList.push_back(std::make_unique<TurretEnemy>(turretPosX, turretY));
        }
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
            enemy->update(playerX, player.y);
        }
    }

    bool checkCollision(const std::unique_ptr<Enemy>& enemy, Nave& player) {
        for (int i = 0; i < player.height(); ++i) {
            for (int j = 0; j < player.width(); ++j) {
                int px = player.x + j;
                int py = player.y + i;
                if (px >= enemy->x && px < enemy->x + enemy->width() &&
                    py >= enemy->y && py < enemy->y + enemy->height()) {
                    // Collision detected
                    if (auto* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
                        if (boss->getCurrentState() == BossEnemy::Holding) {
                            if (player.lives != 0){
                                player.decreaseLife();
                                showSecondShip = true;
                            }else if(player.lives == 0){
                                player.decreaseLife();
                            }
                        }
                    } else if (dynamic_cast<NormalEnemy*>(enemy.get())) {
                        if(showSecondShip == false){
                        player.decreaseLife();
                        } else if (showSecondShip == true){
                            showSecondShip = false;
                        };
                        if (player.lives != -1){ // Si se pierde una vida, dar momento de respiro al jugador
                            clear();    // Limpiar pantalla
                            player.drawLife(COLS / 5, LINES); // Mostrar vidas 
                            mvprintw(LINES / 2, COLS / 2 - 5, "READY"); // Mensaje de alerta
                            refresh();  // Refrescar la pantalla
                            napms(2000);    // Se genera un delay de 2 segundos
                        };
                        resetPositions();
                    }
                    return true;
                }
            }
        }
        return false;
    }

    void checkCollisionBala(Nave& nave) {
        for(int i = 0; i < nave.balas.size(); i++) {

            Bala bala = nave.balas[i];
            
            for(auto& enemy : enemyList) {

                if (!enemy->isAlive)
                    continue;

                int enemyW = enemy->x + enemy->width(),
                    h = enemy->y + enemy->height();

                bool xCheck = bala.xposi > enemy->x && bala.xposi <= enemyW;
                bool yCheck = bala.yposi == enemy->y;// && bala.yposi <= h;

                // si bala golpea un enemigo, destruye al enemigo y destruye la bala
                if (xCheck && yCheck)
                {
                    enemy->isAlive = false;
                    nave.removeBala(i);
                    --i;
                    break;
                }
            }
        }
    }

    void resetPositions() {
        for (size_t i = 0; i < enemyList.size(); ++i) {
            if (i < initialPositions.size()) {  // Check to prevent out-of-range errors
                enemyList[i]->x = initialPositions[i].first;
                enemyList[i]->y = initialPositions[i].second;
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
        case ' ':
            ship.shoot();
            break;
            
    }
}    
