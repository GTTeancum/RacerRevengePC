/* Target configuration companion for the unmodified FFmpeg 7.1.5 headers.
 * FFmpeg's configure emits exactly these two public feature macros.
 * The adapter uses compiler target macros on Linux, and known little-endian
 * x86/x86-64/ARM/ARM64 Windows targets. Unknown Windows targets are rejected.
 * No ABI structs or library implementation are defined here.
 */
#ifndef AVUTIL_AVCONFIG_H
#define AVUTIL_AVCONFIG_H
#if defined(_WIN32)
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || \
    defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__)
#define AV_HAVE_BIGENDIAN 0
#else
#error Unsupported Windows target for FFmpeg public headers
#endif
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define AV_HAVE_BIGENDIAN 1
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define AV_HAVE_BIGENDIAN 0
#else
#error Cannot determine target endianness for FFmpeg public headers
#endif
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM64) || \
    defined(__x86_64__) || defined(__i386__) || defined(__aarch64__)
#define AV_HAVE_FAST_UNALIGNED 1
#else
#define AV_HAVE_FAST_UNALIGNED 0
#endif
#endif /* AVUTIL_AVCONFIG_H */
