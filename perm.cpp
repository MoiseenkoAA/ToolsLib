//==============================================================================
// PROJECT: ToolsLib
//==============================================================================
// FILE:       perm.cpp
//
// AUTHOR:     Andrey A. Moisseenko
//
//==============================================================================

#include "perm.h"

//#include "temp.h"

#ifdef _WIN32

#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine, sDbgTmp *)
{
    return _malloc_dbg(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}
void* __cdecl operator new[](size_t nSize, const char* lpszFileName, int nLine, sDbgTmp *)
{
    return ::operator new(nSize, lpszFileName, nLine, (sDbgTmp *)nullptr);
}
void __cdecl operator delete(void* pData, const char* /*lpszFileName*/, int /*nLine*/, sDbgTmp *)
{
    _free_dbg(pData, _NORMAL_BLOCK);
}
void __cdecl operator delete[](void* pData, const char* lpszFileName, int nLine, sDbgTmp *)
{
    ::operator delete(pData, lpszFileName, nLine, (sDbgTmp *)nullptr);
}
/*
void* __cdecl operator new(size_t nSize)
{
     //if   (nSize == 12)
     {
          static int aa = 0;
          aa++;
          aa++;
          aa++;
          aa++;
     }
     return _malloc_dbg(nSize, _NORMAL_BLOCK, "unknown", -1);
}
void __cdecl operator delete(void* pData)
{
     _free_dbg(pData, _NORMAL_BLOCK);
}
*/
#endif

#if defined(_WIN32) && defined(_DEBUG)
//#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )  
#define TL_NEW new(__FILE__, __LINE__, (sDbgTmp *)nullptr)
#else
#define TL_NEW new
#endif

#if defined(_WIN32) && defined(_DEBUG)
int gTL_DbgAlloc() noexcept
{
    static bool bMemDbg = true;
    if (bMemDbg)
    {
        bMemDbg = false;
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        //_CrtSetBreakAlloc(135);
        //_CrtSetBreakAlloc(216);

        //_CrtSetBreakAlloc(112);
        //_CrtSetBreakAlloc(218);
        //_CrtSetBreakAlloc(444);
        //_CrtSetBreakAlloc(2277);

        static char* pLostOneByte1 = new char[1];
        static char* pLostOneByte2 = TL_NEW char[1];
    }
    return 0;
}

static int sg1 = gTL_DbgAlloc(); // TL_DbgAlloc;

#endif

//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )

#ifdef __TOOLS_USE_WINSOCK2
#pragma message( "Compiling for WinSock 2" )
#else
#pragma message( "Compiling for WinSock 1" )
#endif

#endif

bool gbCMaaPtrThrow = false; // = !noexcept_new

//==============================================================================
