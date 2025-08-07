# OpenAL Music Player with Audio Analysis

A comprehensive C++ music player that uses OpenAL for playback and provides real-time audio analysis including frequency spectrum analysis, dynamic range measurement, and detailed audio characteristics.

## 🎵 Features

- **Multi-Format Support**: MP3, WAV, FLAC, OGG, AIFF, AU
- **Real-time Audio Analysis**: Bass, mid, treble frequency analysis
- **Dynamic Range Analysis**: RMS, peak levels, dynamic range
- **Visual Frequency Display**: 10-band spectrum analyzer with bar charts
- **Runtime Information**: Live playback stats and audio characteristics
- **Cross-platform**: Works on Linux, macOS, and Windows

## 📋 Requirements

### System Dependencies
- **OpenAL**: Audio playback library
- **libsndfile**: Audio file reading (WAV, FLAC, OGG, etc.)
- **C++11 compiler**: GCC, Clang, or MSVC

### Header-only Dependencies
- **minimp3.h**: MP3 decoding (included as header-only)

## 🚀 Installation

### Step 1: Install System Dependencies

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libopenal-dev libsndfile1-dev build-essential
```

#### Fedora/RHEL:
```bash
sudo dnf install openal-soft-devel libsndfile-devel gcc-c++
```

#### macOS (with Homebrew):
```bash
brew install openal-soft libsndfile
```

#### Windows (MSYS2):
```bash
pacman -S mingw-w64-x86_64-openal mingw-w64-x86_64-libsndfile mingw-w64-x86_64-gcc
```

### Step 2: Download minimp3 Header

```bash
# Download the minimp3 header for MP3 support
wget https://raw.githubusercontent.com/lieff/minimp3/master/minimp3.h
```

### Step 3: Compile the Program

```bash
# Compile with all necessary libraries
g++ -o music_player music_player.cpp -lopenal -lsndfile

# If you get compilation errors, try:
g++ -std=c++11 -o music_player music_player.cpp -lopenal -lsndfile
```

## 📁 Project Structure

```
music_player/
├── music_player.cpp      # Main source code
├── minimp3.h            # MP3 decoder header (download required)
├── music_player         # Compiled executable
├── README.md           # This file
└── music/              # Your music files (optional)
    ├── song.mp3
    ├── track.wav
    └── album.flac
```

## 🎮 Usage

### Basic Usage

```bash
# Run with file path as argument
./music_player path/to/your/music.mp3

# Or run interactively (will prompt for file path)
./music_player

# Examples:
./music_player ./music/song.wav
./music_player /home/user/Music/track.flac
./music_player "../music folder/song with spaces.mp3"
```

### Supported File Formats

| Format | Extension | Library Used | Notes |
|--------|-----------|--------------|-------|
| MP3 | `.mp3` | minimp3 | Header-only, no dependencies |
| WAV | `.wav` | libsndfile | Uncompressed, best compatibility |
| FLAC | `.flac` | libsndfile | Lossless compression |
| OGG | `.ogg` | libsndfile | Open source compression |
| AIFF | `.aiff` | libsndfile | Apple format |
| AU | `.au` | libsndfile | Sun/NeXT format |

### M4A/AAC Files
M4A files are not directly supported due to licensing complexity. Convert them first:

```bash
# Convert M4A to WAV (most compatible)
ffmpeg -i input.m4a output.wav

# Convert M4A to MP3 (smaller size)
ffmpeg -i input.m4a -codec:a libmp3lame output.mp3

# Convert M4A to FLAC (lossless)
ffmpeg -i input.m4a output.flac
```

## 📊 Understanding the Output

### File Information Display
When you load a file, you'll see:

```
=== MUSIC FILE INFORMATION ===
File: ./music/song.mp3
Format: MP3                    # Audio format detected
Sample Rate: 44100 Hz          # Samples per second (CD quality = 44100)
Channels: 2                    # 1 = Mono, 2 = Stereo
Frames: 7938000               # Total audio frames
Duration: 180.05 seconds       # Length in seconds
MP3 Info: 15876000 total samples decoded
==============================
```

### Audio Analysis Display
The program performs frequency analysis on the first 3 seconds:

```
=== AUDIO ANALYSIS ===
Bass Level (20-250Hz): 0.045     # Low frequency content (drums, bass guitar)
Mid Level (250-4kHz): 0.032      # Vocal and instrument range
Treble Level (4k-20kHz): 0.018   # High frequency content (cymbals, harmonics)
Overall RMS: 0.234               # Average loudness (see RMS explanation below)
Peak Level: 0.789                # Maximum amplitude reached
Dynamic Range: 0.555             # Difference between peak and average
```

### Frequency Bands Visualization
Shows 10 frequency bands with visual bars:

```
Frequency Bands (10 bands):
Band  1: ████████ (0.045)       # Lowest frequencies
Band  2: ██████ (0.032)         # Low-mid frequencies  
Band  3: ████ (0.028)           # Mid frequencies
...                             # Higher frequencies
Band 10: ██ (0.012)             # Highest frequencies
```

### Runtime Display
During playback, you see real-time analysis:

```
[Runtime] 45.2s/180.5s (25.1%) [MP3] Bass:0.05 Mid:0.03 Treble:0.02 RMS:0.25
```

- **45.2s/180.5s**: Current time / Total duration
- **25.1%**: Playback progress percentage
- **[MP3]**: Audio format
- **Bass/Mid/Treble**: Real-time frequency levels (1-second window)
- **RMS**: Current average loudness

## 🔬 Technical Details

### Audio Analysis Explained

#### RMS (Root Mean Square)
- **What it is**: Mathematical measure of average power/loudness
- **Range**: 0.0 (silent) to 1.0 (maximum)
- **Typical values**:
  - 0.1-0.3: Quiet, dynamic music (classical, acoustic)
  - 0.3-0.6: Normal pop/rock music
  - 0.6-1.0: Loud, compressed music (EDM, modern pop)

#### Peak Level
- **What it is**: Highest amplitude reached in the audio
- **Range**: 0.0 to 1.0
- **Usage**: Prevents clipping, measures headroom

#### Dynamic Range
- **What it is**: Difference between peak and average levels
- **Higher values**: More variation (classical, jazz)
- **Lower values**: More compressed (modern pop, EDM)

#### Frequency Bands
- **Bass (20-250 Hz)**: Kick drums, bass guitar, low fundamentals
- **Mid (250-4000 Hz)**: Vocals, most instruments, presence
- **Treble (4000-20000 Hz)**: Cymbals, harmonics, air, sparkle

### FFT Analysis
The program uses Fast Fourier Transform to convert time-domain audio into frequency-domain data:

1. **Time Domain**: Audio waveform (amplitude over time)
2. **FFT Processing**: Mathematical transformation
3. **Frequency Domain**: Spectrum showing energy at each frequency
4. **Band Calculation**: Sum energy in specific frequency ranges

### Audio Processing Pipeline

```
Audio File → Decoder → PCM Data → FFT Analysis → Frequency Bands → Display
     ↓
OpenAL Buffer → Audio Hardware → Speakers
```

## 🎛️ Code Architecture

### Main Components

#### `MusicPlayer` Class
- **Purpose**: Main controller for audio playback and analysis
- **Key Methods**:
  - `initialize()`: Set up OpenAL context
  - `loadMusic()`: Load and decode audio files
  - `play()`: Start playback and analysis loop
  - `analyzeAudioSegment()`: Perform FFT analysis

#### `SimpleFFT` Class
- **Purpose**: Fast Fourier Transform implementation
- **Methods**:
  - `fft()`: Recursive FFT algorithm
  - `getMagnitudes()`: Convert complex numbers to magnitudes

#### `AudioAnalysis` Struct
- **Purpose**: Store analysis results
- **Fields**: Bass, mid, treble levels, RMS, peak, frequency bands

### File Format Handling

#### MP3 Files (`loadMP3()`)
1. Read entire file into memory buffer
2. Initialize minimp3 decoder
3. Decode frame by frame
4. Extract sample rate, channels from first frame
5. Accumulate PCM data

#### Other Formats (`loadOtherFormat()`)
1. Open file with libsndfile
2. Read audio properties from file header
3. Read all PCM data at once
4. Store in unified format

### Audio Analysis Process

#### Initial Analysis (`analyzeAudioSegment()`)
1. **Mono Conversion**: Average stereo channels if needed
2. **Normalization**: Convert 16-bit samples to [-1, 1] range
3. **RMS Calculation**: √(Σ(sample²)/N)
4. **Peak Detection**: Find maximum absolute value
5. **FFT Preparation**: Pad to power-of-2 size
6. **FFT Execution**: Transform to frequency domain
7. **Band Calculation**: Sum magnitudes in frequency ranges

#### Real-time Analysis
- Analyzes 1-second windows during playback
- Updates bass/mid/treble levels every 500ms
- Tracks current playback position

## 🛠️ Troubleshooting

### Common Compilation Errors

#### "cannot find -lopenal"
```bash
# Install OpenAL development libraries
sudo apt-get install libopenal-dev
```

#### "cannot find -lsndfile"
```bash
# Install libsndfile development libraries
sudo apt-get install libsndfile1-dev
```

#### "minimp3.h: No such file"
```bash
# Download the minimp3 header
wget https://raw.githubusercontent.com/lieff/minimp3/master/minimp3.h
```

#### "undefined reference to 'sf_open'"
```bash
# Link libsndfile properly
g++ -o music_player music_player.cpp -lsndfile -lopenal
```

### Runtime Issues

#### "Could not open OpenAL device"
- **Cause**: Audio system not available
- **Solution**: 
  ```bash
  # Check audio system
  pulseaudio --check -v
  # or restart audio
  sudo systemctl restart pulseaudio
  ```

#### "Could not open file"
- **Cause**: File path incorrect or format unsupported
- **Solution**: Check file exists and is supported format

#### "No audio data decoded"
- **Cause**: Corrupted audio file or unsupported encoding
- **Solution**: Try converting file or use different source

### Performance Issues

#### High CPU Usage
- **Cause**: Real-time FFT analysis
- **Solution**: Reduce analysis frequency by modifying sleep interval

#### Memory Usage
- **Cause**: Loading entire audio file into memory
- **Solution**: For very large files, consider streaming approach

## 🔧 Customization

### Modify Analysis Parameters

#### Change Analysis Window Size
```cpp
// In analyzeAudioSegment(), modify:
size_t windowSize = sampleRate * channels * 2; // 2 seconds instead of 1
```

#### Adjust Frequency Bands
```cpp
// Modify frequency ranges in analyzeAudioSegment():
int bassEnd = (int)(300.0 / binSize);      // Extend bass to 300Hz
int midEnd = (int)(5000.0 / binSize);      // Extend mids to 5kHz
```

#### Change Update Rate
```cpp
// In play() method, modify sleep duration:
std::this_thread::sleep_for(std::chrono::milliseconds(250)); // Update every 250ms
```

### Add New Audio Formats

To support additional formats, modify `loadOtherFormat()` and add format detection:

```cpp
// Add new format detection
case SF_FORMAT_MP3: formatName = "MP3"; break; // If libsndfile supports MP3
```

### Enhance Analysis

#### Add Spectral Centroid
```cpp
double calculateSpectralCentroid(const std::vector<double>& magnitudes) {
    double weightedSum = 0.0, totalSum = 0.0;
    for (size_t i = 0; i < magnitudes.size(); i++) {
        weightedSum += i * magnitudes[i];
        totalSum += magnitudes[i];
    }
    return totalSum > 0 ? weightedSum / totalSum : 0.0;
}
```

## 📈 Future Improvements

### Planned Features
- [ ] Streaming playback for large files
- [ ] Playlist support
- [ ] Real-time equalizer visualization
- [ ] Export analysis data to CSV
- [ ] GUI interface with Qt or GTK
- [ ] Network streaming support
- [ ] Plugin architecture for effects

### Possible Enhancements
- [ ] Beat detection and BPM calculation
- [ ] Spectral analysis (spectral centroid, rolloff)
- [ ] Loudness standards compliance (LUFS)
- [ ] Multi-threading for better performance
- [ ] Configuration file support
- [ ] Keyboard controls during playback

## 📄 License

This project is open source. The minimp3 library is provided under CC0 (public domain).

## 🤝 Contributing

Feel free to submit issues, feature requests, or pull requests to improve this music player.

## 📚 References

- [OpenAL Programming Guide](https://www.openal.org/documentation/)
- [libsndfile Documentation](http://www.mega-nerd.com/libsndfile/)
- [minimp3 GitHub Repository](https://github.com/lieff/minimp3)
- [FFT Algorithm Explanation](https://en.wikipedia.org/wiki/Fast_Fourier_transform)
- [Audio Analysis Fundamentals](https://en.wikipedia.org/wiki/Audio_signal_processing)