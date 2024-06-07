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
    void drawFinalScore() const;
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

void HighScores::drawFinalScore() const{
    int row, col;
    getmaxyx(stdscr, row, col); // Obtener el tamaño de la pantalla
    char header[][37] =
        {
            "     ___  ___ ___  _ __ ___        ",
            "    / __|/ __/ _ \\| '__/ _ \\      ",
            "    \\__ \\ (_| (_) | | |  __/       ",
            "    |___/\\___\\___/|_|  \\___|       ",
            "-----------------------------------",
    };
    clear();  
    // Mostrar el encabezado en pantalla usando ncurse
    for (int i = 0; i < 5; ++i)
    {
        attron(COLOR_PAIR(SHIP_PAIR));
        mvaddstr(row / 2 - 10 + i, (col - 37) / 2, header[i]);
        attroff(COLOR_PAIR(SHIP_PAIR));
    }

    const int MAX_PLAYERS = 10;
    char format[MAX_PLAYERS][60];

    getmaxyx(stdscr, row, col);

    for (int i = 0; i < highScores.size(); ++i) {
        snprintf(format[i], sizeof(format[i]), " NAME: %-10s - SCORE: %d", highScores[i].name.c_str(), highScores[i].score);
    }

    for (int i = 0; i < highScores.size(); ++i) {
        mvaddstr(row / 2 - 4 + i, (col - 37) / 2, format[i]);
    }
    refresh(); 
}

// Obtener la puntuación más baja
int HighScores::getLowestScore() const {
    if (highScores.size() < 10) {
        return 0;                                                           // Si hay menos de 10 puntuaciones, retornar un 0
    }
    return highScores.back().score;                                         // El último elemento del vector es la menor puntuación
}