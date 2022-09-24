#pragma once

#include "common.hpp"

namespace lc_audio
{
    float filter_a1(float frequency, unsigned int sampleRate);
    float filter_out(float f, float a1, float dn1);
    float filter_dn1(float f, float a1, float out);
    float filter_lp(float f, float out);
    float filter_hp(float f, float out);

    void lowPassFilter(std::vector<float> &buffer, unsigned int numChannels, unsigned int sampleRate, float lowPassFrequency, float *value = nullptr);

    void highPassFilter(std::vector<float> &buffer, unsigned int numChannels, unsigned int sampleRate, float highPassFrequency, float *value = nullptr);
};