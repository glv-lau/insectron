#include "SceneHighScores.h"
#include "SceneMenu.h"
#include "../Game.h"
#include "../Managers/AudioManager.h"
#include "../Managers/ScoreManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <sstream>
#include <string>

namespace {
    const sf::Color kScoresNormal(182, 255, 255);
    const sf::Color kScoresSelected(12, 183, 242);

    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }

    void centerText(sf::Text& text, float x, float y) {
        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width * 0.5f,
                       bounds.top + bounds.height * 0.5f);
        text.setPosition(x, y);
    }
}

SceneHighScores::SceneHighScores()
    : m_fontLoaded(false)
{
    AudioManager::instance().stopMusic();
    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;

    m_titleText.setFont(m_font);
    m_titleText.setCharacterSize(40);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("HIGH SCORES");

    m_scoresText.setFont(m_font);
    m_scoresText.setCharacterSize(24);
    m_scoresText.setFillColor(kScoresNormal);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(22);
    m_exitText.setString("PRESS ENTER TO RETURN TO MENU");
    m_exitText.setFillColor(kScoresSelected);

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

    const float scale = std::max(0.55f, std::min(1.6f,
        std::min(static_cast<float>(windowSize.x) / 800.f,
                 static_cast<float>(windowSize.y) / 600.f)));
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;
    m_titleText.setCharacterSize(static_cast<unsigned int>(40.f * scale));
    m_scoresText.setCharacterSize(static_cast<unsigned int>(24.f * scale));
    m_exitText.setCharacterSize(static_cast<unsigned int>(22.f * scale));
    centerText(m_titleText, centerX, centerY - 210.f * scale);
    centerText(m_scoresText, centerX, centerY + 5.f * scale);
    centerText(m_exitText, centerX, centerY + 250.f * scale);

    window.draw(m_titleText);
    window.draw(m_scoresText);
    window.draw(m_exitText);
}
