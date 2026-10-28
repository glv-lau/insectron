#pragma once
#include "Entity.h"
#include <memory>
#include <vector>

class Bullet;

class Player : public Entity {
public:
    Player(const sf::Texture& tex, const sf::Vector2f& pos);

    void update(float dt) override;
    void draw(sf::RenderWindow& win) const override;
    std::vector<std::unique_ptr<Bullet>> shoot(const sf::Texture& bulletTexture);

    bool canShoot() const { return m_shootTimer <= 0.f; }
    void resetShootTimer() { m_shootTimer = getEffectiveCooldown(); }

    void loseLife();
    void addLife();
    int getLives() const { return m_lives; }
    void setLives(int lives);

    void activateRapidFire(float duration);
    void activateSpreadShot(float duration);
    void activateShield(float duration);

    bool hasShield() const { return m_shieldTimer > 0.f; }
    bool isInvulnerable() const { return m_invulnTimer > 0.f || hasShield(); }

private:
    float getEffectiveCooldown() const;

    int m_lives = 3;
    float m_speed = 345.f;
    float m_shootCooldown = 0.18f;
    float m_shootTimer = 0.f;

    float m_invulnTimer = 0.f;
    float m_invulnDuration = 2.f;
    float m_blinkTimer = 0.f;

    float m_rapidFireTimer = 0.f;
    float m_spreadShotTimer = 0.f;
    float m_shieldTimer = 0.f;
};
