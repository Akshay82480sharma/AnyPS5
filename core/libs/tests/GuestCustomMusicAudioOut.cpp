#include "prx/libSceCustomMusicAudioOut/include/Api.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace {

constexpr int ERROR_INVALID_PARAM = static_cast<int>(0x81380001);
constexpr int ERROR_NOT_INITIALIZED = static_cast<int>(0x81380102);

void Require(bool value) {
    if (!value) {
        std::abort();
    }
}

void TestUninitialized() {
    double vol = 0.0;
    bool muted = false;
    std::uint64_t time = 0;

    Require(sceCustomMusicAudioOutFinalize() == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutSetVolume(0.5) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutSetAllMusicVolume(0.5) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutGetAllMusicVolume(&vol) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutSetMuted(true) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutGetMuted(&muted) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutOutput(nullptr) == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutGetLastOutputTime(&time) == ERROR_NOT_INITIALIZED);
}

void TestNullPointers() {
    Require(sceCustomMusicAudioOutGetVolume(nullptr) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutGetAllMusicVolume(nullptr) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutGetMuted(nullptr) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutGetLastOutputTime(nullptr) == ERROR_INVALID_PARAM);
}

void TestInitializeValidation() {
    Require(sceCustomMusicAudioOutInitialize(256, 2, 1) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutInitialize(256, -1, 1) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutInitialize(256, 0, 0) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutInitialize(256, 0, 2) == ERROR_INVALID_PARAM);
    Require(sceCustomMusicAudioOutInitialize(256, 0, 5) == ERROR_INVALID_PARAM);
}

void TestVolumeAndMuteLifecycle() {
    Require(sceCustomMusicAudioOutInitialize(256, 0, 1) == 0);

    double vol = 0.0;
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(std::abs(vol - 0.5) < 0.001);

    Require(sceCustomMusicAudioOutGetAllMusicVolume(&vol) == 0);
    Require(std::abs(vol - 0.5) < 0.001);

    bool muted = true;
    Require(sceCustomMusicAudioOutGetMuted(&muted) == 0);
    Require(!muted);

    Require(sceCustomMusicAudioOutSetVolume(0.75) == 0);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(std::abs(vol - 0.75) < 0.001);

    Require(sceCustomMusicAudioOutGetAllMusicVolume(&vol) == 0);
    Require(std::abs(vol - 0.5) < 0.001);

    Require(sceCustomMusicAudioOutSetVolume(-1.0) == 0);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(vol == 0.0);

    Require(sceCustomMusicAudioOutSetVolume(2.0) == 0);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(vol == 1.0);

    Require(sceCustomMusicAudioOutSetAllMusicVolume(0.25) == 0);
    Require(sceCustomMusicAudioOutGetAllMusicVolume(&vol) == 0);
    Require(std::abs(vol - 0.25) < 0.001);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(std::abs(vol - 0.25) < 0.001);

    Require(sceCustomMusicAudioOutSetMuted(true) == 0);
    Require(sceCustomMusicAudioOutGetMuted(&muted) == 0);
    Require(muted);

    Require(sceCustomMusicAudioOutSetMuted(false) == 0);
    Require(sceCustomMusicAudioOutGetMuted(&muted) == 0);
    Require(!muted);

    Require(sceCustomMusicAudioOutOutput(nullptr) == 0);

    std::uint64_t outTime = 0;
    Require(sceCustomMusicAudioOutGetLastOutputTime(&outTime) == 0);

    Require(sceCustomMusicAudioOutInitialize(256, 1, 4) == 0);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == 0);
    Require(std::abs(vol - 0.25) < 0.001);

    Require(sceCustomMusicAudioOutFinalize() == 0);
    Require(sceCustomMusicAudioOutFinalize() == ERROR_NOT_INITIALIZED);
    Require(sceCustomMusicAudioOutGetVolume(&vol) == ERROR_NOT_INITIALIZED);
}

} // namespace

int main() {
    TestUninitialized();
    TestNullPointers();
    TestInitializeValidation();
    TestVolumeAndMuteLifecycle();
    return 0;
}
