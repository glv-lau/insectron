#ifndef SCENEINITIALS_H
#define SCENEINITIALS_H

#include "Scene.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>

class SceneInitials : public Scene {
public:
    SceneInitials();
    ~SceneInitials() override = default;

    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float dt, Game& game) override;
    void draw(sf::RenderWindow& window) override;

private:
    void updateInputText();
    std::string m_initials;
    int m_cursorIndex;
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_inputText;
    sf::Text m_promptText;
    sf::Text m_continueText;
    bool m_fontLoaded;
};

#endif
