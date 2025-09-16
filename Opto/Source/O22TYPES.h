//-----------------------------------------------------------------------------
//
// O22TYPES.h
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 2018 by Opto 22
//
// We rely on the C99 standarized types (e.g. uint8_t, uint32_t, etc.) for
// portability. Unfortunately, stdint.h is not available in Visual Studio 
// versions prior to 2010, so we have to define them ourselves here.
//-----------------------------------------------------------------------------

#ifndef __O22TYPES_H_
#define __O22TYPES_H_

#ifndef _MSC_VER
#include <stdint.h>
#else

#if (_MSC_VER >= 1600)
#include <stdint.h>

#else

// Older Visual Studio versions do not
// realize that, e.g. char has the same size as __int8
// so we give up on __intX for them.
#if (_MSC_VER < 1300)
   typedef signed char       int8_t;
   typedef signed short      int16_t;
   typedef signed int        int32_t;
   typedef unsigned char     uint8_t;
   typedef unsigned short    uint16_t;
   typedef unsigned int      uint32_t;
#else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
#endif // _MSC_VER < 1300
typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;

#endif // _MSC_VER >= 1600
#endif // ifndef _MSC_VER

#endif /* include guard */
