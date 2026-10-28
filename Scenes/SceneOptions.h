#ifndef SCENEOPTIONS_H
#define SCENEOPTIONS_H

#include "Scene.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

class SceneOptions : public Scene {
public:
    SceneOptions();
    ~SceneOptions() override = default;

    void update(float dt, Game& game) override;
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, Game& game) override;

private:
    void refreshLabels();
    void updateLayout(const sf::Vector2u& windowSize);
    void syncFromGame(const Game& game);
    void updateSelectionColors();
    void adjustSelectedOption(Game& game, int direction);
    int optionAtPosition(const sf::Vector2f& position) const;

    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_exitText;
    sf::Text m_masterText;
    sf::Text m_musicText;
    sf::Text m_sfxText;
    sf::Text m_hudSizeText;
    sf::Text m_resolutionText;
    sf::Text m_fullscreenText;
    sf::Vector2u m_layoutWindowSize;
    bool m_fontLoaded = false;
    bool m_layoutInitialized = false;
    int m_selectedOption = 0;
    int m_lastHoveredOption = -1;
    int m_masterVolume = 100;
    int m_musicVolume = 100;
    int m_sfxVolume = 100;
    int m_hudSize = 50;
    int m_resolutionIndex = 0;
    bool m_fullscreen = false;
};

#endif
