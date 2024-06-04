#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

bool showSecondShip = false;                        // Flag para mostrar la segunda nave
int finalScore = 0;

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
    void moveDown(){
        yposi = yposi + 1;
    }
    
};

class Nave {
public:
    int x, y;                                       // Posición de la nave
    std::vector<std::string> art;                   // ASCII art de la nave
    std::vector<std::string> lifeArt;               // ASCII art de las vidas
    int lives = 2;                                  // Cantidad de vidas
    std::vector<Bala> balas; 
    

    Nave(int posX, int posY) : x(posX), y(posY) {   // Se ocupan las posiciones
        art = {
            "    ^",
            "   /-\\",
            "--¦^¦^¦--"
        };
    }

    void initializeLifeArt() {
        lifeArt = {
            "  /-\\  ",
            "-|^ ^|-"
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
            if (showSecondShip == true){
                if (x < maxWidth - static_cast<int>(30)) ++x;
            };
            if (showSecondShip == false){
                if (x < maxWidth - static_cast<int>(12)) ++x;
            }
    }

    void drawNave() {   // Dibuja la nave
        for (size_t i = 0; i < art.size(); ++i) {
            mvaddstr(y + i, x, art[i].c_str());
        }
        if (showSecondShip) { // Draw la segunda nave si el flag esta set
            for (size_t i = 0; i < art.size(); ++i) {
                mvaddstr(y + i, x + static_cast<int>(18), art[i].c_str()); // Posiciona la segunda nave a la derecha 
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
        if (showSecondShip == true){
            int center1 = x + 4;
            Bala tempbala1 = Bala(center1, y);
            balas.push_back(tempbala1);

            int center2 = x + 22;
            Bala tempbala2 = Bala(center2, y);
            balas.push_back(tempbala2);
        };
        if (showSecondShip == false){
            int centerX = x + 4;
            Bala tempbala = Bala(centerX, y);
            balas.push_back(tempbala);
        }
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
        if (showSecondShip == false) { // Draw the second ship if the flag is set
            if (lives >= 0) {
                --lives;
                x = COLS / 2 - 7; // Se coloca la nave en el centro
                y = LINES - 6;
            }  
        }
        if (showSecondShip == true) { // Draw the second ship if the flag is set
            if (lives >= 0) {
                x = COLS / 2 - 7; // Se coloca la nave en el centro
                y = LINES - 6;
            }  
        }
    }
};


class Enemy {
public:
    int x, y;
    int puntuacion;
    std::vector<std::string> art;
    bool isAlive;

    Enemy(int initialpuntuacion): puntuacion(initialpuntuacion){}
    Enemy(int posX, int posY) : x(posX), y(posY), isAlive(true) {}

    virtual void update(int playerX, int playerY, Nave& player) = 0;

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
    int moveDirection; // 1 para moverse hacia abajo, -1 para moverse hacia arriba
    int moveCounter;
    int moveThreshold;
    float moveProbability; // Probabilidad de que se mueva en el siguiente ciclo
    

public:
    NormalEnemy(int posX, int posY) : Enemy(posX, posY), moveDirection(1), moveCounter(0), moveThreshold(5), moveProbability(0.4) { 
        puntuacion = 100;
        art = {
            "  /---\\  ",
            "  \\-o-/  "
        };

        // Randomize la probabilidad de moverse entre un 30% a 70% para variacion
        moveProbability = 0.3f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(0.4f)));
    }

    void update(int playerX, int playerY, Nave& player) override {
        if (rand() % 100 < moveProbability * 100) { // Convierte a porcentaje
            moveCounter++; // Increment counter 
            if (moveCounter >= moveThreshold) { 
                moveCounter = 0; // Reset counter
                if ((y >= 2+ playerY - static_cast<int>(art.size()) && moveDirection == 1) || (y <= 0 && moveDirection == -1)) {
                    moveDirection *= -1; // Reversa direccion cuando toca el borde de la pantalla
                }
                y += moveDirection; // Se mueve en base a la direccion
            }
        }
    }

};

class TurretEnemy : public Enemy {
private:
    int moveCounter = 0; // Counter
    int shootCounter = 0; // Counter
    int moveThreshold = 10; // Shoot cada 10 updates
    int shootThreshold = 60; // Shoot cada 60 updates

public:
    TurretEnemy(int posX, int posY) : Enemy(posX, posY) {
        puntuacion = 300;
        art = {
            ".'='.",
            "/|=|\\",
        };
    }

    std::vector<Bala> bullets;

    void shoot() {
        if (!isAlive) return; // no dispara si la torreta esta muerta
        int centerX = x + (width() / 2);
        Bala tempBala(centerX, y + height()); // Bullet empieza desde abajo de la torreta
        bullets.push_back(tempBala);
    }

    void drawBullets() {
        for (auto& bullet : bullets) {
            bullet.drawBala();
        }
    }

    void removeAllBullets(){
        bullets.clear();
    }

    void update(int playerX, int playerY, Nave& player) override {
        if (!isAlive) {
            bullets.clear(); // Limpia todas las balas cuando esta muerto
            return; // Stop updating cuando no esta vivo
        }

        // Update movimiento
        moveCounter++;
        if (moveCounter >= moveThreshold) {
            moveCounter = 0;
            x--; // Move left
            if (x + width() < 0) { // si la torreta desaparece de la pantalla mover a la derecha
                x = COLS - 1;
            }
        }

        // Update disparo
        shootCounter++;
        if (shootCounter >= shootThreshold) {
            shootCounter = 0;
            shoot(); // Turret dispara una bala
        }
            
        // Update la posicion de las balas
        for (int i = 0; i < bullets.size(); ++i) {
            bullets[i].moveDown();
            if (bullets[i].yposi >= LINES) { // Remove bullets que desaparecen de la pantalla
                bullets.erase(bullets.begin() + i);
                --i;
            }
        }
    }

    void draw() override {
        if (!isAlive) return; // No dibujar si esta muerto
        Enemy::draw(); 
        drawBullets(); // Draw balas
    }
};

class BossEnemy : public Enemy {

public:
     enum State { Descending, Holding, LateralMove, HasPlayer };

private:
    State currentState;
    int lateralDirection;  // 1 para derecha, -1 para izquierda
    int moveCounter;       // Counter para controlar la velocidad de caida
    int moveThreshold;     // Threshold a llegar antes de moverse hacia abajo

public:

    BossEnemy(int posX) : Enemy(posX, 0), currentState(Descending), lateralDirection(1), moveCounter(0), moveThreshold(10) {
        puntuacion = 500;
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

    void update(int playerX, int playerY, Nave& player) override {
        switch (currentState) {
            case Descending:
                moveCounter++; // Increment el counter
                if (moveCounter >= moveThreshold) {
                    moveCounter = 0; // Reset counter 
                    y += 1; // Se mueve lentamente abajo
                    if (y >= LINES / 1.3) { 
                        currentState = Holding; // Cambia estado a holding
                    }
                }
                break;

            case Holding:
                    currentState = LateralMove; // Cambia a lateralMove si no toca al jugador
                    y = 0; 
                break;

            case HasPlayer:
                y = 0; // Se mueve arriba si toca al jugador
                x += lateralDirection;
                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {
                    lateralDirection *= -1; // Cambia de direccion al tocar los bordes de la pantalla
                }
                break;
            
            case LateralMove:
                x += lateralDirection;
                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {
                    lateralDirection *= -1; // Cambia de direccion al tocar los bordes de la pantalla
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
        if (!isAlive) return; //No dibujar si no esta vivo
        Enemy::draw(); //Use la base de la clase para dibujar
    }
};

class Enemies {
public:

    std::vector<std::unique_ptr<Enemy>> enemyList;
    std::vector<std::pair<int, int>> initialPositions;
    bool enoughLives = true;
    int currentWave = 0;

      void spawnSingleRowOfEnemies() {
        int enemyWidth = 12;  // Ancho aproximado del enemigo
        int turretWidth = 10; 
        int spacing = 10;     // Espacio deseado entre nemigos
        int startX = 10;
        int startY = 5;       // Posicion Y inicial para enemigos normales
        int turretY = startY + 4;  // Y posicion para las torretas

        int maxEnemiesPerRow = (COLS - startX) / (enemyWidth + spacing); // Calcula cuantos enemigos por fila

        // Clear datos anteriores
        initialPositions.clear();
        enemyList.clear();

        // Calcula posicion para los enemigos normales
        for (int i = 0; i < maxEnemiesPerRow; ++i) {
            int posX = startX + i * (enemyWidth + spacing);
            if (posX + enemyWidth >= COLS) break; // Se asegura que no esten fuera de pantalla
            initialPositions.push_back({posX, startY});  // Posicion inicial
            enemyList.push_back(std::make_unique<NormalEnemy>(posX, startY));
            
            // Calcula posicion de las torretas entre los enemigos normales
            int turretPosX = posX + enemyWidth + (spacing - turretWidth) / 2;
            initialPositions.push_back({turretPosX, turretY});  // Gurda posicion inicial
            enemyList.push_back(std::make_unique<TurretEnemy>(turretPosX, turretY));
    }
}

 void spawnWave() {
        int startX = 10;
        int startY = -3;
        int turretY = startY + 15;
        int normalWidth = 12;  // Ancho de NormalEnemy
        int turretWidth = 10;  // Ancho de las torretas

        // Initial spacing
        int spacing = 10;
        int numberOfEnemies;

        if (currentWave== 4){
            currentWave = 0;
        }
            
        // Clear la lista actual de enemigos
        initialPositions.clear();
        enemyList.clear();

        switch (currentWave) {
            case 1:
            numberOfEnemies = 3; // 2 Normal + 1 Turret
            adjustSpacing(normalWidth, numberOfEnemies, spacing);
            
            enemyList.push_back(std::make_unique<NormalEnemy>(startX, startY));
            initialPositions.push_back({startX , startY});  // Record initial position
            
            enemyList.push_back(std::make_unique<NormalEnemy>(startX + normalWidth + spacing, startY));
            initialPositions.push_back({startX + normalWidth + spacing, startY}); 
            
            enemyList.push_back(std::make_unique<TurretEnemy>(startX + 2 * (normalWidth + spacing), turretY));
            initialPositions.push_back({startX + 2 * (normalWidth + spacing), turretY}); 
            break;
        case 2:
            numberOfEnemies = 4; // 2 Normal + 2 Turret
            adjustSpacing(normalWidth, numberOfEnemies, spacing);
            
            enemyList.push_back(std::make_unique<NormalEnemy>(startX, startY));
            initialPositions.push_back({startX, startY});  // Record initial position
            
            enemyList.push_back(std::make_unique<NormalEnemy>(startX + normalWidth + spacing, startY));
            initialPositions.push_back({startX + normalWidth + spacing, startY});  
            
            enemyList.push_back(std::make_unique<TurretEnemy>(startX + 2 * (normalWidth + spacing), turretY));
            initialPositions.push_back({startX + 2 * (normalWidth + spacing), turretY});  
            
            enemyList.push_back(std::make_unique<TurretEnemy>(startX + 3 * (normalWidth + spacing), turretY));
            initialPositions.push_back({startX + 3 * (normalWidth + spacing), turretY});  
            break;
        case 3:
            spawnSingleRowOfEnemies();
            break;
        default:
            // Default oleadas despues de la tercera oleada
            spawnSingleRowOfEnemies();
            break;
    }
}

    void adjustSpacing(int width, int numberOfEnemies, int &spacing) {
        int totalWidth = (width + spacing) * numberOfEnemies - spacing;  // Total width con el espacio inicial
        int availableSpace = COLS - (2 * 10);  // Espacio horizontal disponible

        // Increase spacing si hay campo
        while (totalWidth < availableSpace) {
            spacing += 2;  // Increase spacing gradualmente
            totalWidth = (width + spacing) * numberOfEnemies - spacing;
            if (totalWidth > availableSpace) {
                spacing -= 2;  // Reducir si ya no hay espacio
                break;
            }
        }
    }

     void spawnRowOfEnemies(int startX, int startY, int width, int spacing) {
        int posX = startX;
        while (posX + width < COLS) {
            enemyList.push_back(std::make_unique<NormalEnemy>(posX, startY));
            posX += width + spacing;
        }
    }

    void updateWave() {
        if (areAllNonBossEnemiesDefeated()) {
            currentWave++;
            spawnWave(); // Spawn la siguiente oleada de enemigos
        }
    }

    void spawnBoss(int posX) {
        enemyList.push_back(std::make_unique<BossEnemy>(posX));
    }

    void adjustPosition(int& startX, int& startY) {
        startX += 12; // Space out enemigos

        if (startX >= COLS - 10) {
            startX = 10;
            startY += 4;
        }
    }

    void updateEnemies(int playerX, Nave& player) {
        for (auto& enemy : enemyList) {
            enemy->update(playerX, player.y, player);
        }
    }

    bool checkCollision(const std::unique_ptr<Enemy>& enemy, Nave& player) {
    // Check colision para la primera nave
    for (int i = 0; i < player.height(); ++i) {
        for (int j = 0; j < player.width(); ++j) {
            int px = player.x + j;
            int py = player.y + i;
            if (px >= enemy->x && px < enemy->x + enemy->width() &&
                py >= enemy->y && py < enemy->y + enemy->height()) {
                // Colision detectada para la primera nave
                handleCollision(enemy, player);
                return true;
            }
        }
    }

    // Cehck colision de la segunnda nave
    if (showSecondShip) {
        int secondShipX = player.x + 15; // Calcula la posicion X para la segund anave
        for (int i = 0; i < player.height(); ++i) {
            for (int j = 0; j < player.width(); ++j) {
                int px = secondShipX + j;
                int py = player.y + i;
                if (px >= enemy->x && px < enemy->x + enemy->width() &&
                    py >= enemy->y && py < enemy->y + enemy->height()) {
                    // Colision detectada para la segunda nave
                    handleCollision(enemy, player);
                    return true;
                }
            }
        }
    }

    return false;
}

bool checkBulletCollision(Nave& player) {
    int secondShipOffsetX = 18; // Asume un offset de 18 para la segunda nave

    for (auto& enemy : enemyList) {
        if (auto* turret = dynamic_cast<TurretEnemy*>(enemy.get())) { // Check si el enemigo es un TurretEnemy
            for (int i = 0; i < turret->bullets.size(); ++i) {
                auto& bullet = turret->bullets[i];
                
                // Check si las coordenadas de la bala intersecan con las coordenadas de la primera nave
                bool hitPrimary = bullet.xposi >= player.x && bullet.xposi < player.x + 12 &&
                                  bullet.yposi >= player.y && bullet.yposi < player.y + 3;
                
                // Check si las coordenadas de la bala intersecan con las coordenadas de la segunda nave
                bool hitSecondShip = showSecondShip && // Only check if the second ship is shown
                                     bullet.xposi >= player.x + secondShipOffsetX &&
                                     bullet.xposi < player.x + secondShipOffsetX + 12 &&
                                     bullet.yposi >= player.y &&
                                     bullet.yposi < player.y + 3;

                if (hitPrimary || hitSecondShip) {
                    turret->bullets.erase(turret->bullets.begin() + i); // Borra la bala que colisiono
                    handleBulletCollision(player);
                    return true;  // Return true si hubo colision
                }
            }
        }
    }
    return false;
}

void handleBulletCollision(Nave& player) {
    if(!showSecondShip){
        player.decreaseLife();  // Player loses a life

    };
    if(showSecondShip){
        player.decreaseLife();
        showSecondShip = false;
    }

    if (player.lives != -1) { // Si se pierde una vida, dar momento de respiro al jugador
            clear();    // Limpiar pantalla
            player.drawLife(COLS / 5, LINES); // Mostrar vidas 
            mvprintw(LINES / 2, COLS / 2 - 5, "READY"); // Mensaje de alerta
            refresh();  // Refrescar la pantalla
            napms(2000);    // Se genera un delay de 2 segundos
        }
        clearTurretBullets();
        resetPositions();

}

void handleCollision(const std::unique_ptr<Enemy>& enemy, Nave& player) {
    if (auto* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
            player.decreaseLife();
            if (boss->getCurrentState() == BossEnemy::Holding) {
                boss->setCurrentState(BossEnemy::HasPlayer); // Set the state to HasPlayer
                if (player.lives != -1) {
                    mvprintw(LINES / 2, COLS / 2 - 5, "NAVE CAPTURADA"); // Imprimir mensaje
                    refresh();
                    napms(2000);
                    enoughLives = true;
                    clearTurretBullets();
                    resetPositions();
                } else if (player.lives == -1) {
                    enoughLives = false;
                    clearTurretBullets();
                    resetPositions();
                }
            }
    } else if (dynamic_cast<NormalEnemy*>(enemy.get())) {
        if (!showSecondShip) {
            player.decreaseLife();
        } else {
            player.decreaseLife();
            showSecondShip = false;
        }
        if (player.lives != -1) { // Si se pierde una vida, dar momento de respiro al jugador
            clear();    // Limpiar pantalla
            player.drawLife(COLS / 5, LINES); // Mostrar vidas 
            mvprintw(LINES / 2, COLS / 2 - 5, "READY"); // Mensaje de alerta
            refresh();  // Refrescar la pantalla
            napms(2000);    // Se genera un delay de 2 segundos
        }
        clearTurretBullets();
        resetPositions();
    }
}

    void checkCollisionBala(Nave& nave, int& finalScore) {
        for (int i = 0; i < nave.balas.size(); i++) {
            Bala bala = nave.balas[i];

            for (auto& enemy : enemyList) {

                if (!enemy->isAlive)
                    continue;

                int enemyW = enemy->x + enemy->width();
                int h = enemy->y + enemy->height();

                bool xCheck = bala.xposi > enemy->x && bala.xposi <= enemyW;
                bool yCheck = bala.yposi == enemy->y;

                if (xCheck && yCheck) {

                    if (auto* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
                        if (boss->getCurrentState() == BossEnemy::HasPlayer) {
                            if (enoughLives == true) {
                                showSecondShip = true;
                                clearTurretBullets();
                                resetPositions();
                            }
                        }
                    }  
                    finalScore = finalScore + enemy->puntuacion;
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

    void clearTurretBullets(){
        for (const auto& enemy : enemyList) {
            if (auto* turret = dynamic_cast<TurretEnemy*>(enemy.get())) {
                turret->removeAllBullets();
            }
        }
    }

    bool areAllNonBossEnemiesDefeated(){
         for (const auto& enemy : enemyList) {
            if (enemy->isAlive) {
                return false;  
            }
        }
        return true;  
    }

    bool isBossDead() {
    for (const auto& enemy : enemyList) {
        if (auto* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
            if (boss->isAlive) {
                return false;  
            }
        }
    }
    if (showSecondShip) {
        return false;  
    }
    return true;  
}

    void drawEnemies() {
        for (auto& enemy : enemyList) {
            enemy->draw();
        }
    }

    void spawnNewWave(){
        enemyList.clear();
        spawnSingleRowOfEnemies();
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
        case ' ':
            ship.shoot();
            break;
            
    }
}    
