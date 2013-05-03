//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_

#ifdef BOOST_WINDOWS_API

#include <hash_map> 
using stdext::hash_map; 
using stdext::hash_compare;

#else 

#include <ext/hash_map> 
using __gnu_cxx::hash_map;
namespace __gnu_cxx
{
    template<> struct hash< std::string >
    {
        size_t operator()(const std::string& x) const
        {
            return hash<const char*>()(x.c_str());
        }
    };
}

#endif 

#endif //_HASH_MAP_H_