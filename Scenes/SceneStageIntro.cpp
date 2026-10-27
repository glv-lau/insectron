#include "SceneStageIntro.h"
#include "../Game.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace {
    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }
}

SceneStageIntro::SceneStageIntro(int stage, float duration)
    : m_stage(stage), m_timer(duration)
{
    if (!loadProjectFont(m_font)) {
        return;
    }
    m_fontLoaded = true;

    m_text.setFont(m_font);
    m_text.setCharacterSize(48);
    m_text.setString("STAGE " + std::to_string(m_stage) + ", GO!!");
    m_text.setFillColor(sf::Color::White);
}

void SceneStageIntro::handleEvent(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::KeyPressed) {
        game.setScene(new SceneMatch());
    }
}

void SceneStageIntro::update(float dt, Game& game) {
    (void)game;
    m_timer -= dt;
    if (m_timer <= 0.f) {
        game.setScene(new SceneMatch());
    }
}

void SceneStageIntro::draw(sf::RenderWindow& window) {
    const sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(windowSize.x),
                                               static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(15, 15, 20));
    background.setPosition(0.f, 0.f);
    window.draw(background);

    if (m_fontLoaded) {
        const float scale = std::max(0.55f, std::min(1.6f,
            std::min(static_cast<float>(windowSize.x) / 800.f,
                     static_cast<float>(windowSize.y) / 600.f)));
        m_text.setCharacterSize(static_cast<unsigned int>(48.f * scale));
        const sf::FloatRect bounds = m_text.getLocalBounds();
        m_text.setOrigin(bounds.left + bounds.width * 0.5f,
                         bounds.top + bounds.height * 0.5f);
        m_text.setPosition(static_cast<float>(windowSize.x) * 0.5f,
                           static_cast<float>(windowSize.y) * 0.5f);
        window.draw(m_text);
    }
}
