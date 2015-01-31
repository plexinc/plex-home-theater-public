// //////////////////////////////////////////////////////////
// endian.h
// Copyright (c) 2014 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

#ifndef _ENDIAN_H
#define _ENDIAN_H

// big endian architectures need #define __BYTE_ORDER __BIG_ENDIAN
#ifndef _MSC_VER
#ifdef __APPLE__
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#endif

#if defined(__BYTE_ORDER) && (__BYTE_ORDER != 0) && (__BYTE_ORDER == __BIG_ENDIAN)
#define HASH_LITTLEENDIAN(x) swap(x)
#else
#define HASH_LITTLEENDIAN(x) (x)
#endif

#endif // _ENDIAN_H