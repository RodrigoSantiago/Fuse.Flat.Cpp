//
// Created by Rodrigo on 21/03/2025.
//

#ifndef FLAT_FLAT_BASE_TYPES_H
#define FLAT_FLAT_BASE_TYPES_H

#ifdef WINDOWS_PLATFORM
#define int32 long
#define uint32 unsigned long
#endif

#ifdef UNIX_PLATFORM
#define int32 int
#define uint32 unsigned int
#endif

#endif //FLAT_FLAT_BASE_TYPES_H
