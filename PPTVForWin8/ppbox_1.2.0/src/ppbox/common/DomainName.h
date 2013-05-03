// DomainName.h

#ifndef _PPBOX_COMMON_DOMAIN_NAME_H_
#define _PPBOX_COMMON_DOMAIN_NAME_H_

#include <framework/network/NetName.h>
#include <framework/system/FileTag.h>

#define DEFINE_DOMAIN_NAME(name, value) \
DEFINE_FILE_TAG_NAME(domain_name, name, value) \
static framework::network::NetName const name(FILE_TAG_NAME(name))

#endif // _PPBOX_COMMON_DOMAIN_NAME_H_
