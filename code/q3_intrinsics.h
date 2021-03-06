#if !defined(Q3_INTRINSICS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

// TODO(chris): Intrinsics for different platforms
#if defined(_WIN32)
#include "windows.h"
#endif

#include "intrin.h"

struct bitscan_result
{
    b32 Found;
    u32 Index;
};
inline bitscan_result
BitScanForward(u32 Mask)
{
    bitscan_result Result = {};
    Result.Found = _BitScanForward((unsigned long *)(&Result.Index), Mask);
    return(Result);
}

inline u32
AtomicIncrement(volatile u32 *Value)
{
    u32 Result;
#if defined(_WIN32)
    Result = InterlockedIncrement(Value);
#else
    Assert(!"Not implemeted!");
#endif
    return(Result);
}

inline b32
AtomicCompareExchange(volatile u32 *Destination, u32 Exchange, u32 Comparand)
{
    b32 Result;
#if defined(_WIN32)
    Result = (InterlockedCompareExchange(Destination, Exchange, Comparand) == Comparand);
#else
    Assert(!"Not implemeted!");
#endif
    return(Result);
}

inline u32
GetCurrentThreadID(void)
{
    u32 Result;
#if defined(_WIN32)
    Result = GetCurrentThreadId();
#else
    Assert(!"Not implemeted!");
#endif
    return(Result);
}

#define rdtsc() (u64)__rdtsc()



#define Q3_INTRINSICS_H
#endif
