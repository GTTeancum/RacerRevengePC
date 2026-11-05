#include "../../platform/linux/native/original_application_aural50kg.h"
#include "../../platform/linux/native/original_ui_garage_audio_backing50kg.h"

std::size_t auralFirstBackingSize() {
    return sizeof(rr::platform::OriginalUiGarageAudioBacking50KG);
}
std::size_t auralFirstMainSize() {
    return sizeof(rr::platform::OriginalUiMainAudio50KG);
}
