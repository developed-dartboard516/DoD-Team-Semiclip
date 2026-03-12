
#ifndef _MEMORY_H_
#define _MEMORY_H_

#ifndef __linux__
#include <Windows.h>
#else
#include <string.h>
#include <fcntl.h>
#include <link.h>
#include <unistd.h>
#include <sys/mman.h>
#endif

void allowFullMemAccess(void*, ::size_t);
void memCpy(void*, const void*, ::size_t);
const unsigned char* memCmp(const unsigned char*, ::size_t, int, ::size_t);

#ifndef __linux__
bool memCmpChr(const unsigned char*, const unsigned char*, const unsigned char*);
const unsigned char* memFindRefChr(const unsigned char*, const unsigned char*, unsigned char, ::size_t, ::size_t);
const unsigned char* findPattern(const unsigned char*, ::size_t, const unsigned char*, ::size_t);
const unsigned char* findStrPush(const unsigned char*, ::size_t, const unsigned char*, ::size_t);
#else
void* dlsymComplex(void*, const char*);
#endif

#endif
