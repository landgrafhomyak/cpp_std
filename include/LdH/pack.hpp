#ifndef LdH_PACK_HPP
# define LdH_PACK_HPP

#include "ccheck.hpp"
#include "int.hpp"

namespace LdH
{
    namespace __private
    {
        using pack_deault_buffer_t = ::LdH::u8 *;
    }

    enum PackOrder
    {
        PackOrder_LittleEndian = '<',
        PackOrder_BigEndian = '>',
    };

    template<::LdH::size_t size>
    class pack_padding
    {
    public:
        pack_padding() noexcept = default;
    };

    template<::LdH::u8 value>
    class pack_const
    {
    public:
        pack_const() noexcept = default;
    };

    template<typename T>
    constexpr bool supports_pack =
            ::LdH::is_same<T, s8> || ::LdH::is_same<T, u8> ||
            ::LdH::is_same<T, s16> || ::LdH::is_same<T, u16> ||
            ::LdH::is_same<T, s32> || ::LdH::is_same<T, u32> ||
            ::LdH::is_same<T, s64> || ::LdH::is_same<T, u64>;

    template<::LdH::size_t size>
    constexpr bool supports_pack<pack_padding<size>> = true;

    template<::LdH::size_t size>
    pack_padding<size> pack_padding_v{};

    template<::LdH::u8 value>
    constexpr bool supports_pack<pack_const<value>> = true;

    template<::LdH::u8 value>
    pack_const<value> pack_const_v{};

    namespace __private
    {
        template<PackOrder ORDER, typename u_value_t, class buffer_t, ::LdH::size_t size, size_t offset = 0>
        struct pack_integer
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept;

            static u_value_t unpack(buffer_t buffer) noexcept;
        };

        template<typename u_value_t, class buffer_t, ::LdH::size_t size, ::LdH::size_t offset>
        struct pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((::LdH::u8) (value >> (offset * 8))) & 0xFF;
                if constexpr(offset + 1 < size)
                    ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << (offset * 8);
                if constexpr(offset + 1 < size)
                    value |= ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };

        template<typename u_value_t, class buffer_t, ::LdH::size_t size, ::LdH::size_t offset>
        struct pack_integer<PackOrder_LittleEndian, u_value_t, buffer_t, size, offset>
        {
            static void pack(buffer_t buffer, u_value_t value) noexcept
            {
                buffer[offset] = ((::LdH::u8) (value >> ((size - offset - 1) * 8))) & 0xFF;
                if constexpr(offset < size)
                    ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << ((size - offset - 1) * 8);
                if constexpr(offset < size)
                    value |= ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::unpack(buffer);
                return value;
            }
        };
    }

    template<PackOrder ORDER, typename T, class buffer_t>
    struct _pack1
    {
        static_assert(::LdH::supports_pack<T>, "Requested type can't be packed or unpacked");
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s8, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static void pack(buffer_t buffer, ::LdH::s8 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) (::LdH::u8) value); }

        static void unpack(buffer_t buffer, ::LdH::s8 *storage) noexcept
        { *storage = (::LdH::s8) (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u8, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static void pack(buffer_t buffer, ::LdH::u8 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) value); }

        static void unpack(buffer_t buffer, ::LdH::u8 *storage) noexcept
        { *storage = (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s16, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 2;

        static void pack(buffer_t buffer, short value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) (::LdH::u16) value); }

        static void unpack(buffer_t buffer, ::LdH::s16 *storage) noexcept
        { *storage = (::LdH::s16) (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u16, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 2;

        static void pack(buffer_t buffer, ::LdH::u16 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) value); }

        static void unpack(buffer_t buffer, ::LdH::u16 *storage) noexcept
        { *storage = (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s32, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 4;

        static void pack(buffer_t buffer, ::LdH::s32 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) (::LdH::u32) value); }

        static void unpack(buffer_t buffer, ::LdH::s32 *storage) noexcept
        { *storage = (::LdH::s32) (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u32, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 4;

        static void pack(buffer_t buffer, ::LdH::u32 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) value); }

        static void unpack(buffer_t buffer, ::LdH::u32 *storage) noexcept
        { *storage = (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::s64, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 8;

        static void pack(buffer_t buffer, ::LdH::s64 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) (::LdH::u64) value); }

        static void unpack(buffer_t buffer, ::LdH::s64 *storage) noexcept
        { *storage = (::LdH::s64) (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, class buffer_t>
    struct _pack1<ORDER, ::LdH::u64, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 8;

        static void pack(buffer_t buffer, ::LdH::u64 value) noexcept
        { ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) value); }

        static void unpack(buffer_t buffer, ::LdH::u64 *storage) noexcept
        { *storage = (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer); }
    };

    template<PackOrder ORDER, ::LdH::size_t SIZE, class buffer_t>
    struct _pack1<ORDER, ::LdH::pack_padding<SIZE>, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = SIZE;

        static void pack(buffer_t buffer, ::LdH::pack_padding<SIZE> value) noexcept
        {
            buffer[0] = '\0';
            if constexpr(SIZE > 0)
                ::LdH::_pack1<ORDER, ::LdH::pack_padding<SIZE - 1>, decltype(buffer + 1)>::pack(buffer + 1, {});
        }

        static void unpack(buffer_t buffer, ::LdH::pack_padding<SIZE> *storage) noexcept
        {}
    };

    template<PackOrder ORDER, ::LdH::u8 VALUE, class buffer_t>
    struct _pack1<ORDER, ::LdH::pack_const<VALUE>, buffer_t>
    {
        static constexpr ::LdH::size_t PACKED_SIZE = 1;

        static void pack(buffer_t buffer, ::LdH::pack_const<VALUE> value) noexcept
        {
            buffer[0] = VALUE;
        }

        static void unpack(buffer_t buffer, ::LdH::pack_const<VALUE> *storage) noexcept
        {}
    };

    template<typename T>
    constexpr ::LdH::size_t packed_size = ::LdH::_pack1<PackOrder_BigEndian, T, uint_least8_t *>::PACKED_SIZE;

    template<::LdH::size_t size>
    constexpr ::LdH::size_t packed_size<pack_padding<size>> = size;

    template<PackOrder ORDER, typename T, class buffer_t = ::LdH::__private::pack_deault_buffer_t>
    void pack1(buffer_t buffer, T value) noexcept
    {
        ::LdH::_pack1<ORDER, T, buffer_t>::pack(buffer, value);
    }

    template<PackOrder ORDER, typename T, class buffer_t = ::LdH::__private::pack_deault_buffer_t>
    void unpack1(buffer_t buffer, T *storage) noexcept
    {
        ::LdH::_pack1<ORDER, T, buffer_t>::unpack(buffer, storage);
    }

    namespace __private
    {
        template<typename A, typename ...T>
        struct packed_struct_size
        {
            static constexpr ::LdH::size_t SIZE = ::LdH::packed_size<A> + ::LdH::__private::packed_struct_size<T...>::SIZE;
        };

        template<typename T>
        struct packed_struct_size<T>
        {
            static constexpr ::LdH::size_t SIZE = ::LdH::packed_size<T>;
        };

    }

    template<PackOrder ORDER, typename A, typename ...T>
    class pack_struct
    {
    public:
        static constexpr ::LdH::size_t PACKED_SIZE = ::LdH::__private::packed_struct_size<A, T...>::SIZE;

        template<class buffer_t = ::LdH::__private::pack_deault_buffer_t>
        static void pack(buffer_t buffer, A first_value, T... other_values) noexcept
        {
            ::LdH::_pack1<ORDER, A, buffer_t>::pack(buffer, first_value);
            ::LdH::pack_struct<ORDER, T...>::template pack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_values...);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(buffer_t buffer, A *first_storage, T *... other_storages) noexcept
        {
            ::LdH::_pack1<ORDER, A, buffer_t>::unpack(buffer, first_storage);
            ::LdH::pack_struct<ORDER, T...>::template unpack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_storages...);
        }
    };

    template<PackOrder ORDER, typename A>
    class pack_struct<ORDER, A>
    {
    public:
        static constexpr ::LdH::size_t PACKED_SIZE = ::LdH::packed_size<A>;

        template<class buffer_t = ::LdH::__private::pack_deault_buffer_t>
        static void pack(buffer_t buffer, A value) noexcept
        {
            ::LdH::_pack1<ORDER, A, buffer_t>::pack(buffer, value);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(buffer_t buffer, A *storage) noexcept
        {
            ::LdH::_pack1<ORDER, A, buffer_t>::unpack(buffer, storage);
        }
    };
}

#endif // LdH_PACK_HPP
