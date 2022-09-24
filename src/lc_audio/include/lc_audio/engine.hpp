#pragma once

#include "buffer.hpp"
#include "source.hpp"
#include "file.hpp"

#include <chrono>

namespace lc_audio
{
    class Engine
    {
    public:
        Engine(size_t numBuffers, size_t bufferSize, size_t numChannels, size_t sampleRate, double playFadeInDuration, double pauseFadeOutDuration, const std::string &deviceName = "");
        virtual ~Engine();

        virtual void update() {}

        void play();
        void pause();
        const bool &isPlaying() const;

        virtual void setVolume(float volume);
        virtual const float &getVolume() const;

        virtual void setPitch(float pitch);
        virtual const float &getPitch() const;

        static double getCurrentTime();

        const size_t NUM_BUFFERS, BUFFER_SIZE, NUM_CHANNELS, SAMPLE_RATE;
        const double PLAY_FADE_IN_DURATION, PAUSE_FADE_OUT_DURATION;

    protected:
        std::vector<float> bufferData;
        virtual void updateBufferData() {}

        void startThread();
        void stopThread();

    private:
        ALCdevice *device{nullptr};
        ALCcontext *context{nullptr};

        enum THREAD_ACTION_TYPE
        {
            NONE,
            PLAY,
            PAUSE
        };
        struct ThreadAction
        {
            THREAD_ACTION_TYPE type{NONE};
            double time{0.0}, duration{0.0};
            bool progress{false};
        };

        float volume{1.0f}, pitch{1.0f};
        bool threadVolumeChange{false}, threadPitchChange{false};

        ThreadAction nextThreadAction;
        bool b_isPlaying{false};

        std::unique_ptr<std::thread> thread;
        bool runThread;
        static void thread_func(Engine *instance);
    };
}