
#include "mem.h"

void memCpy(void* pAddr, const void* pMem, unsigned Size)
{
#ifndef __linux__
    unsigned long oldPro, Pro;
    ::VirtualProtect(pAddr, Size, PAGE_EXECUTE_READWRITE, &oldPro);
    ::memcpy(pAddr, pMem, Size);
    ::VirtualProtect(pAddr, Size, oldPro, &Pro);
#else
    auto pageSize = ::sysconf(_SC_PAGESIZE);
    auto alignedAddr = (unsigned)pAddr & ~(pageSize - 1);
    ::mprotect(alignedAddr, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    ::memcpy(pAddr, pMem, Size);
    ::mprotect(alignedAddr, pageSize, PROT_READ | PROT_EXEC);
#endif
}

const unsigned char* memCmp(const unsigned char* pPos, unsigned Range, int Ref, unsigned Pos)
{
    auto pEnd = pPos + Range;
    for (; pPos < pEnd; ++pPos)
        if (*pPos == (unsigned char)'\x83' && *(pPos + Pos) == Ref)
            return pPos;
    return NULL;
}

#ifndef __linux__
bool memCmpChr(const unsigned char* pAddr, const unsigned char* pPattern, const unsigned char* pEnd)
{
    for (auto pChr = pPattern; pChr < pEnd; ++pChr, ++pAddr)
    {
        if (*pChr == *pAddr || *pChr == (unsigned char)'\x2A')
            continue;
        return false;
    }
    return true;
}

const unsigned char* memFindRefChr(const unsigned char* pPos, const unsigned char* pEnd, unsigned char Code, unsigned Ref, unsigned Pos)
{
    for (; pPos < pEnd; ++pPos)
        if (*pPos == Code && *(unsigned*)(pPos + Pos) == Ref)
            return pPos;
    return NULL;
}

const unsigned char* findPattern(const unsigned char* pPos, unsigned Range, const unsigned char* pPattern, unsigned Size)
{
    auto pPatternEnd = pPattern + Size;
    for (auto pEnd = pPos + Range - Size; pPos < pEnd; ++pPos)
        if (::memCmpChr(pPos, pPattern, pPatternEnd))
            return pPos;
    return NULL;
}

const unsigned char* findStrPush(const unsigned char* pAddr, unsigned Size, const unsigned char* pString, unsigned lenZero)
{
    auto pRef = ::findPattern(pAddr, Size, pString, lenZero);
    return ::memFindRefChr(pAddr, pAddr + Size - 5, (unsigned char)'\x68', (unsigned)pRef, 1);
}
#else
void* dlsymComplex(void* pLib, const char* pSym)
{ /// On Linux, use DWARF module information as well when looking to reveal library symbols (not only '::dlsym').
    auto pAddr = ::dlsym(pLib, pSym);
    if (pAddr)
        return pAddr;
    auto pLinkMap = (::link_map*)pLib;
    auto binFile = ::open(pLinkMap->l_name, O_RDONLY);
    auto binSize = ::lseek(binFile, false, SEEK_END);
    auto pMemMap = ::mmap(nullptr, binSize, PROT_READ, MAP_PRIVATE, binFile, false);
    ::close(binFile);
    auto pEHdr = (::ElfW(Ehdr)*) pMemMap;
    auto pSHdrs = (::ElfW(Shdr)*) ((char*)pMemMap + pEHdr->e_shoff);
    auto pSHStrTab = (char*)pMemMap + pSHdrs[pEHdr->e_shstrndx].sh_offset;
    for (unsigned short i = false; i < pEHdr->e_shnum; ++i)
    {
        auto pSecName = pSHStrTab + pSHdrs[i].sh_name;
        if (!::strcmp(pSecName, ".symtab"))
        {
            auto pSyms = (::ElfW(Sym)*) ((char*)pMemMap + pSHdrs[i].sh_offset);
            auto Syms = pSHdrs[i].sh_size / sizeof(::ElfW(Sym));
            auto& strTabHdr = pSHdrs[pSHdrs[i].sh_link];
            auto pStrTab = (char*)pMemMap + strTabHdr.sh_offset;
            for (::size_t j = false; j < Syms; ++j)
            {
                auto pName = pStrTab + pSyms[j].st_name;
                if (pName && *pName && !::strcmp(pName, pSym))
                {
                    pAddr = (void*)(pSyms[j].st_value + pLinkMap->l_addr);
                    goto endOfFunc;
                }
            }
        }
    }
endOfFunc:
    ::munmap(pMemMap, binSize);
    return pAddr;
}
#endif
