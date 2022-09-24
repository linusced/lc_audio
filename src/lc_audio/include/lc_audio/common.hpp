#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <AudioFile.h>
#include <samplerate.h>

namespace lc_audio
{
    class ID
    {
    public:
        const unsigned int &getID() const { return id; }

    protected:
        unsigned int id = -1;
    };

    template <typename T>
    int16_t sampleToInt16(const T &floatingPointSample)
    {
        constexpr T min{-1.0}, max{1.0};
        return std::clamp(floatingPointSample, min, max) * INT16_MAX;
    }
    template <typename T>
    T int16ToSample(const int16_t &intValue)
    {
        return intValue / (T)INT16_MAX;
    }

    template <typename T>
    T dBFSToGain(const T &dBFS)
    {
        constexpr T base{10}, mult{0.05};
        return std::pow(base, dBFS * mult);
    }
    template <typename T>
    T gainTodBFS(const T &gain)
    {
        constexpr T mult{20.0};
        return std::log10(gain) * mult;
    }

    template <typename T>
    T angularVelocity(T frequency)
    {
        return (T)M_PI * (T)2.0L * frequency;
    }

    template <typename T>
    T sineWave(T frequency, T time, T phase)
    {
        return std::sin(angularVelocity(frequency) * time + phase);
    }

    template <typename T>
    T frequency(int octave, int midiNoteID)
    {
        T _1 = 27.5L, _2 = 1.0594630943592953L;
        return (_1 * std::pow((T)2.0L, (T)(octave - 1))) * std::pow(_2, (T)midiNoteID);
    }

    static int midiNoteID(const std::string &note)
    {
        if (note.empty())
            return -1;

        bool isSharp = note.back() == '#';

        switch (std::toupper(note[0]))
        {
        case 'A':
            return isSharp;
        case 'B':
            return 2;
        case 'C':
            return 3 + isSharp;
        case 'D':
            return 5 + isSharp;
        case 'E':
            return 7;
        case 'F':
            return 8 + isSharp;
        case 'G':
            return 10 + isSharp;
        }

        return -1;
    }
}