#include "SceneInitials.h"
#include "../Game.h"
#include "SceneStageIntro.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cctype>
#include <string>

namespace {
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
    m_titleText.setPosition(120.f, 140.f);
    m_titleText.setFillColor(sf::Color::White);

    m_promptText.setFont(m_font);
    m_promptText.setCharacterSize(22);
    m_promptText.setString("3 letras");
    m_promptText.setPosition(300.f, 220.f);
    m_promptText.setFillColor(sf::Color(182, 255, 255));

    m_inputText.setFont(m_font);
    m_inputText.setCharacterSize(42);
    m_inputText.setString("___");
    m_inputText.setPosition(300.f, 280.f);
    m_inputText.setFillColor(sf::Color(12, 183, 242));
    updateInputText();
}

void SceneInitials::updateInputText() {
    std::string display = m_initials;
    while (display.size() < 3) {
        display += "_";
    }
    m_inputText.setString(display);
    const sf::FloatRect bounds = m_inputText.getLocalBounds();
    m_inputText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    m_inputText.setPosition(400.f, 310.f);
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
    window.draw(m_titleText);
    window.draw(m_promptText);
    window.draw(m_inputText);
}
