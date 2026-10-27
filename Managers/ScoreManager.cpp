#include "ScoreManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

ScoreManager::ScoreManager()
    : m_path("scores.txt")
{
    loadHighScores(m_path);
}

ScoreManager& ScoreManager::instance() {
    static ScoreManager manager;
    return manager;
}

std::string ScoreManager::normalizeInitials(const std::string& input) const {
    std::string out;
    for (size_t i = 0; i < input.size() && out.size() < 3; ++i) {
        const unsigned char ch = static_cast<unsigned char>(input[i]);
        if (std::isalpha(ch)) {
            out.push_back(static_cast<char>(std::toupper(ch)));
        }
    }
    while (out.size() < 3) {
        out.push_back('A');
    }
    return out.substr(0, 3);
}

void ScoreManager::loadHighScores(const std::string& path) {
    m_path = path;
    m_scores.clear();

    std::ifstream input(path.c_str());
    if (!input.is_open()) {
        return;
    }

    std::string initials;
    int score = 0;
    while (input >> initials >> score) {
        ScoreEntry entry;
        entry.initials = normalizeInitials(initials);
        entry.score = score;
        m_scores.push_back(entry);
    }
    std::sort(m_scores.begin(), m_scores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });
}

void ScoreManager::saveHighScores(const std::string& path) {
    std::ofstream output(path.c_str(), std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "No se pudo guardar el archivo de puntajes: " << path << std::endl;
        return;
    }

    for (size_t i = 0; i < m_scores.size() && i < 10; ++i) {
        output << m_scores[i].initials << " " << m_scores[i].score << '\n';
    }
}

void ScoreManager::registerScore(int score, const std::string& player) {
    if (score <= 0) {
        return;
    }

    ScoreEntry entry;
    entry.initials = normalizeInitials(player);
    entry.score = score;
    m_scores.push_back(entry);
    std::sort(m_scores.begin(), m_scores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });
    if (m_scores.size() > 10) {
        m_scores.resize(10);
    }
    saveHighScores(m_path);
}

std::vector<int> ScoreManager::getTopScores() const {
    std::vector<int> top;
    for (size_t i = 0; i < m_scores.size(); ++i) {
        top.push_back(m_scores[i].score);
    }
    return top;
}

std::vector<ScoreManager::ScoreEntry> ScoreManager::getEntries() const {
    return m_scores;
}

int ScoreManager::getBestScore() const {
    if (m_scores.empty()) {
        return 0;
    }
    return m_scores.front().score;
}
