
#include "nave-balas.h"

// Definiciones para los colores
#define NORMAL_PAIR 4
#define TURRET_PAIR 5
#define BOSS_PAIR 6


// --------------------CLASE ABSTRACTA ENEMIGO--------------------
class Enemy {
public:
    int x, y;
    int puntuacion;
    std::vector<std::string> art;
    bool isAlive;

    Enemy(int initialpuntuacion): puntuacion(initialpuntuacion){}
    Enemy(int posX, int posY) : x(posX), y(posY), isAlive(true) {}

    // Actualizar posición del enemigo
    virtual void update(int playerX, int playerY, Nave& player) = 0;

    virtual int width() const {                                             // Anchura del enemigo
        return art.empty() ? 0 : art[0].size();
    }

    virtual int height() const {                                            // Altura del enemigo
        return art.size();
    }

    virtual void draw() {                                                   // Dibujar al enemigo
        if (!isAlive) return;

        for (size_t i = 0; i < art.size(); ++i) {
            mvaddstr(y + i, x, art[i].c_str());
        }
    }

    virtual ~Enemy() {}
};

// --------------------ENEMIGO NORMAL--------------------
class NormalEnemy : public Enemy {
private:
    int moveDirection;                                                      // 1 para moverse hacia abajo, -1 para moverse hacia arriba
    int moveCounter;
    int moveThreshold;
    float moveProbability;                                                  // Probabilidad de que se mueva en el siguiente ciclo
    

public:
int animation = 0;
int animationCounter = 0;
    NormalEnemy(int posX, int posY) : Enemy(posX, posY), moveDirection(1), moveCounter(0), moveThreshold(5), moveProbability(0.5) { 
        puntuacion = 100;
        art = {
            "  /---\\  ",
            "  \\-o-/  "
        };

        // Aleatorizar la probabilidad de moverse entre un 30% a 70% para variación
        moveProbability = 0.3f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(0.4f)));
    }
     void changeArtNormal(int flag){
        if(flag==1){
        std::vector<std::string> art2 = {
            "  doob  ",
            "  ~||~  "
        };
        art = art2;
         
        }
        else{
        std::vector<std::string> art2 = {
            "  doob  ",
            "  ^/\\^  "
        };
        art = art2;
        }
     }

    void update(int playerX, int playerY, Nave& player) override {
        if (rand() % 100 < moveProbability * 100) {                         // Convierte a porcentaje
            moveCounter++;                                                  // Incrementar contador
            if (moveCounter >= moveThreshold) { 
                moveCounter = 0;                                            // Inicializar contador
                if ((y >= 2+ playerY - static_cast<int>(art.size()) && moveDirection == 1) || (y <= 0 && moveDirection == -1)) {
                    moveDirection *= -1;                                    // Reversa direccion cuando toca el borde de la pantalla
                }
                y += moveDirection;                                         // Se mueve en base a la direccion
            }
        }
    }

    void draw() override {                                                  // Dibujar enemigo
        if (!isAlive) return;    
        if(animation == 0 && animationCounter == 20){
            animation = 1;
        }
        else if (animation == 1 && animationCounter == 20){
            animation = 0;}

        if(animationCounter == 20){
        animationCounter = -1;
        }
        animationCounter++; 
        changeArtNormal(animation)   ;                                           // No dibujar si esta muerto
        attron(COLOR_PAIR(NORMAL_PAIR));
        Enemy::draw();
        attroff(COLOR_PAIR(NORMAL_PAIR)); 
    }

};

// --------------------ENEMIGO QUE DISPARA--------------------
class TurretEnemy : public Enemy {
private:
    
    int moveCounter = 0;                                                    // Contador
    int shootCounter = 0;                                                   // Contador
    int moveThreshold = 10;                                                 // Disparar cada 10 updates
    int shootThreshold = 60;                                                // Disparar cada 60 updates

public:
int animation = 0;
int animationCounter = 0;
    TurretEnemy(int posX, int posY) : Enemy(posX, posY) {
        puntuacion = 300;
        art = {
            ".'='.",
            "/|=|\\",
        };

    }

    void changeArtTurret(int flag){
        if(flag==1){
        std::vector<std::string> art2 = {
            ".'o'.",
            "\\\\=//",
        };
        art = art2;
         
        }
        else{
        std::vector<std::string> art2  = {
            ".'='.",
            "/|=|\\",
        };
        
        art = art2;
        }
    }

    std::vector<Bala> bullets;


    
    void shoot() {
        if (!isAlive) return;     
                                             // no dispara si la torreta esta muerta
        int centerX = x + (width() / 2);
        Bala tempBala(centerX, y + height());                              // Bullet empieza desde abajo de la torreta
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
            bullets.clear();                                               // Limpia todas las balas cuando esta muerto
            return;                                                        // Stop updating cuando no esta vivo
        }

        // Update movimiento
        if(animation == 0 && animationCounter == 10){
            animation = 1;
        }
        else if (animation == 1 && animationCounter == 10){
            animation = 0;}

        if(animationCounter == 10){
        animationCounter = -1;
        }
        animationCounter++;
        changeArtTurret(animation)   ; 
        moveCounter++;
        if (moveCounter >= moveThreshold) {
            moveCounter = 0;
            x--; // Move left
            if (x + width() < 0) {                                         // si la torreta desaparece de la pantalla mover a la derecha
                x = COLS - 1;
            }
        }

        // Update disparo
        shootCounter++;
        if (shootCounter >= shootThreshold) {
            shootCounter = 0;
            shoot();                                                       // Turret dispara una bala
        }
            
        // Update la posicion de las balas
        for (int i = 0; i < bullets.size(); ++i) {
            bullets[i].moveDown();
            if (bullets[i].yposi >= LINES) {                               // Remove bullets que desaparecen de la pantalla
                bullets.erase(bullets.begin() + i);
                --i;
            }
        }
    }

    void draw() override {
        if (!isAlive) return;                                              // No dibujar si esta muerto
        attron(COLOR_PAIR(TURRET_PAIR));
        Enemy::draw();
        attroff(COLOR_PAIR(TURRET_PAIR)); 
        drawBullets();                                                     // Draw balas
    }
};

// --------------------BOSS GALAGA--------------------
class BossEnemy : public Enemy {
public:
    enum State { Descending, Holding, LateralMove, HasPlayer };

private:
    State currentState;
    int lateralDirection;                        // 1 para derecha, -1 para izquierda
    int moveCounter;                             // Counter para controlar la velocidad de caida
    int moveThreshold;                           // Threshold a llegar antes de moverse hacia abajo

public:
    int animation = 0;
    int animationCounter = 0;
    
    BossEnemy(int posX) : Enemy(posX, 0), currentState(Descending), lateralDirection(1), moveCounter(0), moveThreshold(10) {
        puntuacion = 500;
        // Arte del boss Galaga
        art = {
            "  /---\\",
            " --WWW--",
            "{#o#o#o#}"
        };
    }

    void changeArtBoss(int flag){
        if(flag==1){
            std::vector<std::string> art2 = {
                "  /---\\",
                " --WWW--",
                "{#o#o#o#}"
            };
            art = art2;
        } else {
            std::vector<std::string> art2 = {
                "  /---\\",
                " --WWW--",
                "{0#0#0#0}"
            };
            art = art2;
        }
    }

    State getCurrentState() const {               // Se obtiene el estado actual
        return currentState;
    }

    void setCurrentState(State state) {           // Se guarda el estado deseado
        currentState = state;
    }

    // Actualizar la posición del enemigo
    void update(int playerX, int playerY, Nave& player) override {
        // Update animation
        if (animation == 0 && animationCounter == 25) {
            animation = 1;
        } else if (animation == 1 && animationCounter == 25) {
            animation = 0;
        }

        if (animationCounter == 25) {
            animationCounter = -1;
        }
        animationCounter++;
        changeArtBoss(animation);

        switch (currentState) {
            case Descending:
                moveCounter++;                   // Incrementar el contador
                if (moveCounter >= moveThreshold) {
                    moveCounter = 0;             // Inicializar el contador 
                    y += 1;                      // Se mueve lentamente abajo
                    if (y >= playerY-2) {          // Cambia estado a holding cuando alcanza la posición del jugador
                        currentState = Holding;  // Cambia estado a holding
                    }
                }
                break;

            case Holding:
                currentState = LateralMove;  // Cambia a lateralMove si no toca al jugador
                y = 0; 
                break;

            case HasPlayer:
                y = 0;                          // Se mueve arriba si toca al jugador
                x += lateralDirection;
                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {
                    lateralDirection *= -1;     // Cambia de dirección al tocar los bordes de la pantalla
                }
                break;
            
            case LateralMove:
                x += lateralDirection;          // Se mueve arriba si no toca al jugador
                if (x <= 0 || x >= COLS - static_cast<int>(art[0].size())) {
                    lateralDirection *= -1;     // Cambia de dirección al tocar los bordes de la pantalla
                }
                break;
        }
    }

    int width() const override {                // Ancho del enemigo
        return art.empty() ? 0 : art[0].size();
    }

    int height() const override {               // Altura del ASCII del enemigo
        return art.size();
    }

    void draw() override {
        if (!isAlive) return;                   // No dibujar si no esta vivo
        attron(COLOR_PAIR(BOSS_PAIR));
        Enemy::draw();                          // Dibujar al enemigo
        attroff(COLOR_PAIR(BOSS_PAIR));
    }
};