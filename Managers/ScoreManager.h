#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>
#include <vector>

class ScoreManager {
public:
    struct ScoreEntry {
        std::string initials;
        int score;
    };

    static ScoreManager& instance();

    void loadHighScores(const std::string& path = "scores.txt");
    void saveHighScores(const std::string& path = "scores.txt");
    void registerScore(int score, const std::string& player = "AAA");
    std::vector<int> getTopScores() const;
    std::vector<ScoreEntry> getEntries() const;
    int getBestScore() const;

private:
    ScoreManager();
    std::vector<ScoreEntry> m_scores;
    std::string m_path;
    std::string normalizeInitials(const std::string& input) const;
};

#endif
