


#ifndef SIMPLEFFT_H
#define SIMPLEFFT_H


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


// M4A/AAC support - using alternative approach without mp4v2
// We'll use a simplified version that works with basic M4A files
#include <fstream>
#include <map>


// Simple FFT implementation for audio analysis
class SimpleFFT {
public:
    static void fft(std::vector<std::complex<double>>& data) {
        int n = data.size();
        if (n <= 1) return;
        
        // Divide
        std::vector<std::complex<double>> even, odd;
        for (int i = 0; i < n; i++) {
            if (i % 2 == 0) even.push_back(data[i]);
            else odd.push_back(data[i]);
        }
        
        // Conquer
        fft(even);
        fft(odd);
        
        // Combine
        for (int i = 0; i < n/2; i++) {
            std::complex<double> t = std::polar(1.0, -2 * M_PI * i / n) * odd[i];
            data[i] = even[i] + t;
            data[i + n/2] = even[i] - t;
        }
    }
    
    static std::vector<double> getMagnitudes(const std::vector<std::complex<double>>& fftData) {
        std::vector<double> magnitudes;
        for (const auto& c : fftData) {
            magnitudes.push_back(std::abs(c));
        }
        return magnitudes;
    }
};
#endif