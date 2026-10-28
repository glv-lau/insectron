#include "SceneOptions.h"
#include "SceneMenu.h"
#include "../Game.h"
#include "../Managers/AudioManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstddef>
#include <sstream>

namespace {
    const int kWidths[] = {800, 1024, 1280, 1366, 1600, 1920};
    const int kHeights[] = {600, 768, 720, 768, 900, 1080};
    const sf::Color kMenuNormal(182, 255, 255);
    const sf::Color kMenuSelected(12, 183, 242);
    const sf::Color kMenuTitle = sf::Color::White;

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
    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;
    m_title.setFont(m_font);
    m_title.setCharacterSize(48);
    m_title.setString("OPCIONES");
    m_title.setFillColor(kMenuTitle);

    m_masterText.setFont(m_font);
    m_masterText.setCharacterSize(30);
    m_masterText.setFillColor(kMenuNormal);

    m_musicText.setFont(m_font);
    m_musicText.setCharacterSize(30);
    m_musicText.setFillColor(kMenuNormal);

    m_sfxText.setFont(m_font);
    m_sfxText.setCharacterSize(30);
    m_sfxText.setFillColor(kMenuNormal);

    m_hudSizeText.setFont(m_font);
    m_hudSizeText.setCharacterSize(30);
    m_hudSizeText.setFillColor(kMenuNormal);
    m_resolutionText.setFillColor(kMenuNormal);
    m_fullscreenText.setFillColor(kMenuNormal);

    m_resolutionText.setFont(m_font);
    m_resolutionText.setCharacterSize(30);
    m_resolutionText.setFillColor(kMenuNormal);

    m_fullscreenText.setFont(m_font);
    m_fullscreenText.setCharacterSize(30);
    m_fullscreenText.setFillColor(kMenuNormal);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(30);
    m_exitText.setString("VOLVER");
    m_exitText.setFillColor(kMenuNormal);

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
    const unsigned int bodySize = std::max(12u, static_cast<unsigned int>(30.f * scale));

    m_title.setCharacterSize(static_cast<unsigned int>(48.f * scale));
    m_masterText.setCharacterSize(bodySize);
    m_musicText.setCharacterSize(bodySize);
    m_sfxText.setCharacterSize(bodySize);
    m_hudSizeText.setCharacterSize(bodySize);
    m_resolutionText.setCharacterSize(bodySize);
    m_fullscreenText.setCharacterSize(bodySize);
    m_exitText.setCharacterSize(bodySize);

    centerText(m_title, centerX, centerY - 205.f * scale);
    const float firstOptionY = centerY - 125.f * scale;
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
    ss << "MASTER: " << m_masterVolume << "%";
    m_masterText.setString(ss.str());

    ss.str("");
    ss << "MUSIC: " << m_musicVolume << "%";
    m_musicText.setString(ss.str());

    ss.str("");
    ss << "SFX: " << m_sfxVolume << "%";
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

void SceneOptions::syncFromGame(const Game& game) {
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
}

void SceneOptions::adjustSelectedOption(Game& game, int direction) {
    switch (m_selectedOption) {
        case 0:
            m_masterVolume = std::max(0, std::min(100, m_masterVolume + direction * 10));
            game.setMasterVolume(m_masterVolume);
            break;
        case 1:
            m_musicVolume = std::max(0, std::min(100, m_musicVolume + direction * 10));
            game.setMusicVolume(m_musicVolume);
            break;
        case 2:
            m_sfxVolume = std::max(0, std::min(100, m_sfxVolume + direction * 10));
            game.setSfxVolume(m_sfxVolume);
            break;
        case 3:
            m_hudSize = std::max(10, std::min(100, m_hudSize + direction * 5));
            game.setHudSize(m_hudSize);
            break;
        case 4:
            m_resolutionIndex = (m_resolutionIndex + direction + 6) % 6;
            game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
            break;
        case 5:
            m_fullscreen = !m_fullscreen;
            game.setDisplaySettings(kWidths[m_resolutionIndex], kHeights[m_resolutionIndex], m_fullscreen);
            break;
        default:
            break;
    }
}

int SceneOptions::optionAtPosition(const sf::Vector2f& position) const {
    const sf::Text* options[] = {
        &m_masterText, &m_musicText, &m_sfxText, &m_hudSizeText,
        &m_resolutionText, &m_fullscreenText, &m_exitText
    };
    for (int i = 0; i < 7; ++i) {
        if (options[i]->getGlobalBounds().contains(position)) {
            return i;
        }
    }
    return -1;
}

void SceneOptions::updateSelectionColors() {
    sf::Text* options[] = {
        &m_masterText, &m_musicText, &m_sfxText, &m_hudSizeText,
        &m_resolutionText, &m_fullscreenText, &m_exitText
    };
    for (int i = 0; i < 7; ++i) {
        options[i]->setFillColor(i == m_selectedOption ? kMenuSelected : kMenuNormal);
    }
}

void SceneOptions::handleEvent(const sf::Event& event, Game& game) {
    syncFromGame(game);
    updateLayout(game.getWindow().getSize());

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            m_selectedOption = std::max(0, m_selectedOption - 1);
            AudioManager::instance().playSfx("selecting.wav");
        } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            m_selectedOption = std::min(6, m_selectedOption + 1);
            AudioManager::instance().playSfx("selecting.wav");
        } else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
            adjustSelectedOption(game, -1);
        } else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
            adjustSelectedOption(game, 1);
        } else if ((event.key.code == sf::Keyboard::Return
                    || event.key.code == sf::Keyboard::Space)
                   && m_selectedOption == 6) {
            game.setScene(new SceneMenu());
        } else if (event.key.code == sf::Keyboard::Escape) {
            game.setScene(new SceneMenu());
        }
    } else if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f position(static_cast<float>(event.mouseMove.x),
                                    static_cast<float>(event.mouseMove.y));
        const int hoveredOption = optionAtPosition(position);
        if (hoveredOption != m_lastHoveredOption) {
            if (hoveredOption >= 0) {
                AudioManager::instance().playSfx("selecting.wav");
            }
            m_lastHoveredOption = hoveredOption;
        }
        if (hoveredOption >= 0) {
            m_selectedOption = hoveredOption;
        }
    } else if (event.type == sf::Event::MouseButtonPressed
               && event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f position(static_cast<float>(event.mouseButton.x),
                                    static_cast<float>(event.mouseButton.y));
        const int clickedOption = optionAtPosition(position);
        if (clickedOption >= 0) {
            m_selectedOption = clickedOption;
            if (m_selectedOption == 6) {
                game.setScene(new SceneMenu());
            } else if (m_selectedOption == 5) {
                adjustSelectedOption(game, 1);
            } else {
                const sf::Text* options[] = {
                    &m_masterText, &m_musicText, &m_sfxText, &m_hudSizeText,
                    &m_resolutionText, &m_fullscreenText
                };
                const sf::FloatRect bounds = options[m_selectedOption]->getGlobalBounds();
                const int direction = position.x < bounds.left + bounds.width * 0.5f ? -1 : 1;
                adjustSelectedOption(game, direction);
            }
        }
    }

    refreshLabels();
    updateSelectionColors();
}

void SceneOptions::update(float dt, Game& game) {
    (void)dt;

    syncFromGame(game);
    refreshLabels();
    updateSelectionColors();
}

void SceneOptions::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) {
        return;
    }

    const sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(windowSize.x),
                                               static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(5, 12, 22));
    background.setPosition(0.f, 0.f);
    window.draw(background);
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
