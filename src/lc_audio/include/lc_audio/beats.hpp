#pragma once

#include "filters.hpp"

namespace lc_audio
{
    class Beats
    {
    public:
        Beats(unsigned int bpm, double beatOffset);
        Beats(const std::vector<float> &audioBuffer, unsigned int numChannels, unsigned int sampleRate);

        const unsigned int &getBPM() const;
        const double &getBeatInterval() const;
        const double &getBeatOffset() const;

        void setBPM(unsigned int newBPM);
        void setBeatOffset(double newBeatOffset);

    private:
        unsigned int bpm;
        double beatInterval, beatOffset;
    };
}