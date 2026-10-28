#include "Match.h"
#include "Game.h"
#include "Managers/AssetManager.h"
#include "Managers/AudioManager.h"
#include "SaveGame.h"
#include "Scenes/SceneMenu.h"
#include "Scenes/SceneHighScores.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

namespace {
    bool loadProjectFont(sf::Font& font) {
        return font.loadFromFile("Resources/Tr2n.ttf");
    }
}

Match::Match()
    : Match(false, nullptr)
{
}

Match::Match(const SavedGame& savedGame)
    : Match(true, &savedGame)
{
}

Match::Match(bool resumeFromSaveFile, const SavedGame* savedGame)
    : m_rng(std::random_device{}())
{
    m_playerTex = AssetManager::instance().getTexture("player.png", sf::Color(12, 183, 242), 88, 88);
    m_enemyEasyTex = AssetManager::instance().getTexture("enemy_basic.png", sf::Color(248, 201, 77), 64, 64);
    m_enemyHardTex = AssetManager::instance().getTexture("enemy_hard.png", sf::Color(216, 129, 57), 64, 64);
    m_enemySpecialTex = AssetManager::instance().getTexture("enemy_special.png", sf::Color(186, 66, 40), 64, 64);
    m_enemyBossTex = AssetManager::instance().getTexture("enemy_boss.png", sf::Color(124, 14, 54), 200, 200);
    m_bulletTex = AssetManager::instance().getTexture("bullet_player.png", sf::Color(124, 218, 249), 10, 10);
    m_powerUpTex = AssetManager::instance().getTexture("powerup.png", sf::Color(182, 255, 255), 32, 32);

    if (loadProjectFont(m_font)) {
        m_fontLoaded = true;
        m_hudText.setFont(m_font);
        m_hudText.setCharacterSize(18);
        m_hudText.setFillColor(sf::Color::White);
        m_hudText.setPosition(10.f, 10.f);

        m_waveText.setFont(m_font);
        m_waveText.setCharacterSize(22);
        m_waveText.setFillColor(sf::Color(255, 255, 100));
        m_waveText.setPosition(280.f, 280.f);

        m_gameOverText.setFont(m_font);
        m_gameOverText.setCharacterSize(36);
        m_gameOverText.setFillColor(sf::Color(255, 80, 80));
        m_gameOverText.setPosition(220.f, 250.f);
    }

    const sf::Vector2f worldBounds = Entity::getWorldBounds();
    m_player = std::make_unique<Player>(
        m_playerTex, sf::Vector2f(worldBounds.x * 0.5f, worldBounds.y * 0.86f));

    if (resumeFromSaveFile) {
        if (savedGame != nullptr) {
            m_score = savedGame->score;
            m_level = savedGame->level;
            m_waveInLevel = std::max(0, std::min(WAVES_PER_LEVEL - 1, savedGame->wave));
            m_player->setLives(savedGame->lives);
            const sf::Vector2f savedPosition(
                std::max(20.f, std::min(worldBounds.x - 20.f, savedGame->playerPosition.x)),
                std::max(20.f, std::min(worldBounds.y - 20.f, savedGame->playerPosition.y)));
            m_player->setPosition(savedPosition);
        }
    }

    spawnWave();
    AudioManager::instance().playMusic("wave_music.ogg", true);
    m_waveTransitionTimer = 1.2f;
}

void Match::spawnBoss() {
    m_enemies.clear();
    m_waveEnemyCount = 1;

    const sf::Vector2f worldBounds = Entity::getWorldBounds();
    Enemy boss(m_enemyBossTex, m_bulletTex,
               sf::Vector2f(worldBounds.x * 0.5f, worldBounds.y * 0.16f));
    boss.configureAsBoss(m_level);
    boss.setScoreValue(1200 + m_level * 80);
    m_enemies.push_back(std::move(boss));
    m_bossSummonTimer = 6.f;
}

void Match::spawnBossMinions(const sf::Vector2f& bossPosition) {
    std::uniform_int_distribution<int> countDistribution(1, 3);
    std::uniform_int_distribution<int> typeDistribution(0, 99);
    std::uniform_real_distribution<float> offsetX(-100.f, 100.f);
    std::uniform_real_distribution<float> offsetY(35.f, 90.f);
    const sf::Vector2f world = Entity::getWorldBounds();
    const int count = countDistribution(m_rng);

    for (int i = 0; i < count; ++i) {
        const int type = typeDistribution(m_rng);
        const sf::Vector2f position(
            std::max(32.f, std::min(world.x - 32.f, bossPosition.x + offsetX(m_rng))),
            std::max(32.f, std::min(world.y - 32.f, bossPosition.y + offsetY(m_rng))));
        const sf::Texture* texture = &m_enemyEasyTex;
        sf::Color color(248, 201, 77);
        Enemy::PatternType pattern = Enemy::PatternType::Single;
        int hp = 1;
        int score = 80;
        float cooldown = 1.7f;
        float bulletSpeed = 180.f;

        if (type >= 70) {
            texture = &m_enemySpecialTex;
            color = sf::Color(186, 66, 40);
            pattern = (type % 2 == 0) ? Enemy::PatternType::Circular : Enemy::PatternType::Spiral;
            hp = 2;
            score = 180;
            cooldown = 1.5f;
            bulletSpeed = 200.f;
        } else if (type >= 35) {
            texture = &m_enemyHardTex;
            color = sf::Color(216, 129, 57);
            pattern = Enemy::PatternType::Fan;
            hp = 2;
            score = 140;
            cooldown = 1.6f;
            bulletSpeed = 190.f;
        } else if (type < 15) {
            color = sf::Color(12, 183, 242);
            score = 150;
        }

        m_enemies.emplace_back(*texture, m_bulletTex, position);
        Enemy& minion = m_enemies.back();
        minion.setColor(color);
        minion.setRotation(180.f);
        minion.fitToSize(56.f, 56.f);
        minion.setHitboxLocal(sf::FloatRect(-24.f, -24.f, 48.f, 48.f));
        minion.setVelocity(sf::Vector2f(90.f, 0.f));
        minion.setPattern(pattern);
        minion.setShootCooldown(cooldown);
        minion.setBulletSpeed(bulletSpeed);
        minion.setHP(hp * 7);
        minion.setScoreValue(score);
        minion.setFanCount(5);
        minion.setCircularCount(10);
        if (type < 15) {
            minion.enableReboundBehavior();
        } else if (type >= 35 && type < 70) {
            minion.enableDividerBehavior();
        }
    }

    m_waveEnemyCount += count;
}

void Match::spawnNormalWave(int waveIndex) {
    m_enemies.clear();

    const float baseY = 60.f + waveIndex * 45.f;
    const float speedY = 18.f + m_level * 6.f + waveIndex * 3.f;
    const int minCount = 4 + waveIndex * 3 + (m_level - 1) * 2;
    const int maxCount = minCount + 4 + waveIndex + (m_level - 1);
    const int enemyCount = (waveIndex == WAVES_PER_LEVEL - 2)
        ? maxCount
        : std::uniform_int_distribution<int>(minCount, maxCount)(m_rng);
    m_waveEnemyCount = enemyCount;
    m_highestNormalWaveCount = std::max(m_highestNormalWaveCount, enemyCount);

    std::uniform_int_distribution<int> typeRoll(0, 99);
    const float worldWidth = Entity::getWorldBounds().x;
    std::uniform_real_distribution<float> xDist(80.f, std::max(80.f, worldWidth - 80.f));

    auto addEnemy = [&](float x, float y, const sf::Texture& tex, Enemy::PatternType pattern,
                        float cooldown, float bulletSpeed, int hp, int scoreValue,
                        const sf::Color& color, int fanCount = 5, int circularCount = 12) {
        m_enemies.emplace_back(tex, m_bulletTex, sf::Vector2f(x, y));
        Enemy& e = m_enemies.back();
        e.setColor(color);
        e.setRotation(180.f);
        e.fitToSize(64.f, 64.f);
        e.setHitboxLocal(sf::FloatRect(-28.f, -28.f, 56.f, 56.f));
        e.setVelocity(sf::Vector2f(85.f, 0.f));
        e.setFormationSlot(m_enemies.size() - 1);
        e.setPattern(pattern);
        e.setShootCooldown(cooldown);
        e.setBulletSpeed(bulletSpeed);
        e.setHP(hp * 7);
        e.setScoreValue(scoreValue);
        e.setFanCount(fanCount);
        e.setCircularCount(circularCount);
    };

    for (int i = 0; i < enemyCount; ++i) {
        float x = xDist(m_rng);
        float y = baseY + (i / 8) * 40.f;
        int roll = typeRoll(m_rng);

        if (waveIndex == 0) {
            if (roll < 15) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 1.8f, 190.f, 1, 150,
                         sf::Color(12, 183, 242), 4, 10);
                m_enemies.back().enableReboundBehavior();
            } else if (roll < 70) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 2.0f, 160.f, 1, 100,
                         sf::Color(248, 201, 77), 4, 10);
            } else {
                addEnemy(x, y, m_enemyHardTex, Enemy::PatternType::Fan, 1.8f, 180.f, 2, 180,
                         sf::Color(216, 129, 57), 5, 10);
                m_enemies.back().enableDividerBehavior();
            }
        } else if (waveIndex == 1) {
            if (roll < 18) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 1.6f, 200.f, 1, 180,
                         sf::Color(12, 183, 242), 4, 10);
                m_enemies.back().enableReboundBehavior();
            } else if (roll < 45) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 1.8f, 170.f, 1, 120,
                         sf::Color(248, 201, 77), 4, 10);
            } else if (roll < 80) {
                addEnemy(x, y, m_enemyHardTex, Enemy::PatternType::Fan, 1.5f, 190.f, 3, 240,
                         sf::Color(216, 129, 57), 6, 12);
                m_enemies.back().enableDividerBehavior();
            } else {
                if (typeRoll(m_rng) < 40) {
                    addEnemy(x, y, m_enemySpecialTex, Enemy::PatternType::Single, 2.6f, 520.f, 2, 300,
                             sf::Color(255, 65, 75), 5, 12);
                    m_enemies.back().enableSniperBehavior();
                } else {
                    const Enemy::PatternType specialPattern = (typeRoll(m_rng) % 2 == 0)
                        ? Enemy::PatternType::Circular
                        : Enemy::PatternType::Spiral;
                    addEnemy(x, y, m_enemySpecialTex, specialPattern, 1.6f, 180.f, 2, 300,
                             sf::Color(186, 66, 40), 5, 12);
                }
            }
        } else {
            if (roll < 20) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 1.5f, 210.f, 1, 200,
                         sf::Color(12, 183, 242), 4, 10);
                m_enemies.back().enableReboundBehavior();
            } else if (roll < 40) {
                addEnemy(x, y, m_enemyEasyTex, Enemy::PatternType::Single, 1.7f, 170.f, 1, 140,
                         sf::Color(248, 201, 77), 4, 10);
            } else if (roll < 70) {
                addEnemy(x, y, m_enemyHardTex, Enemy::PatternType::Fan, 1.4f, 200.f, 3, 280,
                         sf::Color(216, 129, 57), 7, 14);
                m_enemies.back().enableDividerBehavior();
            } else {
                if (typeRoll(m_rng) < 45) {
                    addEnemy(x, y, m_enemySpecialTex, Enemy::PatternType::Single, 2.6f, 520.f, 2, 340,
                             sf::Color(255, 65, 75), 8, 14);
                    m_enemies.back().enableSniperBehavior();
                } else {
                    const Enemy::PatternType specialPattern = (typeRoll(m_rng) % 3 == 0)
                        ? Enemy::PatternType::Circular
                        : Enemy::PatternType::Spiral;
                    addEnemy(x, y, m_enemySpecialTex, specialPattern, 1.3f, 210.f, 2, 340,
                             sf::Color(186, 66, 40), 8, 14);
                }
            }
        }
    }
}

void Match::spawnWave() {
    const bool isBossWave = (m_waveInLevel == WAVES_PER_LEVEL - 1);

    if (isBossWave) {
        spawnBoss();
    } else {
        spawnNormalWave(m_waveInLevel);
        m_formation.selectPattern(m_waveInLevel + m_level, m_enemies.size());
    }
    if (m_fontLoaded) {
        std::ostringstream oss;
        if (isBossWave) {
            oss << "JEFE - Nivel " << m_level;
        } else {
            oss << "Oleada " << (m_waveInLevel + 1) << " / " << WAVES_PER_LEVEL
                << "  -  Nivel " << m_level
                << "  -  Enemigos: " << m_waveEnemyCount;
        }
        m_waveText.setString(oss.str());
        const sf::FloatRect bounds = m_waveText.getLocalBounds();
        m_waveText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        const sf::Vector2f worldBounds = Entity::getWorldBounds();
        m_waveText.setPosition(worldBounds.x * 0.5f, worldBounds.y * 0.5f);
    }
}

void Match::tryDropPowerUp(const sf::Vector2f& pos, bool guaranteed) {
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    if (!guaranteed && dist(m_rng) > 0.04f) return;

    std::uniform_int_distribution<int> typeDist(0, 3);
    const auto type = static_cast<PowerUp::Type>(typeDist(m_rng));
    m_powerUps.emplace_back(type, m_powerUpTex, pos);
}

void Match::spawnDividerChildren(const sf::Vector2f& position) {
    const sf::Vector2f world = Entity::getWorldBounds();
    const float left = std::max(24.f, std::min(world.x - 24.f, position.x - 18.f));
    const float right = std::max(24.f, std::min(world.x - 24.f, position.x + 18.f));
    const float y = std::max(24.f, std::min(world.y - 24.f, position.y));
    const sf::Vector2f positions[] = {
        sf::Vector2f(left, y),
        sf::Vector2f(right, y)
    };
    const float angles[] = {-24.f, 24.f};

    for (int i = 0; i < 2; ++i) {
        m_enemies.emplace_back(m_enemyEasyTex, m_bulletTex, positions[i]);
        Enemy& child = m_enemies.back();
        child.setColor(sf::Color(12, 183, 242));
        child.fitToSize(38.f, 38.f);
        child.setHitboxLocal(sf::FloatRect(-16.f, -16.f, 32.f, 32.f));
        child.setHP(7);
        child.setScoreValue(40);
        child.enableReboundBehavior(angles[i]);
    }

    m_waveEnemyCount += 2;
}

void Match::cleanupDead() {
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const std::unique_ptr<Bullet>& b) { return !b->isAlive(); }),
        m_bullets.end());

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const Enemy& e) { return !e.isAlive(); }),
        m_enemies.end());

    m_powerUps.erase(
        std::remove_if(m_powerUps.begin(), m_powerUps.end(),
            [](const PowerUp& p) { return !p.isAlive(); }),
        m_powerUps.end());
}

void Match::checkCollisions() {
    std::vector<sf::Vector2f> dividerPositions;
    for (auto& bullet : m_bullets) {
        if (!bullet->isAlive()) continue;

        if (bullet->isFromPlayer()) {
            for (auto& enemy : m_enemies) {
                if (!enemy.isAlive()) continue;
                if (bullet->getBounds().intersects(enemy.getBounds())) {
                    AudioManager::instance().playSfx("collision_en.wav");
                    enemy.setHP(enemy.getHP() - 1);
                    bullet->destroy();
                    if (enemy.getHP() <= 0) {
                        const sf::Vector2f deathPos = enemy.getPosition();
                        const bool wasBoss = enemy.isBoss();
                        const bool wasDivider = enemy.isDivider();
                        const int enemyScore = enemy.getScoreValue();
                        enemy.destroy();
                        m_score += enemyScore;
                        tryDropPowerUp(deathPos, wasBoss);
                        if (wasDivider) {
                            dividerPositions.push_back(deathPos);
                        }
                    }
                    break;
                }
            }
        } else if (m_player->isAlive() && !m_player->isInvulnerable()
                   && bullet->getBounds().intersects(m_player->getHitboxGlobal())) {
            AudioManager::instance().playSfx("collision.wav");
            m_player->loseLife();
            bullet->destroy();
            if (!m_player->isAlive()) {
                m_gameOver = true;
            }
        }
    }

    for (const sf::Vector2f& position : dividerPositions) {
        spawnDividerChildren(position);
    }

    if (m_player->isAlive()) {
        for (auto& powerUp : m_powerUps) {
            if (!powerUp.isAlive()) continue;
            sf::FloatRect pickupBounds = powerUp.getBounds();
            pickupBounds.left -= 8.f;
            pickupBounds.top -= 8.f;
            pickupBounds.width += 16.f;
            pickupBounds.height += 16.f;
            if (pickupBounds.intersects(m_player->getHitboxGlobal())) {
                powerUp.apply(*m_player);
            }
        }
    }
}

void Match::resetMatch() {
    m_score = 0;
    m_level = 1;
    m_waveInLevel = 0;
    m_waveEnemyCount = 0;
    m_highestNormalWaveCount = 0;
    m_gameOver = false;
    m_paused = false;
    m_waitingForNextWave = false;
    m_waveTransitionTimer = 0.f;
    m_bossSummonTimer = 0.f;
    m_bullets.clear();
    m_enemies.clear();
    m_powerUps.clear();
    const sf::Vector2f worldBounds = Entity::getWorldBounds();
    m_player = std::make_unique<Player>(
        m_playerTex, sf::Vector2f(worldBounds.x * 0.5f, worldBounds.y * 0.86f));
    spawnWave();
}

void Match::handleEvent(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::KeyPressed) {
        if (m_gameOver && event.key.code == sf::Keyboard::R) {
            resetMatch();
            return;
        }

        if (event.key.code == sf::Keyboard::Return) {
            m_paused = !m_paused;
            game.setContinueAvailable(true);
            AudioManager::instance().playSfx("selecting.wav");
            if (m_paused) {
                AudioManager::instance().setMusicUnderwater(true);
            } else {
                AudioManager::instance().setMusicUnderwater(false);
            }
            return;
        }

        if (m_paused && event.key.code == sf::Keyboard::Escape) {
            AudioManager::instance().playSfx("selecting.wav");
            SavedGame savedGame;
            savedGame.initials = game.getPlayerInitials();
            savedGame.score = m_score;
            savedGame.level = m_level;
            savedGame.wave = m_waveInLevel;
            savedGame.lives = m_player->getLives();
            savedGame.playerPosition = m_player->getPosition();
            const sf::Vector2f bounds = Entity::getWorldBounds();
            if (!game.saveGameState(savedGame)) {
                return;
            }
            game.setContinueAvailable(true);
            AudioManager::instance().stopMusic();
            game.setScene(new SceneMenu());
        }
    }
}

void Match::update(float dt, Game& game) {
    (void)game;

    if (m_gameOver) {
        ScoreManager::instance().registerScore(m_score, game.getPlayerInitials());
        game.clearSavedGame();
        game.setContinueAvailable(false);
        game.setScene(new SceneHighScores());
        return;
    }

    if (m_paused) {
        return;
    }

    m_player->update(dt);

    const bool firePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Z)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::X);
    if (firePressed) {
        for (auto& bullet : m_player->shoot(m_bulletTex)) {
            AudioManager::instance().playSfx("shoot.wav");
            m_bullets.push_back(std::move(bullet));
        }
    }

    m_formation.update(m_enemies, dt, static_cast<float>(game.getWindowWidth()),
                       static_cast<float>(game.getWindowHeight()));
    for (auto& enemy : m_enemies) {
        if (!enemy.isAlive()) continue;
        if (enemy.isBoss() || enemy.hasReboundBehavior() || enemy.isSniper()) {
            enemy.setTargetPosition(m_player->getPosition());
        }
        enemy.update(dt);

        if (!enemy.hasReboundBehavior() && enemy.getPosition().y > 650.f) {
            enemy.destroy();
            continue;
        }

        for (auto& bullet : enemy.shoot()) {
            AudioManager::instance().playSfx("shoot.wav");
            bullet->setColor(sf::Color(186, 66, 40));
            m_bullets.push_back(std::move(bullet));
        }
    }

    bool bossAlive = false;
    sf::Vector2f bossPosition;
    for (const auto& enemy : m_enemies) {
        if (enemy.isAlive() && enemy.isBoss()) {
            bossAlive = true;
            bossPosition = enemy.getPosition();
        }
    }
    if (m_bossSummonTimer > 0.f) {
        m_bossSummonTimer -= dt;
    }
    if (bossAlive && m_bossSummonTimer <= 0.f) {
        spawnBossMinions(bossPosition);
        std::uniform_real_distribution<float> summonDelay(7.f, 10.f);
        m_bossSummonTimer = summonDelay(m_rng);
    }

    for (auto& bullet : m_bullets) {
        if (bullet->isAlive()) bullet->update(dt);
    }

    for (auto& powerUp : m_powerUps) {
        if (powerUp.isAlive()) powerUp.update(dt);
    }

    checkCollisions();
    cleanupDead();

    if (m_waveTransitionTimer > 0.f) {
        m_waveTransitionTimer -= dt;
        if (m_waveTransitionTimer <= 0.f) {
            m_waitingForNextWave = false;
            spawnWave();
        }
    }

    if (m_enemies.empty() && !m_waitingForNextWave) {
        m_waitingForNextWave = true;
        m_waveTransitionTimer = WAVE_DELAY;
        ++m_waveInLevel;

        if (m_waveInLevel >= WAVES_PER_LEVEL) {
            m_waveInLevel = 0;
            ++m_level;
        }
    }
}

void Match::drawHud(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;

    std::ostringstream oss;
    oss << "Puntos: " << m_score
        << "   Vidas: " << m_player->getLives()
        << "   Nivel: " << m_level
        << "   Oleada: " << (m_waveInLevel + 1) << "/" << WAVES_PER_LEVEL
        << "   Enemigos: " << m_waveEnemyCount;
    m_hudText.setString(oss.str());
    const sf::FloatRect hudBounds = m_hudText.getLocalBounds();
    const sf::Vector2u windowSize = window.getSize();
    m_hudText.setPosition(static_cast<float>(windowSize.x) - hudBounds.width - 20.f, 16.f);
    window.draw(m_hudText);

    if (m_waveTransitionTimer > 0.f) {
        window.draw(m_waveText);
    }

    if (m_gameOver) {
        std::ostringstream go;
        go << "GAME OVER\nPuntos: " << m_score << "\n[R] Reintentar";
        m_gameOverText.setString(go.str());
        window.draw(m_gameOverText);
    }

    if (m_paused) {
        sf::Text pauseText;
        pauseText.setFont(m_font);
        pauseText.setCharacterSize(28);
        pauseText.setString("PAUSADO\nENTER para reanudar\nESC para guardar y volver");
        pauseText.setFillColor(sf::Color(255, 255, 255));
        const sf::FloatRect pauseBounds = pauseText.getLocalBounds();
        pauseText.setOrigin(pauseBounds.width / 2.f, pauseBounds.height / 2.f);
        pauseText.setPosition(static_cast<float>(windowSize.x) * 0.5f,
                              static_cast<float>(windowSize.y) * 0.5f);
        window.draw(pauseText);
    }
}

void Match::draw(sf::RenderWindow& window) {
    if (m_player->isAlive() || m_gameOver) {
        m_player->draw(window);
    }

    for (const auto& enemy : m_enemies) {
        enemy.draw(window);
    }

    for (const auto& bullet : m_bullets) {
        bullet->draw(window);
    }

    for (const auto& powerUp : m_powerUps) {
        powerUp.draw(window);
    }

    drawHud(window);
}
