// windows.h

#pragma once

#ifdef _WIN32_WINNT
#  undef _WIN32_WINNT
#endif

#define WIN_PHONE    1

#include <..\Include\windows.h> // orignal windows.h

#define WINAPI_DECL     __declspec(dllimport)

#include "MemoryEmulation.h"
#include "SystemEmulation.h"
#include "FileSystemEmulation.h"
#include "ThreadEmulation.h"
#include "NetworkEmulation.h"

using namespace MemoryEmulation;
using namespace SystemEmulation;
using namespace FileSystemEmulation;
using namespace ThreadEmulation;
using namespace NetworkEmulation;
