// DynamicString.h

#ifndef _PPBOX_COMMON_DYNAMIC_STRING_H_
#define _PPBOX_COMMON_DYNAMIC_STRING_H_

#include <framework/system/FileTag.h>

#define DEFINE_DYNAMIC_STRING(name, value) \
DEFINE_FILE_TAG_NAME(dynamic_string, name, value) \
static std::string const name(FILE_TAG_NAME(name))

#endif // _PPBOX_COMMON_DYNAMIC_STRING_H_
