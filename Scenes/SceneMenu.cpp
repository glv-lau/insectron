#include "SceneMenu.h"
#include "../Game.h"
#include "../AssetManager.h"
#include "SceneStageIntro.h"
#include "SceneInitials.h"
#include "SceneOptions.h"
#include "../Match.h"
#include "../AudioManager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <string>

namespace {
    const sf::Color kMenuNormal = sf::Color(182, 255, 255);
    const sf::Color kMenuSelected = sf::Color(12, 183, 242);
    const sf::Color kMenuTitle = sf::Color::White;

    bool loadFontFallback(sf::Font& font) {
        const std::string candidates[] = {
            "arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "C:/Windows/Fonts/consola.ttf",
            "C:/Program Files (x86)/ZinjaI/MinGW32-gcc6/arial.ttf",
            "C:/Users/Lau/Documents/GitHub/cirujainvaders/arial.ttf"
        };

        for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
            if (font.loadFromFile(candidates[i])) {
                return true;
            }
        }
        return false;
    }
}

SceneMenu::SceneMenu()
{
    AudioManager::instance().playMusic("menu_music.ogg", true);
    (void)AssetManager::instance().getTexture("menu_bg.png", sf::Color(40, 40, 40), 800, 600);

    if (!loadFontFallback(m_font)) {
        return;
    }

    m_fontLoaded = true;

    m_title.setFont(m_font);
    m_title.setCharacterSize(48);
    m_title.setString("CIRUJAINVADERS");
    m_title.setFillColor(kMenuTitle);
    m_title.setPosition(160.f, 120.f);

    m_startText.setFont(m_font);
    m_startText.setCharacterSize(30);
    m_startText.setString("START");
    m_startText.setFillColor(kMenuNormal);
    m_startText.setPosition(330.f, 240.f);

    m_continueText.setFont(m_font);
    m_continueText.setCharacterSize(30);
    m_continueText.setString("CONTINUE");
    m_continueText.setFillColor(kMenuNormal);
    m_continueText.setPosition(300.f, 300.f);

    m_optionsText.setFont(m_font);
    m_optionsText.setCharacterSize(30);
    m_optionsText.setString("OPTIONS");
    m_optionsText.setFillColor(kMenuNormal);
    m_optionsText.setPosition(300.f, 360.f);

    m_exitText.setFont(m_font);
    m_exitText.setCharacterSize(30);
    m_exitText.setString("EXIT");
    m_exitText.setFillColor(kMenuNormal);
    m_exitText.setPosition(335.f, 420.f);

    updateSelectionColors();
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
                game.setScene(new Match(true));
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
        if (m_startText.getGlobalBounds().contains(pos)) {
            m_selectedOption = 0;
        } else if (game.hasContinueAvailable() && m_continueText.getGlobalBounds().contains(pos)) {
            m_selectedOption = 1;
        } else if (m_optionsText.getGlobalBounds().contains(pos)) {
            m_selectedOption = game.hasContinueAvailable() ? 2 : 1;
        } else if (m_exitText.getGlobalBounds().contains(pos)) {
            m_selectedOption = game.hasContinueAvailable() ? 3 : 2;
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
            game.setScene(new Match(true));
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
    if (!m_continueVisible && m_selectedOption > 2) {
        m_selectedOption = 2;
    }
    updateSelectionColors();
}

void SceneMenu::draw(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;

    const sf::Vector2u windowSize = window.getSize();
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
