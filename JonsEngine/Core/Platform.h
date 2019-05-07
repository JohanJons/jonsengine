#pragma once

#define WIN32_LEAN_AND_MEAN
// for std::min/max()
#define NOMINMAX

#include <Windows.h>
#include <d3d11.h>

// win32 messages across subsystems
const uint32_t WM_JONS_FULLSCREEN = (WM_USER + 0x0001);
const uint32_t WM_JONS_RESIZE = (WM_USER + 0x0002);
