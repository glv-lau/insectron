#ifndef GAME_H
#define GAME_H

#include "SaveGame.h"
#include "Scenes/Scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

class Game {
public:
    Game(Scene* scene, int score = 0);
    ~Game();

    void play();
    void setScene(Scene* nextScene);
    void setContinueAvailable(bool enabled) { m_continueAvailable = enabled; }
    bool hasContinueAvailable() const { return m_continueAvailable; }

    void setMasterVolume(int volume);
    int getMasterVolume() const;
    void setMusicVolume(int volume);
    int getMusicVolume() const;
    void setSfxVolume(int volume);
    int getSfxVolume() const;
    void setHudSize(int size);
    int getHudSize() const;
    int getWindowWidth() const;
    int getWindowHeight() const;
    bool isFullscreen() const;
    void setDisplaySettings(int width, int height, bool fullscreen);

    void loadConfigFromFile();
    void saveConfigToFile() const;
    bool saveGameState(const SavedGame& state);
    void clearSavedGame();
    void applyVolumeToAudio() const;

    sf::RenderWindow& getWindow() { return m_win; }
    int getScore() const { return m_score; }
    void addScore(int points) { m_score += points; }
    void setPlayerInitials(const std::string& initials) { m_playerInitials = initials; }
    const std::string& getPlayerInitials() const { return m_playerInitials; }
    const SavedGame& getSavedGame() const { return m_savedGame; }

private:
    int m_score;
    bool m_continueAvailable;
    std::string m_playerInitials;
    SavedGame m_savedGame;
    int m_masterVolume;
    int m_musicVolume;
    int m_sfxVolume;
    int m_hudSize;
    int m_windowWidth;
    int m_windowHeight;
    bool m_fullscreen;
    sf::RenderWindow m_win;
    Scene* m_scene;
    Scene* m_nextScene = nullptr;
};

#endif
