#pragma once

#include "common.hpp"

namespace lc_audio
{
    class File
    {
    public:
        File(const std::vector<float> &audioData, unsigned int numChannels, unsigned int sampleRate);
        File(const std::string &filePath);

        void setNumChannels(unsigned int x);
        void setSampleRate(unsigned int x);
        void normalize(float targetRMS);
        void pitchChange(double pitch);

        size_t getSize() const;
        const unsigned int &getNumChannels() const;
        const unsigned int &getSampleRate() const;
        const double &getDuration() const;

        std::vector<float> &getData(size_t index, size_t size, bool *endOfFile);
        const std::vector<float> &getFullBuffer() const;

        static void write(const std::string &filePath, const std::vector<float> &audioData, unsigned int numChannels);

    private:
        std::vector<float> audioData, audioDataSlice;
        unsigned int numChannels, sampleRate;
        double duration;
    };
}