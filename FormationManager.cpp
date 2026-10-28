#include "FormationManager.h"

#include <algorithm>
#include <cmath>

namespace {
    const float kPi = 3.14159265358979323846f;
}

FormationManager::FormationManager()
    : m_pattern(Pattern::SineSweep)
    , m_elapsed(0.f)
    , m_center(400.f, 110.f)
    , m_direction(1.f)
    , m_verticalOffset(0.f)
    , m_formationSize(0)
{
}

void FormationManager::selectPattern(int wave, std::size_t enemyCount) {
    const int choice = (wave + static_cast<int>(enemyCount)) % 9;
    m_pattern = static_cast<Pattern>(choice);
    m_formationSize = enemyCount;
    m_elapsed = 0.f;
    m_center = sf::Vector2f(400.f, 110.f);
    m_direction = (wave % 2 == 0) ? 1.f : -1.f;
    m_verticalOffset = 0.f;
}

void FormationManager::update(std::vector<Enemy>& enemies, float dt,
                              float worldWidth, float worldHeight) {
    m_elapsed += dt;
    if (m_elapsed <= dt * 1.1f) {
        m_center.x = worldWidth * 0.5f;
        m_center.y = worldHeight * 0.16f;
    }
    m_verticalOffset = std::min(worldHeight * 0.22f, m_verticalOffset + 7.f * dt);
    if (m_formationSize == 0) {
        return;
    }

    const std::size_t groupCapacity = 6;
    const std::size_t groupCount = (m_formationSize + groupCapacity - 1) / groupCapacity;
    const std::size_t count = std::min(groupCapacity, m_formationSize);
    const std::size_t columns = std::max<std::size_t>(
        1, static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<float>(count)))));
    const std::size_t rows = (count + columns - 1) / columns;
    const std::size_t maxGroupsPerRow = static_cast<std::size_t>(
        std::max(1.f, worldWidth / 190.f));
    const std::size_t groupColumns = std::max<std::size_t>(
        1, std::min(groupCount, maxGroupsPerRow));
    const std::size_t groupRows = (groupCount + groupColumns - 1) / groupColumns;
    const float groupSpacingX = std::min(210.f, worldWidth / static_cast<float>(groupColumns));
    const float groupSpacingY = 145.f;
    float halfWidth = 0.f;
    float halfHeight = 0.f;
    if (m_pattern == Pattern::RotatingCircle || m_pattern == Pattern::ConcentricRings) {
        halfWidth = std::min(worldWidth * 0.38f, std::max(65.f, static_cast<float>(count) * 10.f));
        halfHeight = halfWidth * 0.68f;
    } else if (m_pattern == Pattern::DoubleHelix) {
        halfWidth = std::min(worldWidth * 0.42f, std::max(65.f, static_cast<float>(count) * 10.f));
        halfHeight = std::min(worldHeight * 0.24f, 100.f);
    } else if (m_pattern == Pattern::VFormation) {
        halfWidth = static_cast<float>(count - 1) * 26.f * 0.5f;
        halfHeight = halfWidth * 0.65f + 18.f;
    } else if (m_pattern == Pattern::Lemniscate || m_pattern == Pattern::RadialAccordion) {
        halfWidth = std::min(worldWidth * 0.38f, std::max(75.f, static_cast<float>(columns) * 36.f));
        halfHeight = std::min(worldHeight * 0.25f, std::max(80.f, static_cast<float>(rows) * 42.f));
    } else if (m_pattern == Pattern::DiagonalCross) {
        halfWidth = std::min(worldWidth * 0.35f, 130.f);
        halfHeight = std::min(worldHeight * 0.22f, 130.f);
    } else {
        halfWidth = static_cast<float>(columns - 1) * 60.f * 0.5f;
        halfHeight = static_cast<float>(rows - 1) * 52.f * 0.5f + 24.f;
    }

    const float groupSpeed = 85.f;
    if (m_pattern != Pattern::RotatingCircle && m_pattern != Pattern::ConcentricRings
        && m_pattern != Pattern::DoubleHelix) {
        m_center.x += m_direction * groupSpeed * dt;
    } else {
        m_center.x += m_direction * groupSpeed * 0.75f * dt;
        m_center.y = worldHeight * 0.16f + m_verticalOffset
            + std::sin(m_elapsed * 0.9f) * 45.f;
    }
    const float groupSpread = static_cast<float>(groupColumns - 1) * groupSpacingX * 0.5f;
    const float centerMargin = std::min(worldWidth * 0.5f, halfWidth + groupSpread + 26.f);
    const float centerLeftBound = centerMargin;
    const float centerRightBound = std::max(centerLeftBound, worldWidth - centerMargin);
    if (m_center.x >= centerRightBound) {
        m_center.x = centerRightBound;
        m_direction = -1.f;
    } else if (m_center.x <= centerLeftBound) {
        m_center.x = centerLeftBound;
        m_direction = 1.f;
    }
    if (m_pattern == Pattern::Lemniscate) {
        const float xAmplitude = std::max(0.f, std::min(worldWidth * 0.40f,
            worldWidth * 0.5f - halfWidth - groupSpread - 26.f));
        const float yAmplitude = std::max(0.f, std::min(95.f, 270.f - halfHeight));
        m_center.x = worldWidth * 0.5f + std::sin(m_elapsed * 0.65f) * xAmplitude;
        m_center.y = worldHeight * 0.20f + m_verticalOffset
            + std::sin(m_elapsed * 1.3f) * yAmplitude;
    } else if (m_pattern == Pattern::DiagonalCross) {
        const float yAmplitude = std::max(0.f, std::min(110.f, 295.f - halfHeight));
        m_center.y = worldHeight * 0.28f + m_verticalOffset
            + std::sin(m_elapsed * 0.55f) * yAmplitude;
    } else if (m_pattern == Pattern::DoubleHelix) {
        m_center.y = worldHeight * 0.18f + m_verticalOffset
            + std::sin(m_elapsed * 0.55f) * 35.f;
    }

    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive() || enemy.isBoss() || enemy.hasReboundBehavior()
            || enemy.isSniper() || !enemy.hasFormationSlot()) {
            continue;
        }

        const std::size_t slot = enemy.getFormationSlot();
        if (slot >= m_formationSize) {
            continue;
        }
        const std::size_t groupIndex = slot / groupCapacity;
        const std::size_t groupStart = groupIndex * groupCapacity;
        const std::size_t groupSize = std::min(groupCapacity, m_formationSize - groupStart);
        const std::size_t localIndex = slot % groupCapacity;
        const std::size_t localColumns = std::max<std::size_t>(
            1, static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<float>(groupSize)))));
        const std::size_t localRows = (groupSize + localColumns - 1) / localColumns;
        const float index = static_cast<float>(localIndex);
        const float column = static_cast<float>(localIndex % localColumns);
        const float row = static_cast<float>(localIndex / localColumns);
        const std::size_t groupColumn = groupIndex % groupColumns;
        const std::size_t groupRow = groupIndex / groupColumns;
        const float groupOffsetX = (static_cast<float>(groupColumn)
            - static_cast<float>(groupColumns - 1) * 0.5f) * groupSpacingX;
        const float groupOffsetY = static_cast<float>(groupRow) * groupSpacingY;
        sf::Vector2f groupCenter(m_center.x + groupOffsetX, m_center.y + groupOffsetY);
        groupCenter.x = std::max(halfWidth + 26.f,
            std::min(worldWidth - halfWidth - 26.f, groupCenter.x));
        groupCenter.y = std::max(halfHeight + 30.f,
            std::min(worldHeight - halfHeight - 30.f, groupCenter.y));
        sf::Vector2f offset;
        sf::Vector2f position;
        switch (m_pattern) {
            case Pattern::RotatingCircle: {
                const float angle = m_elapsed * 0.8f
                    + index * 2.f * kPi / static_cast<float>(groupSize);
                offset = sf::Vector2f(std::cos(angle) * halfWidth,
                                      std::sin(angle) * halfHeight);
                break;
            }
            case Pattern::ConcentricRings: {
                const float ring = ((localIndex + 1) % 2 == 0) ? 1.f : 0.52f;
                const float direction = ((localIndex + 1) % 2 == 0) ? 1.f : -1.f;
                const float angle = direction * m_elapsed * 0.9f
                    + index * 2.f * kPi / static_cast<float>(groupSize);
                offset = sf::Vector2f(std::cos(angle) * halfWidth * ring,
                                      std::sin(angle) * halfHeight * ring);
                break;
            }
            case Pattern::DoubleHelix: {
                const float normalized = (groupSize > 1)
                    ? index / static_cast<float>(groupSize - 1) : 0.5f;
                const float x = (normalized - 0.5f) * 2.f * halfWidth;
                const float phase = ((localIndex + 1) % 2 == 0) ? 0.f : kPi;
                offset = sf::Vector2f(x,
                    std::sin(m_elapsed * 1.6f + normalized * 8.f + phase) * halfHeight);
                break;
            }
            case Pattern::OscillatingColumns: {
                const float phase = (column < 1.f) ? 0.f : kPi;
                offset = sf::Vector2f((column - static_cast<float>(localColumns - 1) * 0.5f) * 60.f,
                                      (row - static_cast<float>(localRows - 1) * 0.5f) * 52.f
                                      + std::sin(m_elapsed * 2.f + phase) * 28.f);
                break;
            }
            case Pattern::VFormation: {
                const float vOffset = index - (static_cast<float>(groupSize) - 1.f) * 0.5f;
                const float y = std::abs(vOffset) * 30.f + std::sin(m_elapsed * 1.4f) * 18.f;
                position = sf::Vector2f(groupCenter.x + vOffset * 26.f, groupCenter.y + y);
                break;
            }
            case Pattern::SineSweep:
                offset = sf::Vector2f((column - static_cast<float>(localColumns - 1) * 0.5f) * 60.f,
                                      (row - static_cast<float>(localRows - 1) * 0.5f) * 52.f
                                      + std::sin(m_elapsed * 2.1f + index * 0.7f) * 55.f);
                break;
            case Pattern::Lemniscate: {
                const float phase = index * 2.f * kPi / static_cast<float>(groupSize);
                offset = sf::Vector2f(std::sin(phase) * 65.f,
                                      std::sin(phase * 2.f) * 70.f);
                break;
            }
            case Pattern::RadialAccordion: {
                const float scale = 0.65f + 0.35f * (0.5f + 0.5f * std::sin(m_elapsed * 1.7f));
                offset = sf::Vector2f(
                    (column - static_cast<float>(localColumns - 1) * 0.5f) * 58.f * scale,
                    (row - static_cast<float>(localRows - 1) * 0.5f) * 46.f * scale);
                break;
            }
            case Pattern::DiagonalCross: {
                const float normalized = (groupSize > 1)
                    ? index / static_cast<float>(groupSize - 1) : 0.5f;
                const float diagonal = ((localIndex + 1) % 2 == 0) ? 1.f : -1.f;
                offset = sf::Vector2f((normalized - 0.5f) * 120.f,
                                      diagonal * (normalized - 0.5f) * 90.f
                                      + (row - static_cast<float>(localRows - 1) * 0.5f) * 28.f);
                break;
            }
            default:
                offset = sf::Vector2f((column - static_cast<float>(localColumns - 1) * 0.5f) * 48.f,
                                      (row - static_cast<float>(localRows - 1) * 0.5f) * 42.f
                                      + std::sin(m_elapsed * 2.1f + index * 0.7f) * 55.f);
                break;
        }

        if (m_pattern != Pattern::VFormation) {
            position = groupCenter + offset;
        }
        enemy.setPosition(position);
        enemy.setVelocity(sf::Vector2f(0.f, 0.f));
    }
}
