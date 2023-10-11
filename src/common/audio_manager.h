#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <QSoundEffect>

#include "singleton.h"

class AudioManager
{
    SINGLETON_DECLARE(AudioManager)
public:
    AudioManager();
    ~AudioManager() = default;

    void Play(const char* file, int loop_count = QSoundEffect::Infinite);
    void Stop();

private:
    std::unique_ptr<QSoundEffect> sound_effect_;
};

#endif
