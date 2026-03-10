
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

void memCpy(void*, const void*, unsigned);
const unsigned char* memCmp(const unsigned char*, unsigned, int, unsigned);

#ifndef __linux__
bool memCmpChr(const unsigned char*, const unsigned char*, const unsigned char*);
const unsigned char* memFindRefChr(const unsigned char*, const unsigned char*, unsigned char, unsigned, unsigned);
const unsigned char* findPattern(const unsigned char*, unsigned, const unsigned char*, unsigned);
const unsigned char* findStrPush(const unsigned char*, unsigned, const unsigned char*, unsigned);
#else
void* dlsymComplex(void*, const char*);
#endif

#endif
