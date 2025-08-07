
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



// M4A/AAC support - using alternative approach without mp4v2
// We'll use a simplified version that works with basic M4A files
#include <fstream>
#include <map>

#include "../util/MusicPlayer.h"


int main(int argc, char* argv[]) {
    std::string musicFile;
    
    if (argc > 1) {
        musicFile = argv[1];
    } else {
        std::cout << "Enter music file path (relative): ";
        std::getline(std::cin, musicFile);
    }
    
    if (musicFile.empty()) {
        musicFile = "./music.m4a"; // Default to M4A file
        std::cout << "Using default path: " << musicFile << std::endl;
    }
    
    MusicPlayer player;
    
    if (!player.initialize()) {
        std::cerr << "Failed to initialize OpenAL" << std::endl;
        return -1;
    }
    
    if (!player.loadMusic(musicFile)) {
        std::cerr << "Failed to load music file: " << musicFile << std::endl;
        return -1;
    }
    
    player.play();
    
    return 0;
}