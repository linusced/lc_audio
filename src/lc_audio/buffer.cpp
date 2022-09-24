#include "lc_audio/buffer.hpp"

std::vector<int16_t> lc_audio::Buffer::sampleData;

lc_audio::Buffer::Buffer()
{
    alGenBuffers(1, &id);
}
lc_audio::Buffer::~Buffer()
{
    alDeleteBuffers(1, &id);
}

void lc_audio::Buffer::update(const std::vector<float> &bufferData, size_t channels, size_t sampleRate)
{
    updateID(id, bufferData, channels, sampleRate);
}

void lc_audio::Buffer::updateID(unsigned int bid, const std::vector<float> &bufferData, size_t channels, size_t sampleRate)
{
    size_t size = bufferData.size();
    sampleData.resize(size);

    for (size_t i = 0; i < size; i++)
        sampleData[i] = sampleToInt16(bufferData[i]);

    alBufferData(bid, (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, sampleData.data(), sampleData.size() * sizeof(int16_t), sampleRate);
}