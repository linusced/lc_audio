#include "lc_audio/beats.hpp"

const unsigned int &lc_audio::Beats::getBPM() const
{
    return bpm;
}
const double &lc_audio::Beats::getBeatInterval() const
{
    return beatInterval;
}
const double &lc_audio::Beats::getBeatOffset() const
{
    return beatOffset;
}

void lc_audio::Beats::setBPM(unsigned int newBPM)
{
    bpm = newBPM;
    beatInterval = 60.0 / bpm;
}
void lc_audio::Beats::setBeatOffset(double newBeatOffset)
{
    beatOffset = newBeatOffset;
}

lc_audio::Beats::Beats(unsigned int bpm, double beatOffset)
    : bpm(bpm), beatInterval(60.0 / bpm), beatOffset(beatOffset) {}

lc_audio::Beats::Beats(const std::vector<float> &audioBuffer, unsigned int numChannels, unsigned int sampleRate)
{
    const size_t samplesPerChannel = audioBuffer.size() / numChannels;

    std::vector<float> lowPassedMonoBuffer(samplesPerChannel);
    for (size_t i = 0; i < samplesPerChannel; i++)
        lowPassedMonoBuffer[i] = audioBuffer[i * numChannels];

    lowPassFilter(lowPassedMonoBuffer, 1, sampleRate, 100.0f);

    constexpr size_t BLOCK_SIZE = 1024, BLOCKS_PER_WINDOW = 64, WINDOW_SIZE = BLOCK_SIZE * BLOCKS_PER_WINDOW;
    constexpr double MIN_PEAK_DISTANCE = 60.0 / 150.0;
    constexpr float VARIANCE_ENERGY_MULT = -0.0000015f, VARIANCE_ENERGY_OFFSET = 1.5142857f;

    const size_t size = lowPassedMonoBuffer.size() - WINDOW_SIZE;
    const double d_sampleRate = sampleRate;
    const float f_sampleRate = sampleRate;

    std::vector<double> peaks;
    peaks.reserve(std::pow(2, 13));
    std::vector<float> blocks(BLOCKS_PER_WINDOW);

    float averageEnergy, varianceEnergy, peakThreshold;
    double time, startSilenceDuration = -1.0;

    for (size_t w = 0, b, i; w < size; w += WINDOW_SIZE)
    {
        averageEnergy = 0.0f;
        for (b = 0; b < BLOCKS_PER_WINDOW; b++)
        {
            blocks[b] = 0.0f;
            for (i = 0; i < BLOCK_SIZE; i++)
                blocks[b] += std::pow(lowPassedMonoBuffer[w + b * BLOCK_SIZE + i], 2.0f);

            if (startSilenceDuration == -1.0 && blocks[b] > 0.0f)
                startSilenceDuration = (w + b * BLOCK_SIZE) / d_sampleRate;

            averageEnergy += blocks[b];
        }

        averageEnergy /= BLOCKS_PER_WINDOW;
        varianceEnergy = 0.0f;

        for (b = 0; b < BLOCKS_PER_WINDOW; b++)
            varianceEnergy += std::pow(averageEnergy - blocks[b], 2.0f);

        varianceEnergy /= BLOCKS_PER_WINDOW;
        varianceEnergy = VARIANCE_ENERGY_MULT * varianceEnergy + VARIANCE_ENERGY_OFFSET;
        peakThreshold = varianceEnergy * averageEnergy;

        for (b = 0; b < BLOCKS_PER_WINDOW; b++)
        {
            time = (w + b * BLOCK_SIZE) / d_sampleRate;
            if ((peaks.empty() || time - peaks.back() > MIN_PEAK_DISTANCE) && blocks[b] > peakThreshold)
                peaks.push_back(time);
        }
    }

    size_t peakIndex, peakSampleIndex;
    float peakSampleValue, value;

    for (auto &p : peaks)
    {
        peakIndex = p * d_sampleRate;
        peakSampleIndex = peakIndex;
        peakSampleValue = std::abs(lowPassedMonoBuffer[peakIndex]);
        for (size_t i = 1; i < BLOCK_SIZE; i++)
        {
            value = std::abs(lowPassedMonoBuffer[peakIndex + i]);
            if (value > peakSampleValue)
            {
                peakSampleValue = value;
                peakSampleIndex = peakIndex + i;
            }
        }

        p = peakSampleIndex / d_sampleRate;
    }

    std::vector<unsigned int> bpmValues;
    std::vector<double> bpmConsecutivePeakTimes;

    for (int numConsecutivePeaks = 8; numConsecutivePeaks > 2; numConsecutivePeaks /= 2)
    {
        size_t maxPeak = peaks.size() - numConsecutivePeaks;
        double peakDiff, newPeakDiff, averagePeakDiff, consecutivePeakTime;

        for (size_t i = 0, j; i < maxPeak; i++)
        {
            consecutivePeakTime = peaks[i];
            peakDiff = peaks[i + 1] - peaks[i];

            for (j = 2; j < numConsecutivePeaks; j++)
                if (std::abs((peaks[i + j] - peaks[i + j - 1]) - peakDiff) > 0.01)
                {
                    peakDiff = 0.0;
                    break;
                }

            if (peakDiff != 0.0)
            {
                bpmValues.push_back(std::round(60.0 / peakDiff));
                bpmConsecutivePeakTimes.push_back(consecutivePeakTime);
                i += numConsecutivePeaks;
            }
        }
    }

    if (bpmValues.empty())
    {
        bpm = beatInterval = beatOffset = 0.0;
        return;
    }

    std::unordered_map<unsigned int, unsigned int> bpmValuesCount;
    for (auto &b : bpmValues)
        bpmValuesCount[b]++;

    unsigned int highestBpmCount = 0;
    for (auto &[b, c] : bpmValuesCount)
        if (c > highestBpmCount)
        {
            bpm = b;
            highestBpmCount = c;
        }

    beatInterval = 60.0 / bpm;
    beatOffset = -1.0;

    for (size_t i = 0; i < bpmValues.size(); i++)
        if (bpmValues[i] == bpm && (beatOffset == -1.0 || bpmConsecutivePeakTimes[i] < beatOffset))
            beatOffset = bpmConsecutivePeakTimes[i];

    beatOffset += beatInterval;
    size_t beatOffsetSample = beatOffset * d_sampleRate,
           beatOffsetStep = BLOCK_SIZE / 4;

    float prevValue, prevAverageValue = 0.0f;
    for (size_t i = beatOffsetSample - BLOCK_SIZE; i >= beatOffsetSample - BLOCK_SIZE * 2; i--)
        prevAverageValue += std::pow(lowPassedMonoBuffer[i], 2.0f);

    prevAverageValue /= BLOCK_SIZE;
    value = -1.0f;

    do
    {
        prevValue = value;
        value = 0.0f;
        for (size_t i = 0; i < beatOffsetStep; i++)
            value += std::pow(lowPassedMonoBuffer[beatOffsetSample - i], 2.0f);

        value /= beatOffsetStep;
        beatOffsetSample -= beatOffsetStep;
    } while (prevValue == -1.0f || (value < prevValue && value > prevAverageValue));

    beatOffset = beatOffsetSample / d_sampleRate;

    for (double t = beatOffset; t > 0.0; t -= beatInterval)
        if (t - beatInterval < startSilenceDuration)
        {
            beatOffset = t;
            break;
        }

    if (std::abs(beatInterval - std::fmod(beatOffset, beatInterval)) < 0.01)
        beatOffset = std::floor(beatInterval / beatOffset) * beatInterval;

    if (beatOffset < 0.005)
        beatOffset = 0.0;
}