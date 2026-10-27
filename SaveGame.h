#ifndef SAVE_GAME_H
#define SAVE_GAME_H

#include <SFML/System/Vector2.hpp>
#include <string>

struct SavedGame {
    std::string initials;
    int score = 0;
    int level = 1;
    int wave = 0;
    int lives = 3;
    sf::Vector2f playerPosition;
};

class SaveGameStore {
public:
    static bool load(SavedGame& state, float screenWidth, float screenHeight);
    static bool save(const SavedGame& state, float screenWidth, float screenHeight);
    static bool clear();
};

#endif
