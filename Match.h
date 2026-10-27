#ifndef MATCH_H
#define MATCH_H

#include "Scenes/Scene.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/Bullet.h"
#include "Entities/PowerUp.h"
#include "Managers/ScoreManager.h"
#include "FormationManager.h"
#include "SaveGame.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <memory>
#include <random>
#include <vector>


class Match : public Scene {
public:
    Match();
    explicit Match(const SavedGame& savedGame);
    ~Match() override = default;

    void update(float dt, Game& game) override;
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, Game& game) override;

private:
    Match(bool resumeFromSaveFile, const SavedGame* savedGame);

    static constexpr int WAVES_PER_LEVEL = 3;
    static constexpr float WAVE_DELAY = 2.5f;

    sf::Texture m_playerTex;
    sf::Texture m_enemyEasyTex;
    sf::Texture m_enemyHardTex;
    sf::Texture m_enemySpecialTex;
    sf::Texture m_enemyBossTex;
    sf::Texture m_bulletTex;
    sf::Texture m_powerUpTex;

    sf::Font m_font;
    bool m_fontLoaded = false;
    sf::Text m_hudText;
    sf::Text m_waveText;
    sf::Text m_gameOverText;


    std::vector<Enemy> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    std::vector<PowerUp> m_powerUps;

    int m_score = 0;
    int m_level = 1;
    int m_waveInLevel = 0;

    bool m_waitingForNextWave = false;
    float m_waveTransitionTimer = 0.f;
    bool m_gameOver = false;
    bool m_paused = false;
    int m_waveEnemyCount = 0;
    int m_highestNormalWaveCount = 0;
    float m_bossSummonTimer = 0.f;
    FormationManager m_formation;


    void spawnWave();
    void spawnNormalWave(int waveIndex);
    void spawnBoss();
    void tryDropPowerUp(const sf::Vector2f& pos, bool guaranteed = false);
    void cleanupDead();
    void checkCollisions();
    void drawHud(sf::RenderWindow& window);
    void resetMatch();
	
	std::mt19937 m_rng;
	std::unique_ptr<Player> m_player;
};

#endif
