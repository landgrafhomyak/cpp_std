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


    template<typename T>
    constexpr bool supports_pack =
            ::LdH::is_same<T, s8> || ::LdH::is_same<T, u8> ||
            ::LdH::is_same<T, s16> || ::LdH::is_same<T, u16> ||
            ::LdH::is_same<T, s32> || ::LdH::is_same<T, u32> ||
            ::LdH::is_same<T, s64> || ::LdH::is_same<T, u64>;

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
                if constexpr(offset < size)
                    ::LdH::__private::pack_integer<PackOrder_BigEndian, u_value_t, buffer_t, size, offset + 1>::pack(buffer, value);
            }

            static u_value_t unpack(buffer_t buffer) noexcept
            {
                u_value_t value = ((u_value_t) (buffer[offset] & 0xFF)) << (offset * 8);
                if constexpr(offset < size)
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

        template<PackOrder ORDER, typename T, class buffer_t>
        struct pack1
        {
            static_assert(::LdH::supports_pack<T>, "Requested type can't be packed or unpacked");
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::s8, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 1;

            static void pack(buffer_t buffer, ::LdH::s8 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) (::LdH::u8) value); }

            static ::LdH::s8 unpack(buffer_t buffer) noexcept
            { return (::LdH::s8) (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::u8, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 1;

            static void pack(buffer_t buffer, ::LdH::u8 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u8f) value); }

            static ::LdH::u8 unpack(buffer_t buffer) noexcept
            { return (::LdH::u8) ::LdH::__private::pack_integer<ORDER, ::LdH::u8f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::s16, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 2;

            static void pack(buffer_t buffer, short value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) (::LdH::u16) value); }

            static ::LdH::s16 unpack(buffer_t buffer) noexcept
            { return (::LdH::s16) (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::u16, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 2;

            static void pack(buffer_t buffer, ::LdH::u16 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u16f) value); }

            static ::LdH::u16 unpack(buffer_t buffer) noexcept
            { return (::LdH::u16) ::LdH::__private::pack_integer<ORDER, ::LdH::u16f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::s32, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 4;

            static void pack(buffer_t buffer, ::LdH::s32 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) (::LdH::u32) value); }

            static ::LdH::s32 unpack(buffer_t buffer) noexcept
            { return (::LdH::s32) (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::u32, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 4;

            static void pack(buffer_t buffer, ::LdH::u32 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u32f) value); }

            static ::LdH::u32 unpack(buffer_t buffer) noexcept
            { return (::LdH::u32) ::LdH::__private::pack_integer<ORDER, ::LdH::u32f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::s64, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 8;

            static void pack(buffer_t buffer, ::LdH::s64 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) (::LdH::u64) value); }

            static ::LdH::s64 unpack(buffer_t buffer) noexcept
            { return (::LdH::s64) (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };

        template<PackOrder ORDER, class buffer_t>
        struct pack1<ORDER, ::LdH::u64, buffer_t>
        {
            static constexpr ::LdH::size_t PACKED_SIZE = 8;

            static void pack(buffer_t buffer, ::LdH::u64 value) noexcept
            { ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::pack(buffer, (::LdH::u64f) value); }

            static ::LdH::u64 unpack(buffer_t buffer) noexcept
            { return (::LdH::u64) ::LdH::__private::pack_integer<ORDER, ::LdH::u64f, buffer_t, PACKED_SIZE>::unpack(buffer); }
        };
    }

    template<typename T>
    constexpr ::LdH::size_t packed_size = ::LdH::__private::pack1<PackOrder_BigEndian, T, uint_least8_t *>::SIZE;

    template<PackOrder ORDER, typename T, class buffer_t = ::LdH::__private::pack_deault_buffer_t>
    void pack1(buffer_t buffer, T value) noexcept
    {
        ::LdH::__private::pack1<ORDER, T, buffer_t>::pack(buffer, value);
    }

    template<PackOrder ORDER, typename T, class buffer_t = ::LdH::__private::pack_deault_buffer_t>
    T unpack1(buffer_t buffer) noexcept
    {
        return ::LdH::__private::pack1<ORDER, T, buffer_t>::unpack(buffer);
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
    class pack_meta
    {
    public:
        static constexpr ::LdH::size_t PACKED_SIZE = ::LdH::__private::packed_struct_size<A, T...>::SIZE;

        template<class buffer_t = ::LdH::__private::pack_deault_buffer_t>
        static void pack(void *buffer, A first_value, T... other_values) noexcept
        {
            ::LdH::__private::pack1<ORDER, A, buffer_t>::pack(first_value, buffer);
            ::LdH::pack_meta<ORDER, T...>::template pack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_values...);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(void *buffer, A *first_storage, T *... other_storages) noexcept
        {
            *first_storage = ::LdH::__private::pack1<ORDER, A, buffer_t>::unpack(buffer);
            ::LdH::pack_meta<ORDER, T...>::template unpack<decltype(buffer + packed_size<A>)>(buffer + packed_size<A>, other_storages...);
        }
    };

    template<PackOrder ORDER, typename A>
    class pack_meta<ORDER, A>
    {
    public:
        static constexpr ::LdH::size_t PACKED_SIZE = ::LdH::packed_size<A>;

        template<class buffer_t = ::LdH::__private::pack_deault_buffer_t>
        static void pack(A value, void *buffer) noexcept
        {
            ::LdH::__private::pack1<ORDER, A, buffer_t>::pack(value, buffer);
        }

        template<class buffer_t = __private::pack_deault_buffer_t>
        static void unpack(A *storage, void *buffer) noexcept
        {
            *storage = ::LdH::__private::pack1<ORDER, A, buffer_t>::unpack(buffer);
        }
    };
}

#endif // LdH_PACK_HPP
