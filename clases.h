#include <ncurses.h>
#include <vector>
#include <iostream>

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
