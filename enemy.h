#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "nave-balas.h"
#include "funcionesEnemigos.h"


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