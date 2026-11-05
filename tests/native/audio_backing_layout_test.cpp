// Link with audio_backing_layout_peer.cpp. Header order must not change ABI.
#include "../../platform/linux/native/original_ui_garage_audio_backing50kg.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include <cstdio>

std::size_t auralFirstBackingSize();
std::size_t auralFirstMainSize();
int main() {
    const auto backing = sizeof(rr::platform::OriginalUiGarageAudioBacking50KG);
    const auto main = sizeof(rr::platform::OriginalUiMainAudio50KG);
    const auto peerBacking = auralFirstBackingSize();
    const auto peerMain = auralFirstMainSize();
    std::printf("backing-first backing=%zu main=%zu; aural-first backing=%zu main=%zu\n",
                backing, main, peerBacking, peerMain);
    return backing == peerBacking && main == peerMain ? 0 : 1;
}
