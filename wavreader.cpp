#include "wavreader.h"

namespace de { namespace ahans {

bool is_big_endian()
{
    const int deadbeef = 0xd3adb33f;
    return reinterpret_cast<const char*>(&deadbeef)[0] == 0xd3;
}

template <typename T>
T swap_endianess(T v)
{
    // unsigned char* pv = reinterpret_cast<unsigned char*>(&v);
    // std::reverse(pv, pv + sizeof(T));
    return v;
}

WavReader::WavReader() = default;

WavReader::~WavReader() = default;

bool WavReader::open(const char* filename)
{
    if (file_) close();

    file_ = SD.open(filename);
    if (!file_) return false;
    
    // read header
    size_t numRead = file_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (   numRead != sizeof(header_)
        || (header_.riff[0] != 'R' || header_.riff[1] != 'I' || header_.riff[2] != 'F' || header_.riff[3] != 'F')
        || (header_.wave[0] != 'W' || header_.wave[1] != 'A' || header_.wave[2] != 'V' || header_.wave[3] != 'E')
        ) {
        close();
        return false;
    }
    if (header_.format_type[0] != 1) {
        // std::cerr << "unsupported format type: " << header_.format_type << std::endl;
        close();
        return false;
    }
    if (header_.channels[0] != 1) {
        // std::cerr << "unsupported number of channels: " << header_.channels[0] << std::endl;
        close();
        return false;
    }
    // fix endianess if necessary
    if (is_big_endian()) {
        header_.overall_size = swap_endianess(header_.overall_size);
        header_.sample_rate = swap_endianess(header_.sample_rate);
        header_.bits_per_sample = swap_endianess(header_.bits_per_sample);
    }

    // std::cout << "overall size of file: " << header_.overall_size << std::endl;

    if (header_.sample_rate < 8000 || header_.sample_rate > 44100) {
        // std::cerr << "sample rate unsupported" << header_.sample_rate << std::endl;
        close();
        return false;
    }

    if (header_.bits_per_sample != 16) {
        // std::cout << "bits per sample unsupported: " << header_.bits_per_sample << std::endl;
        close();
        return false;
    }

    if (header_.data_chunk_header[0] != 'd' || header_.data_chunk_header[1] != 'a' || header_.data_chunk_header[2] != 't' || header_.data_chunk_header[3] != 'a') {
        // std::cerr << "expected 'data' header not found" << std::endl;
        close();
        return false;
    }

    return true;
}

void WavReader::close()
{
    file_.close();
}

size_t WavReader::read(char* buf, size_t max_size)
{
    return file_.readBytes(buf, max_size);
}

const WavReader::WavHeader& WavReader::getHeader() const
{
    return header_;
}

size_t WavReader::numRemainingBytes()
{
    return file_.size() - file_.position();
}

} }

