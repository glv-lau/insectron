#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "Scene.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

class SceneMenu : public Scene {
public:
    SceneMenu();
    ~SceneMenu() override = default;

    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float dt, Game& game) override;
    void draw(sf::RenderWindow& window) override;

private:
    void activateSelection(Game& game);
    void updateSelectionColors();
    void updateLayout(const sf::Vector2u& windowSize);
    int getOptionCount(const Game& game) const;

    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_startText;
    sf::Text m_continueText;
    sf::Text m_optionsText;
    sf::Text m_exitText;
    sf::Vector2u m_layoutWindowSize;
    bool m_fontLoaded = false;
    bool m_continueVisible = false;
    bool m_layoutInitialized = false;
    bool m_layoutContinueVisible = false;
    int m_selectedOption = 0;
};

#endif
