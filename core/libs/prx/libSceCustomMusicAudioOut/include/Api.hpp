#pragma once

#include <cstdint>
#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceCustomMusicAudioOutFinalize();
int APS5_VABI sceCustomMusicAudioOutGetAllMusicVolume(double* out);
int APS5_VABI sceCustomMusicAudioOutGetLastOutputTime(std::uint64_t* out);
int APS5_VABI sceCustomMusicAudioOutGetMuted(bool* out);
int APS5_VABI sceCustomMusicAudioOutGetVolume(double* out);
int APS5_VABI sceCustomMusicAudioOutInitialize(int grain, int a2, int a3);
int APS5_VABI sceCustomMusicAudioOutOutput(const void* buf);
int APS5_VABI sceCustomMusicAudioOutSetAllMusicVolume(double v);
int APS5_VABI sceCustomMusicAudioOutSetMuted(bool muted);
int APS5_VABI sceCustomMusicAudioOutSetVolume(double v);

}
