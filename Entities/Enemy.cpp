#include "Enemy.h"
#include <SFML/System/Vector2.hpp>
#include <cmath>

using namespace sf;

static constexpr float PI_F = 3.14159265358979323846f;

Enemy::Enemy(const Texture& tex, const Texture& bulletTex, const Vector2f& pos)
    : m_bulletTexture(&bulletTex)
    , m_bossRng(std::random_device{}())
{
    setTextureToSize(tex, 64.f, 64.f, true);
    setHitboxLocal(FloatRect(-28.f, -28.f, 56.f, 56.f));
    setPosition(pos);
}

void Enemy::update(float dt) {
    m_timeAlive += dt;

    if (m_isBoss) {
        m_bossRetuneTimer -= dt;
        if (m_bossRetuneTimer <= 0.f) {
            retuneBossTrajectory();
            m_bossRetuneTimer = 4.5f;
        }
        const float blend = std::min(1.f, dt * 0.45f);
        m_bossAx += (m_bossTargetAx - m_bossAx) * blend;
        m_bossAy += (m_bossTargetAy - m_bossAy) * blend;
        m_bossW1 += (m_bossTargetW1 - m_bossW1) * blend;
        m_bossW2 += (m_bossTargetW2 - m_bossW2) * blend;
        m_bossPhaseX += (m_bossTargetPhaseX - m_bossPhaseX) * blend;
        m_bossPhaseY += (m_bossTargetPhaseY - m_bossPhaseY) * blend;
        const Vector2f world = Entity::getWorldBounds();
        const float x = world.x * 0.5f
            + m_bossAx * std::sin(m_bossW1 * m_timeAlive + m_bossPhaseX);
        const float y = world.y * 0.18f + m_bossAy * 0.32f
            * std::cos(m_bossW2 * m_timeAlive + m_bossPhaseY);
        setPosition(Vector2f(std::max(72.f, std::min(world.x - 72.f, x)),
                             std::max(75.f, std::min(world.y * 0.48f, y))));
    }

    if (m_isRebounder) {
        updateRebounder(dt);
    } else if (!m_isBoss && !isSniperAiming()) {
        Entity::update(dt);
    }

    Vector2f p = getPosition();
    const Vector2f bounds = Entity::getWorldBounds();
    if (!m_isBoss && !m_isRebounder && (p.x <= 80.f || p.x >= 720.f)) {
        m_velocity.x = -m_velocity.x;
    }
    if (!m_isRebounder) {
        p.x = std::max(26.f, std::min(bounds.x - 26.f, p.x));
        p.y = std::max(30.f, std::min(bounds.y - 30.f, p.y));
        setPosition(p);
    }

    if (m_shootTimer > 0.f) m_shootTimer -= dt;
    if (m_isSniper && m_sniperAiming) {
        m_sniperAimTimer -= dt;
        if (m_sniperAimTimer <= 0.f) {
            m_sniperAimTimer = 0.f;
            m_sniperFireReady = true;
            m_shootTimer = 0.f;
        }
    }
    if (m_isBoss && m_bossTelegraphActive) {
        m_bossTelegraphTimer -= dt;
        if (m_bossTelegraphTimer <= 0.f) {
            m_bossTelegraphTimer = 0.f;
            m_bossFireReady = true;
            m_shootTimer = 0.f;
        }
    }

    if (m_pattern == PatternType::Spiral) {
        m_spiralAngle += m_spiralDegPerSec * dt;
        if (m_spiralAngle >= 360.f) m_spiralAngle -= 360.f;
    }
}

void Enemy::enableReboundBehavior(float angleOffsetDegrees) {
    m_isRebounder = true;
    m_reboundState = ReboundState::Charging;
    m_reboundTargetReady = false;
    m_reboundCount = 0;
    m_reboundTimer = 0.f;
    m_reboundAngleOffset = angleOffsetDegrees;
    m_velocity = Vector2f(0.f, 0.f);
}

void Enemy::enableSniperBehavior() {
    m_isSniper = true;
    m_shootCooldown = 2.6f;
    m_shootTimer = 1.2f;
    m_bulletSpeed = 520.f;
}

void Enemy::updateRebounder(float dt) {
    if (m_reboundState == ReboundState::Tired) {
        m_velocity = Vector2f(0.f, 0.f);
        m_reboundTimer -= dt;
        if (m_reboundTimer <= 0.f) {
            m_reboundState = ReboundState::Charging;
            m_reboundTargetReady = false;
            m_reboundCount = 0;
        }
        return;
    }

    if (!m_reboundTargetReady) {
        if (!m_hasTarget) {
            m_velocity = Vector2f(0.f, 0.f);
            return;
        }

        Vector2f direction = m_targetPosition - getPosition();
        const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.0001f) {
            direction /= length;
        } else {
            direction = Vector2f(0.f, 1.f);
        }
        const float radians = m_reboundAngleOffset * PI_F / 180.f;
        direction = Vector2f(
            direction.x * std::cos(radians) - direction.y * std::sin(radians),
            direction.x * std::sin(radians) + direction.y * std::cos(radians));
        m_velocity = direction * m_reboundSpeed;
        m_reboundTargetReady = true;
    }

    Entity::update(dt);

    Vector2f position = getPosition();
    const Vector2f bounds = Entity::getWorldBounds();
    const float left = std::min(32.f, bounds.x * 0.5f);
    const float right = std::max(left, bounds.x - left);
    const float top = std::min(32.f, bounds.y * 0.5f);
    const float bottom = std::max(top, bounds.y - top);
    bool bounced = false;

    if (position.x < left) {
        position.x = left;
        m_velocity.x = std::abs(m_velocity.x);
        bounced = true;
    } else if (position.x > right) {
        position.x = right;
        m_velocity.x = -std::abs(m_velocity.x);
        bounced = true;
    }
    if (position.y < top) {
        position.y = top;
        m_velocity.y = std::abs(m_velocity.y);
        bounced = true;
    } else if (position.y > bottom) {
        position.y = bottom;
        m_velocity.y = -std::abs(m_velocity.y);
        bounced = true;
    }
    setPosition(position);

    if (bounced && ++m_reboundCount >= 2) {
        m_reboundState = ReboundState::Tired;
        m_reboundTimer = 2.5f;
        m_velocity = Vector2f(0.f, 0.f);
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (!isAlive() || !isVisible()) {
        return;
    }
    Entity::draw(window);
    if (isSniperAiming()) {
        drawAimLine(window, m_sniperAimPosition, Color(255, 65, 75, 220));
    }
    if (m_isBoss && m_bossTelegraphActive) {
        drawAimLine(window, m_bossAimPosition, Color(255, 115, 145, 210));
    }
}

void Enemy::drawAimLine(sf::RenderWindow& window, const Vector2f& target,
                        const Color& color) const {
    const Vector2f delta = target - getPosition();
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (length <= 0.0001f) {
        return;
    }

    RectangleShape laser(Vector2f(length, 3.f));
    laser.setOrigin(0.f, 1.5f);
    laser.setPosition(getPosition());
    laser.setRotation(std::atan2(delta.y, delta.x) * 180.f / PI_F);
    laser.setFillColor(color);
    window.draw(laser);
}

std::vector<std::unique_ptr<Bullet>> Enemy::shoot() {
    std::vector<std::unique_ptr<Bullet>> out;

    if (m_isSniper) {
        if (m_sniperFireReady) {
            Vector2f direction = m_sniperAimPosition - getPosition();
            const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length > 0.0001f) {
                direction /= length;
            } else {
                direction = Vector2f(0.f, 1.f);
            }
            out.push_back(std::make_unique<Bullet>(
                *m_bulletTexture, getPosition(), direction * m_bulletSpeed));
            m_sniperAiming = false;
            m_sniperFireReady = false;
            m_velocity = m_sniperResumeVelocity;
            m_shootTimer = m_shootCooldown;
            return out;
        }
        if (m_sniperAiming || m_shootTimer > 0.f) {
            return out;
        }
        m_sniperAiming = true;
        m_sniperAimPosition = m_hasTarget
            ? m_targetPosition
            : getPosition() + Vector2f(0.f, 500.f);
        m_sniperResumeVelocity = m_velocity;
        m_velocity = Vector2f(0.f, 0.f);
        m_sniperAimTimer = 0.7f;
        m_shootTimer = m_sniperAimTimer;
        return out;
    }

    if (m_shootTimer > 0.f) return out;

    Vector2f pos = getPosition();
    Vector2f aimDir(0.f, 1.f);
    if (m_hasTarget) {
        Vector2f delta = m_targetPosition - pos;
        float len = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (len > 0.0001f) {
            aimDir = delta / len;
        }
    }

    if (m_isBoss) {
        const float baseAngle = m_spiralAngle;
        const float aimAngle = std::atan2(aimDir.y, aimDir.x) * 180.f / PI_F;
        const auto emit = [&](float angle, float speed) {
            const float radians = angle * PI_F / 180.f;
            out.push_back(std::make_unique<Bullet>(
                *m_bulletTexture, pos,
                Vector2f(std::cos(radians) * speed, std::sin(radians) * speed)));
        };

        if (m_bossShotPattern == 0) {
            for (int arm = 0; arm < 3; ++arm) {
                for (int bullet = 0; bullet < 4; ++bullet) {
                    emit(aimAngle + baseAngle + arm * 120.f + bullet * 8.f,
                         m_bulletSpeed * (0.82f + bullet * 0.05f));
                }
            }
            m_spiralAngle += 17.f;
            if (++m_bossPatternShotCount >= 8) {
                m_bossShotPattern = 1;
                m_bossPatternShotCount = 0;
            }
        } else if (m_bossShotPattern == 1) {
            for (int ring = 0; ring < 2; ++ring) {
                const float speed = m_bulletSpeed * (0.68f + ring * 0.24f);
                const float rotation = baseAngle + ring * 13.f;
                for (int bullet = 0; bullet < 18; ++bullet) {
                    const float angle = rotation + bullet * 20.f;
                    const float gapA = baseAngle + ring * 37.f;
                    const float gapB = gapA + 180.f;
                    const float distanceA = std::abs(std::remainder(angle - gapA, 360.f));
                    const float distanceB = std::abs(std::remainder(angle - gapB, 360.f));
                    if (distanceA < 22.f || distanceB < 22.f) {
                        continue;
                    }
                    emit(angle, speed);
                }
            }
            m_spiralAngle += 11.f;
            if (++m_bossPatternShotCount >= 4) {
                m_bossShotPattern = 2;
                m_bossPatternShotCount = 0;
            }
        } else if (m_bossFireReady) {
            const float chargeAngle = std::atan2(
                m_bossAimPosition.y - pos.y, m_bossAimPosition.x - pos.x) * 180.f / PI_F;
            for (int bullet = -2; bullet <= 2; ++bullet) {
                emit(chargeAngle + bullet * 5.f, m_bulletSpeed * 1.45f);
            }
            m_bossTelegraphActive = false;
            m_bossFireReady = false;
            m_bossShotPattern = 0;
            m_bossPatternShotCount = 0;
        } else {
            m_bossAimPosition = m_hasTarget
                ? m_targetPosition
                : pos + Vector2f(0.f, 500.f);
            m_bossTelegraphActive = true;
            m_bossTelegraphTimer = 0.85f;
            m_shootTimer = m_bossTelegraphTimer;
            return out;
        }
    } else {
        switch (m_pattern) {
            case PatternType::Single: {
                Vector2f vel(aimDir.x * m_bulletSpeed, aimDir.y * m_bulletSpeed);
                out.push_back(std::make_unique<Bullet>(*m_bulletTexture, pos, vel));
                break;
            }
            case PatternType::Fan: {
                float centerDeg = 90.f;
                if (m_hasTarget) {
                    centerDeg = std::atan2(aimDir.y, aimDir.x) * 180.f / PI_F + 90.f;
                }
                float spread = 60.f;
                int n = std::max(1, m_fanCount);
                for (int i = 0; i < n; ++i) {
                    const float step = (n > 1) ? (spread * i) / (n - 1.0f) : 0.f;
                    float angle = centerDeg - spread / 2.f + step;
                    float r = angle * PI_F / 180.f;
                    Vector2f vel(std::cos(r) * m_bulletSpeed, std::sin(r) * m_bulletSpeed);
                    out.push_back(std::make_unique<Bullet>(*m_bulletTexture, pos, vel));
                }
                break;
            }
            case PatternType::Circular: {
                int n = std::max(1, m_circularCount);
                for (int i = 0; i < n; ++i) {
                    float angle = (360.f * i) / n;
                    float r = angle * PI_F / 180.f;
                    Vector2f vel(std::cos(r) * m_bulletSpeed, std::sin(r) * m_bulletSpeed);
                    out.push_back(std::make_unique<Bullet>(*m_bulletTexture, pos, vel));
                }
                break;
            }
            case PatternType::Spiral: {
                float angle = m_spiralAngle;
                float r = angle * PI_F / 180.f;
                Vector2f vel(std::cos(r) * m_bulletSpeed, std::sin(r) * m_bulletSpeed);
                out.push_back(std::make_unique<Bullet>(*m_bulletTexture, pos, vel));
                float r2 = (angle + 20.f) * PI_F / 180.f;
                Vector2f vel2(std::cos(r2) * m_bulletSpeed, std::sin(r2) * m_bulletSpeed);
                out.push_back(std::make_unique<Bullet>(*m_bulletTexture, pos, vel2));
                break;
            }
        }
    }

    m_shootTimer = m_shootCooldown;
    return out;
}

void Enemy::configureAsBoss(int level) {
    m_isBoss = true;
    m_hp = (45 + level * 18) * 7;
    fitToSize(200.f, 200.f);
    setHitboxLocal(FloatRect(-86.f, -86.f, 172.f, 172.f));
    setColor(Color(124, 14, 54));
    setRotation(180.f);
    setPattern(PatternType::Spiral);
    setShootCooldown(0.85f);
    setBulletSpeed(200.f + level * 25.f);
    setSpiralSpeed(190.f);
    setVelocity(Vector2f(80.f, 20.f));
    m_bossRetuneTimer = 0.f;
    retuneBossTrajectory();
}

void Enemy::retuneBossTrajectory() {
    std::uniform_real_distribution<float> amplitudeX(240.f, 330.f);
    std::uniform_real_distribution<float> amplitudeY(155.f, 220.f);
    // Frecuencias más bajas: el jefe conserva su recorrido amplio, pero no
    // cambia de dirección ni atraviesa la pantalla de forma demasiado rápida.
    std::uniform_real_distribution<float> frequency(0.22f, 0.62f);
    std::uniform_real_distribution<float> phase(0.f, 6.28318530718f);
    m_bossTargetAx = amplitudeX(m_bossRng);
    m_bossTargetAy = amplitudeY(m_bossRng);
    m_bossTargetW1 = frequency(m_bossRng);
    m_bossTargetW2 = frequency(m_bossRng);
    m_bossTargetPhaseX = phase(m_bossRng);
    m_bossTargetPhaseY = phase(m_bossRng);
}
