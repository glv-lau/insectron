#include "SceneMatch.h"
#include "../Game.h"
#include "../Managers/AssetManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace {
    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }
}

SceneMatch::SceneMatch() {
    if (!loadProjectFont(m_font))
    {
        return;
    }

    m_fontLoaded = true;

    m_lvltext.setFont(m_font);
    m_lvltext.setCharacterSize(48);
    m_lvltext.setString("Nivel 1");
    m_lvltext.setFillColor(sf::Color::White);
    m_lvltext.setPosition(220.f, 150.f);

    m_puntostext.setFont(m_font);
    m_puntostext.setCharacterSize(24);
    m_puntostext.setString("Puntos: 0");
    m_puntostext.setFillColor(sf::Color::White);
    m_puntostext.setPosition(240.f, 320.f);

    m_vidastext.setFont(m_font);
    m_vidastext.setCharacterSize(24);
    m_vidastext.setString("Vidas: " + std::to_string(m_vidas));
    m_vidastext.setFillColor(sf::Color::White);
    m_vidastext.setPosition(240.f, 350.f);

    m_powerupText.setFont(m_font);
    m_powerupText.setCharacterSize(24);
    m_powerupText.setString("Power-Up: Ninguno");
    m_powerupText.setFillColor(sf::Color::White);
    m_powerupText.setPosition(240.f, 380.f);
}

void SceneMatch::handleEvent(const sf::Event& event, Game& game) {
    m_match.handleEvent(event, game);
}

void SceneMatch::update(float dt, Game& game) {
    m_match.update(dt, game);
}

void SceneMatch::draw(sf::RenderWindow& window) {
    const sf::Vector2u size = window.getSize();
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(size.x),
                                               static_cast<float>(size.y)));
    background.setFillColor(sf::Color(15, 15, 20));
    background.setPosition(0.f, 0.f);
    window.draw(background);

    m_match.draw(window);
}
