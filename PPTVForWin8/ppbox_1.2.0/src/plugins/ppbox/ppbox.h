// ppbox.h

#include "plugins/ppbox/include/IDemuxer.h"
#include "plugins/ppbox/include/IDownloader.h"
#include "plugins/ppbox/include/Name.h"

#include <assert.h>
#include <stdlib.h>

#if (defined _WIN32) || (defined WIN32)
#include <windows.h>
#  define dlopen(n, f) LoadLibraryA(n)
#  define dlsym(m, s) GetProcAddress(m, s)
#  define dlclose(m) FreeLibrary(m)
#else
#  include <dlfcn.h>
typedef void * HMODULE;
#endif

#if (defined WINRT) || (defined WIN_PHONE)
static inline HMODULE LoadLibraryA(
	_In_  LPCSTR lpFileName)
{
	static WCHAR pwFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, lpFileName, -1, pwFileName, MAX_PATH);
	return LoadPackagedLibrary(pwFileName, 0);
}
#endif

#define PPBOX_LIB_NAME ppbox::name_string()

#ifndef PPBOX_EXTERN
#define IMPORT_FUNC(name) \
static FT_ ## name FF_ ## name = NULL; \
FT_ ## name fp_ ## name() \
{ \
    if (FF_ ## name == NULL) \
        FF_ ## name = (FT_ ## name)dlsym(PPBOX_Load(), #name); \
    return FF_ ## name; \
}

#else
#define IMPORT_FUNC(name) \
extern FT_ ## name fp_ ## name();
#endif

extern HMODULE PPBOX_Load();
extern void PPBOX_Unload();

IMPORT_FUNC(PPBOX_StartP2PEngine);
#define PPBOX_StartP2PEngine fp_PPBOX_StartP2PEngine()
IMPORT_FUNC(PPBOX_StopP2PEngine);
#define PPBOX_StopP2PEngine fp_PPBOX_StopP2PEngine()
IMPORT_FUNC(PPBOX_GetLastError);
#define PPBOX_GetLastError fp_PPBOX_GetLastError()
IMPORT_FUNC(PPBOX_GetLastErrorMsg);
#define PPBOX_GetLastErrorMsg fp_PPBOX_GetLastErrorMsg()
IMPORT_FUNC(PPBOX_ScheduleCallback);
#define PPBOX_ScheduleCallback fp_PPBOX_ScheduleCallback()
IMPORT_FUNC(PPBOX_CancelCallback);
#define PPBOX_CancelCallback fp_PPBOX_CancelCallback()

IMPORT_FUNC(PPBOX_Open);
#define PPBOX_Open fp_PPBOX_Open()
IMPORT_FUNC(PPBOX_AsyncOpen);
#define PPBOX_AsyncOpen fp_PPBOX_AsyncOpen()
IMPORT_FUNC(PPBOX_OpenEx);
#define PPBOX_OpenEx fp_PPBOX_OpenEx()
IMPORT_FUNC(PPBOX_AsyncOpenEx);
#define PPBOX_AsyncOpenEx fp_PPBOX_AsyncOpenEx()
IMPORT_FUNC(PPBOX_Close);
#define PPBOX_Close fp_PPBOX_Close()
IMPORT_FUNC(PPBOX_GetDuration);
#define PPBOX_GetDuration fp_PPBOX_GetDuration()
IMPORT_FUNC(PPBOX_GetStreamCount);
#define PPBOX_GetStreamCount fp_PPBOX_GetStreamCount()
IMPORT_FUNC(PPBOX_GetStreamInfoEx);
#define PPBOX_GetStreamInfoEx fp_PPBOX_GetStreamInfoEx()
IMPORT_FUNC(PPBOX_ReadSampleEx2);
#define PPBOX_ReadSampleEx2 fp_PPBOX_ReadSampleEx2()
IMPORT_FUNC(PPBOX_Seek);
#define PPBOX_Seek fp_PPBOX_Seek()

IMPORT_FUNC(PPBOX_DownloadOpen);
#define PPBOX_DownloadOpen fp_PPBOX_DownloadOpen()
IMPORT_FUNC(PPBOX_DownloadClose);
#define PPBOX_DownloadClose fp_PPBOX_DownloadClose()
IMPORT_FUNC(PPBOX_GetDownloadInfo);
#define PPBOX_GetDownloadInfo fp_PPBOX_GetDownloadInfo()

#ifndef PPBOX_EXTERN
HMODULE hMod = NULL;
void PPBOX_Unload()
{
  if (hMod) {
    PPBOX_StopP2PEngine();
    dlclose(hMod);
  }
  hMod = NULL;
}
HMODULE PPBOX_Load()
{
    if (hMod == NULL) {
        hMod = dlopen(PPBOX_LIB_NAME, RTLD_LAZY);
		if (hMod == NULL) {
			assert(false);
			_exit(1);
		}
#ifndef PPBOX_DISABLE_AUTO_START
        PP_int32 ret = PPBOX_StartP2PEngine("12", "161", "08ae1acd062ea3ab65924e07717d5994");
        if (ret != ppbox_success && ret != ppbox_already_start) {
            PPBOX_Unload();
            _exit(1);
        }
#endif
    }
    return hMod;
}
#endif
