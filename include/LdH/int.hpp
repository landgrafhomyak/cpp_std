#ifndef LdH_INT_HPP
# define LdH_INT_HPP

# include <cstdint>
# include <climits>

namespace LdH
{
# ifdef UINT8_MAX
    using u8 = uint8_t;
#  define LdH_U8_STRICT
# else
    using u8 = uint_least8_t;
# endif
    using u8f = uint_fast8_t;

# ifdef INT8_MAX
    using s8 = int8_t;
#  define LdH_S8_STRICT
# else
    using s8 = int_least8_t;
# endif
    using s8f = int_fast8_t;

# ifdef UINT16_MAX
    using u16 = uint16_t;
#  define LdH_U16_STRICT
# else
    using u16 = uint_least16_t;
# endif
    using u16f = uint_fast16_t;

# ifdef INT16_MAX
    using s16 = int16_t;
#  define LdH_S16_STRICT
# else
    using s16 = int_least16_t;
# endif
    using s16f = int_fast16_t;

# ifdef UINT32_MAX
    using u32 = uint32_t;
#  define LdH_U32_STRICT
# else
    using u32 = uint_least32_t;
# endif
    using u32f = uint_fast32_t;

# ifdef INT32_MAX
    using s32 = int32_t;
#  define LdH_S32_STRICT
# else
    using s32 = int_least32_t;
# endif
    using s32f = int_fast32_t;

# ifdef UINT64_MAX
    using u64 = uint64_t;
#  define LdH_U64_STRICT
# else
    using u64 = uint_least64_t;
# endif
    using u64f = uint_fast64_t;

# ifdef INT64_MAX
    using s64 = int64_t;
#  define LdH_S64_STRICT
# else
    using s64 = int_least64_t;
# endif
    using s64f = int_fast64_t;

    template<typename T>
    struct int_type_info
    {
    };

    template<typename T>
    constexpr bool is_unsigned = int_type_info<T>::IS_UNSIGNED;

    template<typename T>
    constexpr T max_value = int_type_info<T>::MAX_VALUE;

    template<typename T>
    constexpr T min_value = int_type_info<T>::MIN_VALUE;

    template<>
    struct int_type_info<u8>
    {
        static constexpr bool IS_UNSIGNED = true;
        static constexpr u8 MAX_VALUE = 0xFF;
        static constexpr u8 MIN_VALUE = 0;
# ifdef LdH_U8_STRICT
        static constexpr u8 REAL_MAX_VALUE = UINT8_MAX;
# else
        static constexpr u8 REAL_MAX_VALUE = UINT_LEAST8_MAX;
# endif
        static constexpr u8 REAL_MIN_VALUE = 0;
    };

    template<>
    struct int_type_info<s8>
    {
        static constexpr bool IS_UNSIGNED = false;
        static constexpr s8 MAX_VALUE = 0x7F;
        static constexpr s8 MIN_VALUE = -0x80;
# ifdef LdH_S8_STRICT
        static constexpr s8 REAL_MAX_VALUE = INT8_MAX;
        static constexpr s8 REAL_MIN_VALUE = INT8_MIN;
# else
        static constexpr s8 REAL_MAX_VALUE = INT_LEAST8_MAX;
        static constexpr s8 REAL_MIN_VALUE = INT_LEAST8_MIN;
# endif
    };

    template<>
    struct int_type_info<u16>
    {
        static constexpr bool IS_UNSIGNED = true;
        static constexpr u16 MAX_VALUE = 0xFFFF;
        static constexpr u16 MIN_VALUE = 0;
# ifdef LdH_U16_STRICT
        static constexpr u16 REAL_MAX_VALUE = UINT16_MAX;
# else
        static constexpr u16 REAL_MAX_VALUE = UINT_LEAST16_MAX;
# endif
        static constexpr u16 REAL_MIN_VALUE = 0;
    };

    template<>
    struct int_type_info<s16>
    {
        static constexpr bool IS_UNSIGNED = false;
        static constexpr s16 MAX_VALUE = 0x7FFF;
        static constexpr s16 MIN_VALUE = -0x8000;
# ifdef LdH_S16_STRICT
        static constexpr s16 REAL_MAX_VALUE = INT16_MAX;
        static constexpr s16 REAL_MIN_VALUE = INT16_MIN;
# else
        static constexpr s16 REAL_MAX_VALUE = INT_LEAST16_MAX;
        static constexpr s16 REAL_MIN_VALUE = INT_LEAST16_MIN;
# endif
    };

    template<>
    struct int_type_info<u32>
    {
        static constexpr bool IS_UNSIGNED = true;
        static constexpr u32 MAX_VALUE = 0xFFFF'FFFFul;
        static constexpr u32 MIN_VALUE = 0;
# ifdef LdH_U32_STRICT
        static constexpr u32 REAL_MAX_VALUE = UINT32_MAX;
# else
        static constexpr u32 REAL_MAX_VALUE = UINT_LEAST32_MAX;
#endif
        static constexpr u32 REAL_MIN_VALUE = 0;
    };

    template<>
    struct int_type_info<s32>
    {
        static constexpr bool IS_UNSIGNED = false;
        static constexpr s32 MAX_VALUE = 0x7FFF'FFFFl;
        static constexpr s32 MIN_VALUE = -0x8000'0000l;
# ifdef LdH_S32_STRICT
        static constexpr s32 REAL_MAX_VALUE = INT32_MAX;
        static constexpr s32 REAL_MIN_VALUE = INT32_MIN;
# else
        static constexpr s32 REAL_MAX_VALUE = INT_LEAST32_MAX;
        static constexpr s32 REAL_MIN_VALUE = INT_LEAST32_MIN;
# endif
    };

    template<>
    struct int_type_info<u64>
    {
        static constexpr bool IS_UNSIGNED = true;
        static constexpr u64 MAX_VALUE = 0xFFFF'FFFF'FFFF'FFFFull;
        static constexpr u64 MIN_VALUE = 0;
# ifdef LdH_U64_STRICT
        static constexpr u64 REAL_MAX_VALUE = UINT64_MAX;
# else
        static constexpr u64 REAL_MAX_VALUE = UINT_LEAST64_MAX;
# endif
        static constexpr u64 REAL_MIN_VALUE = 0;
    };

    template<>
    struct int_type_info<s64>
    {
        static constexpr bool IS_UNSIGNED = false;
        static constexpr s64 MAX_VALUE = 0x7FFF'FFFF'FFFF'FFFFll;
        static constexpr s64 MIN_VALUE = -0x7FFF'FFFF'FFFF'FFFFll - 1;
# ifdef LdH_S64_STRICT
        static constexpr s64 REAL_MAX_VALUE = INT64_MAX;
        static constexpr s64 REAL_MIN_VALUE = INT64_MIN;
# else
        static constexpr s64 REAL_MAX_VALUE = INT_LEAST64_MAX;
        static constexpr s64 REAL_MIN_VALUE = INT_LEAST64_MIN;
# endif
    };

    template<>
    constexpr bool is_unsigned<unsigned int> = true;

    template<>
    constexpr bool is_unsigned<signed int> = false;
}

#endif // LdH_INT_HPP
