#include "lc_audio/source.hpp"

lc_audio::Source::Source()
{
    alGenSources(1, &id);
}
lc_audio::Source::~Source()
{
    stop();
    alDeleteSources(1, &id);
}

void lc_audio::Source::queueBuffers(const std::vector<unsigned int> &buffers)
{
    alSourceQueueBuffers(id, buffers.size(), buffers.data());
    numQueuedBuffers += buffers.size();
}
void lc_audio::Source::queueBuffer(unsigned int bufferID)
{
    alSourceQueueBuffers(id, 1, &bufferID);
    numQueuedBuffers += 1;
}
std::vector<unsigned int> lc_audio::Source::unqueueBuffers(int size)
{
    if (numQueuedBuffers == 0)
        return {};

    std::vector<unsigned int> result(size);
    alSourceUnqueueBuffers(id, size, result.data());
    numQueuedBuffers -= size;
    return result;
}
void lc_audio::Source::unqueueAllBuffers()
{
    if (numQueuedBuffers == 0)
        return;

    std::vector<unsigned int> tempBufferIDs(numQueuedBuffers);
    alSourceUnqueueBuffers(id, numQueuedBuffers, tempBufferIDs.data());
    numQueuedBuffers = 0;
}

unsigned int lc_audio::Source::unqueueProcessedBuffer()
{
    if (numQueuedBuffers == 0)
        return 0;

    unsigned int result;
    alSourceUnqueueBuffers(id, 1, &result);
    numQueuedBuffers -= 1;
    return result;
}

bool lc_audio::Source::isBufferProcessed() const
{
    int result;
    alGetSourcei(id, AL_BUFFERS_PROCESSED, &result);
    return result != 0;
}

void lc_audio::Source::play()
{
    alSourcePlay(id);
}
void lc_audio::Source::stop()
{
    alSourceStop(id);
}
bool lc_audio::Source::isPlaying() const
{
    ALenum result;
    alGetSourcei(id, AL_SOURCE_STATE, &result);
    return result == AL_PLAYING;
}

void lc_audio::Source::setGain(float gain)
{
    this->gain = gain;
    alSourcef(id, AL_GAIN, gain);
}
const float &lc_audio::Source::getGain() const
{
    return gain;
}

void lc_audio::Source::setPitch(float pitch)
{
    this->pitch = pitch;
    alSourcef(id, AL_PITCH, pitch);
}
const float &lc_audio::Source::getPitch() const
{
    return pitch;
}