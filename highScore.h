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
    };

    static bool compareScores(const HighScore& a, const HighScore& b);

    std::string filename;
    std::vector<HighScore> highScores;
};

HighScores::HighScores(const std::string& filename) : filename(filename) {
    readHighScores();
}

// Se comapran puntuaciones
bool HighScores::compareScores(const HighScore& a, const HighScore& b) {
    return a.score > b.score;
}

// Se lee lo que se encuentra en el archivo
void HighScores::readHighScores() {
    std::ifstream file(filename);
    std::string line;

    highScores.clear();

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            HighScore hs;
            if (iss >> hs.name >> hs.score) {
                highScores.push_back(hs);
            }
        }
        file.close();
    }

    // Se guardan de mayor a menor
    std::sort(highScores.begin(), highScores.end(), compareScores);
}

// Se escribe la puntuación
void HighScores::writeHighScores() const {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& hs : highScores) {
            file << hs.name << " " << hs.score << std::endl;
        }
        file.close();
    }
}

// Se actualizan las puntuaciones
void HighScores::updateHighScores(const std::string& name, int score) {
    HighScore newScore{name, score};
    highScores.push_back(newScore);
    std::sort(highScores.begin(), highScores.end(), compareScores);

    // Se mantiene solo el top 10
    if (highScores.size() > 10) {
        highScores.resize(10);
    }

    writeHighScores();
}