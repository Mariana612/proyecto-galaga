
// Librerías que se deben incluir
#include <ncurses.h>
#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL_mixer.h>

// Definiciones de los colores
#define SHIP_PAIR 1
#define LIFE_PAIR 2
#define BULLET_PAIR 3

bool showSecondShip = false;                                       // Flag para mostrar la segunda nave
int finalScore = 0;

// --------------------BALAS--------------------
class Bala {
public:
    int xposi, yposi;

    Bala(int posX, int posY) : xposi(posX), yposi(posY) {}

    // Método para mover la bala hacia arriba
    void moveUp() {
        yposi = yposi - 1;
    }
    
    void drawBala() {
        attron(COLOR_PAIR(BULLET_PAIR));
        mvaddch(yposi, xposi, '|');                                // Dibuja la bala en la posición (x, y)
        attroff(COLOR_PAIR(BULLET_PAIR));
        refresh();
    }
    void moveDown(){
        yposi = yposi + 1;
    }
    
};

// --------------------NAVE DEL JUGADOR--------------------
class Nave {
public:
int animation = 0;
int animationCounter = 0;
    int x, y;                                                     // Posición de la nave
    std::vector<std::string> art;                                 // ASCII art de la nave
    std::vector<std::string> lifeArt;                             // ASCII art de las vidas
    int lives = 2;                                                // Cantidad de vidas
    std::vector<Bala> balas; 
    Mix_Chunk *balaSound;

    Nave(int posX, int posY) : x(posX), y(posY) {                 // Se ocupan las posiciones
        // Arte de la nave
        art = {
            "    ^",
            "   /-\\",
            "--¦^¦^¦--"
        };
        balaSound = Mix_LoadWAV("shot.mp3");                      // Sonido de disparo de la nave
        if (!balaSound) {
            fprintf(stderr, "No se pudo cargar el sonido de la bala: %s\n", Mix_GetError());
        }
    }

    void changeArtNave(int flag){
        if(flag==1){
        std::vector<std::string> art2 =  {
            "  /-\\  ",
            "-|^ ^|-"
        };
        lifeArt = art2;
         
        }
        else{
        std::vector<std::string> art2 =  {
            "       ",
            "       "
        };
        lifeArt = art2;
        }
     }

    ~Nave() {                                                     // Destructor
        if (balaSound) {
            Mix_FreeChunk(balaSound);
        }
    }

    void initializeLifeArt() {
        // Arte de los símbolos de las vidas
        lifeArt = {
            "  /-\\  ",
            "-|^ ^|-"
        };
    }

    int width() const {                                           // Anchura de la nave
        return art.empty() ? 0 : art[0].size();
    }

    int height() const {                                          // Altura de la nave
        return art.size();
    }
    
    void moveLeft() {                                             // Movimiento a la izquierda
        if (x > 0) --x;
    }

    void moveRight(int maxWidth) {    
             // Movimiento a la derecha
            if (showSecondShip == true){
                if (x < maxWidth - static_cast<int>(31)) ++x;     // Si hay una segunda nave, que esta no se pase del límite tampoco
            };
            if (showSecondShip == false){
                if (x < maxWidth - static_cast<int>(12)) ++x;     // No sobrepasarse del límite de la terminal
            }
    }

    void drawNave() {                                           // Dibujar la nave
        for (size_t i = 0; i < art.size(); ++i) {
            attron(COLOR_PAIR(SHIP_PAIR));
            mvaddstr(y + i, x, art[i].c_str());
            attroff(COLOR_PAIR(SHIP_PAIR));
        }
        if (showSecondShip) {                                     // Dibujr la segunda nave si el flag está encendido
            for (size_t i = 0; i < art.size(); ++i) {             // Posiciona la segunda nave a la derecha
                attron(COLOR_PAIR(SHIP_PAIR));
                mvaddstr(y + i, x + static_cast<int>(19), art[i].c_str());
                attroff(COLOR_PAIR(SHIP_PAIR));  
            }
        }
    }

    void drawLife(int maxX, int maxY) {                           // Dibujar las vidas
        if(animation == 0 && animationCounter == 25){
            animation = 1;
        }
        else if (animation == 1 && animationCounter == 25){
            animation = 0;}

        if(animationCounter == 25){
        animationCounter = -1;
        }
        animationCounter++;
        changeArtNave(animation)   ;   
        int startX = maxX - (lifeArt[0].size() + 3) * lives;
        int startY = maxY - lifeArt.size();
        for (int i = 0; i < lives; ++i) {
            for (size_t j = 0; j < lifeArt.size(); ++j) {
                attron(COLOR_PAIR(LIFE_PAIR));
                mvaddstr(startY + j, startX + i * (lifeArt[0].size() + 1), lifeArt[j].c_str());
                attroff(COLOR_PAIR(LIFE_PAIR)); 
            }
        }
    }
    void shoot() {
         if (balaSound) {
            Mix_VolumeChunk(balaSound, 30);                        // El volumen es 30
            Mix_PlayChannel(-1, balaSound, 0);                     // Reproduce el sonido en cualquier canal libre
        }
        if (showSecondShip == true){
            int center1 = x + 4;
            Bala tempbala1 = Bala(center1, y);
            balas.push_back(tempbala1);

            int center2 = x + 23;
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

    void decreaseLife() {                                         // Perder una vida
        if (showSecondShip == false) {                            // Si no hay una segunda nave
            if (lives >= 0) {
                --lives;                                          // Se resta una vida
                x = COLS / 2 - 7;                                 // Se coloca la nave en el centro
                y = LINES - 6;
            }  
        }
        if (showSecondShip == true) {                             // Si hay una segunda nave
            if (lives >= 0) {
                x = COLS / 2 - 7;                                 // Se coloca la nave en el centro
                y = LINES - 6;
            }  
        }
    }
};

// --------------------INPUT DEL USUARIO--------------------
void handleInput(int ch, Nave& ship) {   // Input del jugador
    switch (ch) {
        case KEY_LEFT:
            ship.moveLeft();             // Cuando se presiona la flecha izquierda se mueve a la izquierda
            break;
        case KEY_RIGHT:
            ship.moveRight(COLS);        // Cuando se presiona la flecha derecha se mueve a la derecha
            break;
        case ' ':                        // Cuando se presiona la tecla de espacio se dispara
            ship.shoot();
            break;
            
    }
}    
