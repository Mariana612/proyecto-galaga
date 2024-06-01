class Bala {
public:
    int x, y;

    Bala(int posX, int posY) : x(posX), y(posY) {}

    // Método para mover la bala hacia arriba
    void moveUp() {
        --y;
    }
};