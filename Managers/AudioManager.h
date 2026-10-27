#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class AudioManager {
public:
    static AudioManager& instance();

    void setMasterVolume(int volume);
    int getMasterVolume() const;
    void setMusicVolume(int volume);
    int getMusicVolume() const;
    void setSfxVolume(int volume);
    int getSfxVolume() const;

    void setMusicEnabled(bool enabled);
    void setSfxEnabled(bool enabled);
    bool isMusicEnabled() const;
    bool isSfxEnabled() const;

    void playMusic(const std::string& fileName, bool loop = true);
    void playSfx(const std::string& fileName);

private:
    AudioManager();
    void preloadAssets();
    bool loadBuffer(const std::string& fileName, std::shared_ptr<sf::SoundBuffer>& buffer);
    void updateVolumes();
    int m_masterVolume;
    int m_musicVolume;
    int m_sfxVolume;
    bool m_musicEnabled;
    bool m_sfxEnabled;
    std::string m_lastTrack;
    sf::Sound m_music;
    std::map<std::string, std::shared_ptr<sf::SoundBuffer> > m_buffers;
    std::vector<std::unique_ptr<sf::Sound> > m_sfxPool;
    std::vector<std::string> m_sfxChannelFiles;
    std::size_t m_nextChannel;
};

#endif
