#include "AudioManager.h"

#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

namespace {
    int clampVolume(int value) {
        if (value < 0) return 0;
        if (value > 100) return 100;
        return value;
    }

    std::vector<std::string> resourceCandidates(const std::string& fileName) {
        std::vector<std::string> candidates;
        candidates.push_back("Resources/" + fileName);
        candidates.push_back("resources/" + fileName);
        candidates.push_back(fileName);
        return candidates;
    }

    bool loadBufferFromResources(sf::SoundBuffer& buffer, const std::string& fileName) {
        const std::vector<std::string> candidates = resourceCandidates(fileName);
        for (size_t i = 0; i < candidates.size(); ++i) {
            if (buffer.loadFromFile(candidates[i])) {
                return true;
            }
        }
        return false;
    }

    float sfxMultiplier(const std::string& fileName) {
        if (fileName == "shoot.wav") {
            return 0.06f;
        }
        if (fileName == "collision.wav" || fileName == "collision_en.wav") {
            return 0.06f;
        }
        return 1.f;
    }
}

AudioManager::AudioManager()
    : m_masterVolume(100)
    , m_musicVolume(100)
    , m_sfxVolume(100)
    , m_musicEnabled(true)
    , m_sfxEnabled(true)
    , m_lastTrack("")
    , m_nextChannel(0)
{
    for (int i = 0; i < 16; ++i) {
        m_sfxPool.push_back(std::unique_ptr<sf::Sound>(new sf::Sound()));
    }
    m_sfxChannelFiles.resize(m_sfxPool.size());
    preloadAssets();
    updateVolumes();
}

AudioManager& AudioManager::instance() {
    static AudioManager manager;
    return manager;
}

void AudioManager::setMasterVolume(int volume) {
    m_masterVolume = clampVolume(volume);
    updateVolumes();
}

int AudioManager::getMasterVolume() const {
    return m_masterVolume;
}

void AudioManager::setMusicVolume(int volume) {
    m_musicVolume = clampVolume(volume);
    updateVolumes();
}

int AudioManager::getMusicVolume() const {
    return m_musicVolume;
}

void AudioManager::setSfxVolume(int volume) {
    m_sfxVolume = clampVolume(volume);
    updateVolumes();
}

int AudioManager::getSfxVolume() const {
    return m_sfxVolume;
}

void AudioManager::setMusicEnabled(bool enabled) {
    m_musicEnabled = enabled;
    if (!m_musicEnabled) {
        m_music.stop();
    }
}

void AudioManager::setSfxEnabled(bool enabled) {
    m_sfxEnabled = enabled;
    if (!m_sfxEnabled) {
        for (size_t i = 0; i < m_sfxPool.size(); ++i) {
            m_sfxPool[i]->stop();
        }
    }
}

bool AudioManager::isMusicEnabled() const {
    return m_musicEnabled;
}

bool AudioManager::isSfxEnabled() const {
    return m_sfxEnabled;
}

void AudioManager::playMusic(const std::string& fileName, bool loop) {
    if (!m_musicEnabled) {
        return;
    }

    const std::map<std::string, std::shared_ptr<sf::SoundBuffer> >::const_iterator it =
        m_buffers.find(fileName);
    if (it == m_buffers.end()) {
        std::cerr << "Audio no precargado: " << fileName << ". Se omite.\n";
        m_lastTrack = fileName;
        return;
    }

    m_music.setBuffer(*it->second);
    m_music.setLoop(loop);
    m_music.play();
    m_lastTrack = fileName;
}

void AudioManager::playSfx(const std::string& fileName) {
    if (!m_sfxEnabled) {
        return;
    }

    const std::map<std::string, std::shared_ptr<sf::SoundBuffer> >::const_iterator it =
        m_buffers.find(fileName);
    if (it == m_buffers.end()) {
        return;
    }

    sf::Sound& channel = *m_sfxPool[m_nextChannel];
    const std::size_t channelIndex = m_nextChannel;
    m_nextChannel = (m_nextChannel + 1) % m_sfxPool.size();
    channel.setBuffer(*it->second);
    m_sfxChannelFiles[channelIndex] = fileName;
    const float masterSfx = (static_cast<float>(m_sfxVolume) / 100.f)
        * static_cast<float>(m_masterVolume);
    channel.setVolume(masterSfx * sfxMultiplier(fileName));
    channel.play();
}

bool AudioManager::loadBuffer(const std::string& fileName,
                              std::shared_ptr<sf::SoundBuffer>& buffer) {
    buffer.reset(new sf::SoundBuffer());
    if (!loadBufferFromResources(*buffer, fileName)) {
        buffer.reset();
        std::cerr << "Audio no encontrado durante la precarga: " << fileName << "\n";
        return false;
    }
    m_buffers[fileName] = buffer;
    return true;
}

void AudioManager::preloadAssets() {
    const char* files[] = {
        "shoot.wav", "collision.wav", "collision_en.wav", "selecting.wav",
        "menu_music.ogg", "wave_music.ogg", "boss_music.ogg", "pause_music.ogg"
    };
    const size_t count = sizeof(files) / sizeof(files[0]);
    for (size_t i = 0; i < count; ++i) {
        std::shared_ptr<sf::SoundBuffer> buffer;
        loadBuffer(files[i], buffer);
    }
}

void AudioManager::updateVolumes() {
    const float master = static_cast<float>(m_masterVolume);
    m_music.setVolume((static_cast<float>(m_musicVolume) / 100.f) * master);
    const float sfx = (static_cast<float>(m_sfxVolume) / 100.f) * master;
    for (size_t i = 0; i < m_sfxPool.size(); ++i) {
        m_sfxPool[i]->setVolume(sfx * sfxMultiplier(m_sfxChannelFiles[i]));
    }
}
