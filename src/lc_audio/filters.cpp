#include "lc_audio/filters.hpp"

float lc_audio::filter_a1(float frequency, unsigned int sampleRate)
{
    constexpr float f_pi = (float)M_PI;
    float _tan = std::tan(f_pi * frequency / sampleRate);
    return (_tan - 1.0f) / (_tan + 1.0f);
}
float lc_audio::filter_out(float f, float a1, float dn1)
{
    return a1 * f + dn1;
}
float lc_audio::filter_dn1(float f, float a1, float out)
{
    return f - a1 * out;
}
float lc_audio::filter_lp(float f, float out)
{
    return (f + out) / 2.0f;
}
float lc_audio::filter_hp(float f, float out)
{
    return (f + out * -1.0f) / 2.0f;
}

void lc_audio::lowPassFilter(std::vector<float> &buffer, unsigned int numChannels, unsigned int sampleRate, float lowPassFrequency, float *value)
{
    const size_t numSamplesPerChannel = buffer.size() / numChannels;
    const float a1 = filter_a1(lowPassFrequency, sampleRate);

    float out, dn1;

    for (size_t c = 0, i; c < numChannels; c++)
    {
        dn1 = value ? value[c] : 0.0f;

        for (i = 0; i < numSamplesPerChannel; i++)
        {
            float &f = buffer[i * numChannels + c];
            out = filter_out(f, a1, dn1);
            dn1 = filter_dn1(f, a1, out);
            f = filter_lp(f, out);
        }

        if (value)
            value[c] = dn1;
    }
}

void lc_audio::highPassFilter(std::vector<float> &buffer, unsigned int numChannels, unsigned int sampleRate, float highPassFrequency, float *value)
{
    const size_t numSamplesPerChannel = buffer.size() / numChannels;
    const float a1 = filter_a1(highPassFrequency, sampleRate);

    float out, dn1;

    for (size_t c = 0, i; c < numChannels; c++)
    {
        dn1 = value ? value[c] : 0.0f;

        for (i = 0; i < numSamplesPerChannel; i++)
        {
            float &f = buffer[i * numChannels + c];
            out = filter_out(f, a1, dn1);
            dn1 = filter_dn1(f, a1, out);
            f = filter_hp(f, out);
        }

        if (value)
            value[c] = dn1;
    }
}