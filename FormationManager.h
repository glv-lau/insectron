#ifndef FORMATIONMANAGER_H
#define FORMATIONMANAGER_H

#include "Entities/Enemy.h"
#include <cstddef>
#include <vector>

class FormationManager {
public:
    enum class Pattern {
        RotatingCircle,
        OscillatingColumns,
        VFormation,
        SineSweep,
        Lemniscate,
        RadialAccordion,
        DiagonalCross,
        ConcentricRings,
        DoubleHelix
    };

    FormationManager();

    void selectPattern(int wave, std::size_t enemyCount);
    void update(std::vector<Enemy>& enemies, float dt, float worldWidth, float worldHeight);
    Pattern getPattern() const { return m_pattern; }

private:
    Pattern m_pattern;
    float m_elapsed;
    sf::Vector2f m_center;
    float m_direction;
    float m_verticalOffset;
    std::size_t m_formationSize;
};

#endif
