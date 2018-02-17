#include <fstream>

namespace de { namespace ahans {

class WavReader
{
public:
#pragma pack(push, 1)
    struct WavHeader
    {
        unsigned char riff[4];              // RIFF string
        unsigned int overall_size;          // overall size of file in bytes
        unsigned char wave[4];              // WAVE string
        unsigned char fmt_chunk_marker[4];  // fmt string with trailing null char
        unsigned int length_of_fmt;         // length of the format data
        unsigned char format_type[2];       // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
        unsigned char channels[2];          // no.of channels
        unsigned int sample_rate;           // sampling rate (blocks per second)
        unsigned int byterate;              // SampleRate * NumChannels * BitsPerSample/8
        unsigned char block_align[2];       // NumChannels * BitsPerSample/8
        unsigned short bits_per_sample;     // bits per sample, 8- 8bits, 16- 16 bits etc
        unsigned char data_chunk_header[4]; // DATA string or FLLR string
        unsigned int data_size;             // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
    };
#pragma pack(pop)

public:
    WavReader();
    ~WavReader();

    bool open(const char* filename);
    void close();
    size_t read(char* buf, size_t max_size);
    const WavHeader& getHeader() const;

private:
    std::ifstream file_;
    WavHeader header_;
};

} } // namespace
