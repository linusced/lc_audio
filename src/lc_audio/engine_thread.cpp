#include "lc_audio/engine.hpp"

void lc_audio::Engine::thread_func(Engine *instance)
{
    lc_audio::Source src;
    src.setGain(instance->volume);
    src.setPitch(instance->pitch);

    std::vector<Buffer> buffers(instance->NUM_BUFFERS);

    for (auto &b : buffers)
    {
        instance->updateBufferData();
        b.update(instance->bufferData, instance->NUM_CHANNELS, instance->SAMPLE_RATE);
        src.queueBuffer(b.getID());
    }

    while (instance->runThread)
    {
        if (src.isPlaying() && src.isBufferProcessed())
        {
            unsigned int bid = src.unqueueProcessedBuffer();
            instance->updateBufferData();
            Buffer::updateID(bid, instance->bufferData, instance->NUM_CHANNELS, instance->SAMPLE_RATE);
            src.queueBuffer(bid);
        }

        if (instance->nextThreadAction.type == NONE && instance->threadVolumeChange)
        {
            src.setGain(instance->volume);
            instance->threadVolumeChange = false;
        }

        if (instance->threadPitchChange)
        {
            src.setGain(instance->pitch);
            instance->threadPitchChange = false;
        }

        switch (instance->nextThreadAction.type)
        {
        case THREAD_ACTION_TYPE::PLAY:
        {
            if (!instance->nextThreadAction.progress)
            {
                src.play();
                instance->b_isPlaying = true;
                instance->nextThreadAction.progress = true;
            }

            float gain = (getCurrentTime() - instance->nextThreadAction.time) / instance->nextThreadAction.duration;
            if (gain > 1.0f)
                gain = 1.0f;

            src.setGain(gain * instance->volume);
            instance->threadVolumeChange = false;

            if (gain == 1.0f)
            {
                instance->nextThreadAction.type = THREAD_ACTION_TYPE::NONE;
                instance->nextThreadAction.progress = false;
            }
        }
        break;
        case THREAD_ACTION_TYPE::PAUSE:
        {
            if (!instance->nextThreadAction.progress)
            {
                instance->b_isPlaying = false;
                instance->nextThreadAction.progress = true;
            }

            float gain = 1.0 - ((getCurrentTime() - instance->nextThreadAction.time) / instance->nextThreadAction.duration);
            src.setGain(gain * instance->volume);
            instance->threadVolumeChange = false;

            if (gain <= 0.0f)
            {
                src.stop();
                instance->nextThreadAction.type = THREAD_ACTION_TYPE::NONE;
                instance->nextThreadAction.progress = false;
            }
        }
        break;
        }
    }
}