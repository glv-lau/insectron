#include "Game.h"
#include "Managers/AudioManager.h"
#include "Entities/Entity.h"
#include "SaveGame.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

namespace {
    const std::string kConfigFile = "config.txt";
    int clampVolume(int value) {
        if (value < 0) return 0;
        if (value > 100) return 100;
        return value;
    }
}

Game::Game(Scene* scene, int score)
    : m_score(score)
    , m_continueAvailable(false)
    , m_playerInitials("AAA")
    , m_masterVolume(100)
    , m_musicVolume(100)
    , m_sfxVolume(100)
    , m_hudSize(100)
    , m_windowWidth(800)
    , m_windowHeight(600)
    , m_fullscreen(false)
    , m_win(sf::VideoMode(800, 600), "Insectron")
    , m_scene(scene)
{
    m_win.setFramerateLimit(60);
    Entity::setWorldBounds(static_cast<float>(m_windowWidth),
                           static_cast<float>(m_windowHeight));
    loadConfigFromFile();
    setDisplaySettings(m_windowWidth, m_windowHeight, m_fullscreen);
    applyVolumeToAudio();

    if (SaveGameStore::load(m_savedGame, static_cast<float>(m_windowWidth),
                            static_cast<float>(m_windowHeight))) {
        m_continueAvailable = true;
        m_score = m_savedGame.score;
        m_playerInitials = m_savedGame.initials;
    }
}

Game::~Game() {
    saveConfigToFile();
    delete m_scene;
}

void Game::play() {
    sf::Clock clock;

    while (m_win.isOpen()) {
        const float dt = clock.restart().asSeconds();

        sf::Event event;
        while (m_win.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_win.close();
            }
            if (m_scene) {
                m_scene->handleEvent(event, *this);
            }
        }

        if (m_scene) {
            m_scene->update(dt, *this);
            m_win.clear(sf::Color(15, 15, 20));
            m_scene->draw(m_win);
            m_win.display();
        }

        if (m_nextScene) {
            delete m_scene;
            m_scene = m_nextScene;
            m_nextScene = nullptr;
        }
    }
}

void Game::setScene(Scene* nextScene) {
    m_nextScene = nextScene;
}

void Game::setMasterVolume(int volume) {
    m_masterVolume = clampVolume(volume);
    applyVolumeToAudio();
    saveConfigToFile();
}

int Game::getMasterVolume() const {
    return m_masterVolume;
}

void Game::setMusicVolume(int volume) {
    m_musicVolume = clampVolume(volume);
    applyVolumeToAudio();
    saveConfigToFile();
}

int Game::getMusicVolume() const {
    return m_musicVolume;
}

void Game::setSfxVolume(int volume) {
    m_sfxVolume = clampVolume(volume);
    applyVolumeToAudio();
    saveConfigToFile();
}

int Game::getSfxVolume() const {
    return m_sfxVolume;
}

void Game::setHudSize(int size) {
    m_hudSize = clampVolume(size);
    saveConfigToFile();
}

int Game::getHudSize() const {
    return m_hudSize;
}

int Game::getWindowWidth() const { return m_windowWidth; }
int Game::getWindowHeight() const { return m_windowHeight; }
bool Game::isFullscreen() const { return m_fullscreen; }

void Game::setDisplaySettings(int width, int height, bool fullscreen) {
    m_windowWidth = std::max(640, std::min(1920, width));
    m_windowHeight = std::max(480, std::min(1080, height));
    m_fullscreen = fullscreen;
    const sf::Uint32 style = fullscreen ? sf::Style::Fullscreen : sf::Style::Default;
    m_win.create(sf::VideoMode(static_cast<unsigned int>(m_windowWidth),
                               static_cast<unsigned int>(m_windowHeight)),
                 "Insectron", style);
    m_win.setFramerateLimit(60);
    Entity::setWorldBounds(static_cast<float>(m_windowWidth),
                           static_cast<float>(m_windowHeight));
    saveConfigToFile();
}

void Game::applyVolumeToAudio() const {
    AudioManager::instance().setMasterVolume(m_masterVolume);
    AudioManager::instance().setMusicVolume(m_musicVolume);
    AudioManager::instance().setSfxVolume(m_sfxVolume);
}

void Game::loadConfigFromFile() {
    std::ifstream input(kConfigFile.c_str());
    if (!input.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        const std::string::size_type pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 1);
        std::istringstream iss(value);
        int numericValue = 0;
        iss >> numericValue;

        if (key == "master") {
            m_masterVolume = clampVolume(numericValue);
        } else if (key == "music") {
            m_musicVolume = clampVolume(numericValue);
        } else if (key == "sfx") {
            m_sfxVolume = clampVolume(numericValue);
        } else if (key == "hud") {
            m_hudSize = clampVolume(numericValue);
        } else if (key == "width") {
            m_windowWidth = numericValue;
        } else if (key == "height") {
            m_windowHeight = numericValue;
        } else if (key == "fullscreen") {
            m_fullscreen = numericValue != 0;
        }
    }
}

void Game::saveConfigToFile() const {
    std::ofstream output(kConfigFile.c_str(), std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        return;
    }

    output << "master=" << m_masterVolume << "\n";
    output << "music=" << m_musicVolume << "\n";
    output << "sfx=" << m_sfxVolume << "\n";
    output << "hud=" << m_hudSize << "\n";
    output << "width=" << m_windowWidth << "\n";
    output << "height=" << m_windowHeight << "\n";
    output << "fullscreen=" << (m_fullscreen ? 1 : 0) << "\n";
}

bool Game::saveGameState(const SavedGame& state) {
    if (!SaveGameStore::save(state, static_cast<float>(m_windowWidth),
                             static_cast<float>(m_windowHeight))) {
        return false;
    }
    m_savedGame = state;
    m_savedGame.playerPosition.x = std::max(
        20.f, std::min(static_cast<float>(m_windowWidth) - 20.f, state.playerPosition.x));
    m_savedGame.playerPosition.y = std::max(
        20.f, std::min(static_cast<float>(m_windowHeight) - 20.f, state.playerPosition.y));
    return true;
}

void Game::clearSavedGame() {
    SaveGameStore::clear();
    m_savedGame = SavedGame();
}
