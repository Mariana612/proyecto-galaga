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

private:
    struct HighScore {
        std::string name;
        int score;

        HighScore(const std::string& name, int score) : name(name), score(score) {}
    };

    static bool compareScores(const HighScore& a, const HighScore& b);

    std::string filename;
    std::vector<HighScore> highScores;
};

// Constructor
HighScores::HighScores(const std::string& filename) : filename(filename) {
    readHighScores();
}

// Comparar dos puntajes
bool HighScores::compareScores(const HighScore& a, const HighScore& b) {
    return a.score > b.score;  // Descending order
}

// Obtener el puntaje del .txt
void HighScores::readHighScores() {
    std::ifstream file(filename);
    std::string line;

    highScores.clear();

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            int score;
            if (iss >> name >> score) {
                highScores.emplace_back(name, score);
            }
        }
        file.close();
    }

    // Ordenar los puntajes de mayor a menor
    std::sort(highScores.begin(), highScores.end(), compareScores);
}

// Escrbir los puntajes en el .txt
void HighScores::writeHighScores() const {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& hs : highScores) {
            file << hs.name << " " << hs.score << std::endl;
        }
        file.close();
    }
}

// Agregar nuevo puntaje
void HighScores::updateHighScores(const std::string& name, int score) {
    HighScore newScore{name, score};
    highScores.push_back(newScore);
    std::sort(highScores.begin(), highScores.end(), compareScores);

    // Solo mantener los top 10 puntajes
    if (highScores.size() > 10) {
        highScores.resize(10);
    }

    writeHighScores();
}