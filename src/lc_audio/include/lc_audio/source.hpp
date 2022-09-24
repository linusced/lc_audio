#pragma once

#include "common.hpp"

namespace lc_audio
{
    class Source : public ID
    {
    public:
        Source();
        ~Source();

        void queueBuffers(const std::vector<unsigned int> &buffers);
        void queueBuffer(unsigned int bufferID);
        std::vector<unsigned int> unqueueBuffers(int numBuffers);
        unsigned int unqueueProcessedBuffer();
        void unqueueAllBuffers();

        bool isBufferProcessed() const;

        void play();
        void stop();
        bool isPlaying() const;

        void setGain(float gain);
        const float &getGain() const;

        void setPitch(float pitch);
        const float &getPitch() const;

    private:
        int numQueuedBuffers{0};
        float gain{1.0f}, pitch{1.0f};
    };
}