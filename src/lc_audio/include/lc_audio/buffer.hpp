#pragma once

#include "common.hpp"

namespace lc_audio
{
    class Buffer : public ID
    {
    public:
        Buffer();
        ~Buffer();

        void update(const std::vector<float> &bufferData, size_t channels, size_t sampleRate);
        static void updateID(unsigned int bid, const std::vector<float> &bufferData, size_t channels, size_t sampleRate);

    private:
        static std::vector<int16_t> sampleData;
    };
}