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

    if (!m_isBoss) {
        Entity::update(dt);
    }

    Vector2f p = getPosition();
    const Vector2f bounds = Entity::getWorldBounds();
    if (!m_isBoss && (p.x <= 80.f || p.x >= 720.f)) {
        m_velocity.x = -m_velocity.x;
    }
    p.x = std::max(26.f, std::min(bounds.x - 26.f, p.x));
    p.y = std::max(30.f, std::min(bounds.y - 30.f, p.y));
    setPosition(p);

    if (m_shootTimer > 0.f) m_shootTimer -= dt;

    if (m_pattern == PatternType::Spiral) {
        m_spiralAngle += m_spiralDegPerSec * dt;
        if (m_spiralAngle >= 360.f) m_spiralAngle -= 360.f;
    }
}

std::vector<std::unique_ptr<Bullet>> Enemy::shoot() {
    std::vector<std::unique_ptr<Bullet>> out;
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
        const auto emit = [&](float angle, float speed) {
            const float radians = angle * PI_F / 180.f;
            out.push_back(std::make_unique<Bullet>(
                *m_bulletTexture, pos,
                Vector2f(std::cos(radians) * speed, std::sin(radians) * speed)));
        };

        switch (m_bossShotPattern % 4) {
            case 0: { // Flor: petalos radiales con rotacion progresiva.
                for (int petal = 0; petal < 8; ++petal) {
                    for (int ring = 0; ring < 3; ++ring) {
                        emit(baseAngle + petal * 45.f + ring * 8.f,
                             m_bulletSpeed * (0.72f + ring * 0.12f));
                    }
                }
                break;
            }
            case 1: { // Trisquel: tres brazos curvos entrelazados.
                for (int arm = 0; arm < 3; ++arm) {
                    for (int step = 0; step < 5; ++step) {
                        emit(baseAngle + arm * 120.f + step * 13.f,
                             m_bulletSpeed * (0.65f + step * 0.09f));
                    }
                }
                break;
            }
            case 2: { // Estrella de cinco puntas: vertices y diagonales.
                for (int point = 0; point < 7; ++point) {
                    emit(baseAngle + point * (360.f / 7.f), m_bulletSpeed);
                    emit(baseAngle + point * (360.f / 7.f) + 25.7f,
                         m_bulletSpeed * 0.78f);
                }
                break;
            }
            case 3: { // Ondas: anillos alternados con velocidades diferentes.
                for (int wave = 0; wave < 2; ++wave) {
                    for (int i = 0; i < 16; ++i) {
                        emit(baseAngle + i * 22.5f + wave * 11.25f,
                             m_bulletSpeed * (0.62f + wave * 0.22f));
                    }
                }
                break;
            }
        }

        ++m_bossShotPattern;
        m_spiralAngle += 18.f;
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
    m_hp = 45 + level * 18;
    fitToSize(200.f, 200.f);
    setHitboxLocal(FloatRect(-86.f, -86.f, 172.f, 172.f));
    setColor(Color(124, 14, 54));
    setRotation(180.f);
    setPattern(PatternType::Spiral);
    setShootCooldown(0.28f);
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
