#include "SceneMenu.h"
#include "../Game.h"
#include "../Managers/AssetManager.h"
#include "SceneStageIntro.h"
#include "SceneInitials.h"
#include "SceneOptions.h"
#include "../Match.h"
#include "../Managers/AudioManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <string>

namespace {
    const sf::Color kMenuNormal = sf::Color(182, 255, 255);
    const sf::Color kMenuSelected = sf::Color(12, 183, 242);
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

SceneMenu::SceneMenu()
{
    AudioManager::instance().playMusic("menu_music.ogg", true);
    (void)AssetManager::instance().getTexture("menu_bg.png", sf::Color(40, 40, 40), 800, 600);

    if (!loadProjectFont(m_font)) {
        return;
    }

    m_fontLoaded = true;

    m_title.setFont(m_font);
    m_title.setCharacterSize(48);
    m_title.setString("INSECTRON");
    m_title.setFillColor(kMenuTitle);

    m_startText.setFont(m_font);
    m_startText.setCharacterSize(30);
    m_startText.setString("START");
    m_startText.setFillColor(kMenuNormal);

    m_continueText.setFont(m_font);
    m_continueText.setCharacterSize(30);
    m_continueText.setString("CONTINUE");
    m_continueText.setFillColor(kMenuNormal);

    m_optionsText.setFont(m_font);
    m_optionsText.setCharacterSize(30);
    m_optionsText.setString("OPTIONS");
    m_optionsText.setFillColor(kMenuNormal);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(30);
    m_exitText.setString("EXIT");
    m_exitText.setFillColor(kMenuNormal);

    updateSelectionColors();
}

void SceneMenu::updateLayout(const sf::Vector2u& windowSize) {
    if (m_layoutInitialized
        && windowSize.x == m_layoutWindowSize.x
        && windowSize.y == m_layoutWindowSize.y
        && m_continueVisible == m_layoutContinueVisible) {
        return;
    }

    m_layoutWindowSize = windowSize;
    m_layoutContinueVisible = m_continueVisible;
    m_layoutInitialized = true;

    const float scale = std::max(0.55f, std::min(1.6f,
        std::min(static_cast<float>(windowSize.x) / 800.f,
                 static_cast<float>(windowSize.y) / 600.f)));
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;
    const float rowSpacing = 55.f * scale;
    const float firstRowY = centerY - (m_continueVisible ? 1.5f : 1.f) * rowSpacing;

    m_title.setCharacterSize(static_cast<unsigned int>(48.f * scale));
    m_startText.setCharacterSize(static_cast<unsigned int>(30.f * scale));
    m_continueText.setCharacterSize(static_cast<unsigned int>(30.f * scale));
    m_optionsText.setCharacterSize(static_cast<unsigned int>(30.f * scale));
    m_exitText.setCharacterSize(static_cast<unsigned int>(30.f * scale));

    centerText(m_title, centerX, centerY - (m_continueVisible ? 165.f : 140.f) * scale);
    centerText(m_startText, centerX, firstRowY);
    centerText(m_continueText, centerX, firstRowY + rowSpacing);
    centerText(m_optionsText, centerX, firstRowY + rowSpacing * (m_continueVisible ? 2.f : 1.f));
    centerText(m_exitText, centerX, firstRowY + rowSpacing * (m_continueVisible ? 3.f : 2.f));
}

int SceneMenu::getOptionCount(const Game& game) const {
    return game.hasContinueAvailable() ? 4 : 3;
}

void SceneMenu::activateSelection(Game& game) {
    const int optionCount = getOptionCount(game);
    if (m_selectedOption < 0 || m_selectedOption >= optionCount) {
        m_selectedOption = 0;
    }

    if (game.hasContinueAvailable()) {
        switch (m_selectedOption) {
            case 0:
                game.setContinueAvailable(false);
                game.setScene(new SceneInitials());
                break;
            case 1:
                game.setScene(new Match(game.getSavedGame()));
                break;
            case 2:
                game.setScene(new SceneOptions());
                break;
            case 3:
                game.getWindow().close();
                break;
            default:
                break;
        }
    } else {
        switch (m_selectedOption) {
            case 0:
                game.setContinueAvailable(false);
                game.setScene(new SceneInitials());
                break;
            case 1:
                game.setScene(new SceneOptions());
                break;
            case 2:
                game.getWindow().close();
                break;
            default:
                break;
        }
    }
}

void SceneMenu::updateSelectionColors() {
    m_startText.setFillColor(m_selectedOption == 0 ? kMenuSelected : kMenuNormal);
    if (m_continueVisible) {
        m_continueText.setFillColor(m_selectedOption == 1 ? kMenuSelected : kMenuNormal);
        m_optionsText.setFillColor(m_selectedOption == 2 ? kMenuSelected : kMenuNormal);
        m_exitText.setFillColor(m_selectedOption == 3 ? kMenuSelected : kMenuNormal);
    } else {
        m_optionsText.setFillColor(m_selectedOption == 1 ? kMenuSelected : kMenuNormal);
        m_exitText.setFillColor(m_selectedOption == 2 ? kMenuSelected : kMenuNormal);
    }
}

void SceneMenu::handleEvent(const sf::Event& event, Game& game) {
    m_continueVisible = game.hasContinueAvailable();
    updateLayout(game.getWindow().getSize());

    if (event.type == sf::Event::KeyPressed) {
        const int optionCount = getOptionCount(game);
        m_selectedOption = std::max(0, std::min(optionCount - 1, m_selectedOption));
        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            m_selectedOption = std::max(0, m_selectedOption - 1);
            AudioManager::instance().playSfx("selecting.wav");
        } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            m_selectedOption = std::min(optionCount - 1, m_selectedOption + 1);
            AudioManager::instance().playSfx("selecting.wav");
        } else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
            activateSelection(game);
        } else if (event.key.code == sf::Keyboard::Escape) {
            game.getWindow().close();
        }
        updateSelectionColors();
    }

    if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f pos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        int hoveredOption = -1;
        if (m_startText.getGlobalBounds().contains(pos)) {
            hoveredOption = 0;
        } else if (game.hasContinueAvailable() && m_continueText.getGlobalBounds().contains(pos)) {
            hoveredOption = 1;
        } else if (m_optionsText.getGlobalBounds().contains(pos)) {
            hoveredOption = game.hasContinueAvailable() ? 2 : 1;
        } else if (m_exitText.getGlobalBounds().contains(pos)) {
            hoveredOption = game.hasContinueAvailable() ? 3 : 2;
        }

        if (hoveredOption != m_lastHoveredOption) {
            if (hoveredOption >= 0) {
                AudioManager::instance().playSfx("selecting.wav");
            }
            m_lastHoveredOption = hoveredOption;
        }
        if (hoveredOption >= 0) {
            m_selectedOption = hoveredOption;
        }
        updateSelectionColors();
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f pos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (m_startText.getGlobalBounds().contains(pos)) {
            m_selectedOption = 0;
            activateSelection(game);
        } else if (game.hasContinueAvailable() && m_continueText.getGlobalBounds().contains(pos)) {
            m_selectedOption = 1;
            game.setScene(new Match(game.getSavedGame()));
        } else if (m_optionsText.getGlobalBounds().contains(pos)) {
            m_selectedOption = game.hasContinueAvailable() ? 2 : 1;
            game.setScene(new SceneOptions());
        } else if (m_exitText.getGlobalBounds().contains(pos)) {
            m_selectedOption = game.hasContinueAvailable() ? 3 : 2;
            game.getWindow().close();
        }
    }
}

void SceneMenu::update(float dt, Game& game) {
    (void)dt;
    m_continueVisible = game.hasContinueAvailable();
    updateLayout(game.getWindow().getSize());
    if (!m_continueVisible && m_selectedOption > 2) {
        m_selectedOption = 2;
    }
    updateSelectionColors();
}

void SceneMenu::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;

    const sf::Vector2u windowSize = window.getSize();
    updateLayout(windowSize);
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(windowSize.x),
                                               static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(5, 12, 22));
    background.setPosition(0.f, 0.f);
    window.draw(background);

    window.draw(m_title);
    window.draw(m_startText);
    if (m_continueVisible) {
        window.draw(m_continueText);
    }
    window.draw(m_optionsText);
    window.draw(m_exitText);
}
