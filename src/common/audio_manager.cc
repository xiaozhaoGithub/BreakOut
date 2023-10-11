#include "audio_manager.h"

AudioManager::AudioManager()
{
    sound_effect_ = std::make_unique<QSoundEffect>();
    sound_effect_->setVolume(0.25f);
}

void AudioManager::Play(const char* file, int loop_count)
{
    sound_effect_->setLoopCount(loop_count);
    sound_effect_->setSource(QUrl::fromLocalFile(file));
    sound_effect_->play();
}

void AudioManager::Stop()
{
    sound_effect_->stop();
}
