#include "SaveGame.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>

namespace {
    const char* const kSaveFile = "partida_guardada.txt";
    const char* const kLegacySaveFile = "savegame.txt";

    bool parseInteger(const std::string& value, int& result) {
        std::istringstream input(value);
        char trailing = '\0';
        return (input >> result) && !(input >> trailing);
    }

    bool parseFloat(const std::string& value, float& result) {
        std::istringstream input(value);
        char trailing = '\0';
        return (input >> result) && !(input >> trailing) && std::isfinite(result);
    }

    bool validState(const SavedGame& state) {
        return state.initials.size() == 3
            && state.score >= 0
            && state.level >= 1
            && state.wave >= 0
            && state.lives >= 1
            && state.lives <= 5
            && std::isfinite(state.playerPosition.x)
            && std::isfinite(state.playerPosition.y);
    }

    sf::Vector2f clampPosition(const sf::Vector2f& position,
                               float screenWidth, float screenHeight) {
        const float maxX = std::max(20.f, screenWidth - 20.f);
        const float maxY = std::max(20.f, screenHeight - 20.f);
        return sf::Vector2f(std::max(20.f, std::min(maxX, position.x)),
                            std::max(20.f, std::min(maxY, position.y)));
    }

    bool readKeyValue(std::istream& input, SavedGame& state,
                      float screenWidth, float screenHeight) {
        std::map<std::string, std::string> values;
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }
            const std::string::size_type separator = line.find('=');
            if (separator == std::string::npos
                || !values.insert(std::make_pair(line.substr(0, separator),
                                                 line.substr(separator + 1))).second) {
                return false;
            }
        }

        int score = 0;
        int level = 0;
        int wave = 0;
        int lives = 0;
        std::map<std::string, std::string>::const_iterator initials = values.find("initials");
        std::map<std::string, std::string>::const_iterator scoreValue = values.find("score");
        std::map<std::string, std::string>::const_iterator levelValue = values.find("level");
        std::map<std::string, std::string>::const_iterator waveValue = values.find("wave");
        std::map<std::string, std::string>::const_iterator livesValue = values.find("lives");
        if (initials == values.end() || scoreValue == values.end()
            || levelValue == values.end() || waveValue == values.end()
            || livesValue == values.end()
            || !parseInteger(scoreValue->second, score)
            || !parseInteger(levelValue->second, level)
            || !parseInteger(waveValue->second, wave)
            || !parseInteger(livesValue->second, lives)) {
            return false;
        }

        sf::Vector2f position(screenWidth * 0.5f, screenHeight * 0.86f);
        std::map<std::string, std::string>::const_iterator xValue = values.find("x");
        std::map<std::string, std::string>::const_iterator yValue = values.find("y");
        if ((xValue == values.end()) != (yValue == values.end())
            || (xValue != values.end()
                && (!parseFloat(xValue->second, position.x)
                    || !parseFloat(yValue->second, position.y)))) {
            return false;
        }

        SavedGame loaded;
        loaded.initials = initials->second;
        loaded.score = score;
        loaded.level = level;
        loaded.wave = wave;
        loaded.lives = lives;
        loaded.playerPosition = clampPosition(position, screenWidth, screenHeight);
        if (!validState(loaded)) {
            return false;
        }
        state = loaded;
        return true;
    }

    bool readLegacy(std::istream& input, SavedGame& state,
                    float screenWidth, float screenHeight) {
        SavedGame loaded;
        if (!(input >> loaded.initials >> loaded.score >> loaded.level
                    >> loaded.wave >> loaded.lives)) {
            return false;
        }

        loaded.playerPosition = sf::Vector2f(screenWidth * 0.5f, screenHeight * 0.86f);
        std::string x;
        std::string y;
        if (input >> x) {
            if (!(input >> y) || !parseFloat(x, loaded.playerPosition.x)
                || !parseFloat(y, loaded.playerPosition.y)) {
                return false;
            }
        }

        std::string trailing;
        if (input >> trailing || !validState(loaded)) {
            return false;
        }
        loaded.playerPosition = clampPosition(loaded.playerPosition, screenWidth, screenHeight);
        state = loaded;
        return true;
    }

    bool readFile(std::istream& input, SavedGame& state,
                  float screenWidth, float screenHeight) {
        input >> std::ws;
        const int first = input.peek();
        if (first == std::char_traits<char>::eof()) {
            return false;
        }
        if (std::isalpha(static_cast<unsigned char>(first))) {
            std::string line;
            std::getline(input, line);
            if (line.find('=') == std::string::npos) {
                input.clear();
                input.seekg(0);
                return readLegacy(input, state, screenWidth, screenHeight);
            }
            input.clear();
            input.seekg(0);
        }
        return readKeyValue(input, state, screenWidth, screenHeight);
    }
}

bool SaveGameStore::load(SavedGame& state, float screenWidth, float screenHeight) {
    if (!std::isfinite(screenWidth) || !std::isfinite(screenHeight)
        || screenWidth < 40.f || screenHeight < 40.f) {
        std::cerr << "No se pudo cargar la partida: dimensiones de pantalla inválidas.\n";
        return false;
    }

    std::ifstream input(kSaveFile);
    if (!input.is_open()) {
        input.clear();
        input.open(kLegacySaveFile);
    }
    if (!input.is_open()) {
        return false;
    }

    if (!readFile(input, state, screenWidth, screenHeight)) {
        std::cerr << "El archivo de partida está dañado o contiene valores inválidos.\n";
        return false;
    }
    return true;
}

bool SaveGameStore::save(const SavedGame& state, float screenWidth, float screenHeight) {
    if (!std::isfinite(screenWidth) || !std::isfinite(screenHeight)
        || screenWidth < 40.f || screenHeight < 40.f || !validState(state)) {
        std::cerr << "No se pudo guardar la partida: estado inválido.\n";
        return false;
    }

    std::ofstream output(kSaveFile, std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "No se pudo abrir el archivo de partida para escritura.\n";
        return false;
    }

    const sf::Vector2f position = clampPosition(state.playerPosition, screenWidth, screenHeight);
    output << "initials=" << state.initials << '\n'
           << "score=" << state.score << '\n'
           << "level=" << state.level << '\n'
           << "wave=" << state.wave << '\n'
           << "lives=" << state.lives << '\n'
           << "x=" << std::setprecision(std::numeric_limits<float>::max_digits10)
           << position.x << '\n'
           << "y=" << position.y << '\n';
    output.close();
    if (!output) {
        std::cerr << "No se pudo escribir el archivo de partida.\n";
        return false;
    }
    return true;
}

bool SaveGameStore::clear() {
    bool success = true;
    const char* const files[] = {kSaveFile, kLegacySaveFile};
    for (std::size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i) {
        errno = 0;
        if (std::remove(files[i]) != 0 && errno != ENOENT) {
            std::cerr << "No se pudo eliminar el archivo de partida: "
                      << files[i] << '\n';
            success = false;
        }
    }
    return success;
}
