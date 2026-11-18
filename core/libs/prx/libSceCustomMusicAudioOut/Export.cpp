#include "prx/libSceCustomMusicAudioOut/include/Api.hpp"
#include "prx/libSceAudioOut/include/AudioOut.hpp"
#include <cmath>
#include <cstdint>
#include <mutex>
#include <optional>

namespace {

constexpr int SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM = static_cast<int>(0x81380001);
constexpr int SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_PORT_FULL = static_cast<int>(0x81380002);
constexpr int SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_GENERIC = static_cast<int>(0x81380003);
constexpr int SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED_AUDIO = static_cast<int>(0x81380004);
constexpr int SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED = static_cast<int>(0x81380102);

constexpr int SCE_AUDIO_OUT_ERROR_INVALID_PARAM = static_cast<int>(0x80260006);
constexpr int SCE_AUDIO_OUT_ERROR_PORT_FULL = static_cast<int>(0x8026000D);
constexpr int SCE_AUDIO_OUT_ERROR_NOT_INITIALIZED = static_cast<int>(0x80260002);

struct PortState {
    int handle = -1;
    int storedLevel = 0x4000;
    bool muted = false;
};

std::mutex s_mutex;
std::optional<PortState> s_port;
int s_allMusicLevel = 0x4000;
bool s_audioOutInitDone = false;

int mapAudioOutError(int error) {
    if (error == 0) {
        return 0;
    }
    if (error == SCE_AUDIO_OUT_ERROR_INVALID_PARAM) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    if (error == SCE_AUDIO_OUT_ERROR_PORT_FULL) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_PORT_FULL;
    }
    if (error == SCE_AUDIO_OUT_ERROR_NOT_INITIALIZED) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED_AUDIO;
    }
    return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_GENERIC;
}

int applyPortLevel(PortState& port, int level) {
    port.storedLevel = level;
    int vol[8];
    for (int i = 0; i < 8; ++i) {
        vol[i] = port.muted ? 0 : level;
    }
    int ret = sceAudioOutSetVolume(port.handle, 0xff, vol);
    return mapAudioOutError(ret);
}

} // namespace

extern "C" {

int APS5_VABI sceCustomMusicAudioOutInitialize(int grain, int a2, int a3) {
    if ((a3 != 1 && a3 != 4) || (a2 != 0 && a2 != 1)) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_audioOutInitDone) {
        s_audioOutInitDone = true;
        static_cast<void>(sceAudioOutInit());
    }
    uint32_t param = static_cast<uint32_t>(a3) | (a2 == 1 ? 0x10000u : 0u);
    int handle = sceAudioOutOpen(0xff, 1, 0, static_cast<uint32_t>(grain), 48000, param);
    if (handle < 0) {
        return mapAudioOutError(handle);
    }
    if (s_port.has_value() && s_port->handle >= 0) {
        static_cast<void>(sceAudioOutClose(s_port->handle));
    }
    s_port = PortState{
        .handle = handle,
        .storedLevel = s_allMusicLevel,
        .muted = false,
    };
    int vol[8];
    for (int i = 0; i < 8; ++i) {
        vol[i] = s_allMusicLevel;
    }
    static_cast<void>(sceAudioOutSetVolume(handle, 0xff, vol));
    return 0;
}

int APS5_VABI sceCustomMusicAudioOutFinalize() {
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    if (s_port->handle >= 0) {
        int ret = sceAudioOutClose(s_port->handle);
        int mapped = mapAudioOutError(ret);
        if (mapped != 0) {
            return mapped;
        }
    }
    s_port.reset();
    return 0;
}

int APS5_VABI sceCustomMusicAudioOutSetVolume(double v) {
    if (v < 0.0) {
        v = 0.0;
    } else if (v > 1.0) {
        v = 1.0;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int level = static_cast<int>(std::round(v * 32768.0));
    return applyPortLevel(*s_port, level);
}

int APS5_VABI sceCustomMusicAudioOutGetVolume(double* out) {
    if (out == nullptr) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    double v = static_cast<double>(s_port->storedLevel) / 32768.0;
    if (v < 0.0) {
        v = 0.0;
    } else if (v > 1.0) {
        v = 1.0;
    }
    *out = v;
    return 0;
}

int APS5_VABI sceCustomMusicAudioOutSetAllMusicVolume(double v) {
    if (v < 0.0) {
        v = 0.0;
    } else if (v > 1.0) {
        v = 1.0;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int level = static_cast<int>(std::round(v * 32768.0));
    s_allMusicLevel = level;
    return applyPortLevel(*s_port, level);
}

int APS5_VABI sceCustomMusicAudioOutGetAllMusicVolume(double* out) {
    if (out == nullptr) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int level = s_allMusicLevel;
    if (level < 0) {
        level = 0x4000;
    } else if (level > 0x8000) {
        level = 0x8000;
    }
    double v = static_cast<double>(level) / 32768.0;
    if (v < 0.0) {
        v = 0.0;
    } else if (v > 1.0) {
        v = 1.0;
    }
    *out = v;
    return 0;
}

int APS5_VABI sceCustomMusicAudioOutSetMuted(bool muted) {
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int vol[8];
    for (int i = 0; i < 8; ++i) {
        vol[i] = muted ? 0 : s_port->storedLevel;
    }
    int ret = sceAudioOutSetVolume(s_port->handle, 0xff, vol);
    s_port->muted = muted;
    if (ret == 0) {
        return 0;
    }
    if (ret < 0) {
        return mapAudioOutError(ret);
    }
    return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_GENERIC;
}

int APS5_VABI sceCustomMusicAudioOutGetMuted(bool* out) {
    if (out == nullptr) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    *out = s_port->muted;
    return 0;
}

int APS5_VABI sceCustomMusicAudioOutOutput(const void* buf) {
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int ret = sceAudioOutOutput(s_port->handle, buf);
    if (ret >= 0) {
        return 0;
    }
    return mapAudioOutError(ret);
}

int APS5_VABI sceCustomMusicAudioOutGetLastOutputTime(std::uint64_t* out) {
    if (out == nullptr) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_port.has_value()) {
        return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_NOT_INITIALIZED;
    }
    int ret = sceAudioOutGetLastOutputTime(s_port->handle, out);
    if (ret == 0) {
        return 0;
    }
    if (ret < 0) {
        return mapAudioOutError(ret);
    }
    return SCE_CUSTOM_MUSIC_AUDIO_OUT_ERROR_GENERIC;
}

}
