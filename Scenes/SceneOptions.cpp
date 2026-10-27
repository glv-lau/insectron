#include "SceneOptions.h"
#include "SceneMenu.h"
#include "../Game.h"
#include "../Managers/AssetManager.h"
#include "../Managers/AudioManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <sstream>

namespace {
    const int kWidths[] = {800, 1024, 1280, 1366, 1600, 1920};
    const int kHeights[] = {600, 768, 720, 768, 900, 1080};
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

SceneOptions::SceneOptions()
    : m_masterVolume(100)
    , m_musicVolume(100)
    , m_sfxVolume(100)
    , m_hudSize(50)
{
    (void)AssetManager::instance().getTexture("hud.png", sf::Color(70, 70, 70), 800, 120);

    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;
    m_title.setFont(m_font);
    m_title.setCharacterSize(48);
    m_title.setString("OPCIONES");
    m_title.setFillColor(sf::Color::White);

    m_masterText.setFont(m_font);
    m_masterText.setCharacterSize(24);
    m_masterText.setFillColor(sf::Color::White);

    m_musicText.setFont(m_font);
    m_musicText.setCharacterSize(24);
    m_musicText.setFillColor(sf::Color::White);

    m_sfxText.setFont(m_font);
    m_sfxText.setCharacterSize(24);
    m_sfxText.setFillColor(sf::Color::White);

    m_hudSizeText.setFont(m_font);
    m_hudSizeText.setCharacterSize(24);
    m_hudSizeText.setFillColor(sf::Color::White);
    m_resolutionText.setFillColor(sf::Color::White);
    m_fullscreenText.setFillColor(sf::Color::White);

    m_resolutionText.setFont(m_font);
    m_resolutionText.setCharacterSize(24);
    m_resolutionText.setFillColor(sf::Color::White);

    m_fullscreenText.setFont(m_font);
    m_fullscreenText.setCharacterSize(24);
    m_fullscreenText.setFillColor(sf::Color::White);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(24);
    m_exitText.setString("VOLVER");
    m_exitText.setFillColor(sf::Color::White);

    m_masterVolume = 100;
    m_musicVolume = 100;
    m_sfxVolume = 100;
    m_hudSize = 50;
    m_resolutionIndex = 0;
    m_fullscreen = false;
    refreshLabels();
}

void SceneOptions::updateLayout(const sf::Vector2u& windowSize) {
    if (m_layoutInitialized
        && windowSize.x == m_layoutWindowSize.x
        && windowSize.y == m_layoutWindowSize.y) {
        return;
    }

    m_layoutWindowSize = windowSize;
    m_layoutInitialized = true;

    const float scale = std::max(0.55f, std::min(1.6f,
        std::min(static_cast<float>(windowSize.x) / 800.f,
                 static_cast<float>(windowSize.y) / 600.f)));
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;
    const unsigned int bodySize = std::max(12u, static_cast<unsigned int>(24.f * scale));

    m_title.setCharacterSize(static_cast<unsigned int>(42.f * scale));
    m_masterText.setCharacterSize(bodySize);
    m_musicText.setCharacterSize(bodySize);
    m_sfxText.setCharacterSize(bodySize);
    m_hudSizeText.setCharacterSize(bodySize);
    m_resolutionText.setCharacterSize(bodySize);
    m_fullscreenText.setCharacterSize(bodySize);
    m_exitText.setCharacterSize(bodySize);

    centerText(m_title, centerX, centerY - 190.f * scale);
    const float firstOptionY = centerY - 105.f * scale;
    const float optionSpacing = 47.f * scale;
    centerText(m_masterText, centerX, firstOptionY);
    centerText(m_musicText, centerX, firstOptionY + optionSpacing);
    centerText(m_sfxText, centerX, firstOptionY + optionSpacing * 2.f);
    centerText(m_hudSizeText, centerX, firstOptionY + optionSpacing * 3.f);
    centerText(m_resolutionText, centerX, firstOptionY + optionSpacing * 4.f);
    centerText(m_fullscreenText, centerX, firstOptionY + optionSpacing * 5.f);
    centerText(m_exitText, centerX, firstOptionY + optionSpacing * 6.f);
}

void SceneOptions::refreshLabels() {
    std::stringstream ss;
    ss << "VOL MEN MASTER: " << m_masterVolume << "%";
    m_masterText.setString(ss.str());

    ss.str("");
    ss << "VOL MUSICA (.ogg): " << m_musicVolume << "%";
    m_musicText.setString(ss.str());

    ss.str("");
    ss << "VOL SFX (.wav): " << m_sfxVolume << "%";
    m_sfxText.setString(ss.str());

    ss.str("");
    ss << "HUD: " << m_hudSize << "%";
    m_hudSizeText.setString(ss.str());

    ss.str("");
    ss << "RESOLUCION: " << kWidths[m_resolutionIndex] << "x" << kHeights[m_resolutionIndex];
    m_resolutionText.setString(ss.str());

    ss.str("");
    ss << "PANTALLA COMPLETA: " << (m_fullscreen ? "SI" : "NO");
    m_fullscreenText.setString(ss.str());

    if (m_layoutInitialized) {
        const float centerX = static_cast<float>(m_layoutWindowSize.x) * 0.5f;
        sf::Text* labels[] = {
            &m_masterText, &m_musicText, &m_sfxText, &m_hudSizeText,
            &m_resolutionText, &m_fullscreenText
        };
        for (std::size_t i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i) {
            centerText(*labels[i], centerX, labels[i]->getPosition().y);
        }
    }
}

void SceneOptions::handleEvent(const sf::Event& event, Game& game) {
    if (event.type != sf::Event::KeyPressed) {
        return;
    }

    m_masterVolume = game.getMasterVolume();
    m_musicVolume = game.getMusicVolume();
    m_sfxVolume = game.getSfxVolume();
    m_hudSize = game.getHudSize();
    const int currentWidth = game.getWindowWidth();
    const int currentHeight = game.getWindowHeight();
    for (int i = 0; i < 6; ++i) {
        if (kWidths[i] == currentWidth && kHeights[i] == currentHeight) {
            m_resolutionIndex = i;
        }
    }
    m_fullscreen = game.isFullscreen();

    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
        m_selectedOption = std::max(0, m_selectedOption - 1);
        AudioManager::instance().playSfx("selecting.wav");
    } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
        m_selectedOption = std::min(6, m_selectedOption + 1);
        AudioManager::instance().playSfx("selecting.wav");
    } else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
        switch (m_selectedOption) {
            case 0:
                m_masterVolume = std::max(0, m_masterVolume - 10);
                game.setMasterVolume(m_masterVolume);
                break;
            case 1:
                m_musicVolume = std::max(0, m_musicVolume - 10);
                game.setMusicVolume(m_musicVolume);
                break;
            case 2:
                m_sfxVolume = std::max(0, m_sfxVolume - 10);
                game.setSfxVolume(m_sfxVolume);
                break;
            case 3:
                m_hudSize = std::max(10, m_hudSize - 5);
                game.setHudSize(m_hudSize);
                break;
            case 4:
                m_resolutionIndex = (m_resolutionIndex + 5) % 6;
                game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
                break;
            case 5:
                m_fullscreen = !m_fullscreen;
                game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
                break;
            default:
                break;
        }
    } else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
        switch (m_selectedOption) {
            case 0:
                m_masterVolume = std::min(100, m_masterVolume + 10);
                game.setMasterVolume(m_masterVolume);
                break;
            case 1:
                m_musicVolume = std::min(100, m_musicVolume + 10);
                game.setMusicVolume(m_musicVolume);
                break;
            case 2:
                m_sfxVolume = std::min(100, m_sfxVolume + 10);
                game.setSfxVolume(m_sfxVolume);
                break;
            case 3:
                m_hudSize = std::min(100, m_hudSize + 5);
                game.setHudSize(m_hudSize);
                break;
            case 4:
                m_resolutionIndex = (m_resolutionIndex + 1) % 6;
                game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
                break;
            case 5:
                m_fullscreen = !m_fullscreen;
                game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
                break;
            default:
                break;
        }
    } else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
        if (m_selectedOption == 6) {
            game.setScene(new SceneMenu());
        }
    } else if (event.key.code == sf::Keyboard::Escape) {
        game.setScene(new SceneMenu());
    }

    refreshLabels();
}

void SceneOptions::update(float dt, Game& game) {
    (void)dt;

    m_masterVolume = game.getMasterVolume();
    m_musicVolume = game.getMusicVolume();
    m_sfxVolume = game.getSfxVolume();
    m_hudSize = game.getHudSize();
    m_fullscreen = game.isFullscreen();
    refreshLabels();

    m_masterText.setFillColor(sf::Color::White);
    m_musicText.setFillColor(sf::Color::White);
    m_sfxText.setFillColor(sf::Color::White);
    m_hudSizeText.setFillColor(sf::Color::White);
    m_resolutionText.setFillColor(sf::Color::White);
    m_fullscreenText.setFillColor(sf::Color::White);
    m_exitText.setFillColor(sf::Color::White);

    switch (m_selectedOption) {
        case 0: m_masterText.setFillColor(sf::Color::Yellow); break;
        case 1: m_musicText.setFillColor(sf::Color::Yellow); break;
        case 2: m_sfxText.setFillColor(sf::Color::Yellow); break;
        case 3: m_hudSizeText.setFillColor(sf::Color::Yellow); break;
        case 4: m_resolutionText.setFillColor(sf::Color::Yellow); break;
        case 5: m_fullscreenText.setFillColor(sf::Color::Yellow); break;
        case 6: m_exitText.setFillColor(sf::Color::Yellow); break;
        default: break;
    }
}

void SceneOptions::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) {
        return;
    }

    const sf::Vector2u windowSize = window.getSize();
    const float panelWidth = std::max(0.f, static_cast<float>(windowSize.x) - 40.f);
    const float panelHeight = std::max(0.f, static_cast<float>(windowSize.y) - 40.f);
    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setFillColor(sf::Color(45, 45, 55));
    panel.setPosition((static_cast<float>(windowSize.x) - panelWidth) * 0.5f,
                      (static_cast<float>(windowSize.y) - panelHeight) * 0.5f);
    window.draw(panel);

    updateLayout(windowSize);
    window.draw(m_title);
    window.draw(m_masterText);
    window.draw(m_musicText);
    window.draw(m_sfxText);
    window.draw(m_hudSizeText);
    window.draw(m_resolutionText);
    window.draw(m_fullscreenText);
    window.draw(m_exitText);
}
