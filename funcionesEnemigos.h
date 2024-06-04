#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "enemy.h"
#include "nave-balas.h"



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
        int availableSpace = COLS - (2 * 10);                            // Espacio horizontal disponible

        // Increase spacing si hay campo
        while (totalWidth < availableSpace) {
            spacing += 2;                                                // Increase spacing gradualmente
            totalWidth = (width + spacing) * numberOfEnemies - spacing;
            if (totalWidth > availableSpace) {
                spacing -= 2;                                           // Reducir si ya no hay espacio
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
        int secondShipX = player.x + 15;                                           // Calcula la posicion X para la segunda nave
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
    int secondShipOffsetX = 18;                                                        // Asume un offset de 18 para la segunda nave

    for (auto& enemy : enemyList) {
        if (auto* turret = dynamic_cast<TurretEnemy*>(enemy.get())) {                  // Check si el enemigo es un TurretEnemy
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
                    turret->bullets.erase(turret->bullets.begin() + i);                 // Borra la bala que colisiono
                    handleBulletCollision(player);
                    return true;                                                        // Return true si hubo colision
                }
            }
        }
    }
    return false;
}

void handleBulletCollision(Nave& player) {
    if(!showSecondShip){
        player.decreaseLife();                                                           // Player loses a life

    };
    if(showSecondShip){
        player.decreaseLife();
        showSecondShip = false;
    }

    if (player.lives != -1) {                                                            // Si se pierde una vida, dar momento de respiro al jugador
            clear();                                                                     // Limpiar pantalla
            player.drawLife(COLS / 5, LINES);                                            // Mostrar vidas 
            mvprintw(LINES / 2, COLS / 2 - 5, "READY");                                  // Mensaje de alerta
            refresh();                                                                   // Refrescar la pantalla
            napms(2000);                                                                 // Se genera un delay de 2 segundos
        }
        clearTurretBullets();
        resetPositions();

}

void handleCollision(const std::unique_ptr<Enemy>& enemy, Nave& player) {
    if (auto* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
            player.decreaseLife();
            if (boss->getCurrentState() == BossEnemy::Holding) {
                boss->setCurrentState(BossEnemy::HasPlayer);                             // Set the state to HasPlayer
                if (player.lives != -1) {
                    mvprintw(LINES / 2, COLS / 2 - 5, "NAVE CAPTURADA");                 // Imprimir mensaje
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
        if (player.lives != -1) {                                        // Si se pierde una vida, dar momento de respiro al jugador
            clear();                                                     // Limpiar pantalla
            player.drawLife(COLS / 5, LINES);                            // Mostrar vidas 
            mvprintw(LINES / 2, COLS / 2 - 5, "READY");                  // Mensaje de alerta
            refresh();                                                   // Refrescar la pantalla
            napms(2000);                                                 // Se genera un delay de 2 segundos
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
