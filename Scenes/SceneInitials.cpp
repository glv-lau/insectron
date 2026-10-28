#include "SceneInitials.h"
#include "../Game.h"
#include "SceneStageIntro.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cctype>
#include <string>

namespace {
    void centerText(sf::Text& text, float x, float y) {
        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width * 0.5f,
                       bounds.top + bounds.height * 0.5f);
        text.setPosition(x, y);
    }

    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }
}

SceneInitials::SceneInitials()
    : m_initials("")
    , m_cursorIndex(0)
    , m_fontLoaded(false)
{
    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;
    m_titleText.setFont(m_font);
    m_titleText.setCharacterSize(36);
    m_titleText.setString("INGRESE SUS INICIALES");
    m_titleText.setFillColor(sf::Color::White);

    m_promptText.setFont(m_font);
    m_promptText.setCharacterSize(22);
    m_promptText.setString("3 letras");
    m_promptText.setFillColor(sf::Color(182, 255, 255));

    m_inputText.setFont(m_font);
    m_inputText.setCharacterSize(42);
    m_inputText.setString("___");
    m_inputText.setFillColor(sf::Color(12, 183, 242));

    m_continueText.setFont(m_font);
    m_continueText.setCharacterSize(18);
    m_continueText.setString("ENTER PARA CONTINUAR");
    m_continueText.setFillColor(sf::Color(12, 183, 242));
    updateInputText();
}

void SceneInitials::updateInputText() {
    std::string display = m_initials;
    while (display.size() < 3) {
        display += "_";
    }
    m_inputText.setString(display);
}

void SceneInitials::handleEvent(const sf::Event& event, Game& game) {
    if (event.type != sf::Event::KeyPressed) {
        return;
    }

    if (event.key.code == sf::Keyboard::Escape) {
        game.getWindow().close();
        return;
    }

    if (event.key.code == sf::Keyboard::BackSpace) {
        if (!m_initials.empty()) {
            m_initials.erase(m_initials.size() - 1);
            if (m_cursorIndex > 0) {
                --m_cursorIndex;
            }
            updateInputText();
        }
        return;
    }

    if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
        if (m_initials.size() == 3) {
            std::string initials = m_initials;
            while (initials.size() < 3) initials += 'A';
            game.setPlayerInitials(initials.substr(0, 3));
            game.setScene(new SceneStageIntro());
        }
        return;
    }

    const sf::Keyboard::Key key = event.key.code;
    if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
        const char c = static_cast<char>('A' + (key - sf::Keyboard::A));
        if (m_initials.size() < 3) {
            m_initials.push_back(c);
            ++m_cursorIndex;
            updateInputText();
        }
    }
}

void SceneInitials::update(float dt, Game& game) {
    (void)dt;
    (void)game;
}

void SceneInitials::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) {
        return;
    }

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
    m_titleText.setCharacterSize(static_cast<unsigned int>(36.f * scale));
    m_promptText.setCharacterSize(static_cast<unsigned int>(22.f * scale));
    m_inputText.setCharacterSize(static_cast<unsigned int>(42.f * scale));
    m_continueText.setCharacterSize(static_cast<unsigned int>(18.f * scale));
    m_continueText.setFillColor(m_initials.size() == 3
        ? sf::Color(12, 183, 242)
        : sf::Color(90, 110, 125));
    centerText(m_titleText, centerX, centerY - 112.f * scale);
    centerText(m_promptText, centerX, centerY - 35.f * scale);
    centerText(m_inputText, centerX, centerY + 45.f * scale);
    centerText(m_continueText, centerX, centerY + 135.f * scale);

    sf::RectangleShape inputBox(sf::Vector2f(190.f * scale, 72.f * scale));
    inputBox.setOrigin(inputBox.getSize() * 0.5f);
    inputBox.setPosition(centerX, centerY + 45.f * scale);
    inputBox.setFillColor(sf::Color(5, 12, 22));
    inputBox.setOutlineColor(sf::Color(12, 183, 242));
    inputBox.setOutlineThickness(2.f * scale);
    window.draw(inputBox);
    window.draw(m_titleText);
    window.draw(m_promptText);
    window.draw(m_inputText);
    window.draw(m_continueText);
}
