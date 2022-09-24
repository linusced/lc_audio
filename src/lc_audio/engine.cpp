#include "lc_audio/engine.hpp"

lc_audio::Engine::Engine(size_t numBuffers, size_t bufferSize, size_t numChannels, size_t sampleRate, double playFadeInDuration, double pauseFadeOutDuration, const std::string &deviceName)
    : NUM_BUFFERS(numBuffers), BUFFER_SIZE(bufferSize), NUM_CHANNELS(numChannels), SAMPLE_RATE(sampleRate), PLAY_FADE_IN_DURATION(playFadeInDuration), PAUSE_FADE_OUT_DURATION(pauseFadeOutDuration)
{
    device = alcOpenDevice(deviceName.empty() ? nullptr : deviceName.data());
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    bufferData.resize(BUFFER_SIZE);
}
lc_audio::Engine::~Engine()
{
    if (thread)
    {
        runThread = false;
        thread->join();
    }

    alcDestroyContext(context);
    alcCloseDevice(device);
}

void lc_audio::Engine::play()
{
    if (!b_isPlaying)
    {
        nextThreadAction.type = THREAD_ACTION_TYPE::PLAY;
        nextThreadAction.time = getCurrentTime();
        nextThreadAction.duration = PLAY_FADE_IN_DURATION;
        nextThreadAction.progress = false;
    }
}
void lc_audio::Engine::pause()
{
    if (thread && b_isPlaying)
    {
        nextThreadAction.type = THREAD_ACTION_TYPE::PAUSE;
        nextThreadAction.time = getCurrentTime();
        nextThreadAction.duration = PAUSE_FADE_OUT_DURATION;
        nextThreadAction.progress = false;
    }
    else
        nextThreadAction.type = THREAD_ACTION_TYPE::NONE;
}
const bool &lc_audio::Engine::isPlaying() const
{
    return b_isPlaying;
}

void lc_audio::Engine::setVolume(float volume)
{
    this->volume = std::clamp(volume, 0.0f, 1.0f);

    if (thread)
        threadVolumeChange = true;
}
const float &lc_audio::Engine::getVolume() const
{
    return volume;
}

void lc_audio::Engine::setPitch(float pitch)
{
    this->pitch = std::clamp(pitch, 0.5f, 1.5f);

    if (thread)
        threadPitchChange = true;
}
const float &lc_audio::Engine::getPitch() const
{
    return pitch;
}

void lc_audio::Engine::startThread()
{
    if (!thread)
    {
        if (nextThreadAction.type != THREAD_ACTION_TYPE::NONE)
            nextThreadAction.time = getCurrentTime();

        runThread = true;
        thread = std::make_unique<std::thread>(thread_func, this);
    }
}
void lc_audio::Engine::stopThread()
{
    if (thread)
    {
        runThread = false;
        thread->join();
        thread.reset();
    }
}

double lc_audio::Engine::getCurrentTime()
{
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
}