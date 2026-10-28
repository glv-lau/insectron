#pragma once
#include "Entity.h"
#include "Bullet.h"
#include <cstddef>
#include <memory>
#include <random>
#include <vector>

class Enemy : public Entity {
public:
    using PatternType = Bullet::PatternType;

    Enemy(const sf::Texture& tex, const sf::Texture& bulletTex, const sf::Vector2f& pos);

    ~Enemy() override = default;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

    std::vector<std::unique_ptr<Bullet>> shoot();

    void setHP(int hp) { m_hp = hp; }
    int getHP() const { return m_hp; }
    void setShootCooldown(float cd) { m_shootCooldown = cd; }
    void setPattern(PatternType p) { m_pattern = p; }
    void setCircularCount(int c) { m_circularCount = c; }
    void setFanCount(int c) { m_fanCount = c; }
    void setBulletSpeed(float s) { m_bulletSpeed = s; }
    void setSpiralSpeed(float degPerSec) { m_spiralDegPerSec = degPerSec; }
    void setScoreValue(int score) { m_scoreValue = score; }
    int getScoreValue() const { return m_scoreValue; }

    void setVelocity(const sf::Vector2f& v) { m_velocity = v; }
    void setTargetPosition(const sf::Vector2f& pos) { m_targetPosition = pos; m_hasTarget = true; }

    void configureAsBoss(int level);
    bool isBoss() const { return m_isBoss; }
    void enableReboundBehavior(float angleOffsetDegrees = 0.f);
    bool hasReboundBehavior() const { return m_isRebounder; }
    void enableSniperBehavior();
    bool isSniper() const { return m_isSniper; }
    bool isSniperAiming() const { return m_isSniper && m_sniperAiming; }
    void enableDividerBehavior() { m_isDivider = true; }
    bool isDivider() const { return m_isDivider; }
    void setFormationSlot(std::size_t slot) { m_formationSlot = slot; m_hasFormationSlot = true; }
    bool hasFormationSlot() const { return m_hasFormationSlot; }
    std::size_t getFormationSlot() const { return m_formationSlot; }

private:
    enum class ReboundState {
        Charging,
        Tired
    };

    const sf::Texture *m_bulletTexture;

    int m_hp = 1;
    int m_scoreValue = 100;

    float m_shootCooldown = 1.2f;
    float m_shootTimer = 0.f;
    PatternType m_pattern = PatternType::Single;

    int m_circularCount = 12;
    int m_fanCount = 5;
    float m_bulletSpeed = 200.f;
    float m_spiralDegPerSec = 90.f;
    float m_spiralAngle = 0.f;

    float m_timeAlive = 0.f;
    bool m_isBoss = false;
    bool m_isRebounder = false;
    bool m_reboundTargetReady = false;
    ReboundState m_reboundState = ReboundState::Charging;
    int m_reboundCount = 0;
    float m_reboundTimer = 0.f;
    float m_reboundSpeed = 340.f;
    float m_reboundAngleOffset = 0.f;
    bool m_isSniper = false;
    bool m_sniperAiming = false;
    bool m_sniperFireReady = false;
    float m_sniperAimTimer = 0.f;
    sf::Vector2f m_sniperAimPosition;
    sf::Vector2f m_sniperResumeVelocity;
    bool m_isDivider = false;
    bool m_hasFormationSlot = false;
    std::size_t m_formationSlot = 0;
    sf::Vector2f m_targetPosition;
    bool m_hasTarget = false;
    float m_bossRetuneTimer = 0.f;
    float m_bossAx = 300.f;
    float m_bossAy = 180.f;
    float m_bossW1 = 0.7f;
    float m_bossW2 = 1.1f;
    float m_bossPhaseX = 0.f;
    float m_bossPhaseY = 0.f;
    float m_bossTargetAx = 300.f;
    float m_bossTargetAy = 180.f;
    float m_bossTargetW1 = 0.7f;
    float m_bossTargetW2 = 1.1f;
    float m_bossTargetPhaseX = 0.f;
    float m_bossTargetPhaseY = 0.f;
    std::mt19937 m_bossRng;
    unsigned int m_bossShotPattern = 0;
    unsigned int m_bossPatternShotCount = 0;
    bool m_bossTelegraphActive = false;
    bool m_bossFireReady = false;
    float m_bossTelegraphTimer = 0.f;
    sf::Vector2f m_bossAimPosition;

    void retuneBossTrajectory();
    void updateRebounder(float dt);
    void drawAimLine(sf::RenderWindow& window, const sf::Vector2f& target,
                     const sf::Color& color) const;
};
