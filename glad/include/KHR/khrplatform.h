/*
 * KHR platformu tanımları
 */

#ifndef __khrplatform_h_
#define __khrplatform_h_

/* Temel veri tipleri */
#if defined(_WIN64)
    typedef signed   long long int khronos_intptr_t;
    typedef unsigned long long int khronos_uintptr_t;
    typedef signed   long long int khronos_ssize_t;
    typedef unsigned long long int khronos_usize_t;
#elif defined(_WIN32)
    typedef signed   long  int     khronos_intptr_t;
    typedef unsigned long  int     khronos_uintptr_t;
    typedef signed   long  int     khronos_ssize_t;
    typedef unsigned long  int     khronos_usize_t;
#else
    #include <stdint.h>
    #include <sys/types.h>
    typedef intptr_t                khronos_intptr_t;
    typedef uintptr_t               khronos_uintptr_t;
    typedef ssize_t                 khronos_ssize_t;
    typedef size_t                  khronos_usize_t;
#endif

#if defined(_MSC_VER)
    typedef signed   __int8         khronos_int8_t;
    typedef unsigned __int8         khronos_uint8_t;
    typedef signed   __int16        khronos_int16_t;
    typedef unsigned __int16        khronos_uint16_t;
    typedef signed   __int32        khronos_int32_t;
    typedef unsigned __int32        khronos_uint32_t;
    typedef signed   __int64        khronos_int64_t;
    typedef unsigned __int64        khronos_uint64_t;
#else
    #include <stdint.h>
    typedef int8_t                  khronos_int8_t;
    typedef uint8_t                 khronos_uint8_t;
    typedef int16_t                 khronos_int16_t;
    typedef uint16_t                khronos_uint16_t;
    typedef int32_t                 khronos_int32_t;
    typedef uint32_t                khronos_uint32_t;
    typedef int64_t                 khronos_int64_t;
    typedef uint64_t                khronos_uint64_t;
#endif

#endif /* __khrplatform_h_ */