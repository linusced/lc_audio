#include "lc_audio/file.hpp"

lc_audio::File::File(const std::vector<float> &audioData, unsigned int numChannels, unsigned int sampleRate)
    : audioData(audioData), numChannels(numChannels), sampleRate(sampleRate), duration((audioData.size() / numChannels) / (double)sampleRate) {}

lc_audio::File::File(const std::string &filePath)
{
    std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
    if (extension == "wav" || extension == "aif")
    {
        AudioFile<float> file;
        if (!file.load(filePath))
            throw std::invalid_argument("lc_audio::File::load Failed to load lc_audio file \"" + filePath + '\"');

        numChannels = file.getNumChannels();
        sampleRate = file.getSampleRate();

        size_t numSamplesPerChannel = file.getNumSamplesPerChannel();
        duration = numSamplesPerChannel / (double)sampleRate;

        audioData.resize(numSamplesPerChannel * numChannels);

        for (size_t i = 0, c; i < numSamplesPerChannel; i++)
            for (c = 0; c < numChannels; c++)
                audioData[i * numChannels + c] = file.samples[c][i];
    }
    else
        throw std::invalid_argument("lc_audio::File::load Unsupported lc_audio file format \"" + filePath + '\"');
}

void lc_audio::File::setNumChannels(unsigned int x)
{
    if (x == numChannels)
        return;

    size_t numSamplesPerChannel = audioData.size() / numChannels;
    audioDataSlice = audioData;
    audioData.resize(numSamplesPerChannel * x);

    if (x > numChannels)
    {
        for (size_t i = 0, c; i < numSamplesPerChannel; i++)
            for (c = 0; c < x; c++)
                audioData[i * x + c] = audioDataSlice[i * numChannels + (c < numChannels ? c : numChannels - 1)];
    }
    else
    {
        for (size_t i = 0, c; i < numSamplesPerChannel; i++)
            for (c = 0; c < x; c++)
                audioData[i * x + c] = audioDataSlice[i * numChannels + c];
    }

    audioDataSlice.clear();
    numChannels = x;
}

void lc_audio::File::setSampleRate(unsigned int x)
{
    if (x == sampleRate)
        return;

    SRC_DATA sampleData;
    sampleData.src_ratio = x / (double)sampleRate;

    audioDataSlice = audioData;
    audioData.resize(audioData.size() * sampleData.src_ratio);

    sampleData.data_in = audioDataSlice.data();
    sampleData.input_frames = audioDataSlice.size() / numChannels;

    sampleData.data_out = audioData.data();
    sampleData.output_frames = audioData.size() / numChannels;

    int result = src_simple(&sampleData, 1, numChannels);
    if (result != 0)
        throw std::runtime_error("lc_audio::File::setSampleRate Sample rate conversion error: " + std::string(src_strerror(result)) + '\n');

    audioDataSlice.clear();
    sampleRate = x;
}

void lc_audio::File::normalize(float targetRMS)
{
    float s{0.0f};
    for (auto &sample : audioData)
        s += std::pow(sample, 2.0f);

    float a = std::sqrt((audioData.size() * std::pow(dBFSToGain(targetRMS), 2.0f)) / s);

    for (auto &sample : audioData)
        sample = std::clamp(sample * a, -1.0f, 1.0f);
}

void lc_audio::File::pitchChange(double pitch)
{
    audioDataSlice = audioData;
    audioData.resize(audioData.size() / pitch);
    size_t numSamplesPerChannel = audioData.size() / numChannels;

    double index;
    float prevValue, nextValue;
    for (size_t i = 0, c; i < numSamplesPerChannel; i++)
        for (c = 0; c < numChannels; c++)
        {
            index = i * pitch;
            prevValue = audioDataSlice[std::floor(index) * numChannels + c];
            nextValue = audioDataSlice[std::ceil(index) * numChannels + c];

            index -= (int)index;
            audioData[i * numChannels + c] = index * (nextValue - prevValue) + prevValue;
        }

    duration = numSamplesPerChannel / (double)sampleRate;
    audioDataSlice.clear();
}

size_t lc_audio::File::getSize() const
{
    return audioData.size();
}
const unsigned int &lc_audio::File::getNumChannels() const
{
    return numChannels;
}
const unsigned int &lc_audio::File::getSampleRate() const
{
    return sampleRate;
}
const double &lc_audio::File::getDuration() const
{
    return duration;
}

std::vector<float> &lc_audio::File::getData(size_t index, size_t size, bool *endOfFile)
{
    if (endOfFile != nullptr)
        *endOfFile = false;

    audioDataSlice.resize(size);
    size_t audioDataSize = audioData.size();

    for (size_t i = 0; i < size; i++)
        if (i + index < audioDataSize)
        {
            audioDataSlice[i] = audioData[i + index];
        }
        else
        {
            audioDataSlice[i] = 0.0f;
            if (endOfFile != nullptr)
                *endOfFile = true;
        }

    return audioDataSlice;
}

const std::vector<float> &lc_audio::File::getFullBuffer() const
{
    return audioData;
}

void lc_audio::File::write(const std::string &filePath, const std::vector<float> &audioData, unsigned int numChannels)
{
    AudioFileFormat format = AudioFileFormat::Wave;
    if (filePath.substr(filePath.find_last_of('.') + 1) == "aif")
        format = AudioFileFormat::Aiff;

    const size_t bufferSizePerChannels = audioData.size() / numChannels;
    AudioFile<float>::AudioBuffer buffer;
    buffer.resize(numChannels);

    for (size_t c = 0; c < numChannels; c++)
        buffer[c].resize(bufferSizePerChannels);

    for (size_t i = 0, c; i < bufferSizePerChannels; i++)
        for (c = 0; c < numChannels; c++)
            buffer[c][i] = audioData[i * numChannels + c];

    AudioFile<float> file;
    if (!file.setAudioBuffer(buffer))
        throw std::runtime_error("lc_audio::File::write Failed to write file\n");

    file.save(filePath, format);
}