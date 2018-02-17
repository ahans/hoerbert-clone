#include "wavreader.h"

#include <iostream>

using de::ahans::WavReader;

int main(int argc, char** argv)
{
    WavReader wavReader;
    if (!wavReader.open(argv[1])) return 1;

    short buf[512];
    size_t count = 0;
    while (true) {
        size_t bytes_read = wavReader.read(reinterpret_cast<char*>(buf), sizeof(buf));
        if (bytes_read <= 0) break;
        count += bytes_read;
    }
    std::cout << "sizeof(header) == " << sizeof(WavReader::WavHeader) << std::endl;
    std::cout << "sample rate: " << wavReader.getHeader().sample_rate << std::endl;
    std::cout << "channels: " << static_cast<int>(wavReader.getHeader().channels[0]) << std::endl;
    std::cout << "bits per channel: " << wavReader.getHeader().bits_per_sample << std::endl;
    std::cout << "bytes read: " << count << std::endl;
}
