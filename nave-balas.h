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
