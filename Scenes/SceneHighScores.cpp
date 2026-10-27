#include "SceneHighScores.h"
#include "SceneMenu.h"
#include "../Game.h"
#include "../Managers/ScoreManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <sstream>
#include <string>

namespace {
    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }
}

SceneHighScores::SceneHighScores()
    : m_fontLoaded(false)
{
    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;

    m_titleText.setFont(m_font);
    m_titleText.setCharacterSize(40);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("HIGH SCORES");
    m_titleText.setPosition(240.f, 60.f);

    m_scoresText.setFont(m_font);
    m_scoresText.setCharacterSize(24);
    m_scoresText.setFillColor(sf::Color(182, 255, 255));
    m_scoresText.setPosition(220.f, 140.f);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(22);
    m_exitText.setString("PRESS ENTER TO RETURN TO MENU");
    m_exitText.setPosition(170.f, 520.f);
    m_exitText.setFillColor(sf::Color(12, 183, 242));

    std::ostringstream oss;
    const std::vector<ScoreManager::ScoreEntry> entries = ScoreManager::instance().getEntries();
    if (entries.empty()) {
        oss << "1. AAA 0000\n2. AAA 0000\n3. AAA 0000\n4. AAA 0000\n5. AAA 0000\n6. AAA 0000\n7. AAA 0000\n8. AAA 0000\n9. AAA 0000\n10. AAA 0000\n";
    } else {
        for (size_t i = 0; i < entries.size() && i < 10; ++i) {
            oss << (i + 1) << ". " << entries[i].initials << "   " << entries[i].score << "\n";
        }
    }
    m_scoresText.setString(oss.str());
}

void SceneHighScores::handleEvent(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Escape) {
            game.setScene(new SceneMenu());
        }
    }
}

void SceneHighScores::update(float dt, Game& game) {
    (void)dt;
    (void)game;
}

void SceneHighScores::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;

    const sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(windowSize.x),
                                               static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(5, 12, 22));
    background.setPosition(0.f, 0.f);
    window.draw(background);

    window.draw(m_titleText);
    window.draw(m_scoresText);
    window.draw(m_exitText);
}
