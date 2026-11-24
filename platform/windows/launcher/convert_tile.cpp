#include "original_ipu_decoder.h"
#include <cstring>

extern "C" __declspec(dllexport) int decodeTile(const unsigned char* data, size_t size,
                                                unsigned width, unsigned height, unsigned char* pixels) {
    std::vector<unsigned char> rgba;
    std::string message;
    if (!rr::assets::decodeOriginalIpuRgba(data, size, width, height, rgba, message)) return 0;
    std::memcpy(pixels, rgba.data(), rgba.size());
    return 1;
}
