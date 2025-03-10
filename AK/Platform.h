
#pragma once

#ifdef __i386__
#    define AK_ARCH_I386 1
#endif

#ifdef __x86_64__
#    define AK_ARCH_X86_64 1
#endif

#if defined(__APPLE__) && defined(__MACH__)
#    define AK_OS_MACOS
#    define AK_OS_BSD_GENERIC
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#    define AK_OS_BSD_GENERIC
#endif

#define ARCH(arch) (defined(AK_ARCH_##arch) && AK_ARCH_##arch)

#ifdef ALWAYS_INLINE
#    undef ALWAYS_INLINE
#endif
#define ALWAYS_INLINE [[gnu::always_inline]] inline

#ifdef NEVER_INLINE
#    undef NEVER_INLINE
#endif
#define NEVER_INLINE [[gnu::noinline]]

#ifdef FLATTEN
#    undef FLATTEN
#endif
#define FLATTEN [[gnu::flatten]]

#ifndef __pranaos__
#    include <unistd.h>
#    define PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

ALWAYS_INLINE int count_trailing_zeroes_32(unsigned int val)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(val);
#else
    for (u8 i = 0; i < 32; ++i) {
        if ((val >> i) & 1) {
            return i;
        }
    }
    return 0;
#endif
}

ALWAYS_INLINE int count_trailing_zeroes_32_safe(unsigned int val)
{
    if (val == 0)
        return 32;
    return count_trailing_zeroes_32(val);
}

#ifdef AK_OS_BSD_GENERIC
#    define CLOCK_MONOTONIC_COARSE CLOCK_MONOTONIC
#    define CLOCK_REALTIME_COARSE CLOCK_REALTIME
#endif
