// PpboxStatusInterface.h

#ifndef _TOOLS_STATUS_TOOLS_STATUS_INTERFACE_H_
#define _TOOLS_STATUS_TOOLS_STATUS_INTERFACE_H_


#ifdef BOOST_HAS_DECLSPEC

#ifdef PPBOX_SOURCE
# define PPBOX_DECL __declspec(dllexport)
#else
# define PPBOX_DECL __declspec(dllimport)
#endif  // PPBOX_SOURCE

#else
# define PPBOX_DECL
#endif

#if __cplusplus
extern "C" {
#endif // __cplusplus

struct PeerBlock
{
    char name[256];
    size_t size;
    const void * ptr;
};

struct PeerInfo
{
    size_t number;
    char peer_pid[20];
    PeerBlock *block;
};

PPBOX_DECL bool PPBOX_StatusStart(const char *ip_address);
PPBOX_DECL bool PPBOX_StatusStop();

enum ErrorEnum
{
    ok, 
    no_update, 
    peer_closed, 
    other_error, 
};

PPBOX_DECL bool PPBOX_StatusGetInfo(PeerInfo *peer_info);

#if __cplusplus
}
#endif // __cplusplus

#endif // _TOOLS_STATUS_TOOLS_STATUS_INTERFACE_H_
