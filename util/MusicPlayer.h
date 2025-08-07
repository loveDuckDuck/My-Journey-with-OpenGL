

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H


#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <complex>
#include "SimpleFFT.h"

// Minimp3 header-only library for MP3 support
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

// M4A/AAC support - using alternative approach without mp4v2
// We'll use a simplified version that works with basic M4A files
#include <fstream>
#include <map>

struct AudioAnalysis
{
    double bassLevel;                   // 20-250 Hz
    double midLevel;                    // 250-4000 Hz
    double trebleLevel;                 // 4000-20000 Hz
    double overallRMS;                  // Root Mean Square (volume)
    double peakLevel;                   // Peak amplitude
    double dynamicRange;                // Difference between peak and average
    std::vector<double> frequencyBands; // 10 frequency bands
};

class MusicPlayer
{
private:
    ALCdevice *device;
    ALCcontext *context;
    ALuint source;
    ALuint buffer;

    // For non-MP3 files
    SNDFILE *audioFile;
    SF_INFO audioInfo;

    // For MP3 files
    mp3dec_t mp3Decoder;
    mp3dec_frame_info_t mp3FrameInfo;
    std::ifstream mp3FileStream;
    std::vector<uint8_t> mp3Buffer;
    bool isMp3File;

    // For M4A files - simplified approach
    std::vector<uint8_t> m4aBuffer;
    bool isM4aFile;

    std::vector<short> audioData;
    std::chrono::steady_clock::time_point startTime;
    bool isInitialized;

    // Audio properties (unified for both formats)
    int sampleRate;
    int channels;
    int totalFrames;
    double duration;
    std::string formatName;

    // Audio analysis
    AudioAnalysis currentAnalysis;
    std::vector<double> analysisBuffer;
    size_t analysisBufferSize;
    size_t currentPlayPosition;

public:
    MusicPlayer() : device(nullptr), context(nullptr), source(0), buffer(0),
                    audioFile(nullptr), isMp3File(false), isM4aFile(false),
                    isInitialized(false), sampleRate(0), channels(0), totalFrames(0), duration(0.0),
                    analysisBufferSize(2048), currentPlayPosition(0)
    {
        memset(&audioInfo, 0, sizeof(audioInfo));
        memset(&mp3FrameInfo, 0, sizeof(mp3FrameInfo));
        memset(&currentAnalysis, 0, sizeof(currentAnalysis));
    }

    ~MusicPlayer()
    {
        cleanup();
    }

    bool initialize()
    {
        // Initialize OpenAL
        device = alcOpenDevice(nullptr);
        if (!device)
        {
            std::cerr << "Error: Could not open OpenAL device" << std::endl;
            return false;
        }

        context = alcCreateContext(device, nullptr);
        if (!context)
        {
            std::cerr << "Error: Could not create OpenAL context" << std::endl;
            return false;
        }

        alcMakeContextCurrent(context);

        // Generate source and buffer
        alGenSources(1, &source);
        alGenBuffers(1, &buffer);

        if (alGetError() != AL_NO_ERROR)
        {
            std::cerr << "Error: Failed to generate OpenAL objects" << std::endl;
            return false;
        }

        isInitialized = true;
        return true;
    }

    AudioAnalysis analyzeAudioSegment(size_t startSample, size_t numSamples)
    {
        AudioAnalysis analysis;
        memset(&analysis, 0, sizeof(analysis));

        if (startSample >= audioData.size() || numSamples == 0)
        {
            return analysis;
        }

        // Ensure we don't go beyond audio data
        numSamples = std::min(numSamples, audioData.size() - startSample);

        // Convert to mono if stereo for analysis
        std::vector<double> monoData;
        for (size_t i = startSample; i < startSample + numSamples; i += channels)
        {
            if (channels == 1)
            {
                monoData.push_back(audioData[i] / 32768.0); // Normalize to [-1, 1]
            }
            else
            {
                // Average stereo channels
                double sample = audioData[i] / 32768.0;
                if (i + 1 < audioData.size())
                {
                    sample = (sample + audioData[i + 1] / 32768.0) / 2.0;
                }
                monoData.push_back(sample);
            }
        }

        if (monoData.empty())
            return analysis;

        // Calculate RMS (volume level)
        double sumSquares = 0.0;
        double peak = 0.0;
        for (double sample : monoData)
        {
            sumSquares += sample * sample;
            peak = std::max(peak, std::abs(sample));
        }
        analysis.overallRMS = sqrt(sumSquares / monoData.size());
        analysis.peakLevel = peak;
        analysis.dynamicRange = peak - analysis.overallRMS;

        // Pad to power of 2 for FFT
        size_t fftSize = 1;
        while (fftSize < monoData.size())
            fftSize *= 2;

        std::vector<std::complex<double>> fftData(fftSize);
        for (size_t i = 0; i < monoData.size(); i++)
        {
            fftData[i] = std::complex<double>(monoData[i], 0.0);
        }

        // Perform FFT
        SimpleFFT::fft(fftData);
        std::vector<double> magnitudes = SimpleFFT::getMagnitudes(fftData);

        // Calculate frequency bins
        double binSize = (double)sampleRate / fftSize;

        // Define frequency ranges
        int bassEnd = (int)(250.0 / binSize);
        int midEnd = (int)(4000.0 / binSize);
        int trebleEnd = std::min((int)(20000.0 / binSize), (int)(magnitudes.size() / 2));

        // Calculate bass level (20-250 Hz)
        double bassSum = 0.0;
        for (int i = 1; i < bassEnd && i < magnitudes.size() / 2; i++)
        {
            bassSum += magnitudes[i];
        }
        analysis.bassLevel = bassSum / std::max(1, bassEnd - 1);

        // Calculate mid level (250-4000 Hz)
        double midSum = 0.0;
        for (int i = bassEnd; i < midEnd && i < magnitudes.size() / 2; i++)
        {
            midSum += magnitudes[i];
        }
        analysis.midLevel = midSum / std::max(1, midEnd - bassEnd);

        // Calculate treble level (4000-20000 Hz)
        double trebleSum = 0.0;
        for (int i = midEnd; i < trebleEnd && i < magnitudes.size() / 2; i++)
        {
            trebleSum += magnitudes[i];
        }
        analysis.trebleLevel = trebleSum / std::max(1, trebleEnd - midEnd);

        // Calculate 10 frequency bands for detailed analysis
        analysis.frequencyBands.resize(10);
        int bandsPerBin = (magnitudes.size() / 2) / 10;
        for (int band = 0; band < 10; band++)
        {
            double bandSum = 0.0;
            int startBin = band * bandsPerBin + 1; // Skip DC component
            int endBin = std::min((band + 1) * bandsPerBin, (int)(magnitudes.size() / 2));

            for (int i = startBin; i < endBin; i++)
            {
                bandSum += magnitudes[i];
            }
            analysis.frequencyBands[band] = bandSum / std::max(1, endBin - startBin);
        }

        return analysis;
    }

    void printAudioAnalysis(const AudioAnalysis &analysis)
    {
        std::cout << "\n=== AUDIO ANALYSIS ===" << std::endl;
        std::cout << "Bass Level (20-250Hz): " << std::fixed << std::setprecision(3)
                  << analysis.bassLevel << std::endl;
        std::cout << "Mid Level (250-4kHz): " << analysis.midLevel << std::endl;
        std::cout << "Treble Level (4k-20kHz): " << analysis.trebleLevel << std::endl;
        std::cout << "Overall RMS: " << analysis.overallRMS << std::endl;
        std::cout << "Peak Level: " << analysis.peakLevel << std::endl;
        std::cout << "Dynamic Range: " << analysis.dynamicRange << std::endl;

        // Visual frequency bands display
        std::cout << "\nFrequency Bands (10 bands): " << std::endl;
        for (int i = 0; i < analysis.frequencyBands.size(); i++)
        {
            double normalized = std::min(analysis.frequencyBands[i] * 100, 50.0); // Scale for display
            int barLength = (int)(normalized);
            std::cout << "Band " << std::setw(2) << (i + 1) << ": ";
            for (int j = 0; j < barLength && j < 50; j++)
            {
                std::cout << "█";
            }
            std::cout << " (" << std::setprecision(3) << analysis.frequencyBands[i] << ")" << std::endl;
        }
        std::cout << "======================" << std::endl;
    }

    bool isMP3File(const std::string &filePath)
    {
        std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension == "mp3";
    }

    bool isM4AFile(const std::string &filePath)
    {
        std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension == "m4a" || extension == "aac" || extension == "mp4";
    }

    bool loadM4A(const std::string &filePath)
    {
        std::cerr << "M4A support currently requires additional libraries (mp4v2/faad2)." << std::endl;
        std::cerr << "For M4A files, please convert to MP3, WAV, or FLAC format." << std::endl;
        std::cerr << "You can convert using: ffmpeg -i input.m4a output.wav" << std::endl;
        return false;

        // Note: Full M4A support would require:
        // 1. MP4 container parsing (mp4v2 or custom parser)
        // 2. AAC audio decoding (faad2 or custom decoder)
        // This simplified version suggests conversion instead
    }
    bool loadMP3(const std::string &filePath)
    {
        // Open MP3 file
        mp3FileStream.open(filePath, std::ios::binary);
        if (!mp3FileStream.is_open())
        {
            std::cerr << "Error: Could not open MP3 file: " << filePath << std::endl;
            return false;
        }

        // Read entire file into buffer
        mp3FileStream.seekg(0, std::ios::end);
        size_t fileSize = mp3FileStream.tellg();
        mp3FileStream.seekg(0, std::ios::beg);

        mp3Buffer.resize(fileSize);
        mp3FileStream.read(reinterpret_cast<char *>(mp3Buffer.data()), fileSize);
        mp3FileStream.close();

        // Initialize MP3 decoder
        mp3dec_init(&mp3Decoder);

        // Decode MP3 data
        audioData.clear();
        size_t offset = 0;
        short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
        int samples = 0;
        int totalSamples = 0;
        bool firstFrame = true;

        while (offset < mp3Buffer.size())
        {
            samples = mp3dec_decode_frame(&mp3Decoder, mp3Buffer.data() + offset,
                                          mp3Buffer.size() - offset, pcm, &mp3FrameInfo);

            if (samples == 0)
            {
                // Skip non-audio data
                offset += 1;
                continue;
            }

            if (firstFrame)
            {
                // Set audio properties from first frame
                sampleRate = mp3FrameInfo.hz;
                channels = mp3FrameInfo.channels;
                formatName = "MP3";
                firstFrame = false;
            }

            // Add decoded samples to audio data
            for (int i = 0; i < samples * channels; i++)
            {
                audioData.push_back(pcm[i]);
            }

            totalSamples += samples;
            offset += mp3FrameInfo.frame_bytes;
        }

        totalFrames = totalSamples;
        duration = (double)totalFrames / sampleRate;

        if (audioData.empty())
        {
            std::cerr << "Error: No audio data decoded from MP3 file" << std::endl;
            return false;
        }

        return true;
    }

    bool loadOtherFormat(const std::string &filePath)
    {
        // Open audio file with libsndfile
        audioFile = sf_open(filePath.c_str(), SFM_READ, &audioInfo);
        if (!audioFile)
        {
            std::cerr << "Error: Could not open file: " << filePath << std::endl;
            std::cerr << "libsndfile error: " << sf_strerror(nullptr) << std::endl;
            return false;
        }

        // Set unified audio properties
        sampleRate = audioInfo.samplerate;
        channels = audioInfo.channels;
        totalFrames = audioInfo.frames;
        duration = (double)totalFrames / sampleRate;

        // Determine format name
        switch (audioInfo.format & SF_FORMAT_TYPEMASK)
        {
        case SF_FORMAT_WAV:
            formatName = "WAV";
            break;
        case SF_FORMAT_FLAC:
            formatName = "FLAC";
            break;
        case SF_FORMAT_OGG:
            formatName = "OGG";
            break;
        case SF_FORMAT_AIFF:
            formatName = "AIFF";
            break;
        case SF_FORMAT_AU:
            formatName = "AU";
            break;
        default:
            formatName = "Unknown";
            break;
        }

        // Read audio data
        audioData.resize(totalFrames * channels);
        sf_count_t readFrames = sf_read_short(audioFile, audioData.data(), audioData.size());

        if (readFrames != totalFrames * channels)
        {
            std::cerr << "Warning: Read " << readFrames << " samples, expected "
                      << totalFrames * channels << std::endl;
        }

        return true;
    }

    bool loadMusic(const std::string &filePath)
    {
        if (!isInitialized)
        {
            std::cerr << "Error: Player not initialized" << std::endl;
            return false;
        }

        // Check file format
        isMp3File = isMP3File(filePath);
        isM4aFile = isM4AFile(filePath);

        bool loadSuccess = false;
        if (isMp3File)
        {
            loadSuccess = loadMP3(filePath);
        }
        else if (isM4aFile)
        {
            loadSuccess = loadM4A(filePath);
        }
        else
        {
            loadSuccess = loadOtherFormat(filePath);
        }

        if (!loadSuccess)
        {
            return false;
        }

        // Print file information
        printMusicInfo(filePath);

        // Perform initial audio analysis on first few seconds
        size_t analysisLength = std::min((size_t)(sampleRate * channels * 3), audioData.size()); // 3 seconds
        currentAnalysis = analyzeAudioSegment(0, analysisLength);
        printAudioAnalysis(currentAnalysis);

        // Determine OpenAL format
        ALenum format;
        if (channels == 1)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (channels == 2)
        {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            std::cerr << "Error: Unsupported channel count: " << channels << std::endl;
            return false;
        }

        // Upload audio data to OpenAL buffer
        alBufferData(buffer, format, audioData.data(),
                     audioData.size() * sizeof(short), sampleRate);

        if (alGetError() != AL_NO_ERROR)
        {
            std::cerr << "Error: Failed to upload audio data to buffer" << std::endl;
            return false;
        }

        // Attach buffer to source
        alSourcei(source, AL_BUFFER, buffer);

        return true;
    }

    void printMusicInfo(const std::string &filePath)
    {
        std::cout << "\n=== MUSIC FILE INFORMATION ===" << std::endl;
        std::cout << "File: " << filePath << std::endl;
        std::cout << "Format: " << formatName << std::endl;
        std::cout << "Sample Rate: " << sampleRate << " Hz" << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Frames: " << totalFrames << std::endl;
        std::cout << "Duration: " << std::fixed << std::setprecision(2)
                  << duration << " seconds" << std::endl;

        if (isMp3File)
        {
            std::cout << "MP3 Info: " << audioData.size() << " total samples decoded" << std::endl;
        }

        std::cout << "==============================\n"
                  << std::endl;
    }

    void play()
    {
        if (!isInitialized)
        {
            std::cerr << "Error: Player not initialized" << std::endl;
            return;
        }

        alSourcePlay(source);
        startTime = std::chrono::steady_clock::now();

        std::cout << "Playing music... Press Ctrl+C to stop\n"
                  << std::endl;

        // Runtime information loop
        while (true)
        {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);

            if (state != AL_PLAYING)
            {
                std::cout << "\nPlayback finished!" << std::endl;
                break;
            }

            printRuntimeInfo();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void

    void printRuntimeInfo()
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           currentTime - startTime)
                           .count() /
                       1000.0;

        double progress = (elapsed / duration) * 100.0;

        // Get current playback position
        ALfloat offsetSeconds;
        alGetSourcef(source, AL_SEC_OFFSET, &offsetSeconds);

        // Update current play position for analysis
        currentPlayPosition = (size_t)(offsetSeconds * sampleRate * channels);

        // Analyze current audio segment (1 second window)
        size_t windowSize = sampleRate * channels; // 1 second
        if (currentPlayPosition + windowSize < audioData.size())
        {
            AudioAnalysis realTimeAnalysis = analyzeAudioSegment(currentPlayPosition, windowSize);

            // Clear screen and show runtime info with audio analysis
            std::cout << "\r\033[K"; // Clear current line
            std::cout << "[Runtime] " << std::fixed << std::setprecision(1)
                      << elapsed << "s/" << duration << "s (" << progress << "%) "
                      << "[" << formatName << "] "
                      << "Bass:" << std::setprecision(2) << realTimeAnalysis.bassLevel
                      << " Mid:" << realTimeAnalysis.midLevel
                      << " Treble:" << realTimeAnalysis.trebleLevel
                      << " RMS:" << realTimeAnalysis.overallRMS
                      << std::flush;
        }
        else
        {
            std::cout << "\r[Runtime] Elapsed: " << std::fixed << std::setprecision(1)
                      << elapsed << "s / " << duration << "s "
                      << "(" << std::setprecision(1) << progress << "%) "
                      << "[Position: " << std::setprecision(1) << offsetSeconds << "s] "
                      << "[Format: " << formatName << "] "
                      << std::flush;
        }


        
    }

    void cleanup()
    {
        if (source)
        {
            alSourceStop(source);
            alDeleteSources(1, &source);
            source = 0;
        }

        if (buffer)
        {
            alDeleteBuffers(1, &buffer);
            buffer = 0;
        }

        if (audioFile)
        {
            sf_close(audioFile);
            audioFile = nullptr;
        }

        if (isMp3File)
        {
            mp3Buffer.clear();
        }

        if (isM4aFile)
        {
            m4aBuffer.clear();
        }

        if (context)
        {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
            context = nullptr;
        }

        if (device)
        {
            alcCloseDevice(device);
            device = nullptr;
        }

        isInitialized = false;
    }
};
#endif