#include <fstream>
#include <algorithm>
#include <sstream>

#include "funcionesEnemigos.h"

class HighScores {
public:
    HighScores(const std::string& filename);
    void readHighScores();
    void writeHighScores() const;
    void updateHighScores(const std::string& name, int score);
    int getLowestScore() const;

private:
    // Estructura para cada jugador y su puntaje
    struct HighScore {
        std::string name;
        int score;
    };

    static bool compareScores(const HighScore& a, const HighScore& b);

    std::string filename;                                                   // Nombre del archivo
    std::vector<HighScore> highScores;                                      // Vector de los puntajes
};

// Constructor
HighScores::HighScores(const std::string& filename) : filename(filename) {
    readHighScores();                                                       // Se leen las puntuaciones
}

// Se comapran puntuaciones
bool HighScores::compareScores(const HighScore& a, const HighScore& b) {
    return a.score > b.score;
}

// Se lee lo que se encuentra en el archivo
void HighScores::readHighScores() {
    std::ifstream file(filename);                                           // Se abre el archivo
    std::string line;

    highScores.clear();                                                     // Se limpia el vector

    if (file.is_open()) {
        while (std::getline(file, line)) {                                  // Se lee la línea del archivo
            std::istringstream iss(line);                                   // Se extrae el nombre y el puntaje
            HighScore hs;                                                   // Se crea una estructura para cada línea
            if (iss >> hs.name >> hs.score) {
                highScores.push_back(hs);                                   // Se guarda en un vector
            }
        }
        file.close();                                                       // Se cierra el archivo
    }

    // Se guardan de mayor a menor
    std::sort(highScores.begin(), highScores.end(), compareScores);
}

// Se escribe la puntuación
void HighScores::writeHighScores() const {
    std::ofstream file(filename);                                           // Se abre el archivo

    if (file.is_open()) {
        for (const auto& hs : highScores) {                                 // Se escribe el nombre, un espacio, la puntuación y un enter
            file << hs.name << " " << hs.score << std::endl;
        }
        file.close();                                                       // Se cierra el archivo
    }
}

// Se actualizan las puntuaciones
void HighScores::updateHighScores(const std::string& name, int score) { 
    HighScore newScore{name, score};                                        // Nombre y puntuación nueva
    highScores.push_back(newScore);                                         // Se guarda en un vector
    std::sort(highScores.begin(), highScores.end(), compareScores);         // Se comparan las puntuaciones

    // Se mantiene solo el top 10
    if (highScores.size() > 10) {
        highScores.resize(10);
    }

    writeHighScores();                                                      // Se escriben las puntuaciones
}

// Obtener la puntuación más baja
int HighScores::getLowestScore() const {
    if (highScores.empty()) {
        return 0;                                                           // Si noy hay puntuaciones, retornar un 0
    }
    return highScores.back().score;                                         // El último elemento del vector es la menor puntuación
}